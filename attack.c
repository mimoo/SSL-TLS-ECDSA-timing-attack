#ifdef __i386__
#  define RDTSC_DIRTY "%eax", "%ebx", "%ecx", "%edx"
#elif __x86_64__
#  define RDTSC_DIRTY "%rax", "%rbx", "%rcx", "%rdx"
#else
# error unknown platform
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define BUFSIZE 1024*1024*10

typedef unsigned long long ticks;

ticks cpu_ticks;			// duration of request in cpu ticks
unsigned long long nano_seconds;	// duration of request in nanoseconds
unsigned long long cpu_speed;		// aprox cpu speed
unsigned char *receive_buffer;			// buffer to save the response

/**
 * error
 *
 * function for error messages
 * 
 */
void error(char *msg)
{
  perror(msg);
}

/**
 * get_ticks
 *
 * runs assembly code to get number of cpu ticks 
 * 
 * @return	number of cpu ticks since cpu started
 */
ticks get_ticks()
{
  ticks           ret = 0;
  unsigned long   minor = 0;
  unsigned long   mayor = 0;

  asm             volatile(
			   "cpuid \n"
			   "rdtsc"
			   :               "=a"(minor),
					   "=d"(mayor)
			   : "a" (0)
			   : "%ebx", "%ecx"
			   );

  ret = ((((ticks) mayor) << 32) | ((ticks) minor));

  return ret;
}
/** 
 * get_aprox_cpu_speed
 *
 * calculates the current cpu clock speed
 *
 * @return	cpu speed in number of ticks
 */
unsigned long long get_aprox_cpu_speed(){

  unsigned long long speed;
  ticks start_ticks, end_ticks;
  start_ticks = get_ticks();
  sleep(1);
  end_ticks = get_ticks();
  speed = (end_ticks - start_ticks);

  return speed;

}

void init(){

  cpu_speed = 0;
  cpu_ticks = 0;
  nano_seconds = 0;
  if(receive_buffer != NULL){
    bzero(receive_buffer, BUFSIZE);
  }
  else{
    receive_buffer = malloc(BUFSIZE);
  }
}

/** 
 * send_request
 *
 * send a request to the given ip adress and port.
 *
 * @param ip Kommandozeilenparameter
 * @param port_no
 * @param request
 */
uint64_t send_request(unsigned int index, char* ip, int port_no, char* request, int len){
	
  int sockfd, n, ii;
  struct hostent *server;
  struct sockaddr_in serv_addr;
  uint64_t start_ticks, end_ticks;
	
  if(port_no <= 0){
    error("ERROR wrong port number");
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    error("ERROR opening socket");
  }
  server = gethostbyname(ip);
  if (server == NULL){
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr,
	(char *) &serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(port_no);
  if (connect(sockfd, (struct sockaddr *) & serv_addr, sizeof(serv_addr)) < 0){
    error("ERROR connecting");
  }
  bzero(receive_buffer, BUFSIZE);

  // Write all but the very last byte	
  n = write(sockfd, request, len - 1);

  // Now send the last byte, which also starts processing at server side.
  n = write(sockfd, request + len - 1, 1);

  // Start the timer...
  register unsigned cyc_high, cyc_low;               
  asm volatile("RDTSCP\n\t"                     
	       "mov %%edx, %0\n\t"             
	       "mov %%eax, %1\n\t"             
	       : "=r" (cyc_high), "=r" (cyc_low)     
	       :: RDTSC_DIRTY);                      
  start_ticks = ((uint64_t)cyc_high << 32) | cyc_low;

  /* We get rid of the error so we can process faster */
  /* if (n < 0){ 
 	error("ERROR writing to socket"); 
  } */

  // Read the first byte
  read(sockfd, receive_buffer, 1);
  
  // Stop the timer
  register unsigned cyc_high, cyc_low;             
  asm volatile("RDTSCP\n\t"                        
	       "mov %%edx, %0\n\t"           
	       "mov %%eax, %1\n\t"           
	       "CPUID\n\t"                   
	       : "=r" (cyc_high), "=r" (cyc_low)   
	       :: RDTSC_DIRTY);                    
  end_ticks = ((uint64_t)cyc_high << 32) | cyc_low; 

  // read the rest of the message
  n = read(sockfd, &receive_buffer[1], BUFSIZE) + 1;

  // save the answer
  char output_file[200];
  sprintf(output_file, "responses/resp_%i", index);
  FILE* output = fopen(output_file, "wb");

  // analyze the packet and re-read if n is too small compared to the length
  int length;
  int node = 1;
  int jj;
  ii = 0;

  // ClientHello.random
  fprintf(output, "6d70d7a74344e7ccf7c3ace7c77ff39f9d9b2ea56d26ac9292224aa32f17c10b");

  // ServerHello.random
  for(jj = 11; jj < 11 + 32; jj++){
    fprintf(output, "%02x", receive_buffer[jj]);
  }

  // Let's skip the Certificate message
  while(node != 3){
    length = (receive_buffer[ii + 3] << 8) + receive_buffer[ii + 4];
    ii += 5 + length;
    node++;
  }
  
  // Parse the message and the signature
  ii += 9;
  length = receive_buffer[ii+3];

  // ServerKeyExchange.params
  for(jj = ii; jj < ii + 4 + length; jj++){
    fprintf(output, "%02x", receive_buffer[jj]);
  }

  //
  fprintf(output, " ");
  
  // ServerKeyExchange.Signature
  jj = ii + 4 + length + 4;
  length = (receive_buffer[jj - 2] << 8) + receive_buffer[jj - 1];
  for(ii = jj; ii < jj + length; ii++){
    fprintf(output, "%02x", receive_buffer[ii]);
  }

  // close file
  fclose(output);
	
  // Close socket
  close(sockfd);

  // Return cycles
  return end_ticks - start_ticks;
}

/** 
 * convert_ticks_to_nanosecs
 *
 * calculates how much time in nanoseconds has passed 
 * at given ticks on a cpu with a given frequency
 * saves the result to nano_seconds
 *
 * @param no_ticks	number of cpu ticks 
 * @param speed		cpu speed in Hz
 */
void convert_ticks_to_nanosecs(ticks no_ticks, long long speed){
	
  nano_seconds = (double)no_ticks / (double)speed * 1000000000.0;

}

/** 
 * calculate_time
 *
 * gets current cpu speed and calculates time from cpu ticks and cpu speed
 * 
 */
void calculate_time(){

  cpu_speed = get_aprox_cpu_speed();
  convert_ticks_to_nanosecs(cpu_ticks, cpu_speed); 
	
}

/** 
 * get_response
 *
 * returns the response of the last request
 * 
 * @return	the response of the last request
 */
char* get_response(){

  return receive_buffer;
}

/** 
 * get_cpu_ticks
 *
 * returns the number of cpu ticks the last request has taken
 * 
 * @return	number of cpu ticks
 */
ticks get_cpu_ticks(){

  return cpu_ticks;
}

/** 
 * get_time
 *
 * returns the time in nanoseconds the last request has taken
 * 
 * @return	time in nanoseconds
 */
long long get_time(){

  return nano_seconds;
}

/** 
 * get_speed
 *
 * returns the cpu speed calculated by last calculte_time() call
 * 
 * @return	cpu speed in Hz
 */
unsigned long long get_speed(){

  return cpu_speed;
}


/** 
 * main
 *
 * has no actual function
 *
 */
int main(int argc, char *argv[])
{
  char clienthello[] = "\x16\x03\x01\x01\x2e\x01\x00\x01\x2a\x03\x03\x6d\x70\xd7\xa7\x43\x44\xe7\xcc\xf7\xc3\xac\xe7\xc7\x7f\x\
f3\x9f\x9d\x9b\x2e\xa5\x6d\x26\xac\x92\x92\x22\x4a\xa3\x2f\x17\xc1\x0b\x00\x00\x94\xc0\x30\xc0\x2c\xc0\x28\xc0\x24\xc0\x14\xc0\x0a\x\
00\xa3\x00\x9f\x00\x6b\x00\x6a\x00\x39\x00\x38\x00\x88\x00\x87\xc0\x32\xc0\x2e\xc0\x2a\xc0\x26\xc0\x0f\xc0\x05\x00\x9d\x00\x3d\x00\x\
35\x00\x84\xc0\x2f\xc0\x2b\xc0\x27\xc0\x23\xc0\x13\xc0\x09\x00\xa2\x00\x9e\x00\x67\x00\x40\x00\x33\x00\x32\x00\x9a\x00\x99\x00\x45\x\
00\x44\xc0\x31\xc0\x2d\xc0\x29\xc0\x25\xc0\x0e\xc0\x04\x00\x9c\x00\x3c\x00\x2f\x00\x96\x00\x41\x00\x07\xc0\x11\xc0\x07\xc0\x0c\xc0\x\
02\x00\x05\x00\x04\xc0\x12\xc0\x08\x00\x16\x00\x13\xc0\x0d\xc0\x03\x00\x0a\x00\x15\x00\x12\x00\x09\x00\x14\x00\x11\x00\x08\x00\x06\x\
00\x03\x00\xff\x01\x00\x00\x6d\x00\x0b\x00\x04\x03\x00\x01\x02\x00\x0a\x00\x34\x00\x32\x00\x0e\x00\x0d\x00\x19\x00\x0b\x00\x0c\x00\x\
18\x00\x09\x00\x0a\x00\x16\x00\x17\x00\x08\x00\x06\x00\x07\x00\x14\x00\x15\x00\x04\x00\x05\x00\x12\x00\x13\x00\x01\x00\x02\x00\x03\x\
00\x0f\x00\x10\x00\x11\x00\x23\x00\x00\x00\x0d\x00\x20\x00\x1e\x06\x01\x06\x02\x06\x03\x05\x01\x05\x02\x05\x03\x04\x01\x04\x02\x04\x\
03\x03\x01\x03\x02\x03\x03\x02\x01\x02\x02\x02\x03\x00\x0f\x00\x01\x01";

  FILE* output_time = fopen("output.csv", "w");

  int iteration = atoi(argv[1]);

  uint64_t cycles;
  unsigned int ii;
  for(ii = 0; ii < iteration; ii++){
    init();

    printf("#%i\n", ii);
    cycles = send_request(ii, "127.0.0.1", 4433, clienthello, 307);
    fprintf(output_time, "%" PRIu64 "\n", cycles);

  }
  fclose(output_time);

  return 0;
}


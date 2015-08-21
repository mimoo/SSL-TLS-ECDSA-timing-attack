#ifdef __i386__
#  define RDTSC_DIRTY "%eax", "%ebx", "%ecx", "%edx"
#elif __x86_64__
#  define RDTSC_DIRTY "%rax", "%rbx", "%rcx", "%rdx"
#else
# error unknown platform
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
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

unsigned char *receive_buffer; // buffer to save the response

void error(char *msg)
{
  perror(msg);
}

void init(){
  if(receive_buffer != NULL){
    bzero(receive_buffer, BUFSIZE);
  }
  else{
    receive_buffer = malloc(BUFSIZE);
  }
}

uint64_t send_request(unsigned int index, char* ip, int port_no, char* request, int len){
  
  int sockfd, n, ii;
  struct hostent *server;
  struct sockaddr_in serv_addr;
  uint64_t start_ticks, end_ticks;
  
  if(port_no <= 0){
    error("ERROR wrong port number");
  }

  // open socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // disable Nagel's algorith on the socket
  char* flag;
  int result = setsockopt(sockfd,            /* socket affected */
			  IPPROTO_TCP,     /* set option at TCP level */
			  TCP_NODELAY,     /* name of option */
			  (char *) &flag,  /* the cast is historical cruft */
			  sizeof(int));    /* length of option value */




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
  asm volatile("RDTSCP\n\t"                        
	              "mov %%edx, %0\n\t"           
	              "mov %%eax, %1\n\t"           
	       : "=r" (cyc_high), "=r" (cyc_low)   
	       :: RDTSC_DIRTY);                    
  end_ticks = ((uint64_t)cyc_high << 32) | cyc_low; 

  // read the rest of the message
  n = read(sockfd, &receive_buffer[1], BUFSIZE) + 1;

  // save the answer
  FILE* response_file = fopen("responses.log", "a");
  fprintf(response_file, "{ ");

  // analyze the packet and re-read if n is too small compared to the length
  int length;
  int node = 1;
  int jj;
  ii = 0;

  // ClientHello.random
  fprintf(response_file, "'client_random': '6d70d7a74344e7ccf7c3ace7c77ff39f9d9b2ea56d26ac9292224aa32f17c10b', ");

  // ServerHello.random
  fprintf(response_file, "'server_random': '");
  for(jj = 11; jj < 11 + 32; jj++){
    fprintf(response_file, "%02x", receive_buffer[jj]);
  }
  fprintf(response_file, "', ");

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
  fprintf(response_file, "'server_params': '");
  for(jj = ii; jj < ii + 4 + length; jj++){
    fprintf(response_file, "%02x", receive_buffer[jj]);
  }
  fprintf(response_file, "', ");
  
  // ServerKeyExchange.Signature
  jj = ii + 4 + length + 4;
  length = (receive_buffer[jj - 2] << 8) + receive_buffer[jj - 1];
  fprintf(response_file, "'server_signature': '");
  for(ii = jj; ii < jj + length; ii++){
    fprintf(response_file, "%02x", receive_buffer[ii]);
  }
  fprintf(response_file, "', ");

  // cycles
  fprintf(response_file, " 'time': %" PRIu64 " }\n", end_ticks - start_ticks);

  // close file
  fclose(response_file);

  // Close socket
  close(sockfd);

  return end_ticks - start_ticks;
}

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

  int iteration = atoi(argv[1]);

  uint64_t cycles;
  unsigned int ii;

  for(ii = 0; ii < iteration; ii++){
    init();
    printf("#%i\n", ii);
    cycles = send_request(ii, "12.12.12.12", 4433, clienthello, 307);
  }

  return 0;
}


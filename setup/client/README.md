# How to setup the client (the attacker)

## The attack

* first compile the timer/packet sender `gcc attack.c -o attack`

* configure your machine (see configuring your machine bellow)

* run the attack through `./run_client.sh 1000` to get 1000 signatures

* if you are running the server as well check the `/setup/server/` directory and follow instructions there

* check the results in `responses.log` (will get erased before every `run_client.sh` run)

* use tools in `/tools/` or `/setup/client/offline/` to analyze data and/or mount the attack

## Configuring your machine

### Frequency Scaling

I disable the **frequency scaling** so as to get good results counting the CPU cycles.

```
sudo apt-get install cpufrequtils
echo GOVERNOR="performance" >> /etc/default/cpufrequtils
sudo /etc/init.d/cpufrequtils restart
cpufreq-info
```

You can also disable ondemand daemon so that it doesn't cancel this after reboot: (although that might not be what you want to do)

```
sudo update-rc.d ondemand disable
```

Don't forget to disable this after your attack :o)

### Disable Nagle's algorithm in Linux

http://stackoverflow.com/questions/17842406/how-would-one-disable-nagles-algorithm-in-linux

From [wikipedia](https://en.wikipedia.org/wiki/Nagle's_algorithm): 

> Nagle's algorithm, named after John Nagle, is a means of improving the efficiency of TCP/IP networks by reducing the number of packets that need to be sent over the network.

> Nagle's document, Congestion Control in IP/TCP Internetworks (RFC 896) describes what he called the "small packet problem", where an application repeatedly emits data in small chunks, frequently only 1 byte in size. Since TCP packets have a 40 byte header (20 bytes for TCP, 20 bytes for IPv4), this results in a 41 byte packet for 1 byte of useful information, a huge overhead. This situation often occurs in Telnet sessions, where most keypresses generate a single byte of data that is transmitted immediately. Worse, over slow links, many such packets can be in transit at the same time, potentially leading to congestion collapse.

> Nagle's algorithm works by combining a number of small outgoing messages, and sending them all at once. Specifically, as long as there is a sent packet for which the sender has received no acknowledgment, the sender should keep buffering its output until it has a full packet's worth of output, so that output can be sent all at once.

### Dedicate one whole core to your attack only

http://stackoverflow.com/questions/13583146/whole-one-core-dedicated-to-single-process

1st answer

> Right now, the best way to accomplish what you want is to do the following:
> * Add the parameter isolcpus=[cpu_number] to the Linux kernel command line form the boot loader during boot. This will instruct the Linux scheduler to no run any regular tasks on that CPU unless specifically requested using cpu affinity.
> * Use IRQ affinity to set other CPUs to handle all interrupts so that your isolated CPU will not received any interrupt.
> * Use CPU affinity to fix your specific task to the isolated CPU.
> This will give you the best that Linux can provide with regard to CPU isolation without out of tree and in development patches .

2nd answer

> Add the kernel parameter `isolcpus=` to the boot loader during boot or GRUB configuration file. Then the Linux scheduler will not schedule any regular process on the reserved CPU core(s), unless specifically requested with taskset. For example, to reserve CPU cores 0 and 1, add `isolcpus=0,1` kernel parameter. Upon boot, then use taskset to safely assign the reserved CPU cores to your program.
> Source(s)
   http://xmodulo.com/2013/10/run-program-process-specific-cpu-cores-linux.html
   http://www.linuxtopia.org/online_books/linux_kernel/kernel_configuration/re46.html

### Power Management

> Modern processors power management features can cause unwanted delays in time-sensitive application processing by transitioning your processors into power-saving C-states.
To see if your application is being affected by power management transitions, boot your kernel with the command line options:
processor.max_cstate=1 idle=poll
This will disable power management in the system (but will also consume more power).
For more fine-grained controlof when power-management is turned off, use the PM QOS interface in your application to tell the kernel when to disable power saving state transitions.

from https://access.redhat.com/articles/65410


## Optimizations already made

### CPU affinity

Only use one CPU for your app. If you are using the `run_client.sh` utility this is already done for you. But you can modify it to use another CPU (0 by default)

```
taskset -c 0 my_program
```

### All but one byte

Send everything but one byte, then send one byte and start the counter, this will not work if Nagel's algorithm has not been disabled first.

### Stop the counter as soon as you receive a response

Modifying OpenSSL to stop after signing something with ECDSA, we can check that the client doesn't receive anything. This proves that OpenSSL sends all the TLS record messages at once after finishing signing with ECDSA.

## Open Questions for better results

### UDP?

Since there are less rules on UDP packets, we could use that to attack ECDSA on a DTLS server, which should share the same ECDSA implementation as the TLS code.



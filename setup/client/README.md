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

I disable the frequency scaling so as to get good results counting the CPU cycles.

```
sudo apt-get install cpufrequtils

sudo cpufreq-set -c 1 -g performance
sudo /etc/init.d/cpufrequtils restart
cpufreq-info
```

or add `GOVERNOR="performance"` to `/etc/default/cpufrequtils`

this also works: `sudo cpupower -c 1 frequency-info`
`sudo cpupower -c 1 frequency-set`
You can also disable ondemand daemon so that it doesn't cancel this after reboot: (although that might not be what you want to do)

```
sudo update-rc.d ondemand disable
```

Don't forget to disable this after your attack :o)

Note: if you have the `intel_pstate` driver you need to disable it and use another driver (`acpi-cpufreq` for ex):
> You can check your settings with `cpufreq-info`. It will show a block of information for every core your processor has. Just check if all of then are in perfomance mode, and at the maximum speed of your processor.

https://wiki.archlinux.org/index.php/CPU_frequency_scaling
https://wiki.archlinux.org/index.php/Kernel_modules

`sudo cpufreq-set -c 1 -d 2600000`

> The idea that frequency can be set to a single frequency is fiction for Intel Core processors. Even if the scaling driver selects a single P state the actual frequency the processor will run at is selected by the processor itself.

> intel_pstate can be disabled at boot-time with kernel arg intel_pstate=disable 

`GRUB_CMDLINE_LINUX_DEFAULT="isolcpus=1 intel_pstate=disable"`

### Dedicate one whole core to your attack only

http://stackoverflow.com/questions/13583146/whole-one-core-dedicated-to-single-process

* Add the parameter `isolcpus=1` to linux boot arguments. [How to do this](http://askubuntu.com/questions/19486/how-do-i-add-a-kernel-boot-parameter)

* Use IRQ affinity to stop interrupting on that CPU. [See redhat](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Performance_Tuning_Guide/s-cpu-irq.html)

* Use CPU affinity to use only the cpu 1 for your attack, this is already done (see *optimizations already made*)
2nd answer

### Power Management

> Modern processors power management features can cause unwanted delays in time-sensitive application processing by transitioning your processors into power-saving C-states.
To see if your application is being affected by power management transitions, boot your kernel with the command line options:
processor.max_cstate=1 idle=poll
This will disable power management in the system (but will also consume more power).
For more fine-grained controlof when power-management is turned off, use the PM QOS interface in your application to tell the kernel when to disable power saving state transitions.

from https://access.redhat.com/articles/65410

## Optimizations already made

### CPU priority

Use `nice` to set the CPU priority to `20`.

### CPU affinity

Only use one CPU for your app. If you are using the `run_client.sh` utility this is already done for you. But you can modify it to use another CPU (0 by default)

```
taskset -c 0 my_program
```

### All but one byte

Send everything but one byte, then send one byte and start the counter, this will not work if Nagel's algorithm has not been disabled first.

### Stop the counter as soon as you receive a response

Modifying OpenSSL to stop after signing something with ECDSA, we can check that the client doesn't receive anything. This proves that OpenSSL sends all the TLS record messages at once after finishing signing with ECDSA.

### Disable Nagle's algorithm in Linux

From [wikipedia](https://en.wikipedia.org/wiki/Nagle's_algorithm): 

> Nagle's algorithm, named after John Nagle, is a means of improving the efficiency of TCP/IP networks by reducing the number of packets that need to be sent over the network.

Disabling it allows us to send what we want to send as fast as possible.

## Open Questions for better results

### UDP?

Since there are less rules on UDP packets, we could use that to attack ECDSA on a DTLS server, which should share the same ECDSA implementation as the TLS code.



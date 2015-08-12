# How to setup the client (the attacker)

## The attack

* first compile the timer/packet sender `gcc attack.c -o attack`

* configure your machine (see configuring your machine bellow)

* run the attack through `./run_client.sh 1000` to get 1000 signatures

* if you are running the server as well check the `/setup/server/` directory and follow instructions there

* check the results in `responses.log`

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

### Only one CPU

Only use one CPU ! If you are using the `run_server.sh` utility this is already done for you. But you can modify it to use another CPU (0 by default)

```
taskset -c 0 my_program
```

### Disable Nagle's algorithm in Linux

http://stackoverflow.com/questions/17842406/how-would-one-disable-nagles-algorithm-in-linux

### Dedicate one whole core to your attack only

http://stackoverflow.com/questions/13583146/whole-one-core-dedicated-to-single-process





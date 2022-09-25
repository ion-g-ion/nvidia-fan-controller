# nvidia-fan-controller

PWM chasis fan controller for Nvidia Tesla GPUs. 

Controll the chasis fans based on the temperature of the GPU.

## How to make it run?

Make sure that the the path defined in `const char *fan_file = "/sys/class/hwmon/hwmon2/pwm2";` points to the right fan of your system.
`pwmconfig` is a useful tool to identify the fans.

The file is compiled using: 

```
 nvcc nvidia-fans.c -o nvidia-fans  -lnvidia-ml
```

In order to run it as a service, create the file `/etc/systemd/system/nvidia-fans.service` with the following content

```
[Unit]
Description=Nvidia fan controller

[Install]
WantedBy=multi-user.target

[Service]
Type=simple
ExecStart=/usr/local/nvidia-fans
WorkingDirectory=/usr/local
Restart=always
RestartSec=5
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=%n
```

Make sure to `ExecStart` points to the location of the executable (`nvidia-fans` can be made executable with `chmod +x nvidia-fans`).
The service is started with 

```
systemctl enable --now nvidia-fans.service
```

## Details

The following is the characteristic curve for the controller. The varibles cand be adapted in the code.
```
^ (pwm)       
|
|    (tmax,fmax) ,------
|               /
|              /
|_____________/ 
|             (tmin,fmin)
|------------------->  (temp)   
```
Moreover, a hysteresis behaviour is implemented: the fan speed decreses with a delay.
The delay is a multiple of `delta_t` and can be adapted in `max_relax`.


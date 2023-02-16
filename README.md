# rootkit
```
> make -C /lib/modules/$(uname -r)/build M=$PWD modules
> insmod rootkit.ko
> lsmod | grep rootkit
> rmmod rootkit
```

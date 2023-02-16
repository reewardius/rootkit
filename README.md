# rootkit
```
> # in the rootkit.c file, give the absolute path to the file
> make -C /lib/modules/$(uname -r)/build M=$PWD modules
> insmod rootkit.ko
> lsmod | grep rootkit
> rmmod rootkit
```

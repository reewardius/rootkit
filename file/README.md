```
sudo apt-get install build-essential linux-headers-$(uname -r)
sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules
sudo insmod rootkit.ko hidden_path=/path/to/hidden/file
lsmod | grep rootkit
```

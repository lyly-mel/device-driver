sudo insmod simpleDriver.ko
sudo mknod /dev/simpleDriver c 415 0
sudo chmod 666 /dev/simpleDriver
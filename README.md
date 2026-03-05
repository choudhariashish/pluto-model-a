# pluto-model-a
Model A Repository

## Jetson Orin Nano devkit Image
Download and prepare an SDCARD image for Orin Nano devkit
=> jetson-orin-nano-devkit-super-SD-image_JP6.2.1.zip


## Node Network

### Set releavant hostnames for nodes.
```
sudo hostnamectl set-hostname <brain/drive/arm...>
```

### Add user "pluto" and set password.
```
sudo add user pluto
su -
usermod -aG sudo pluto
```

### IP Configuration.
```
sudo ifconfig eth0 192.168.55.101...200
```

# pluto-model-a
Model A Repository

## Beaglebone AI64 Image
Download and prepare an eMMC flasher sdcard for bbai64 from
=> https://files.beagle.cc/file/beagleboard-public-2021/images/bbai64-emmc-flasher-debian-12.9-xfce-arm64-2025-03-05-12gb.img.xz

Insert the sdcard, hold the Boot button and then power on. The led lights will start blinking in a running pattern. Light will be off once the sdcard image is copied to the eMMC.

### Login
```
sudo -s
passwd # Update root password.
vim /etc/ssh/sshd_config # Edit to allow "PermitRootLogin yes".
systemctl restart sshd
```

### Network
```
ip a
wpa_passphrase <wifi name> <wifi password> > /etc/wpa_supplicant/wpa_supplicant.con # Comment the line that looks like "#psk=b3d43352".
wpa_supplicant -B -c /etc/wpa_supplicant/wpa_supplicant.conf -i wlan0
dhclient wlan0

# To make it permanent, Add the following in /etc/network/interfaces
auto wlan0
iface wlan0 inet dhcp
    wpa-ssid "wifi name"
    wpa-psk "wifi password"

ip a # Note the new IP address on wlan0.
```

### Install Bazel
sudo apt-get update
sudo apt-get install bazel-bootstrap
sudo apt-get install bazel-rules-cc
sudo apt-get install bazel-platforms
bazel --version
bazel 8.1.1


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

#### IP Configuration for BeagleBone AI64
```
ssh pluto@192.168.7.2
sudo nano /etc/network/interfaces
```
and add,
```
auto eth0
iface eth0 inet static
    address 192.168.55.102
    netmask 255.255.255.0
    gateway 192.168.55.1

sudo systemctl restart networking
```

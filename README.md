# pluto-model-a
Model A Repository

## Image
Download and prepare a eMMC flasher sdcard for bbai64 from,
from => https://files.beagle.cc/file/beagleboard-public-2021/images/bbai64-emmc-flasher-debian-12.9-xfce-arm64-2025-03-05-12gb.img.xz

Insert the sdcard, hold the Boot button and then power on. The led lights will start blinking in a running pattern. Light will be off once the sdcard image is copied to the eMMC.

## Login
```
sudo -s
passwd # Update root password.
vim /etc/ssh/sshd_config # Edit to allow "PermitRootLogin yes".
systemctl restart sshd
```

## Network
### Build Node
```
ip a
wpa_passphrase <wifi name> <wifi password> > /etc/wpa_supplicant/wpa_supplicant.con # Comment the "#psk=b3d43352" line.
wpa_supplicant -B -c /etc/wpa_supplicant/wpa_supplicant.conf -i wlan0
dhclient wlan0
ip a # Note the new IP address on wlan0.
```

### Robot Node
```
ifconfig eth0 20.0.0.X
```
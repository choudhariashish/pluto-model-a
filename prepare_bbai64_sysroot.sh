rsync -avz --numeric-ids --rsync-path="sudo rsync" --exclude={/dev/*,/proc/*,/sys/*,/tmp/*,/run/*,/mnt/*,/media/*,lost+found} root@192.168.55.102:/ /tmp/bbai64_sysroot

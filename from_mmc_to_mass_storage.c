1 board.c-> init_mmc()-> msm_add_sdcc()-> devices-msm7x27.c
sysfs: devices/platform/msm_sdcc.[1~4]/mmc_host/mmc*
[1] for BT storage.

2 driver/mmc/: /dev/block/mmcblkxxx(partition)

3 system notify the event of new device, then mount the device in vold:
-> void/main.cpp-> process_config()
Get and analysis the rules in "system/vold/vold.fstab":

-> dev_mount <label> <mount_point> <part> <sysfs_path1...>
 @label: Just a label
 @mount_point: where to mount on?
 @part: which partition of the block device
 @sysfs_path: the path to search for


4 when enable the mass storage, vold umount the device. Then share it with PC.
VolumeManager.cpp-> VolumeManager::shareVolume()
write the path "/dev/block/vold/%d:%d <- (MAJOR, MINOR)" to "/sys/devices/platform/usb_mass_storage/lun%d/file"(const char *LUN_FILES[]).

p.s
Each event of device is handled like this.
"/sys/devices/platform/usb_mass_storage/lun%d/file" is created by "f_mass_storage.c",
as well as "/sys/devices/platform/usb_mass_storage/lun%d/ro"(used for cdrom or ro fs).

5 fsg_store_file() open and store the info(inode+filp) of "/dev/block/vold/%d:%d" into "struct fsg_lun", then we can operate it in "f_mass_storage.c".

6 when data request is coming, USB pass the request to mass storage.
mass storage analysis the scsi CMD and read/write via vfs of block device.

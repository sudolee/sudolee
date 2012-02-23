/*
 * Show how mass stroage work from mmc/sdcard in android.
 *
 */

0 board.c-> mmc_init() -> 
sys/devices/...

1 driver/mmc/...

2 vold get and analysis the rule in "system/vold/vold.fstab", then mount mmc on given node.
-> vold/main.cpp-> process_config()

## Format: dev_mount <label> <mount_point> <part> <sysfs_path1...> 
## label        - Label for the volume
## mount_point  - Where the volume will be mounted
## part         - Partition # (1 based), or 'auto' for first usable partition.
## <sysfs_path> - List of sysfs paths to source devices
e.g,
## Example of a standard sdcard mount for the emulator / Dream
# Mounts the first usable partition of the specified device
	dev_mount sdcard /mnt/sdcard auto /devices/platform/goldfish_mmc.0 /devices/platform/msm_sdcc.2/mmc_host/mmc1

## Example of a dual card setup
# dev_mount left_sdcard  /sdcard1  auto /devices/platform/goldfish_mmc.0 /devices/platform/msm_sdcc.2/mmc_host/mmc1
# dev_mount right_sdcard /sdcard2  auto /devices/platform/goldfish_mmc.1 /devices/platform/msm_sdcc.3/mmc_host/mmc1

## Example of specifying a specific partition for mounts
# dev_mount sdcard /sdcard 2 /devices/platform/goldfish_mmc.0 /devices/platform/msm_sdcc.2/mmc_host/mmc1

3 when mass storage is enabled, vold umount the mmc/sdcard and then:
VolumeManager.cpp-> VolumeManager::shareVolume()
write the path "/dev/block/vold/%d:%d(major, minor)" to "/sys/devices/platform/usb_mass_storage/lun%d/file" in "char *LUN_FILES[]";

p.s,
	The node "/sys/devices/platform/usb_mass_storage/lun%d/file" is created within "f_mass_stroage.c",
	as well as "/sys/devices/platform/usb_mass_storage/lun%d/ro"(used for cdrom OR ro fs)

4 fsg_store_file() open and store the info(inode+filp) of block device into "struct fsg_lun",
  then we can handle.

5 when gadget recieves request from PC, the scsi CMD pass by USB to mass storage.
mass storage get and analysis the CMD and read/write data via VFS.

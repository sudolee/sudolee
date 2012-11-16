/*
 *  HID driver for RF card
 *
 *  Copyright (c) 2011 sudo.lee <desoxydate@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/cdev.h>
#include "hid-ids.h"

#define MAX_MSG_LEN 32
#define MAX_DATASIZE 30

static struct usb_device *__dev;

struct msg {
#define SINGLE_MSG 0x82
#define MULTI_MSG 0x02
	u8 data_status;
#define START_FLAG 0x55
	u8 start_flag;
	union {
		u8 len;
		u8 data[MAX_DATASIZE];
	}
};

//static ssize_t rf_msg_read(struct file *file, char __user *buffer, size_t size, loff_t *f_pos)
static ssize_t rf_msg_read(u16 index)
{
	int ret = 0;
	struct msg msg;
	u8 i, res;

	/* 0x02 0x55 0x01 0x00 0x56 */
	memset(&msg, 0, sizeof(msg));
	msg.data_status = 0x02;
	msg.start_flag = 0x55;
	msg.len = 0x05;
	msg.data[1] = 0x81;
	msg.data[2] = 0x00;
	msg.data[3] = 0x10;
	msg.data[4] = 0x00;
	msg.data[5] = 0x00;
	msg.data[6] = 0x94;

	ret = usb_control_msg(__dev, usb_sndctrlpipe(__dev, 0),
			      HID_REQ_SET_REPORT,
			      USB_DIR_OUT | USB_TYPE_CLASS |
			      USB_RECIP_INTERFACE, (HID_OUTPUT_REPORT + 1) << 8,
			      index, (void *)&msg, RF_MSG_LEN,
			      USB_CTRL_SET_TIMEOUT);
	if (ret < 0) {
		printk(KERN_ERR "usb_control_msg SEND error(%d).", ret);
		goto err_exit;
	}
	printk(KERN_ALERT
	       ">>>>>>> data_status=0x%02x, start_flag=0x%02x, len=0x%02x\n",
	       msg.data_status, msg.start_flag, msg.len);

	memset(&msg, 0, sizeof(msg));

	ret = usb_control_msg(__dev, usb_rcvctrlpipe(__dev, 0),
			      HID_REQ_GET_REPORT,
			      USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			      (HID_INPUT_REPORT + 1) << 8, index, (void *)&msg,
			      RF_MSG_LEN, USB_CTRL_GET_TIMEOUT);
	if (ret < 0) {
		printk(KERN_ERR "usb_control_msg RECIEVE error(%d).", ret);
		goto err_exit;
	}

	printk(KERN_ALERT
	       ">>>>>>> data_status=0x%02x, start_flag=0x%02x, len=0x%02x\n",
	       msg.data_status, msg.start_flag, msg.len);
	for (i = 0; i < MAX_MSG_LEN; i++) {
		printk(KERN_INFO "0x%02x ", msg.data[i]);
	}

err_exit:
	return ret;
}

/*
 * Sending HID_REQ_GET_REPORT changes the operation mode of the ps3 controller
 * to "operational".  Without this, the ps3 controller will not report any
 * events.
 */
static int rf_set_operational(struct hid_device *hdev)
{
	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	struct usb_device *dev = interface_to_usbdev(intf);
	__u16 ifnum = intf->cur_altsetting->desc.bInterfaceNumber;
	int ret;
	char *buf = kmalloc(18, GFP_KERNEL);

	if (!buf)
		return -ENOMEM;

	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
			      HID_REQ_GET_REPORT,
			      USB_DIR_IN | USB_TYPE_CLASS |
			      USB_RECIP_INTERFACE,
			      (3 << 8) | 0xf2, ifnum, buf, 17,
			      USB_CTRL_GET_TIMEOUT);
	if (ret < 0)
		dev_err(&hdev->dev, "can't set operational mode\n");

	kfree(buf);

	return ret;
}

static ssize_t rf_read(struct file *file, char __user * buffer, size_t size,
		       loff_t * f_pos)
{
	return 0;
}

static ssize_t rf_write(struct file *file, const char __user * buffer,
			size_t size, loff_t * f_pos)
{
	return 0;
}

static int rf_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int rf_open(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations rf_fops = {
	.owner = THIS_MODULE,
	.open = rf_open,
	.read = rf_read,
	.write = rf_write,
	.release = rf_release,
};

static inline int rf_card_init(struct hid_device *hdev)
{
	/* TO DO */
	return 0;
}

static int rf_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;
	unsigned long quirks = id->driver_data;
	struct rf_msg *sc;
	dev_t cdev_num;
	struct cdev *cdev = NULL;

	struct usb_interface *__intf = to_usb_interface(hdev->dev.parent);
	struct usb_host_interface *interface = __intf->cur_altsetting;
	__dev = interface_to_usbdev(__intf);

	printk(KERN_ALERT ">>>>>>> Enter: file->%s, func->%s().", __FILE__,
	       __func__);

	sc = kzalloc(sizeof(*sc), GFP_KERNEL);
	if (sc == NULL) {
		dev_err(&hdev->dev, "can't alloc apple descriptor\n");
		return -ENOMEM;
	}
//      sc->quirks = quirks;
//      hid_set_drvdata(hdev, sc);

	ret = hid_parse(hdev);
	if (ret) {
		dev_err(&hdev->dev, "parse failed\n");
		goto err_free;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		dev_err(&hdev->dev, "hw start failed\n");
		goto err_free;
	}

	rf_msg_read(interface->desc.bInterfaceNumber);

	rf_card_init(hdev);
	ret = alloc_chrdev_region(&cdev_num, 0, 1, "rf-card");
	if (!ret)
		goto err_stop;

	cdev = cdev_alloc();
	if (!cdev)
		goto err_cdev_num;

	cdev_init(cdev, &rf_fops);
	ret = cdev_add(cdev, cdev_num, 1);
	if (ret < 0)
		goto err_cdev_del;

#if 0
	ret = rf_set_operational(hdev);
	if (ret < 0)
		goto err_stop;
#endif

	printk(KERN_ALERT ">>>>>>> func->%s() successful.", __func__);
	return 0;

err_cdev_del:
	cdev_del(cdev);
err_cdev_num:
	unregister_chrdev_region(cdev_num, 1);
err_stop:
	hid_hw_stop(hdev);
err_free:
	kfree(sc);
	return ret;
}

static void rf_remove(struct hid_device *hdev)
{
	hid_hw_stop(hdev);
	kfree(hid_get_drvdata(hdev));
}

static const struct hid_device_id rf_device[] = {
	{HID_USB_DEVICE(USB_VENDOR_ID_RF, USB_DEVICE_ID_RF)},
	{}
};

MODULE_DEVICE_TABLE(hid, rf_device);

static struct hid_driver rf_driver = {
	.name = "rf-card",
	.id_table = rf_device,
	.probe = rf_probe,
	.remove = rf_remove,
//      .report_fixup = rf_report_fixup,
};

static int __init rf_init(void)
{
	return hid_register_driver(&rf_driver);
}

static void __exit rf_exit(void)
{
	hid_unregister_driver(&rf_driver);
}

module_init(rf_init);
module_exit(rf_exit);
MODULE_LICENSE("GPL");

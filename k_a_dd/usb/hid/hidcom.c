/*
 *  HID driver for RF card
 *
 *  Copyright (c) 2011 sudolee <desoxydate@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>
#include <linux/input.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define DEBUG

#define VENDOR_ID		0x2933
#define PRODUCT_ID		0x0101
#define MSG_MAX_LEN		32
#define MSG_HEAD_LEN	3	/* (0x02/0x82), (0x55), (length of msg) */
#define MAX_DATASIZE	30	/* include data length and check byte */

struct msg {
#define SINGLE_MSG	0x02
#define MULTI_MSG	0x82
	unsigned char data_status;
#define START_FLAG	0x55
	unsigned char start_flag;
	union {
		unsigned char len;
		unsigned char value[MAX_DATASIZE];
	};
};

/* Check format of msg and check-byte
 * return 1->error, 0->successful.
 */
inline int check_msg(struct msg *msg)
{
	unsigned char count, check;

	if ((msg->data_status == SINGLE_MSG || msg->data_status == MULTI_MSG)
	    && msg->start_flag == START_FLAG && msg->len <= (MSG_MAX_LEN - 4)) {
		for (count = 0, check = 0; count <= msg->len; ++count)
			check ^= msg->value[count];
		if (check == msg->value[count])
			return 0;
	}
	return 1;
}

/* Search device in first 20 of /dev/hiddev%d
 *  matched with specified vendor id and product id.
 */
int rf_open(void)
{
	int fd = -1, i = 0;
	char path[256] = { 0 };
	struct hiddev_devinfo dev_info;

	for (i = 0; i < 20; i++) {
		sprintf(path, "/dev/hiddev%d", i);
		if ((fd = open(path, O_RDONLY)) >= 0) {
			ioctl(fd, HIDIOCGDEVINFO, &dev_info);
			if ((dev_info.vendor == VENDOR_ID)
			    && (dev_info.product == PRODUCT_ID))
				return fd;
			close(fd);	/* not expected, simple close */
		}
	}
	return -ENODEV;
}

int rf_close(int fd)
{
	return close(fd);
}

/* get one 32-bytes msg
 * return received bytes
 */
static ssize_t msg_in(int fd, void *buf)
{
	struct hiddev_report_info rep_info_i;
	struct hiddev_usage_ref_multi ref_multi_i;
	int msg_len = 0, i, res;

	rep_info_i.report_type = HID_REPORT_TYPE_FEATURE;
	rep_info_i.report_id = HID_REPORT_ID_FIRST;
	rep_info_i.num_fields = 1;

	memset(&ref_multi_i, 0, sizeof(struct hiddev_usage_ref_multi));

	ref_multi_i.uref.report_type = HID_REPORT_TYPE_FEATURE;
	ref_multi_i.uref.report_id = HID_REPORT_ID_FIRST;
	ref_multi_i.uref.field_index = 0;
	ref_multi_i.uref.usage_index = 0;
	ref_multi_i.num_values = MSG_MAX_LEN;

	res = ioctl(fd, HIDIOCGREPORT, &rep_info_i);
	if (res < 0)
		return res;
	res = ioctl(fd, HIDIOCGUSAGES, &ref_multi_i);
	if (res < 0)
		return res;

	msg_len = ref_multi_i.values[2] + 4;
	for (i = 0; i < msg_len; ++i)
		*((unsigned char *)buf + i) = ref_multi_i.values[i];

#ifdef DEBUG
	printf("msg in(debug)...\n");
	for (i = 0; i < msg_len; ++i)
		printf("0x%02x ", ref_multi_i.values[i]);
	printf("\n");
#endif

	if (check_msg((struct msg *)buf))
		return -1;
	return msg_len;
}

/* buf may be multi 32-bytes msgs.
 */
ssize_t rcv_msg(int fd, void *buf, size_t count)
{
	struct msg in_msg;
	int msg_len = 0, res = 0;

	do {
		res = msg_in(fd, &in_msg);
		if (res < 0)
			return -1;

		if ((count - msg_len - res + 4) < 0)
			return -1;	/* buffer will overflow */
		memcpy((unsigned char *)buf + msg_len, in_msg.value + 1, res - 4);
		msg_len += (res - 4);
	} while (in_msg.data_status == MULTI_MSG);

	return msg_len;
}

/* output one 32-bytes msg
 * return send bytes
 */
static ssize_t msg_out(int fd, void *buf)
{
	struct hiddev_report_info rep_info_u;
	struct hiddev_usage_ref_multi ref_multi_u;
	struct msg *msg = (struct msg *)buf;
	int msg_len, i, res;

	rep_info_u.report_type = HID_REPORT_TYPE_FEATURE;
	rep_info_u.report_id = HID_REPORT_ID_FIRST;
	rep_info_u.num_fields = 1;

	memset(&ref_multi_u, 0, sizeof(struct hiddev_usage_ref_multi));

	ref_multi_u.uref.report_type = HID_REPORT_TYPE_FEATURE;
	ref_multi_u.uref.report_id = HID_REPORT_ID_FIRST;
	ref_multi_u.uref.field_index = 0;
	ref_multi_u.uref.usage_index = 0;

	msg_len = msg->len + 4;
	ref_multi_u.num_values = msg_len;
	for (i = 0; i < msg_len; ++i)
		ref_multi_u.values[i] = *((unsigned char *)buf + i);

#ifdef DEBUG
	printf("msg out(debug)...\n");
	for (i = 0; i < msg_len; ++i)
		printf("0x%02x ", ref_multi_u.values[i]);
	printf("\n");
#endif

	res = ioctl(fd, HIDIOCSUSAGES, &ref_multi_u);
	if (res < 0)
		return res;
	res = ioctl(fd, HIDIOCSREPORT, &rep_info_u);
	if (res < 0)
		return res;
	return msg_len;
}

inline unsigned char get_check_byte(struct msg *msg)
{
	unsigned char check, i;

	/* include data length byte */
	for (check = 0, i = 0; i < msg->len + 1; ++i)
		check ^= msg->value[i];
	return check;
}

/* buf may be multi 32-bytes msgs.
 */
ssize_t send_msg(int fd, void *buf, size_t count)
{
	struct msg out_msg = { MULTI_MSG, START_FLAG, };
	int res, msg_len;

	for (msg_len = 0; count > 0;) {
		if (count <= MAX_DATASIZE) {
			out_msg.data_status = SINGLE_MSG;
			out_msg.len = count;
			memcpy(out_msg.value + 1, (unsigned char *)buf + msg_len, count);
			out_msg.value[count + 1] = get_check_byte(&out_msg);
		} else {
			out_msg.len = MAX_DATASIZE;
			memcpy(out_msg.value + 1, (unsigned char *)buf + msg_len, MAX_DATASIZE);
			out_msg.value[count + 1] = get_check_byte(&out_msg);
		}
		res = msg_out(fd, &out_msg);
		if (res < 0)
			return -1;

		msg_len = res - 4;
		count -= msg_len;
	}

	return msg_len;
}

/* User should make sure rcv_buffer enough to hold coming msgs.
 * when both send_buffer and rcv_buffer is "null", just do nothing.
 * if only send, return send bytes.
 * if only recieve, return recieve bytes.
 * if both send and recieve, only return recieve bytes.
 */
ssize_t send_data(int fd, void *send_buffer, size_t send_len, void *rcv_buffer,
		  size_t rcv_len)
{
	int res = 0;

	if (send_buffer && send_len) {
		res = send_msg(fd, send_buffer, send_len);
		if (res < 0)
			return res;
	}

	if (rcv_buffer && rcv_len) {
		res = rcv_msg(fd, rcv_buffer, rcv_len);
		if (res < 0)
			return res;
	}

	return res;
}

inline void debug(void *buf, int count)
{
	int debug;
	printf("Debug...\n");
	for (debug = 0; debug < count; ++debug)
		printf("0x%02x ", (int)((unsigned char *)buf)[debug]);
	printf("\n");
}

int main(int argc, char **argv)
{
	int fd, res;
	unsigned char buf[MSG_MAX_LEN] = { 0 };

	fd = rf_open();
	if (fd < 0) {
		printf("Can't find device.");
		exit(1);
	}

	/* 0x02 0x55 0x01 0x00 0x01 */
	buf[0] = 0x00;
	res = send_data(fd, buf, 1, buf, sizeof(buf));
	if (res < 0) {
		printf("send data ERROR.\n");
		exit(1);
	}
#ifdef DEBUG
	debug(buf, res);
#endif

	memset(buf, 0, sizeof(buf));
	sleep(1);

	/* 02 55 05 81 00 10 00 00 94 */
	buf[0] = 0x81;
	buf[1] = 0x00;
	buf[2] = 0x10;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = send_data(fd, buf, 5, buf, sizeof(buf));
	if (res < 0) {
		printf("send data ERROR.\n");
		exit(1);
	}
#ifdef DEBUG
	debug(buf, res);
#endif

	rf_close(fd);
	exit(0);
}

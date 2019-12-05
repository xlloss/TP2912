/*
 * Copyright (C) 2005-2006 Micronas USA Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "tp2912"
#define DEVICE_NUM 1

#define TP2912_REG_HUE 0x0f
#define TP2912_REG_SATURATION 0x10
#define TP2912_REG_CONTRAST 0x11
#define TP2912_REG_BRIGHTNESS 0x12
#define TP2912_REG_COLOR_KILLER 0x14
#define TP2912_REG_GAIN 0x3c
#define HDELAY_H_REG 0x10
#define HDELAY_L_REG 0x11

enum {
	TP2912_IOCTL_HUE = 0x100,
	TP2912_IOCTL_SATURATION,
	TP2912_IOCTL_CONTRAST,
	TP2912_IOCTL_BRIGHTNESS,
	TP2912_IOCTL_COLOR_KILLER,
	TP2912_IOCTL_GAIN,
};

//struct TP2912 {
//	struct class *TP2912_class;
//	struct device *TP2912_device;
//	unsigned int TP2912_major
//};

//struct TP2912 TP2912;

int loop_test;

/* TVI 720P V1 */
unsigned char tp2912_bt656_to_tvi[] = {
    0x02, 0x8b,
    0x03, 0x42,
    0x07, 0xc1,
    0x08, 0xe0,
    0x09, 0x28,
    0x0a, 0x28,
    0x0b, 0x45,
    0x0c, 0x04,
    0x0d, 0x04,
    0x0e, 0x28,
    0x0f, 0xe4,
    0x10, 0x6c,
    0x11, 0xf5,
    0x12, 0x05,
    0x16, 0xeb,
    0x19, 0xf0,
    0x1a, 0x10,
    0x1c, 0xa6,
    0x1d, 0xe6,
    0x1e, 0xf8,
    0x20, 0x48,
    0x21, 0xbb,
    0x22, 0x2e,
    0x23, 0x8b,
    0x3c, 0x50,
    0x3e, 0x1d,
    0x41, 0x01,
    0x42, 0x52,
    0x43, 0x07,
    0x44, 0x49,
    0x45, 0x8b,
};

static int write_reg(struct i2c_client *client, u8 reg, u8 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

static int write_regs(struct i2c_client *client, const u8 *regs, int count)
{
	int ret;
	int i;

	for (i = 0; i <  count; i += 2) {
		ret = i2c_smbus_write_byte_data(client, regs[i], regs[i + 1]);
		if (ret < 0)
			return ret;
	}
	return 0;
}

static int read_reg(struct i2c_client *client, u8 reg, u8 channel)
{
	return i2c_smbus_read_byte_data(client, (reg) | (channel << 6));
}

static int tp2912_initial_hw(struct i2c_client *client, unsigned char *regs)
{
	write_regs(client, tp2912_bt656_to_tvi, sizeof(tp2912_bt656_to_tvi));

    if (loop_test) {
        write_reg(client, HDELAY_H_REG, 0x00);
        write_reg(client, HDELAY_L_REG, 0x90);
    }

	return 0;
}

static long tp2912_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{

	switch (cmd) {
		case TP2912_IOCTL_HUE:
		break;

		case TP2912_IOCTL_SATURATION:
		break;

		case TP2912_IOCTL_CONTRAST:
		break;

		case TP2912_IOCTL_BRIGHTNESS:
		break;

		case TP2912_IOCTL_COLOR_KILLER:
		break;

		case TP2912_IOCTL_GAIN:
		break;

		default:
			return -EINVAL;
	}

	return 0;
}

static int tp2912_parse_dt(struct i2c_client *client)
{
    struct device_node *np = client->dev.of_node;

    if (of_property_read_u32(np, "loop_test", &loop_test)) {
        dev_info(&client->dev, "DTS loop_test read fail\r\n");
        loop_test = 0;
    }

    return 0;
}

static struct file_operations tp2912_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tp2912_ioctl,
};


static struct miscdevice tp2912_dev = {
	.minor			= MISC_DYNAMIC_MINOR,
	.name			= DEVICE_NAME,
	.fops			= &tp2912_fops,
};

static int tp2912_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{
	/* struct i2c_adapter *adapter = client->adapter; */
	struct class *dev_class;
	int ret;
	dev_t devt;

	dev_info(&client->dev, "chip found @ 0x%02x (%s)\n",
			client->addr << 1, client->adapter->name);

	ret = misc_register(&tp2912_dev);
	if (ret != 0)
		goto out;


    tp2912_parse_dt(client);
	tp2912_initial_hw(client, tp2912_bt656_to_tvi);

	return 0;

out:
	return -EINVAL;
}

static int tp2912_remove(struct i2c_client *client)
{
	misc_deregister(&tp2912_dev);
	return 0;
}

static const struct i2c_device_id tp2912_id[] = {
	{ "TP2912", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tp2912_id);

static struct i2c_driver tp2912_driver = {
	.driver = {
		.name	= "tp2912",
	},
	.probe		= tp2912_probe,
	.remove		= tp2912_remove,
	.id_table	= tp2912_id,
};

module_i2c_driver(tp2912_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("tp2912 V4L2 i2c driver");
MODULE_AUTHOR("Slash.huang@regulus.com.tw");

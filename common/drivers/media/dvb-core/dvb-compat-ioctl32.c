/*
 * dvb-compat-ioctl32.c: Conversion between 32bit and 64bit native ioctls.
 *
 * Copyright (C) 2016  Ricardo Pontes  (rjgpp.1994@hotmail.com)
 *
 * These routines maintain argument size conversion between 32bit and 64bit
 * ioctls.
 */

#include <linux/device.h>
#include "dvb-compat-ioctl32.h"
#include "dmxdev.h"
#include "dvb_frontend.h"

static long dvb_fe_property_compat(struct file *filp,
			unsigned int cmd, unsigned long args, unsigned int native_cmd)
{
	struct dtv_properties_compat __user *tvps_compat;
	struct dtv_property_compat __user *tvp_compat;
	struct dtv_properties __user *tvps;
	struct dtv_property __user *tvp;
	compat_uptr_t props_compat;
	__u32 num;
	int ret;
	int i;

	tvps_compat = (struct dtv_properties_compat __user *)args;

	ret = get_user(num, &tvps_compat->num);
	ret |= get_user(props_compat, &tvps_compat->props);

	if (ret) {
		return -EFAULT;
	}

	tvp_compat = compat_ptr(props_compat);

	tvps = compat_alloc_user_space(sizeof(struct dtv_properties) + sizeof(struct dtv_property) * num);
	tvp = (struct dtv_property*)((char*)tvps + sizeof(struct dtv_properties));

	ret = put_user(num, &tvps->num);
	ret |= put_user(tvp, &tvps->props);

	if (ret) {
		return -EFAULT;
	}

	for (i = 0; i < num; i++) {
		compat_uptr_t reserved2;
		ret |= copy_in_user(&tvp[i], &tvp_compat[i], (8 * sizeof(__u32)) + (32 * sizeof(__u8)));
		ret |= get_user(reserved2, &tvp_compat[i].u.buffer.reserved2);
		ret |= put_user(compat_ptr(reserved2), &tvp[i].u.buffer.reserved2);
	}

	if (ret) {
		return -EFAULT;
	}

	ret = dvb_generic_ioctl(filp, native_cmd, (unsigned long) tvps);

	for(i = 0; i < num; i++) {
		if(copy_in_user(&tvp_compat[i].result, &tvp[i].result, sizeof(int)))
			return -EFAULT;
	}
	
	if (native_cmd == FE_GET_PROPERTY) {
		for (i = 0; i < num; i++) {
			void *reserved2;
			ret |= copy_in_user(&tvp_compat[i], &tvp[i], (8 * sizeof(__u32)) + (32 * sizeof(__u8)));
			ret |= get_user(reserved2, &tvp[i].u.buffer.reserved2);
			ret |= put_user(ptr_to_compat(reserved2), &tvp_compat[i].u.buffer.reserved2);
		}

		if (ret) {
			return -EFAULT;
		}
	}

	return ret;
}

long dvb_frontend_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args)
{
	struct dvb_device *dvbdev = filp->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	unsigned long ret;
	
	dev_dbg(fe->dvb->device, "%s: (%d)\n", __func__, _IOC_NR(cmd));

	args = (unsigned long)compat_ptr(args);

	if (cmd == FE_SET_PROPERTY_COMPAT) {
		ret = dvb_fe_property_compat(filp, cmd, args, FE_SET_PROPERTY);
	}
	else if (cmd == FE_GET_PROPERTY_COMPAT) {
		ret = dvb_fe_property_compat(filp, cmd, args, FE_GET_PROPERTY);
	}
	else {
		ret = dvb_generic_ioctl(filp, cmd, args);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(dvb_frontend_compat_ioctl);

long dvb_demux_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args)
{
	struct dvb_device *dvbdev = filp->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	unsigned long ret;

	args = (unsigned long)compat_ptr(args);
	ret = dvb_demux_ioctl(filp, cmd, args);
	return ret;
}
EXPORT_SYMBOL_GPL(dvb_demux_compat_ioctl);


long dvb_dvr_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args)
{
	struct dvb_device *dvbdev = filp->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	unsigned long ret;

	args = (unsigned long)compat_ptr(args);
	ret = dvb_dvr_ioctl(filp, cmd, args);
	return ret;
}
EXPORT_SYMBOL_GPL(dvb_dvr_compat_ioctl);


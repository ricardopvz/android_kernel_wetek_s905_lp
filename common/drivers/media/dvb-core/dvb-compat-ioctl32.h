/*
 * dvb-compat-ioctl32.h: Conversion between 32bit and 64bit native ioctls.
 *
 * Copyright (C) 2016  Ricardo Pontes  (rjgpp.1994@hotmail.com)
 *
 * These routines maintain argument size conversion between 32bit and 64bit
 * ioctls.
 */

#ifndef _DVBCOMPATIOCTL32_H_
#define _DVBCOMPATIOCTL32_H_

#include <linux/compat.h>
#include <uapi/linux/dvb/frontend.h>
#include "dvbdev.h"

struct dtv_property_compat {
	__u32 cmd;
	__u32 reserved[3];
	union {
		__u32 data;
		struct dtv_fe_stats st;
		struct {
			__u8 data[32];
			__u32 len;
			__u32 reserved1[3];
			compat_uptr_t reserved2;
		} buffer;
	} u;
	int result;
} __attribute__ ((packed));

struct dtv_properties_compat {
	__u32 num;
	compat_uptr_t props;
};

#define FE_SET_PROPERTY_COMPAT		   _IOW('o', 82, struct dtv_properties_compat)
#define FE_GET_PROPERTY_COMPAT		   _IOR('o', 83, struct dtv_properties_compat)

long dvb_frontend_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args);
			
long dvb_demux_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args);
			
long dvb_dvr_compat_ioctl(struct file *filp,
			unsigned int cmd, unsigned long args);
			
#endif


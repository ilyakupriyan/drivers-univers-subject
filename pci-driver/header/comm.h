#ifndef COMM_H
#define COMM_H

#include <linux/ioctl.h>

#define MAGIC_NUM 0xC4

#define RD_MAC _IOR(MAGIC_NUM, 0, long long)

#endif
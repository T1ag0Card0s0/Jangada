#pragma once
#include "device.h"
#include <stddef.h>

#define IOCTL_CLASS_UART 0x00010000
#define IOCTL_CLASS_SPI 0x00020000
#define IOCTL_CLASS_GPIO 0x00030000
#define IOCTL_CLASS_TIMER 0x00040000

typedef struct driver_ops
{
    const char *name;
    const char *class;
    int (*init)(device_t *dev);
    int (*deinit)(device_t *dev);
    int (*read)(device_t *dev, void *buf, size_t len);
    int (*write)(device_t *dev, const void *buf, size_t len);
    int (*ioctl)(device_t *dev, uint32_t cmd, void *arg);
    void (*isr)(device_t *dev);
} driver_ops_t;

#define DRV_OK 0
#define DRV_ERR (-1)
#define DRV_ENOSYS (-2)
#define DRV_EBUSY (-3)
#define DRV_EINVAL (-4)
#define DRV_ENOMEM (-5)

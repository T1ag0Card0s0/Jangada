#ifndef REGISTRY_H
#define REGISTRY_H

#include "device.h"

int device_register(device_t *dev);
device_t *device_find(const char *name);
int device_open(device_t *dev);
int device_close(device_t *dev);
int device_read(device_t *dev, void *buf, size_t len);
int device_write(device_t *dev, const void *buf, size_t len);
int device_ioctl(device_t *dev, uint32_t cmd, void *arg);

#endif // REGISTRY_H

// registry.c
#include "registry.h"
#include "jgd_mutex.h"
#include <stdio.h>
#include <string.h>

static device_t registry[MAX_DEVICES];
static uint8_t registry_count = 0;
static jgd_mutex_t registry_lock;

int device_register(device_t *dev)
{
    if (!dev || !dev->ops || dev->name[0] == '\0')
    {
        return DRV_EINVAL;
    }
    jgd_mutex_lock(&registry_lock);
    if (registry_count >= MAX_DEVICES)
    {
        jgd_mutex_unlock(&registry_lock);
        return DRV_ENOMEM;
    }
    registry[registry_count++] = dev;
    jgd_mutex_unlock(&registry_lock);
    return DRV_OK;
}

device_t *device_find(const char *name)
{
    for (uint8_t i = 0; i < registry_count; i++)
    {
        if (strncmp(registry[i]->name, name, DEVICE_NAME_MAX) == 0)
        {
            return registry[i];
        }
    }
    return NULL;
}

int device_open(device_t *dev)
{
    if (!dev)
    {
        return DRV_EINVAL;
    }
    dev->open_count++;
    if (dev->open_count == 1 && dev->ops->init)
    {
        return dev->ops->init(dev);
    }
    return DRV_OK;
}

int device_close(device_t *dev)
{
    if (!dev || dev->open_count == 0)
    {
        return DRV_EINVAL;
    }
    if (--dev->open_count == 0 && dev->ops->deinit)
    {
        return dev->ops->deinit(dev);
    }
    return DRV_OK;
}

int device_read(device_t *dev, void *buf, size_t len)
{
    if (!dev || !dev->ops->read)
    {
        return DRV_ENOSYS;
    }
    return dev->ops->read(dev, buf, len);
}

int device_write(device_t *dev, const void *buf, size_t len)
{
    if (!dev || !dev->ops->write)
    {
        return DRV_ENOSYS;
    }
    return dev->ops->write(dev, buf, len);
}

int device_ioctl(device_t *dev, uint32_t cmd, void *arg)
{
    if (!dev || !dev->ops->ioctl)
    {
        return DRV_ENOSYS;
    }
    return dev->ops->ioctl(dev, cmd, arg);
}

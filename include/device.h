#pragma once
#include <stddef.h>
#include <stdint.h>

#define DEVICE_NAME_MAX 16
#define MAX_DEVICES 32

typedef struct driver_ops driver_ops_t;

typedef struct device
{
    char name[DEVICE_NAME_MAX];
    const driver_ops_t *ops;
    void *priv;
    uint8_t open_count;
    uint8_t flags;
} device_t;

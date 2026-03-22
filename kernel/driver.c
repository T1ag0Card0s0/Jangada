#include "driver.h"
#include <string.h>

#define MAX_DRIVERS 16

static driver_t *drivers[MAX_DRIVERS];
static int driver_count_internal = 0;

void register_driver(driver_t *drv)
{
    if (driver_count_internal < MAX_DRIVERS)
    {
        drivers[driver_count_internal++] = drv;
    }
}

void driver_init_all(void)
{
    for (int i = 0; i < device_count; i++)
    {
        device_t *dev = &device_table[i];

        for (int j = 0; j < driver_count_internal; j++)
        {
            if (strcmp(dev->compatible, drivers[j]->compatible) == 0)
            {
                drivers[j]->init(dev);
            }
        }
    }
}

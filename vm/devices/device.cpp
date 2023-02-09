#include "device.hpp"

#include <assert.h>

Device* FindDevice(Device* devices, size_t addr)
{
    assert(devices != NULL);

    for (size_t i = 0; devices[i].name; i++)
    {
        if (devices[i].concreteDevice == NULL)
            continue;

        if (devices[i].lowAddr <= addr && devices[i].highAddr >= addr)
            return &devices[i];
    }

    return NULL;
}

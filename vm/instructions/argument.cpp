#include "argument.hpp"

size_t DataSzToBytesSz(DataSize sz)
{

    switch (sz)
    {
    case DataWord:
        return 8;

    case DataHalfWord:
        return 4;

    case DataDByte:
        return 2;

    case DataByte:
        return 1;
    }
    return 0;
}

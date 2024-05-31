#ifndef RESOURCE_H
#define RESOURCE_H
#include <random>
#include <uuid_v4/uuid_v4.h>
#include "RandomHelper.hpp"

using ResourceHandle = UUIDv4::UUID;

class Resource{
public:
    ResourceHandle resourceHandle = RandomHelper::UUIDGenerator.getUUID();
};
#endif
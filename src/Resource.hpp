#ifndef RESOURCE_H
#define RESOURCE_H
#include <random>
#include <uuid_v4/uuid_v4.h>

using ResourceHandle = UUIDv4::UUID;

class Resource{
private:
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
public:
    ResourceHandle resourceHandle = uuidGenerator.getUUID();
};
#endif
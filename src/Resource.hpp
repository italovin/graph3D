#ifndef RESOURCE_H
#define RESOURCE_H
#include <random>
#include <uuid_v4/uuid_v4.h>
#include "RandomHelper.hpp"
#include "Base.hpp"

using ResourceHandle = UUIDv4::UUID;

template <typename T>
class Resource{
public:
    ResourceHandle resourceHandle = RandomHelper::UUIDGenerator.getUUID();
    Ref<T> object;
    Resource() = default;
    Resource(Ref<T> object):object(std::move(object)){}
    T& operator*() const { return *object;}
    T* operator->() const { return object.get();}
};
#endif

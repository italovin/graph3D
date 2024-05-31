#include <uuid_v4/uuid_v4.h>
class RandomHelper{
public:
    static inline UUIDv4::UUIDGenerator<std::mt19937_64> UUIDGenerator;
};
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Universal constants to be used in the engine
namespace Constants
{
    // Maximum number of objects to group in a single render group
    // This also defines the size of the uniform buffers in standard shaders
    const unsigned long long maxObjectsToGroup = 8192;
}

#endif
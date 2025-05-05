#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
#include <gli/gli.hpp>
#include <vector>

class Texture{
private:
    gli::texture handle;
public:
    Texture() = default;
    // Generic load function for common image formats or compressed textures
    bool Load(const std::string &filePath, bool isSRGB = true, bool mirrorVertically = false);
    bool LoadFromMemory(const std::vector<unsigned char> &pixels, gli::format format, int width, int height, bool mirrorVertically = false);
    gli::format GetFormat() const;
    // Get GLenum equivalent to texture internal format. You also can force srgb return value
    static GLenum GliInternalFormatToGLenum(gli::format format, bool forceSRGB = false);
    static GLenum GliClientFormatToGLenum(gli::format format);
    static GLenum GliTypeToGLenum(gli::format format);
    bool IsCompressed() const;
    gli::texture::extent_type GetDimensions() const;
    int GetSize() const;
    const void* GetData() const;
public:
};
#endif

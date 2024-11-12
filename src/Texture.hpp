#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
#include <vector>
#include <variant>

enum class TextureInternalFormat{
    RGB, RGBA
};

enum class TextureSizedInternalFormat{
    RGB8, RGBA8, RGB32F, RGBA32F
};

enum class TexturePixelDataType{
    UnsignedByte, Float
};

struct TexturePixels{
    TexturePixelDataType dataType;
    TextureInternalFormat format;
    TextureSizedInternalFormat sizedFormat;
    std::variant<std::vector<GLubyte>, std::vector<GLfloat>> data;
};

class Texture{
private:
    size_t width = 0;
    size_t height = 0;
    TexturePixels pixels;
public:
    Texture() = delete;
    Texture(size_t width, size_t height);
    // Pixels data vector size must be multiple of format size (RGB = 3, RGBA = 4)
    bool SetPixelsData(const std::vector<GLubyte> &pixelsData, TextureInternalFormat internalFormat);
    bool SetPixelsData(const std::vector<GLubyte> &pixelsData, int channels);
    // Pixels data vector size must be multiple of format size (RGB = 3, RGBA = 4)
    bool SetPixelsData(const std::vector<GLfloat> &pixelsData, TextureInternalFormat internalFormat);
    bool SetPixelsData(const std::vector<GLfloat> &pixelsData, int channels);
    const TexturePixels &GetPixels();
    size_t GetWidth() const;
    size_t GetHeight() const;
    int GetChannelsCount() const;
    GLenum GetPixelDataTypeGLenum() const;
    GLenum GetFormatGLenum() const;
public:
};
#endif

#include "Texture.hpp"
#include "VertexArray.hpp"

Texture::Texture(size_t width, size_t height){
    this->width = width;
    this->height = height;
}

bool Texture::SetPixelsData(const std::vector<GLubyte> &pixelsData, TextureInternalFormat internalFormat){
    int multiple = internalFormat == TextureInternalFormat::RGBA ? 4 : 3;
    if(pixelsData.size() % multiple != 0)
        return false;

    this->pixels.data = pixelsData;
    TextureSizedInternalFormat sizedFormat = TextureSizedInternalFormat::RGB8;
    switch(internalFormat){
        case TextureInternalFormat::RGB: sizedFormat = TextureSizedInternalFormat::RGB8; break;
        case TextureInternalFormat::RGBA: sizedFormat = TextureSizedInternalFormat::RGBA8; break;
        default: sizedFormat = TextureSizedInternalFormat::RGB8; break;
    }
    this->pixels.format = internalFormat;
    this->pixels.sizedFormat = sizedFormat;
    this->pixels.dataType = TexturePixelDataType::UnsignedByte;
    return true;
}

bool Texture::SetPixelsData(const std::vector<GLubyte> &pixelsData, int channels){
    TextureInternalFormat format;
    switch(channels){
        case 3: format = TextureInternalFormat::RGB; break;
        case 4: format = TextureInternalFormat::RGBA; break;
        default: return false;
    }
    return SetPixelsData(pixelsData, format);
}

bool Texture::SetPixelsData(const std::vector<GLfloat> &pixelsData, TextureInternalFormat internalFormat){
    int multiple = internalFormat == TextureInternalFormat::RGBA ? 4 : 3;
    if(pixelsData.size() % multiple != 0)
        return false;

    this->pixels.data = pixelsData;
    TextureSizedInternalFormat sizedFormat = TextureSizedInternalFormat::RGB8;
    switch(internalFormat){
        case TextureInternalFormat::RGB: sizedFormat = TextureSizedInternalFormat::RGB32F; break;
        case TextureInternalFormat::RGBA: sizedFormat = TextureSizedInternalFormat::RGBA32F; break;
        default: sizedFormat = TextureSizedInternalFormat::RGB8; break;
    }
    this->pixels.format = internalFormat;
    this->pixels.sizedFormat = sizedFormat;
    this->pixels.dataType = TexturePixelDataType::Float;
    return true;
}

bool Texture::SetPixelsData(const std::vector<GLfloat> &pixelsData, int channels){
    TextureInternalFormat format;
    switch(channels){
        case 3: format = TextureInternalFormat::RGB; break;
        case 4: format = TextureInternalFormat::RGBA; break;
        default: return false;
    }
    return SetPixelsData(pixelsData, format);
}

const TexturePixels &Texture::GetPixels(){
    return this->pixels;
}

size_t Texture::GetWidth() const{
    return this->width;
}

size_t Texture::GetHeight() const{
    return this->height;
}

int Texture::GetChannelsCount() const{
    auto format = this->pixels.sizedFormat;
    if(format == TextureSizedInternalFormat::RGB8 || format == TextureSizedInternalFormat::RGB32F){
        return 3;
    } else if(format == TextureSizedInternalFormat::RGBA8 || format == TextureSizedInternalFormat::RGBA32F){
        return 4;
    } else {
        return 3;
    }
}

GLenum Texture::GetPixelDataTypeGLenum() const{
    switch(pixels.dataType){
        case TexturePixelDataType::UnsignedByte: return GL_UNSIGNED_BYTE;
        case TexturePixelDataType::Float: return GL_FLOAT;
        default: return GL_UNSIGNED_BYTE;
    }
}

GLenum Texture::GetFormatGLenum() const{
    switch(pixels.format){
        case TextureInternalFormat::RGB: return GL_RGB;
        case TextureInternalFormat::RGBA: return GL_RGBA;
        default: return GL_RGB;
    }
}

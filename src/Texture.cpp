#include "Texture.hpp"
#include <iostream>
Texture::Texture(size_t width, size_t height){
    this->width = width;
    this->height = height;
}

bool Texture::SetPixelsData(const std::vector<GLubyte> &pixelsData, TextureFormat format){
    int multiple = 0;
    switch(format){
        case TextureFormat::RGB: multiple = 3; break;
        case TextureFormat::RGBA: multiple = 4; break;
        case TextureFormat::RED: multiple = 1; break;
        default: multiple = 3; break;
    }
    if(pixelsData.size() % multiple != 0)
        return false;

    this->pixels.data = pixelsData;
    TextureSizedInternalFormat sizedFormat = TextureSizedInternalFormat::RGB8;
    switch(format){
        case TextureFormat::RGB: sizedFormat = TextureSizedInternalFormat::RGB8; break;
        case TextureFormat::RGBA: sizedFormat = TextureSizedInternalFormat::RGBA8; break;
        case TextureFormat::RED: sizedFormat = TextureSizedInternalFormat::R8; break;
        default: sizedFormat = TextureSizedInternalFormat::RGB8; break;
    }
    this->pixels.format = format;
    this->pixels.sizedFormat = sizedFormat;
    this->pixels.dataType = TexturePixelDataType::UnsignedByte;
    return true;
}

bool Texture::SetPixelsData(const std::vector<GLubyte> &pixelsData, int channels){
    TextureFormat format;
    switch(channels){
        case 3: format = TextureFormat::RGB; break;
        case 4: format = TextureFormat::RGBA; break;
        default: return false;
    }
    return SetPixelsData(pixelsData, format);
}

bool Texture::SetPixelsData(const std::vector<GLfloat> &pixelsData, TextureFormat format){
    int multiple = 0;
    switch(format){
        case TextureFormat::RGB: multiple = 3; break;
        case TextureFormat::RGBA: multiple = 4; break;
        case TextureFormat::RED: multiple = 1; break;
        default: multiple = 3; break;
    }
    if(pixelsData.size() % multiple != 0)
        return false;

    this->pixels.data = pixelsData;
    TextureSizedInternalFormat sizedFormat = TextureSizedInternalFormat::RGB8;
    switch(format){
        case TextureFormat::RGB: sizedFormat = TextureSizedInternalFormat::RGB32F; break;
        case TextureFormat::RGBA: sizedFormat = TextureSizedInternalFormat::RGBA32F; break;
        case TextureFormat::RED: sizedFormat = TextureSizedInternalFormat::R32F; break;
        default: sizedFormat = TextureSizedInternalFormat::RGB8; break;
    }
    this->pixels.format = format;
    this->pixels.sizedFormat = sizedFormat;
    this->pixels.dataType = TexturePixelDataType::Float;
    return true;
}

bool Texture::SetPixelsData(const std::vector<GLfloat> &pixelsData, int channels){
    TextureFormat format;
    switch(channels){
        case 3: format = TextureFormat::RGB; break;
        case 4: format = TextureFormat::RGBA; break;
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
        case TextureFormat::RGB: return GL_RGB;
        case TextureFormat::RGBA: return GL_RGBA;
        case TextureFormat::RED: return GL_RED;
        default: return GL_RGB;
    }
}

GLenum Texture::GetSizedFormatGLenum() const
{
    switch(pixels.sizedFormat){
        case TextureSizedInternalFormat::RGB8: return GL_RGB8;
        case TextureSizedInternalFormat::RGBA8: return GL_RGBA8;
        case TextureSizedInternalFormat::R8: return GL_R8;
        case TextureSizedInternalFormat::RGB32F: return GL_RGB32F;
        case TextureSizedInternalFormat::RGBA32F: return GL_RGBA32F;
        case TextureSizedInternalFormat::R32F: return GL_R32F;
        default: return GL_RGB8;
    }
}

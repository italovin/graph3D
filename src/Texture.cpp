#include "Texture.hpp"
#include <filesystem>
#include <stb/stb_image.h>

bool Texture::Load(const std::string &filePath, bool isSRGB, bool mirrorVertically)
{
    if(!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
        return false;
    // Try compressed formats (dds or ktx) first
    handle = gli::load(filePath);

    if(!handle.empty())
        return true;

    //Using common image formats
    // sail::image image(filePath);
    // if(!image.is_valid())
    //     return false;
    // // sail images stores pixels in Y = 0 = top orientation.
    // if(mirrorVertically)
    //     image.mirror(SailOrientation::SAIL_ORIENTATION_MIRRORED_VERTICALLY);
    // glm::ivec2 dimensions = glm::ivec2(image.width(), image.height());

    // gli::format format;

    // switch(image.pixel_format()){
    //     case SailPixelFormat::SAIL_PIXEL_FORMAT_BPP24_RGB:
    //     format = isSRGB ? gli::format::FORMAT_RGB8_SRGB_PACK8 : gli::format::FORMAT_RGB8_UNORM_PACK8; break;
    //     case SailPixelFormat::SAIL_PIXEL_FORMAT_BPP32_RGBA:
    //     format = isSRGB ? gli::format::FORMAT_RGBA8_SRGB_PACK8 : gli::format::FORMAT_RGBA8_UNORM_PACK8; break;
    //     case SailPixelFormat::SAIL_PIXEL_FORMAT_BPP64_RGBA:
    //     format = gli::format::FORMAT_RGBA16_UNORM_PACK16; break;
    //     default: format = gli::format::FORMAT_RGB8_SRGB_PACK8; break;
    // }
    stbi_set_flip_vertically_on_load(mirrorVertically ? 1 : 0);
    glm::ivec2 dimensions;
    int channels = 0;
    unsigned char *image = stbi_load(filePath.c_str(), &dimensions.x, &dimensions.y, &channels, 0);
    gli::format format;
    switch(channels){
        case 3:
        format = isSRGB ? gli::format::FORMAT_RGB8_SRGB_PACK8 : gli::format::FORMAT_RGB8_UNORM_PACK8; break;
        case 4:
        format = isSRGB ? gli::format::FORMAT_RGBA8_SRGB_PACK8 : gli::format::FORMAT_RGBA8_UNORM_PACK8; break;
        default: format = gli::format::FORMAT_RGB8_SRGB_PACK8; break;
    }
    handle = gli::texture2d(format, dimensions);

    std::memcpy(handle.data(), image, channels*sizeof(unsigned char)*dimensions.x*dimensions.y);
    stbi_image_free(image);

    return true;
}

bool Texture::LoadFromMemory(const std::vector<unsigned char> &pixels, gli::format format, int width, int height, bool mirrorVertically)
{
    handle = gli::texture2d(format, glm::ivec2(width, height));
    if(handle.empty() || handle.size() < sizeof(unsigned char)*pixels.size())
        return false;

    std::memcpy(handle.data(), pixels.data(), sizeof(unsigned char)*pixels.size());
    if(mirrorVertically)
        handle = gli::flip(handle);
    return true;
}

gli::format Texture::GetFormat() const
{
    return handle.format();
}

GLenum Texture::GliInternalFormatToGLenum(gli::format format, bool forceSRGB) {
    switch (format) {
        // Formatos não comprimidos (mais comuns)
        case gli::FORMAT_RGBA8_UNORM_PACK8: return forceSRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        case gli::FORMAT_RGBA8_SRGB_PACK8: return GL_SRGB8_ALPHA8;
        case gli::FORMAT_RGB8_UNORM_PACK8: return forceSRGB ? GL_SRGB8 : GL_RGB8;
        case gli::FORMAT_RGB8_SRGB_PACK8: return GL_SRGB8;
        case gli::FORMAT_R8_UNORM_PACK8: return GL_R8;
        case gli::FORMAT_RG8_UNORM_PACK8: return GL_RG8;
        case gli::FORMAT_RGBA16_SFLOAT_PACK16: return GL_RGBA16F;
        case gli::FORMAT_RGB16_SFLOAT_PACK16: return GL_RGB16F;
        case gli::FORMAT_RGBA32_SFLOAT_PACK32: return GL_RGBA32F;
        case gli::FORMAT_RGB32_SFLOAT_PACK32: return GL_RGB32F;
        case gli::FORMAT_R32_SFLOAT_PACK32: return GL_R32F;
        case gli::FORMAT_RG32_SFLOAT_PACK32: return GL_RG32F;

        // Formatos comprimidos
        /// DXT1 / BC1
        case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8: return forceSRGB ? GL_COMPRESSED_SRGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8: return forceSRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case gli::FORMAT_RGB_DXT1_SRGB_BLOCK8:   return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8:  return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;

        /// DXT3 / BC2
        case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16: return forceSRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;

        /// DXT5 / BC3
        case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16: return forceSRGB ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

        /// BC4
        case gli::FORMAT_R_ATI1N_UNORM_BLOCK8:   return GL_COMPRESSED_RED_RGTC1;
        case gli::FORMAT_R_ATI1N_SNORM_BLOCK8:   return GL_COMPRESSED_SIGNED_RED_RGTC1;

        /// BC5
        case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16: return GL_COMPRESSED_RG_RGTC2;
        case gli::FORMAT_RG_ATI2N_SNORM_BLOCK16: return GL_COMPRESSED_SIGNED_RG_RGTC2;

        /// BC6H
        case gli::FORMAT_RGB_BP_UFLOAT_BLOCK16: return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
        case gli::FORMAT_RGB_BP_SFLOAT_BLOCK16:  return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;

        /// BC7
        case gli::FORMAT_RGBA_BP_UNORM_BLOCK16: return forceSRGB ? GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB : GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case gli::FORMAT_RGBA_BP_SRGB_BLOCK16:   return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;

        /// ETC2
        case gli::FORMAT_RGB_ETC2_UNORM_BLOCK8: return forceSRGB ? GL_COMPRESSED_SRGB8_ETC2 : GL_COMPRESSED_RGB8_ETC2;
        case gli::FORMAT_RGB_ETC2_SRGB_BLOCK8: return GL_COMPRESSED_SRGB8_ETC2;
        case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8: return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC : GL_COMPRESSED_RGBA8_ETC2_EAC;
        case gli::FORMAT_RGBA_ETC2_SRGB_BLOCK8: return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;

        /// ASTC
        // 4x4
        case gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16:   return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR : GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        case gli::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16:    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        // 5x5
        case gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16:   return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR : GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
        case gli::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16:    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
        // 6x6
        case gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16:   return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR : GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
        case gli::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16:    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
        // 8x8
        case gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16:   return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR : GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
        case gli::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16:    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
        // 10x10
        case gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16: return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR : GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
        case gli::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16:  return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
        // 12x12
        case gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16: return forceSRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR : GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
        case gli::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16:  return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;

        // Caso não suportado
        default:
            return GL_NONE; // Retorna GL_NONE se o formato não for mapeado
    }
}

GLenum Texture::GliClientFormatToGLenum(gli::format format)
{
    size_t channels = gli::component_count(format);
    switch(channels){
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_NONE;
    }
}

GLenum Texture::GliTypeToGLenum(gli::format format)
{
    if(gli::is_unorm(format) || gli::is_srgb(format)){
        return GL_UNSIGNED_BYTE;
    } else if(gli::is_float(format)){
        return GL_FLOAT;
    }
    return GL_NONE;
}

bool Texture::IsCompressed() const
{
    return gli::is_compressed(handle.format());
}

gli::texture::extent_type Texture::GetDimensions() const
{
    return handle.extent();
}

int Texture::GetSize() const{
    return handle.size();
}

const void *Texture::GetData() const
{
    return handle.data();
}

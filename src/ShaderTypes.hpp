#ifndef SHADER_DATA_TYPE_H
#define SHADER_DATA_TYPE_H

//Associated with shader types
enum class ShaderDataType{
    Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Sampler2D, Sampler2DArray
};

enum class ShaderQualifierType{
    In_Out, Uniform
};
#endif
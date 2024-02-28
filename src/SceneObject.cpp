#include <glad/glad.h>
#include "VertexBuffer.cpp"

class SceneObject{
private:
    unsigned int handle;
    VertexBuffer vbo;
public:
    SceneObject(){
        glGenVertexArrays(1, &handle);
    }
    void Bind(){
        glBindVertexArray(handle);
    }
    void SetupObject(float data[], unsigned int dataSize){
        Bind();
        vbo.BufferData(data, dataSize);
    }
};
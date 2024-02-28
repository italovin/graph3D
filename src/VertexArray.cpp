#include <glad/glad.h>

class VertexArray {
private:
    unsigned int handle;
public:
    VertexArray(){
        glGenVertexArrays(1, &handle);
    }
    void Bind(){
        glBindVertexArray(handle);
    }
};
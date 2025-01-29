#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL_video.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Window{
private:
    SDL_Window *handle;
    std::string title;
    int width = 0;
    int height = 0;
    int contextMajorVersion = 1;
    int contextMinorVersion = 0;
    int contextPatchVersion = 0;
public:
    bool Create(const std::string &title, int width, int height);
    void SetContextVersion(int majorVersion, int minorVersion, int patchVersion);
    int GetGLSLVersion();
    // Create a context from this window and make current
    SDL_GLContext MakeContextCurrent();
    SDL_Window* GetHandle() const;
    int GetWidth() const;
    int GetHeight() const;
};
#endif
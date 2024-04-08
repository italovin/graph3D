#include "Input.hpp"

glm::vec2 Input::mousePosition = glm::vec2(0);
float Input::mouseXDelta = 0;
float Input::mouseYDelta = 0;
bool Input::firstMouseMove = true;
Window Input::registeredWindow = Window();
std::map<int, KeyState> Input::monitoredKeys = std::map<int, KeyState>();

void Input::MonitorKey(int key, KeyState keyState){
    monitoredKeys.insert(std::make_pair(key, keyState));
}

void Input::FirstMove(){
    if(firstMouseMove){
        glm::vec2 mousePos = Input::GetMousePosition();
        mousePosition = mousePos;
        firstMouseMove = false;
    }
}

void Input::SetupCallbacks(){
    registeredWindow.SetMouseCallback(MouseCallback);
    registeredWindow.SetKeyCallback(KeyCallback);
}

void Input::RegisterWindow(const Window &window){
    registeredWindow = window;
    SetupCallbacks();
}

void Input::MouseCallback(GLFWwindow *window, double xPos, double yPos){
    Input::FirstMove();
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(monitoredKeys.count(key) == 0){
        Input::MonitorKey(key, KEY_UP);
    }
}

void Input::Update(const Window &window){
    if(!firstMouseMove){
        glm::vec2 mousePos = Input::GetMousePosition();
        glm::vec2 delta = mousePos - mousePosition;
        mouseXDelta = delta.x;
        mouseYDelta = -delta.y;
        mousePosition = mousePos;
    }
    for (auto &&i : monitoredKeys)
    {
        KeyState lastKeyState = i.second;
        int key = i.first;
        int state = glfwGetKey(registeredWindow.GetHandle(), key);
        if(lastKeyState == KEY_UP && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_DOWN;
        } else if((lastKeyState == KEY_DOWN || lastKeyState == KEY_HELD) && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_HELD;
        } else if(state == GLFW_RELEASE){
            monitoredKeys[key] = KEY_UP;
        }
    }
    
    
}

glm::vec2 Input::GetMousePosition(){
    double x,y;
    if(registeredWindow.GetHandle() != nullptr)
        glfwGetCursorPos(registeredWindow.GetHandle(), &x, &y);
    glm::vec2 mousePos((float)x, (float)y);
    return mousePos;
}

float Input::GetMouseDeltaX(){
    return mouseXDelta;
}

float Input::GetMouseDeltaY(){
    return mouseYDelta;
}

bool Input::GetKeyHeld(int key){
    if(monitoredKeys.count(key) == 0)
        return false;
    return monitoredKeys[key] == KEY_HELD || monitoredKeys[key] == KEY_DOWN;
}

bool Input::GetKeyDown(int key){
    if(monitoredKeys.count(key) == 0)
        return false;
    return monitoredKeys[key] == KEY_DOWN;
}
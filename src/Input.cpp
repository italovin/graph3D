#include "Input.hpp"

glm::vec2 Input::mousePosition = glm::vec2(0);
float Input::mouseXDelta = 0;
float Input::mouseYDelta = 0;
bool Input::firstMouseMove = true;
std::vector<int> Input::joysticksAxesCount = std::vector<int>();
std::vector<const float*> Input::joysticksAxes = std::vector<const float*>();
Window Input::registeredWindow = Window();
std::unordered_map<int, KeyState> Input::monitoredKeys = std::unordered_map<int, KeyState>();
std::unordered_map<int, MouseButtonState> Input::monitoredMouseButtons = std::unordered_map<int, MouseButtonState>();

void Input::MonitorKey(int key, KeyState keyState){
    monitoredKeys.insert(std::make_pair(key, keyState));
}

void Input::MonitorMouseButton(int button, MouseButtonState mouseButtonState){
    monitoredMouseButtons.insert(std::make_pair(button, mouseButtonState));
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
    registeredWindow.SetMouseButtonCallback(MouseButtonCallback);
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
        Input::MonitorKey(key, KEY_RELEASE);
    }
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if(monitoredMouseButtons.count(button) == 0){
        Input::MonitorMouseButton(button, MOUSE_BTN_RELEASE);
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
        if((lastKeyState == KEY_UP || lastKeyState == KEY_RELEASE) && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_DOWN;
        } else if((lastKeyState == KEY_DOWN || lastKeyState == KEY_HELD) && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_HELD;
        } else if((lastKeyState == KEY_DOWN || lastKeyState == KEY_HELD) && state == GLFW_RELEASE){
            monitoredKeys[key] = KEY_UP;
        } else if(lastKeyState == KEY_UP && state == GLFW_RELEASE){
            monitoredKeys[key] = KEY_RELEASE;
        }
    }
    for (auto &&i : monitoredMouseButtons){
        MouseButtonState lastMouseBtnState = i.second;
        int button = i.first;
        int state = glfwGetMouseButton(registeredWindow.GetHandle(), button);
        if((lastMouseBtnState == MOUSE_BTN_UP || lastMouseBtnState == MOUSE_BTN_RELEASE) && state == GLFW_PRESS){
            monitoredMouseButtons[button] = MOUSE_BTN_DOWN;
        } else if((lastMouseBtnState == MOUSE_BTN_DOWN || lastMouseBtnState == MOUSE_BTN_HELD) && state == GLFW_PRESS){
            monitoredMouseButtons[button] = MOUSE_BTN_HELD;
        } else if((lastMouseBtnState == MOUSE_BTN_DOWN || lastMouseBtnState == MOUSE_BTN_HELD) && state == GLFW_RELEASE){
            monitoredMouseButtons[button] = MOUSE_BTN_UP;
        } else if(lastMouseBtnState == MOUSE_BTN_UP && state == GLFW_RELEASE){
            monitoredMouseButtons[button] = MOUSE_BTN_RELEASE;
        }
    }
    int id = GLFW_JOYSTICK_1;
    bool updatingJoysticks = true;
    while(!updatingJoysticks){
        updatingJoysticks = glfwJoystickPresent(id) == GLFW_TRUE;
        if(updatingJoysticks){
            if(id >= joysticksAxes.size() && id >= joysticksAxesCount.size()){
                int axesCount = 0;
                joysticksAxes.push_back(glfwGetJoystickAxes(id, &axesCount));
                joysticksAxesCount.push_back(axesCount);
            } else {
                int axesCount = 0;
                joysticksAxes[id] = glfwGetJoystickAxes(id, &axesCount);
                joysticksAxesCount[id] = axesCount;
            }
        }
        id++;
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

KeyState Input::GetKeyState(int key){
    if(monitoredKeys.count(key) == 0)
        return KEY_RELEASE;
    return monitoredKeys[key];
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

bool Input::GetKeyUp(int key){
    if(monitoredKeys.count(key) == 0)
        return false;
    return monitoredKeys[key] == KEY_UP;
}

MouseButtonState Input::GetMouseButtonState(int button){
    if(monitoredMouseButtons.count(button) == 0)
        return MOUSE_BTN_RELEASE;
    return monitoredMouseButtons[button];
}

bool Input::GetMouseButtonHeld(int button){
    if(monitoredMouseButtons.count(button) == 0)
        return false;
    return monitoredMouseButtons[button] == MOUSE_BTN_HELD || monitoredMouseButtons[button] == MOUSE_BTN_DOWN;
}

bool Input::GetMouseButtonDown(int button){
    if(monitoredMouseButtons.count(button) == 0)
        return false;
    return monitoredMouseButtons[button] == MOUSE_BTN_DOWN;
}

bool Input::GetMouseButtonUp(int button){
    if(monitoredMouseButtons.count(button) == 0)
        return false;
    return monitoredMouseButtons[button] == MOUSE_BTN_UP;
}

bool Input::IsJoystickPresent(int jid){
    return glfwJoystickPresent(jid) == GLFW_TRUE;
}
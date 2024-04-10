#include "Input.hpp"

Input::Mouse Input::mouse = { .position = glm::vec2(0), .xDelta = 0, .yDelta = 0, .firstMove = true,
.monitoredButtons = std::unordered_map<int, MouseButtonState>()};
std::map<int, Input::Joystick> Input::joysticks = std::map<int, Joystick>();
Window Input::registeredWindow = Window();
std::unordered_map<int, KeyState> Input::monitoredKeys = std::unordered_map<int, KeyState>();
int Input::positiveHorizontalKey = GLFW_KEY_D;
int Input::negativeHorizontalKey = GLFW_KEY_A;
int Input::positiveVerticalKey = GLFW_KEY_W;
int Input::negativeVerticalKey = GLFW_KEY_S;

void Input::MonitorKey(int key, KeyState keyState){
    monitoredKeys.insert(std::make_pair(key, keyState));
}

void Input::MonitorMouseButton(int button, MouseButtonState mouseButtonState){
    mouse.monitoredButtons.insert(std::make_pair(button, mouseButtonState));
}

void Input::FirstMove(glm::vec2 mousePos){
    if(mouse.firstMove){
        mouse.position = mousePos;
        mouse.firstMove = false;
    }
}

void Input::SetupCallbacks(){
    registeredWindow.SetMouseCallback(MouseCallback);
    registeredWindow.SetKeyCallback(KeyCallback);
    registeredWindow.SetMouseButtonCallback(MouseButtonCallback);
    glfwSetJoystickCallback(JoystickCallback);
}

void Input::RegisterWindow(const Window &window){
    registeredWindow = window;
    SetupCallbacks();
}

void Input::MouseCallback(GLFWwindow *window, double xPos, double yPos){
    Input::FirstMove(glm::vec2((float)xPos, (float)yPos));
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(monitoredKeys.count(key) == 0){
        Input::MonitorKey(key, KEY_UNKNOWN);
    }
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if(mouse.monitoredButtons.count(button) == 0){
        Input::MonitorMouseButton(button, MOUSE_BTN_UNKNOWN);
    }
}

void Input::JoystickCallback(int jid, int event){
    if(event == GLFW_DISCONNECTED){
        std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
        joysticks.erase(it);
    }
}
std::map<int, Input::Joystick>::iterator Input::GetJoystick(int jid){
    std::map<int, Joystick>::iterator it = joysticks.find(jid);
    return it;
}

void Input::Update(const Window &window){
    if(registeredWindow.GetHandle() == nullptr){
        return;
    }
    if(!mouse.firstMove){
        double x,y;
        glfwGetCursorPos(registeredWindow.GetHandle(), &x, &y);
        glm::vec2 mousePos((float)x, (float)y);
        glm::vec2 delta = mousePos - mouse.position;
        mouse.xDelta = delta.x;
        mouse.yDelta = -delta.y;
        mouse.position = mousePos;
    }
    for (auto &&i : monitoredKeys)
    {
        KeyState lastKeyState = i.second;
        int key = i.first;
        int state = glfwGetKey(registeredWindow.GetHandle(), key);
        if((lastKeyState == KEY_UP || lastKeyState == KEY_RELEASE || lastKeyState == KEY_UNKNOWN) && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_DOWN;
        } else if((lastKeyState == KEY_DOWN || lastKeyState == KEY_HELD) && state == GLFW_PRESS){
            monitoredKeys[key] = KEY_HELD;
        } else if((lastKeyState == KEY_DOWN || lastKeyState == KEY_HELD || lastKeyState == KEY_UNKNOWN) && state == GLFW_RELEASE){
            monitoredKeys[key] = KEY_UP;
        } else if(lastKeyState == KEY_UP && state == GLFW_RELEASE){
            monitoredKeys[key] = KEY_RELEASE;
        }
    }
    for (auto &&i : mouse.monitoredButtons){
        MouseButtonState lastMouseBtnState = i.second;
        int button = i.first;
        int state = glfwGetMouseButton(registeredWindow.GetHandle(), button);
        if((lastMouseBtnState == MOUSE_BTN_UP || lastMouseBtnState == MOUSE_BTN_RELEASE || lastMouseBtnState == MOUSE_BTN_UNKNOWN) && state == GLFW_PRESS){
            mouse.monitoredButtons[button] = MOUSE_BTN_DOWN;
        } else if((lastMouseBtnState == MOUSE_BTN_DOWN || lastMouseBtnState == MOUSE_BTN_HELD) && state == GLFW_PRESS){
            mouse.monitoredButtons[button] = MOUSE_BTN_HELD;
        } else if((lastMouseBtnState == MOUSE_BTN_DOWN || lastMouseBtnState == MOUSE_BTN_HELD  || lastMouseBtnState == MOUSE_BTN_UNKNOWN) && state == GLFW_RELEASE){
            mouse.monitoredButtons[button] = MOUSE_BTN_UP;
        } else if(lastMouseBtnState == MOUSE_BTN_UP && state == GLFW_RELEASE){
            mouse.monitoredButtons[button] = MOUSE_BTN_RELEASE;
        }
    }
    for(int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++){
        if(Input::IsJoystickPresent(i)){
            if(joysticks.count(i) == 0){
                Joystick newJoystick;
                newJoystick.name = glfwGetGamepadName(i);
                glfwGetGamepadState(i, &newJoystick.state);
                joysticks.insert(std::make_pair(i, newJoystick));
            } else {
                glfwGetGamepadState(i, &joysticks[i].state);
            }
        }
    }
    
}

glm::vec2 Input::GetMousePosition(){
    return mouse.position;
}

float Input::GetMouseDeltaX(){
    return mouse.xDelta;
}

float Input::GetMouseDeltaY(){
    return mouse.yDelta;
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

float Input::GetAxis(KeyboardAxis axis){
    if(axis == KEYB_AXIS_HORIZONTAL){
        if(Input::GetKeyHeld(positiveHorizontalKey)){
            return 1;
        } else if(Input::GetKeyHeld(negativeHorizontalKey)){
            return -1;
        } else {
            return 0;
        }
    } else if(axis == KEYB_AXIS_VERTICAL){
        if(Input::GetKeyHeld(positiveVerticalKey)){
            return 1;
        } else if(Input::GetKeyHeld(negativeVerticalKey)){
            return -1;
        } else {
            return 0;
        }
    }
    return 0;
}

MouseButtonState Input::GetMouseButtonState(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return MOUSE_BTN_RELEASE;
    return mouse.monitoredButtons[button];
}

bool Input::GetMouseButtonHeld(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MOUSE_BTN_HELD || mouse.monitoredButtons[button] == MOUSE_BTN_DOWN;
}

bool Input::GetMouseButtonDown(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MOUSE_BTN_DOWN;
}

bool Input::GetMouseButtonUp(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MOUSE_BTN_UP;
}

bool Input::IsJoystickPresent(int jid){
    return glfwJoystickIsGamepad(jid) == GLFW_TRUE;
}

int Input::JoysticksCount(){
    return joysticks.size();
}

float Input::GetJoystickAxisLeftX(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return it->second.state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
}

float Input::GetJoystickAxisLeftY(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return -it->second.state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]; //GLFW joystick +Y is down, then negate it
}

float Input::GetJoystickAxisRightX(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return it->second.state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
}

float Input::GetJoystickAxisRightY(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return -it->second.state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
}
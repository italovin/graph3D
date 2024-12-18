#include "Input.hpp"

Window Input::registeredWindow = Window();

Input::Mouse Input::mouse = Input::Mouse(glm::vec2(0), 0, 0, true, std::unordered_map<int, MouseButtonState>());

Input::Keyboard Input::keyboard = Input::Keyboard(std::unordered_map<int, KeyState>(), GLFW_KEY_D, GLFW_KEY_A, GLFW_KEY_W, GLFW_KEY_S);

std::map<int, Input::Joystick> Input::joysticks = std::map<int, Joystick>();
int Input::maxJoysticksToSearch = 8;

void Input::MonitorKey(int key, KeyState keyState){
    keyboard.monitoredKeys.insert(std::make_pair(key, keyState));
}

void Input::MonitorMouseButton(int button, MouseButtonState mouseButtonState){
    mouse.monitoredButtons.insert(std::make_pair(button, mouseButtonState));
}

void Input::FirstMove(glm::vec2 mousePos){
    mouse.position = mousePos;
    mouse.firstMove = false;
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
    if(mouse.firstMove){
        Input::FirstMove(glm::vec2((float)xPos, (float)yPos));
    }
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(keyboard.monitoredKeys.count(key) == 0){
        Input::MonitorKey(key, KeyState::Unknown);
    }
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if(mouse.monitoredButtons.count(button) == 0){
        Input::MonitorMouseButton(button, MouseButtonState::Unknown);
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
    UpdateMouse();
    UpdateKeyboard();
    UpdateJoysticks();
}

void Input::UpdateMouse(){
    if(!mouse.firstMove){
        double x,y;
        glfwGetCursorPos(registeredWindow.GetHandle(), &x, &y);
        glm::vec2 mousePos((float)x, (float)y);
        glm::vec2 delta = mousePos - mouse.position;
        mouse.xDelta = delta.x;
        mouse.yDelta = -delta.y;
        mouse.position = mousePos;
    }
    for (auto &&i : mouse.monitoredButtons){
        MouseButtonState lastMouseBtnState = i.second;
        int button = i.first;
        int state = glfwGetMouseButton(registeredWindow.GetHandle(), button);
        if((lastMouseBtnState == MouseButtonState::Up || lastMouseBtnState == MouseButtonState::Release || lastMouseBtnState == MouseButtonState::Unknown) && state == GLFW_PRESS){
            mouse.monitoredButtons[button] = MouseButtonState::Down;
        } else if((lastMouseBtnState == MouseButtonState::Down || lastMouseBtnState == MouseButtonState::Held) && state == GLFW_PRESS){
            mouse.monitoredButtons[button] = MouseButtonState::Held;
        } else if((lastMouseBtnState == MouseButtonState::Down || lastMouseBtnState == MouseButtonState::Held  || lastMouseBtnState == MouseButtonState::Unknown) && state == GLFW_RELEASE){
            mouse.monitoredButtons[button] = MouseButtonState::Up;
        } else if(lastMouseBtnState == MouseButtonState::Up && state == GLFW_RELEASE){
            mouse.monitoredButtons[button] = MouseButtonState::Release;
        }
    }
}

void Input::UpdateKeyboard(){
    for (auto &&i : keyboard.monitoredKeys)
    {
        KeyState lastKeyState = i.second;
        int key = i.first;
        int state = glfwGetKey(registeredWindow.GetHandle(), key);
        if((lastKeyState == KeyState::Up || lastKeyState == KeyState::Release || lastKeyState == KeyState::Unknown) && state == GLFW_PRESS){
            keyboard.monitoredKeys[key] = KeyState::Down;
        } else if((lastKeyState == KeyState::Down || lastKeyState == KeyState::Held) && state == GLFW_PRESS){
            keyboard.monitoredKeys[key] = KeyState::Held;
        } else if((lastKeyState == KeyState::Down || lastKeyState == KeyState::Held || lastKeyState == KeyState::Unknown) && state == GLFW_RELEASE){
            keyboard.monitoredKeys[key] = KeyState::Up;
        } else if(lastKeyState == KeyState::Up && state == GLFW_RELEASE){
            keyboard.monitoredKeys[key] = KeyState::Release ;
        }
    }
}

void Input::UpdateJoysticks(){
    for(int i = GLFW_JOYSTICK_1; i < maxJoysticksToSearch; i++){
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
    if(keyboard.monitoredKeys.count(key) == 0)
        return KeyState::Release;
    return keyboard.monitoredKeys[key];
}

bool Input::GetKeyHeld(int key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Held || keyboard.monitoredKeys[key] == KeyState::Down;
}

bool Input::GetKeyDown(int key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Down;
}

bool Input::GetKeyUp(int key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Up;
}

float Input::GetAxis(KeyboardAxis axis){
    if(axis == KeyboardAxis::Horizontal){
        if(Input::GetKeyHeld(keyboard.positiveHorizontalKey)){
            return 1;
        } else if(Input::GetKeyHeld(keyboard.negativeHorizontalKey)){
            return -1;
        } else {
            return 0;
        }
    } else if(axis == KeyboardAxis::Vertical){
        if(Input::GetKeyHeld(keyboard.positiveVerticalKey)){
            return 1;
        } else if(Input::GetKeyHeld(keyboard.negativeVerticalKey)){
            return -1;
        } else {
            return 0;
        }
    }
    return 0;
}

MouseButtonState Input::GetMouseButtonState(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return MouseButtonState::Release;
    return mouse.monitoredButtons[button];
}

bool Input::GetMouseButtonHeld(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MouseButtonState::Held || mouse.monitoredButtons[button] == MouseButtonState::Down;
}

bool Input::GetMouseButtonDown(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MouseButtonState::Down;
}

bool Input::GetMouseButtonUp(int button){
    if(mouse.monitoredButtons.count(button) == 0)
        return false;
    return mouse.monitoredButtons[button] == MouseButtonState::Up;
}

void Input::SetMaxJoysticksToSearch(int maxJoysticks){
    maxJoysticksToSearch = maxJoysticks <= GLFW_JOYSTICK_LAST + 1 ? maxJoysticks : GLFW_JOYSTICK_LAST + 1;
    maxJoysticksToSearch = maxJoysticksToSearch >= 0 ? maxJoysticksToSearch : -maxJoysticksToSearch;
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

float Input::GetJoystickLeftTrigger(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return -it->second.state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
}

float Input::GetJoystickRightTrigger(int jid){
    if(!Input::IsJoystickPresent(jid))
        return 0;
    std::map<int, Joystick>::iterator it = Input::GetJoystick(jid);
    return -it->second.state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
}
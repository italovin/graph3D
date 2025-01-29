#include "Input.hpp"

Input::Mouse Input::mouse = Input::Mouse(glm::vec2(0), 0, 0, true, std::unordered_map<int, MouseButtonState>());

Input::Keyboard Input::keyboard = Input::Keyboard(std::unordered_map<SDL_Keycode, KeyState>(), SDLK_d, SDLK_a, SDLK_w, SDLK_s);

std::map<int, Input::GameController> Input::gameControllers = std::map<int, GameController>();

bool Input::quitState = false;

bool Input::mouseInitialFakeMotion = false;

void Input::MonitorKey(SDL_Keycode key, KeyState keyState){
    keyboard.monitoredKeys.try_emplace(key, keyState);
}

void Input::MonitorMouseButton(int button, MouseButtonState mouseButtonState){
    mouse.monitoredButtons.try_emplace(button, mouseButtonState);
}

void Input::Update(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // This also pump events
        switch(event.type){
            case SDL_QUIT:
                quitState = true;
                break;
            case SDL_MOUSEMOTION:
                if (!mouseInitialFakeMotion) { // Fake mouse motion from SDL2
                    mouseInitialFakeMotion = true;
                } else if(mouse.firstMove){ // Real first mouse motion - Initialize mouse state
                    int x,y;
                    SDL_GetMouseState(&x, &y);
                    mouse.xDelta = 0.0f;
                    mouse.yDelta = 0.0f;
                    mouse.position = glm::vec2((float)x, (float)y);
                    mouse.firstMove = false;
                }
                break;
            case SDL_KEYDOWN: // This starts monitoring the specific key in undefined state
                MonitorKey(event.key.keysym.sym, KeyState::Unknown);
                break;
            case SDL_MOUSEBUTTONDOWN: // This starts monitoring the specific mouse button in undefined state
                MonitorMouseButton(event.button.button, MouseButtonState::Unknown);
                break;
            case SDL_CONTROLLERDEVICEADDED:
                {
                    SDL_GameController *controllerHandle = SDL_GameControllerOpen(event.cdevice.which);
                    if(!controllerHandle) // Exceptional case when controller added cannot be opened
                        break;

                    std::string controllerName = SDL_GameControllerName(controllerHandle);
                    GameController controller(controllerName, controllerHandle);
                    gameControllers.try_emplace(event.cdevice.which, controller);
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                SDL_GameControllerClose(gameControllers[event.cdevice.which].controller);
                gameControllers.erase(event.cdevice.which);
                break;
            default:
                break;
        }
    }
    UpdateMouse();
    UpdateKeyboard();
}

void Input::UpdateMouse(){
    int x = 0, y = 0;
    int mouseState = SDL_GetMouseState(&x, &y); // Obtém posição e estado do mouse
    if(!mouse.firstMove){
        glm::vec2 newMousePos((float)x, (float)y);
        glm::vec2 delta = newMousePos - mouse.position;
        mouse.xDelta = delta.x;
        mouse.yDelta = -delta.y; // Inverter o eixo Y, se necessário
        mouse.position = newMousePos;
    }
    for (auto &&i : mouse.monitoredButtons){
        MouseButtonState lastMouseBtnState = i.second;
        int button = i.first;
        int state = mouseState && SDL_BUTTON(button);
        if((lastMouseBtnState == MouseButtonState::Up || lastMouseBtnState == MouseButtonState::Released || lastMouseBtnState == MouseButtonState::Unknown) && state == 1){
            mouse.monitoredButtons[button] = MouseButtonState::Down;
        } else if((lastMouseBtnState == MouseButtonState::Down || lastMouseBtnState == MouseButtonState::Held) && state == 1){
            mouse.monitoredButtons[button] = MouseButtonState::Held;
        } else if((lastMouseBtnState == MouseButtonState::Down || lastMouseBtnState == MouseButtonState::Held  || lastMouseBtnState == MouseButtonState::Unknown) && !(state == 1)){
            mouse.monitoredButtons[button] = MouseButtonState::Up;
        } else if(lastMouseBtnState == MouseButtonState::Up && !(state == 1)){
            mouse.monitoredButtons[button] = MouseButtonState::Released;
        }
    }
}

void Input::UpdateKeyboard(){
    const Uint8 *keyStates = SDL_GetKeyboardState(nullptr);
    for (auto &&i : keyboard.monitoredKeys)
    {
        KeyState lastKeyState = i.second;
        int key = i.first;
        int state = keyStates[SDL_GetScancodeFromKey(key)];
        if((lastKeyState == KeyState::Up || lastKeyState == KeyState::Released || lastKeyState == KeyState::Unknown) && state == 1){
            keyboard.monitoredKeys[key] = KeyState::Down;
        } else if((lastKeyState == KeyState::Down || lastKeyState == KeyState::Held) && state == 1){
            keyboard.monitoredKeys[key] = KeyState::Held;
        } else if((lastKeyState == KeyState::Down || lastKeyState == KeyState::Held || lastKeyState == KeyState::Unknown) && state == 0){
            keyboard.monitoredKeys[key] = KeyState::Up;
        } else if(lastKeyState == KeyState::Up && state == 0){
            keyboard.monitoredKeys[key] = KeyState::Released ;
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

KeyState Input::GetKeyState(SDL_Keycode key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return KeyState::Released;
    return keyboard.monitoredKeys[key];
}

bool Input::GetKeyHeld(SDL_Keycode key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Held || keyboard.monitoredKeys[key] == KeyState::Down;
}

bool Input::GetKeyDown(SDL_Keycode key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Down;
}

bool Input::GetKeyUp(SDL_Keycode key){
    if(keyboard.monitoredKeys.count(key) == 0)
        return false;
    return keyboard.monitoredKeys[key] == KeyState::Up;
}

float Input::GetKeyAxis(Axis axis){
    if(axis == Axis::Horizontal){
        if(Input::GetKeyHeld(keyboard.positiveHorizontalKey)){
            return 1;
        } else if(Input::GetKeyHeld(keyboard.negativeHorizontalKey)){
            return -1;
        } else {
            return 0;
        }
    } else if(axis == Axis::Vertical){
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
        return MouseButtonState::Released;
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

bool Input::GetQuitState(){
    return quitState;
}

float Input::GetAxis(Axis axis, int controllerIndex)
{
    float keyboardAxis = GetKeyAxis(axis);
    float gamepadAxis = 0.0f;
    if(axis == Axis::Horizontal){
        gamepadAxis = GetControllerAxisLeftX(controllerIndex);
    } else if(axis == Axis::Vertical){
        gamepadAxis = GetControllerAxisLeftY(controllerIndex);
    } else {
        gamepadAxis = 0.0f;
    }
    return glm::abs(keyboardAxis) > glm::abs(gamepadAxis) ? keyboardAxis : gamepadAxis;
}

int Input::ControllersCount()
{
    return gameControllers.size();
}

float Input::GetControllerAxisLeftX(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_LEFTX);
    float ratio = value < 0 ? 32768.0f : 32767.0f;
    return (float)value/ratio;
}

float Input::GetControllerAxisLeftY(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = -SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_LEFTY);//controller +Y is down, then negate it
    float ratio = value < 0 ? 32768.0f : 32767.0f;
    return (float)value/ratio;
}

float Input::GetControllerAxisRightX(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_RIGHTX);
    float ratio = value < 0 ? 32768.0f : 32767.0f;
    return (float)value/ratio;
}

float Input::GetControllerAxisRightY(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = -SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_RIGHTY);
    float ratio = value < 0 ? 32768.0f : 32767.0f;
    return (float)value/ratio;
}

float Input::GetControllerLeftTrigger(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    return (float)value/32767.0f;
}

float Input::GetControllerRightTrigger(int index){
    if(gameControllers.count(index) == 0)
        return 0;
    int value = SDL_GameControllerGetAxis(gameControllers[index].controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    return (float)value/32767.0f;
}
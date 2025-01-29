#ifndef INPUT_H
#define INPUT_H
#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>
#include <unordered_map>
#include <map>
#include <iostream>

enum class Axis{
    Horizontal,
    Vertical
};

enum class KeyState{
    Held, //key is helding down; in this state at second input update
    Down, //key is down at exact input udpate or frame
    Up, //key is up at exact input udpate or frame
    Released, //key is released for long
    Unknown //undefined state
};

enum class MouseButtonState{
    Held,
    Down,
    Up,
    Released,
    Unknown
};

class Input{
private:
    struct Mouse{
        glm::vec2 position;
        float xDelta;
        float yDelta;
        bool firstMove;
        std::unordered_map<int, MouseButtonState> monitoredButtons;
        Mouse(glm::vec2 position, float xDelta, float yDelta, bool firstMove, const std::unordered_map<int, MouseButtonState> &monitoredButtons):
        position(position),xDelta(xDelta),yDelta(yDelta),firstMove(firstMove),monitoredButtons(monitoredButtons){}
    };
    static Mouse mouse;
    struct Keyboard{
        std::unordered_map<SDL_Keycode, KeyState> monitoredKeys;
        SDL_Keycode positiveHorizontalKey;
        SDL_Keycode negativeHorizontalKey;
        SDL_Keycode positiveVerticalKey;
        SDL_Keycode negativeVerticalKey;
        Keyboard(const std::unordered_map<SDL_Keycode, KeyState> &monitoredKeys, SDL_Keycode positiveHorizontalKey, SDL_Keycode negativeHorizontalKey, SDL_Keycode positiveVerticalKey, SDL_Keycode negativeVerticalKey):
        monitoredKeys(monitoredKeys),positiveHorizontalKey(positiveHorizontalKey),negativeHorizontalKey(negativeHorizontalKey),positiveVerticalKey(positiveVerticalKey),negativeVerticalKey(negativeVerticalKey){}
    }; 
    static Keyboard keyboard;
    struct GameController{
        std::string name;
        SDL_GameController* controller;
        GameController() = default;
        GameController(const std::string &name, SDL_GameController* controller):name(name),controller(controller){}
    };
    static std::map<int, GameController> gameControllers;
    // Check quit event
    static bool quitState;
    // Auxiliar boolean to check initial fake mouse motion
    static bool mouseInitialFakeMotion;
    static void MonitorKey(SDL_Keycode key, KeyState keyState);
    static void MonitorMouseButton(int button, MouseButtonState mouseButtonState);
    static void UpdateMouse();
    static void UpdateKeyboard();
    static void UpdateJoysticks();
public:
    static void Update();
    static glm::vec2 GetMousePosition();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();
    static KeyState GetKeyState(SDL_Keycode key);
    static bool GetKeyHeld(SDL_Keycode key);
    static bool GetKeyDown(SDL_Keycode key);
    static bool GetKeyUp(SDL_Keycode key);
    // Get floating values from default defined axes
    static float GetKeyAxis(Axis axis);
    static MouseButtonState GetMouseButtonState(int button);
    static bool GetMouseButtonHeld(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    // Return true if the user has requested to quit the application
    static bool GetQuitState();
    // Get generic axes values for inputs like keyboard and gamepads. 
    // Always returns max absolute value from input devices
    static float GetAxis(Axis axis, int controllerIndex = 0);
    static int ControllersCount();
    static float GetControllerAxisLeftX(int index);
    // This function is used to get the left stick y-axis value of a controller
    // This value is negated from implementation, then Y+ is up
    static float GetControllerAxisLeftY(int index);
    static float GetControllerAxisRightX(int index);
    // This function is used to get the right stick y-axis value of a controller
    // This value is negated from implementation, then Y+ is up
    static float GetControllerAxisRightY(int index);
    static float GetControllerLeftTrigger(int index);
    static float GetControllerRightTrigger(int index);
};
#endif

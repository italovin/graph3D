#ifndef INPUT_H
#define INPUT_H
#include <glm/glm.hpp>
#include "Window.hpp"
#include <unordered_map>
#include <map>
#include <iostream>

enum KeyboardAxis{
    KEYB_AXIS_HORIZONTAL,
    KEYB_AXIS_VERTICAL
};

enum KeyState{
    KEY_HELD, //key is helding down; in this state at second input update
    KEY_DOWN, //key is down at exact input udpate or frame
    KEY_UP, //key is up at exact input udpate or frame
    KEY_RELEASE, //key is released for long
    KEY_UNKNOWN //undefined state
};

enum MouseButtonState{
    MOUSE_BTN_HELD,
    MOUSE_BTN_DOWN,
    MOUSE_BTN_UP,
    MOUSE_BTN_RELEASE,
    MOUSE_BTN_UNKNOWN
};

class Input{
private:
    struct Mouse{
        glm::vec2 position;
        float xDelta;
        float yDelta;
        bool firstMove;
        std::unordered_map<int, MouseButtonState> monitoredButtons;
    };
    static Mouse mouse;
    struct Joystick{
        std::string name;
        GLFWgamepadstate state;
    };
    static std::map<int, Joystick> joysticks;
    static Window registeredWindow;
    static std::unordered_map<int, KeyState> monitoredKeys;
    static int positiveHorizontalKey;
    static int negativeHorizontalKey;
    static int positiveVerticalKey;
    static int negativeVerticalKey;
    static void MonitorKey(int key, KeyState keyState);
    static void MonitorMouseButton(int button, MouseButtonState mouseButtonState);
    static void FirstMove(glm::vec2 mousePos);
    static void SetupCallbacks();
    static void MouseCallback(GLFWwindow *window, double xPos, double yPos);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void JoystickCallback(int jid, int event);
    static std::map<int, Input::Joystick>::iterator GetJoystick(int jid);
public:
    static void RegisterWindow(const Window &window);
    static void Update(const Window &window);
    static glm::vec2 GetMousePosition();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();
    static KeyState GetKeyState(int key);
    static bool GetKeyHeld(int key);
    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);
    static float GetAxis(KeyboardAxis axis);
    static MouseButtonState GetMouseButtonState(int button);
    static bool GetMouseButtonHeld(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static bool IsJoystickPresent(int jid);
    static int JoysticksCount();
    static float GetJoystickAxisLeftX(int jid);
    static float GetJoystickAxisLeftY(int jid);
    static float GetJoystickAxisRightX(int jid);
    static float GetJoystickAxisRightY(int jid);
};
#endif
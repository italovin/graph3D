#ifndef INPUT_H
#define INPUT_H
#include <glm/glm.hpp>
#include "Window.hpp"
#include <unordered_map>
#include <vector>

enum KeyState{
    KEY_HELD, //key is helding down; in this state at second input update
    KEY_DOWN, //key is down at exact input udpate or frame
    KEY_UP, //key is up at exact input udpate or frame
    KEY_RELEASE //key is released for long
};

enum MouseButtonState{
    MOUSE_BTN_HELD,
    MOUSE_BTN_DOWN,
    MOUSE_BTN_UP,
    MOUSE_BTN_RELEASE
};

class Input{
private:
    static glm::vec2 mousePosition;
    static float mouseXDelta;
    static float mouseYDelta;
    static bool firstMouseMove;
    static std::vector<int> joysticksAxesCount;
    static std::vector<const float*> joysticksAxes;
    static Window registeredWindow;
    static std::unordered_map<int, KeyState> monitoredKeys;
    static std::unordered_map<int, MouseButtonState> monitoredMouseButtons;
    static void MonitorKey(int key, KeyState keyState);
    static void MonitorMouseButton(int button, MouseButtonState mouseButtonState);
    static void FirstMove();
    static void SetupCallbacks();
    static void MouseCallback(GLFWwindow *window, double xPos, double yPos);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
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
    static MouseButtonState GetMouseButtonState(int button);
    static bool GetMouseButtonHeld(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static bool IsJoystickPresent(int jid);
};
#endif
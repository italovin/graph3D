#ifndef INPUT_H
#define INPUT_H
#include <glm/glm.hpp>
#include "Window.hpp"
#include <map>
#include <iostream>

enum KeyState{
    KEY_HELD,
    KEY_DOWN,
    KEY_UP
};
class Input{
private:
    static glm::vec2 mousePosition;
    static float mouseXDelta;
    static float mouseYDelta;
    static bool firstMouseMove;
    static Window registeredWindow;
    static std::map<int, KeyState> monitoredKeys;
    static void MonitorKey(int key, KeyState keyState);
    static void FirstMove();
    static void SetupCallbacks();
public:
    static void RegisterWindow(const Window &window);
    static void MouseCallback(GLFWwindow *window, double xPos, double yPos);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void Update(const Window &window);
    static glm::vec2 GetMousePosition();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();
    static bool GetKeyHeld(int key);
    static bool GetKeyDown(int key);
};
#endif
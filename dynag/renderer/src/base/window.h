#ifndef BASE_WINDOW
#define BASE_WINDOW

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "model.h"
#include "camera.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include <iostream>

#include <chrono>
#include <thread>
#include <vector>
#include <variant>
#include <memory>

// Create basic structure for ImGui text. With this structure
// text gui has only one values for a text. 
struct guiText
{
    char* str;
    float* val;
    guiText() { str = 0; val = 0; }
    guiText(char* _str, float* _val) { str = _str; val = _val; }
};

// Gui can have multiple text section by creating generic text section struct.
struct guiTextSection
{
    std::vector<guiText>* vText;
    std::string sTitle;
    ImVec2 vPosition;
    ImVec2 vSize;
};

class WindowBase
{
protected:
    // Window properties and its variables.
    float fScrWidth = 0.0f;
    float fScrHeight = 0.0f;
    std::chrono::duration<long, std::nano> durDeltaTime = std::chrono::nanoseconds(0);
    std::chrono::steady_clock::time_point tpLastFrame = std::chrono::steady_clock::now();
    float fLastX = 0.0f;
    float fXOffSet = 0.0f;
    float fLastY = 0.0f;
    float fYOffSet = 0.0f;
    bool bFirstMouse = true;
    glm::vec4 vLightPosition = glm::vec4(0.0f);

    // Drawable objects vectors. It stores object's pointer to call when they 
        // need to draw. 
    std::vector<ModelBase*> vPermanentDrawables;
    std::vector<ModelBase*> vInstantaneousDrawables;

    // GuiText temporary vector for gui render.
    std::vector<guiText> vGuiText;

    // Drawing of window which called by render function.
    virtual void draw() {};

    // Sleep the window for sync the FPS of window when the FPS is higher
    // than dynamic system FPS (which calculated in Python side).
    void preciseSleep(double seconds);

    GLFWwindow* getGLFWwindow() const;

    // Projection view for vertex-shaders for each shader.
    glm::mat4 mProjectionView = glm::mat4(1.0f);

public:
    // Base window if GLFW.
    GLFWwindow* wWindow = nullptr;

    // Camera class. 
    Camera* cmCamera = nullptr;

    // FPS variables and dt which is in nanoseconds.
    float fFPS = (float)1e-7;
    float fFPSLimit = 100.0;
    std::chrono::nanoseconds secDt{ static_cast<long int>(1000000000.0f / fFPSLimit) };

    // Text vector pointer vector which stores different 
    // guiTextSection pointer.
    std::vector<guiTextSection*> vGuiTextSection;

    // Basic Window constructor.
    WindowBase() {};
    virtual ~WindowBase();

    WindowBase(const WindowBase&) = delete;
    WindowBase& operator=(const WindowBase&) = delete;

    bool shouldClose();

    // Render the Window.
    virtual void render() {};

    // Render the Dear ImGui.
    virtual void renderGUI() {};

    // Create empty guiText vector and its adress.
    int createGuiText(const char* title, ImVec2 position, ImVec2 size);

    // Adding str and val to related guiText vector such as guiOBS.
    void addItem2GuiText(int iGuiTextInd, const char* str, float* val);

    void setGuiTextLineValue(int iGuiTextInd, int iLineNumber, float* fValue);

    void setGuiTextLine(int iGuiTextInd, int iLineNumber, char* cText);

    // Callbacks.

    // When window size changed, GLFW call this callback function.
    virtual void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {};

    // When mouse moves, GLFW call this call function.
    virtual void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    // When mouse scroll changed (wheel rotate etc.), GLFW call this function.
    virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticFrameBufferSizeCallback(GLFWwindow* window, int width, int height);

    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);

    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


    // Adding model as permanent drawable objects of the Window.
    // It drawing the model until the window closed.
    void addPermanentDrawables(ModelBase* drawable);

    // Adding model as instantaneous drawable objects of the Window.
    // It drawing the model only at that render time.
    void addInstantaneousDrawables(ModelBase* drawable);

};
#endif 
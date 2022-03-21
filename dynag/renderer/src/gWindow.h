/*
* DynaG Renderer C++ / Windowing
*
* Window class for handling window implementation based on https://learnopengl.com
* It is using GLFW for windowing operations.
* 
* Author : @MGokcayK
*
* C.Date : 04/05/2021
* Update : 04/05/2021 - Create & Implementation // @MGokcayK
*/

#ifndef GWINDOW_H
#define GWINDOW_H

#include "camera.h"
#include "model.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include <chrono>
#include <thread>

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

// Class for GLFW-Window.
class Window
{
private:
    // Window properties and its variables.
    float fScrWidth;
    float fScrHeight;
    std::chrono::duration<long, std::nano> durDeltaTime;
    std::chrono::steady_clock::time_point tpLastFrame = std::chrono::steady_clock::now();
    float fLastX = 0.0f;
    float fXOffSet = 0.0f;
    float fLastY = 0.0f;
    float fYOffSet = 0.0f;
    bool bFirstMouse = true;
    unsigned int iUbo; // uniform buffer objects for UBObjects
    unsigned int iLight; // uniform buffer objects for LightBlocks
    unsigned int iFog; // uniform buffer objects for FogBlocks
    unsigned int iDepthMapFbo; // depth frame buffer for shadow
    unsigned int iDepthMap; // depth texture
    glm::vec4 vLightPosition;


    // Drawable objects vectors. It stores object's pointer to call when they 
    // need to draw. 
    std::vector<Model*> vPermanentDrawables;
	std::vector<Model*> vInstantaneousDrawables;

    // GuiText temporary vector for gui render.
    std::vector<guiText> vGuiText;

    // Drawing of window which called by render function.
    void draw();

    // Sleep the window for sync the FPS of window when the FPS is higher
    // than dynamic system FPS (which calculated in Python side).
    void preciseSleep(double seconds);

    // Projection view for vertex-shaders for each shader.
    glm::mat4 mProjectionView = glm::mat4(1.0f);

public:
    // Base window if GLFW.
    GLFWwindow* wWindow;

    // Camera class. 
    Camera* cmCamera;

    // FPS variables and dt which is in nanoseconds.
    float fFPS = (float)1e-7;
    float fFPSLimit = 100.0; 
    std::chrono::nanoseconds secDt{static_cast<long int>( 1000000000.0f/fFPSLimit)};

    // Text vector pointer vector which stores different 
    // guiTextSection pointer.
    std::vector<guiTextSection*> vGuiTextSection;

    // Basic Window constructor.
    Window() {};

    // Window constructor with Width, Height and Title parameters.
    Window(const unsigned int SCR_WIDTH,
           const unsigned int SCR_HEIGHT,
           const char* title);
    
    // Render the Window.
    void render();

    // Render the Dear ImGui.
    void renderGUI();

    // Create empty guiText vector and its adress.
    int createGuiText(const char* title, ImVec2 position, ImVec2 size);

    // Adding str and val to related guiText vector such as guiOBS.
    void addItem2GuiText(int iGuiTextInd, const char* str, float* val);

    void setGuiTextLineValue(int iGuiTextInd, int iLineNumber, float* fValue);

    void setGuiTextLine(int iGuiTextInd, int iLineNumber, char* cText);

    // Adding model as permanent drawable objects of the Window.
    // It drawing the model until the window closed.
    void addPermanentDrawables(Model* drawable);

    // Adding model as instantaneous drawable objects of the Window.
    // It drawing the model only at that render time.
	void addInstantaneousDrawables(Model* drawable);

    // Callbacks for OpenGL.

    // When window size changed, GLFW call this callback function.
    void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
    
    // When mouse moves, GLFW call this call function.
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    // When mouse scroll changed (wheel rotate etc.), GLFW call this function.
    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticFrameBufferSizeCallback(GLFWwindow* window, int width, int height);

    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    // GLFW need static callback functions. To handle it, some capsulation should
    // be done. This function makes it.
    static void staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        
};
#endif
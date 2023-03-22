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

#ifndef GL_WINDOW
#define GL_WINDOW

#include "base/window.h"
#include "gl_model.h"

#include "imgui/imgui_impl_opengl3.h"

namespace OpenGL
{

    // Class for GLFW-Window.
    class Window : public WindowBase
    {
    private:
        unsigned int iUbo = 0; // uniform buffer objects for UBObjects
        unsigned int iLight = 0; // uniform buffer objects for LightBlocks
        unsigned int iFog = 0; // uniform buffer objects for FogBlocks
        unsigned int iDepthMapFbo = 0; // depth frame buffer for shadow
        unsigned int iDepthMap = 0; // depth texture

        virtual void draw();

    public:
        // Window constructor with Width, Height and Title parameters.
        Window(const unsigned int _width,
            const unsigned int _height,
            const char* _title);

        // Render the Window.
        virtual void render();

        // Render the Dear ImGui.
        virtual void renderGUI();        

        // When window size changed, GLFW call this callback function.
        virtual void frameBufferSizeCallback(GLFWwindow* window, int width, int height);

        // When mouse moves, GLFW call this call function.
        virtual void mouseCallback(GLFWwindow* window, double xpos, double ypos);

        // When mouse scroll changed (wheel rotate etc.), GLFW call this function.
        virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    };
    
} // end of OpenGL namespace
#endif
/*
* DynaG Renderer C++ / Render API
*
* Render API create a shared libs for compiled OS to use 
* Rendering C++ codes in python.
* 
* Author : @MGokcayK
*
* C.Date : 04/05/2021
* Update : 04/05/2021 - Initialization & Implementation // @MGokcayK
* Update : 05/01/2022 - Adding new guiText methods // @MGokcayK
*/

#ifdef _WIN32
	#ifdef DynaG_renderer_EXPORTS
		#define RENDERER_API __declspec(dllexport)
	#else
		#define RENDERER_API __declspec(dllimport)
	#endif
	#define strdup _strdup
#elif linux
	#define RENDERER_API 
#endif

#include "gl/gl_app.h"
#include "vk/vk_app.h"

// RENDERER API for creating shared libraries to call methods from Python.
extern "C" 
{
// Creating application.
RENDERER_API ApplicationBase * createApplication(const unsigned int _width,
												  const unsigned int _height,
												  const char* _title,
												  const bool _vulkan);

// Close application.
RENDERER_API void close(ApplicationBase* app);

// Check the window is closed or not.
RENDERER_API bool isClose(ApplicationBase* app);

// Render the application.
RENDERER_API void render(ApplicationBase* app);

// Terminate the application.
RENDERER_API void terminateApplication();

// Create model with its shaders.
RENDERER_API ModelBase * createModel(ApplicationBase* app, char* model_path, char* vertex_shader_file_path, char* fragment_shader_file_path);

// Add model as permanent drawables object to application.
RENDERER_API void addPermanent2App(ApplicationBase* app, ModelBase * model);

// Add model as instantaneous drawables object to application.
RENDERER_API void addInstantaneous2App(ApplicationBase* app, ModelBase * model);

// Translate model to locations. Locations will be in OpenGL/Vulkan coordinates.
RENDERER_API void translateModel(ModelBase * model, float x, float y, float z);

// Rotate model to angle. From will be an rotation around arbitrary angle.
RENDERER_API void rotateModel(ModelBase * model, float angle, float x, float y, float z);

// Scale model to the ratios for each axis.
RENDERER_API void scaleModel(ModelBase * model, float x, float y, float z);

// Get FPS from the application.
RENDERER_API float getFps(ApplicationBase * app);

// Set FPS of the application.
RENDERER_API void setFps(ApplicationBase * app, float fps);

// Get Camera pointer of the application.
RENDERER_API Camera * getCamera(ApplicationBase * app);

// Set Camera position of each axis.
RENDERER_API void setCameraPos(Camera * camera, float x, float y, float z);

// Get camera position as float array.
RENDERER_API float* getCameraPos(Camera * camera);

// Check whether the window is visible or not.
RENDERER_API bool isVisible(ApplicationBase * app);

// Hide the window.
RENDERER_API void hideWindow(ApplicationBase * app);

// Show the window.
RENDERER_API void showWindow(ApplicationBase * app);

// Create guiText vector.
RENDERER_API int createGuiTextVector(ApplicationBase * app, const char* title,
													float pos_x, float pos_y,
													float size_x, float size_y);

// Add the str and val to guiText vector.
RENDERER_API void addGuiTextAllLines(ApplicationBase * app, int v_guiText_ind, int size, char** _str, float* _val);

RENDERER_API void addGuiTextLine(ApplicationBase * app, int v_guiText_ind, char* _text, float* _val);

// Set the guiText of vector.
RENDERER_API void setGuiTextAllValues(ApplicationBase * app, int v_guiText_ind, int size, float* _val);

RENDERER_API void setGuiTextAllTextLines(ApplicationBase * app, int v_guiText_ind, int size, char** _text);

RENDERER_API void setGuiTextLineValue(ApplicationBase * app, int v_guiText_ind, int iLineNumber, float* _val);

RENDERER_API void setGuiTextLine(ApplicationBase * app, int v_guiText_ind, int iLineNumber, char* _text);

// Set uniforms of OpenGL.
RENDERER_API void setBool(OpenGL::Model * model, char* name, bool value);

RENDERER_API void setInt(OpenGL::Model * model, char* name, int value);

RENDERER_API void setFloat(OpenGL::Model * model, char* name, float value);

RENDERER_API void setVec2(OpenGL::Model * model, char* name, float* value);

RENDERER_API void setVec3(OpenGL::Model * model, char* name, float* value);

RENDERER_API void setVec4(OpenGL::Model * model, char* name, float* value);

RENDERER_API void setMat2(OpenGL::Model * model, char* name, float* value);

RENDERER_API void setMat3(OpenGL::Model * model, char* name, float* value);

RENDERER_API void setMat4(OpenGL::Model * model, char* name, float* value);
}
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

#include "opengl/gl_window.h"

// RENDERER API for creating shared libraries to call methods from Python.

// Creating window.
extern "C" RENDERER_API WindowBase* createWindow(const unsigned int WIDTH,
										      const unsigned int HEIGHT,
										      const char* title);

// Close window.
extern "C" RENDERER_API void close(WindowBase* window);

// Check the window is closed or not.
extern "C" RENDERER_API bool isClose(WindowBase* window);

// Render the window.
extern "C" RENDERER_API void render(WindowBase* window);

// Terminate the OpenGL.
extern "C" RENDERER_API void terminateWindow();

// Create model with its shaders.
extern "C" RENDERER_API ModelBase* createModel(char* model_path, char* vertex_shader_file_path, char* fragment_shader_file_path);

// Add model as permanent drawables object to window.
extern "C" RENDERER_API void addPermanent2Window(OpenGL::Window* window, ModelBase* model);

// Add model as instantaneous drawables object to window.
extern "C" RENDERER_API void addInstantaneous2Window(OpenGL::Window* window, ModelBase* model);

// Translate model to locations. Locations will be in OpenGL coordinates.
extern "C" RENDERER_API void translateModel(ModelBase * model, float x, float y, float z);

// Rotate model to angle. From will be an rotation around arbitrary angle.
extern "C" RENDERER_API void rotateModel(ModelBase * model, float angle, float x, float y, float z);

// Scale model to the ratios for each axis.
extern "C" RENDERER_API void scaleModel(ModelBase * model, float x, float y, float z);

// Get FPS from the window.
extern "C" RENDERER_API float getFps(WindowBase* window);

// Set FPS of the window.
extern "C" RENDERER_API void setFps(WindowBase* window, float fps);

// Get Camera pointer of the window.
extern "C" RENDERER_API Camera* getCamera(WindowBase* window);

// Set Camera position of each axis.
extern "C" RENDERER_API void setCameraPos(Camera* camera, float x, float y, float z);

// Get camera position as float array.
extern "C" RENDERER_API float* getCameraPos(Camera* camera);

// Check whether the window is visible or not.
extern "C" RENDERER_API bool isVisible(WindowBase* window);

// Hide the window.
extern "C" RENDERER_API void hideWindow(WindowBase* window);

// Show the window.
extern "C" RENDERER_API void showWindow(WindowBase* window);

// Create guiText vector.
extern "C" RENDERER_API int createGuiTextVector(WindowBase * window, const char* title,
													float pos_x, float pos_y,
													float size_x, float size_y);

// Add the str and val to guiText vector.
extern "C" RENDERER_API void addGuiTextAllLines(WindowBase* window, int v_guiText_ind, int size, char** _str, float* _val);

extern "C" RENDERER_API void addGuiTextLine(WindowBase * window, int v_guiText_ind, char* _text, float* _val);

// Set the guiText of vector.
extern "C" RENDERER_API void setGuiTextAllValues(WindowBase* window, int v_guiText_ind, int size, float* _val);

extern "C" RENDERER_API void setGuiTextAllTextLines(WindowBase * window, int v_guiText_ind, int size, char** _text);

extern "C" RENDERER_API void setGuiTextLineValue(WindowBase * window, int v_guiText_ind, int iLineNumber, float* _val);

extern "C" RENDERER_API void setGuiTextLine(WindowBase * window, int v_guiText_ind, int iLineNumber, char* _text);

extern "C" RENDERER_API void setBool(OpenGL::Model * model, char* name, bool value);

extern "C" RENDERER_API void setInt(OpenGL::Model * model, char* name, int value);

extern "C" RENDERER_API void setFloat(OpenGL::Model * model, char* name, float value);

extern "C" RENDERER_API void setVec2(OpenGL::Model * model, char* name, float* value);

extern "C" RENDERER_API void setVec3(OpenGL::Model * model, char* name, float* value);

extern "C" RENDERER_API void setVec4(OpenGL::Model * model, char* name, float* value);

extern "C" RENDERER_API void setMat2(OpenGL::Model * model, char* name, float* value);

extern "C" RENDERER_API void setMat3(OpenGL::Model * model, char* name, float* value);

extern "C" RENDERER_API void setMat4(OpenGL::Model * model, char* name, float* value);
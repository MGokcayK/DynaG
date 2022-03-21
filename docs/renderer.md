# **DynaG-renderer**
The API is used for rendering. The API uses [OpenGL](https://www.opengl.org) as a graphics library, [Assimp](https://www.assimp.org) as an asset importer, [GLFW](https://www.glfw.org) as a tool for creating windows, contexts and surfaces, receiving input and events, [GLAD](https://github.com/Dav1dde/glad) as a loader for OpenGL, [glm](https://github.com/g-truc/glm) as a mathematics library for OpenGL, [ImGUI](https://github.com/ocornut/imgui) as a GUI manager and [stb](https://github.com/nothings/stb) as a image loader. glm, ImGUI and stb are header only libraries and they do not need compile. On the other hand rest of the libraries need to compile and compilation details of the DynaG-renderer can be found in [build](renderer_build.md) page.

CMake is used for building the API. If end user want to add new features into the API, new files (.h, .cpp etc.) should be in the `src` folder in `renderer` folder. 

<br/>

---
<br/>


## **Details**
All implementation based on the most pure OpenGL [tutorial](https://learnopengl.com). The codes taken from the examples and modified around purpose of the API. The API is separeted from DynaG-dynamics; therefore, it can be usefull for any other applications as well. 

Firstly, the API focus on rendering 3D objects; and for now, the API does not support any simple rendering such as cube, sphere, line or cirle. It will be added in the future. 

The API does not support shadows, waters etc. Main focus of the API is rendering 3D objects properly. Therefore, complicated task such as shadowing is not implemented. 

<br/>

### **Shader**
In *Shader* class, vertice and fragment shaders are used for shadering. In `resources` folder, default models and shaders can be found. If user want to write custom shader, it can be used. 

The API support uniform modification from Python. With the support, some small effects can be done in shader.


<br/>

### **GUI**
GUI created with ImGUI and only text rendering is enabled by default. The text rendering support only one numerical value on one line. To render text, user need to create *GuiText* and add lines to it. After that, lines can be setted dynamically. 

<br/>

---
<br/>

## **API Methods**
The API methods can be defined in `ren_api.h`. These methods created in `.dll` file or `.so` file which depends on OS and loaded in Python side. 

<br/>

### **rendererAPI.py**
`rendererAPI.py` is a file that contains implementation of Python side of API. With this file, Python and C++ can connect each other with `ctypes` module of Python. 

<br/>

### **renderer_impl.py**
`renderer_impl.py` contains *Renderer* class which manages rendering. It uses methods from `rendererAPI.py`. The reason behind the approach is managing rendering in Python easily. It does not required. Yet, it makes life easy.

<br/>

#### ***Coordinate System Difference***
*Renderer* class take care of coordinate system difference. When translate the model in *Renderer*, it converts from NED(north-east-down) to OpenGL frame. This is important and tricky point. 
# **DynaG-dynamics**
To calculate dynamcis of DynaG, DynaG-dynamics API is used. DynaG-dynamics written in C++ and connect with Python with shared libraries DLL (for Windows) and so (for Linux) as a part of API. By using Python's `ctypes` module, API can be used by loading shared libraries.

If any C++ files changed somehow (adding new methods etc.) API should be re-compiled. To build and compile the API, [CMake](https://cmake.org) is used. 

<br/>

---

<br/>

## **Build CMake for Windows OS**
In Windows, [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/) is suggested compiler to build DLL file. To use MSVC for building API in Windows, there is two way. 

<br/>

---

_**First**_ way is using CMake GUI. In GUI firstly set source code folder which is `...path_to_DynaG_folder.../dynag/dynamics`. After that set build folder as `...path_to_DynaG_folder.../dynag/dynamics/build`. If there is no `build` folder, GUI pop up an dialog box to warn you and accept it. After setting the folder, click the `Configure` button. Select the MSVC version and press `Finish`. If there is no error (which written in RED color (warnings are also in RED))which means `Configuring done` message appear at the end of gui, click the `Generate` button. Click the `Open Project` button to open MSVC solution. Lastly, compile the MSVC solution and make sure that `config` in MSVC is `Release`.

<br/>

---

_**Second**_ way is using cmd. Firstly call the 
```bash
cd ...path_to_DynaG_folder.../dynag/dynamics
```
command. Create the `build` folder and change the directory to folder, call the 
```bash
mkdir build && cd build
```
command. Then, to build (like `Configure` and `Generate` in GUI), call the 
```bash
cmake ..
```
command. To compile in `Release` config, call the 
```bash
cmake --build . --config Release
```
command. 

These two way creates the `DynaG-dynamics.dll` file which is shared libraries for Python and C++ linkage. The file should be in `...path_to_DynaG_folder.../dynag/dynamics/bin` folder. Even it setted to output to the `../bin` folder, make sure that your last compiled DLL file should be in the folder.

<br/>

---
<br/>

## **Build CMake for Linux**
In Linux, [GCC](https://gcc.gnu.org) is suggested compiler to build so file. To use GCC for building API in Linux, there is two way. 

<br/>

---

_**First**_ way is using CMake GUI. In GUI firstly set source code folder which is `...path_to_DynaG_folder.../dynag/dynamics`. After that set build folder as `...path_to_DynaG_folder.../dynag/dynamics/build`. If there is no `build` folder, GUI pop up an dialog box to warn you and accept it. After setting the folder, click the `Configure` button. Select the `Unix Makefiles` as generator and click the `Finish`. If there is no error (which written in RED color (warnings are also in RED)) which means `Configuring done` message appear at the end of gui, click the `Generate` button. Open the terminal, call the 

```bash
cd ...path_to_DynaG_folder.../dynag/dynamics/build
```
command to be in the build folder. Then to compile, call the 
```bash
make
```
command. Build type is assigned as `Release`. Therefore just `make` commant will work. 

<br/>

---

_**Second**_ way is using terminal. Firstly call the 
```bash
cd ...path_to_DynaG_folder.../dynag/dynamics
```
command. Create the `build` folder and change the directory to folder, call the 
```bash
mkdir build && cd build
```
command. Then, to build (like `Configure` and `Generate` in GUI), call the 
```bash
cmake -G "Unix Makefiles" .. #for the Unix Makefiles generator
```
command. Build type is assigned as `Release`. Yet, if you want to make sure, call the 
```bash
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
```
command. To compile, call the 
```bash
make
```
command. 

These two way creates the `libDynaG-dynamics.so` file which is shared libraries for Python and C++ linkage. The file should be in `...path_to_DynaG_folder.../dynag/dynamics/bin` folder. Even it setted to output to the `../bin` folder, make sure that your last compiled so file should be in the folder.

---
---

## **Dependent Libraries**
There is several libraries for dynamics API. Some of them should be in shared libraries. Therefore, in some OS, these libraries should be re-compiled. [yaml-cpp](https://github.com/jbeder/yaml-cpp) which used for loading yaml file should be re-compiled. After compilation, `yaml-cpp.dll` and `yaml-cpp.lib` (for Windows) or `libyaml-cpp.so` (for Linux) should be in `...path_to_DynaG_folder.../dynag/dynamics/libs` folder.

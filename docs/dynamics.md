# **DynaG-dynamics**
The API is used for calculating dynamics of any dynamic system which can be represented with ODE (Ordinary Differential Equations). The API uses [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) as a numerical calculations, [yaml-cpp](https://github.com/jbeder/yaml-cpp) as a yaml loader and [stb](https://github.com/nothings/stb) as a image loader. Eigen and stb are header only libraries and they do not need compile. On the other hand yaml-cpp need to compile and compilation details of the DynaG-dynamics can be found in [build](dynamics_build.md) page.

CMake is used for building the API. If end user want to add new features into the API, new files (.h, .cpp etc.) should be in the `src` folder in `dynamics` folder. 

<br/>

---
<br/>


## **DynamicSystem Class**
The API based on `DynamicSystem` class. It is base class and the class has lots of methods for calculating states of system. Core methods implemented in base class and other methods setted as virtual methods. Most of the virtual methods do not have to overrided except the pure virtual methods. `dynamics`, `reset` and `getNumberOfObservations` methods setted as pure virtual methods; therefore, they should be overrided. 

DynamicSystem class calculate a step of dynamic system with `step` method and $4^{th}$ order Runge-Kutta (RK4) implementation is done in the method. 

If user want to define new dynamic system, child class should set action size (with `setActionSize` method) and register some states into VectorSpace.

<br/>

### **VectorSpace**
`VectorSpace` (VS) is a special struct for RK4 implementations. Base *DynamicsSystem* class lots of the VS structure such as *vsState* for holding states, *vsObservation* for holding observations and *vsAction* for holding action. VS has its own operators to calculate RK4 properly. 

State of *vsState*, *vsObservation* and *vsAction* should be registered. These can be done by using `registerState`, `registerObservation` and `registerAction` methods. These methods defined in *DynamicSystem* class. Tracking the state of VS can be hard; therefore, each state has it own name which defined by user. `registerVector` method in VS is used for registering state. It means that state is a subvector of VS. `registerState`, `registerObservation` and `registerAction` used `registerVector` for registering.

VS has vectors to hold some values such as current value (vValues), normalized values (vNormalizedValues) and normalizer (vNormalizer). Normalized values calculated by dividing the current value with normalizer. Normalized values can be used in reinforcement learning models. During the registiration, normalizer values can be defined explicitly. If not, 1 is taken as normalizer for the state. 

Normalized value of state also be important. To make sure that the state of the system is not diverged or get NaN values, divergence control done in VS with `isDiverged` method. The method checks whether normalized value of any state in *vsState* is bigger than `NORM_LIMIT` which default equal to 20. If one of the state is diverged, system generate `READY` signal as false and it creates a `sim_failure` signal in Python side which reset the environment. 

<br/>

### **YAML**

Yaml files can be used as a parameter holder. With yaml-cpp, files can be readed by C++. If yaml is used, `node_map` can be used as a yaml-node holder to read and write node values from Python with related API method. 

<br/>

---
<br/>

## **API Methods**
The API methods can be defined in `dyn_api.h`. These methods created in `.dll` file or `.so` file which depends on OS and loaded in Python side. 

<br/>

### **dynamicsAPI.py**
`dynamicsAPI.py` is a file that contains implementation of Python side of API. With this file, Python and C++ can connect each other with `ctypes` module of Python. 

<br/>

### **dynamics_impl.py**
`dynamics_impl.py` contains class representation of dynamic system in Python. It uses methods from `dynamicsAPI.py`. The reason behind the approach is managing dynamic systems in Python easily. It does not required. Yet, it makes life easy.
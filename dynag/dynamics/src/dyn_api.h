/*
* DynaG Dynamics C++ / Dynamics API
*
* Dynamics API create a shared libs for compiled OS to use 
* Dynamics C++ codes in python.
* 
* Author : @MGokcayK
*
* C.Date : 21/09/2021
* Update : 21/09/2021 - Initialization & Implementation // @MGokcayK
* Update : 06/11/2021 - Adding new methods for api. // @MGokcayK
* Update : 22/12/2021 - Adding getAllStates and getAllStateDots methods. // @MGokcayK
*/

#pragma once
#ifdef _WIN32
	#ifdef DynaG_dynamics_EXPORTS
		#define DYNAMICS_API __declspec(dllexport)
	#else
		#define DYNAMICS_API __declspec(dllimport)
	#endif
#elif linux
	#define DYNAMICS_API 
#endif

#include "helicopter/helicopter_dynamics.h"

// DYNAMICS API for creating shared libraries to call methods from Python.

// Creating Helicopter Dynamics.
extern "C" DYNAMICS_API HelicopterDynamics* createHelicopterDynamics(char* yaml_path, float* dt);

// return is system is ready to simulation
extern "C" DYNAMICS_API bool ready(); 

// Get all action of Dynamic System
extern "C" DYNAMICS_API float* getAllAction(DynamicSystem * DS, int* size_s);

// Get all normalized action of Dynamic System
extern "C" DYNAMICS_API float* getAllNormalizedAction(DynamicSystem * DS, int* size_s);

// Get action of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getAction(DynamicSystem* DS, char* name, int* size_s);

// Get normalized action of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getNormalizedAction(DynamicSystem* DS, char* name, int* size_s);

// Get all state of Dynamic System
extern "C" DYNAMICS_API float* getAllState(DynamicSystem * DS, int* size_s);

// Get all normalized state of Dynamic System
extern "C" DYNAMICS_API float* getAllNormalizedState(DynamicSystem * DS, int* size_s);

// Get state of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getState(DynamicSystem* DS, char* name, int* size_s);

// Get normalized state of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getNormalizedState(DynamicSystem* DS, char* name, int* size_s);

// Get all statedots of Dynamic System
extern "C" DYNAMICS_API float* getAllStateDot(DynamicSystem * DS, int* size_s);

// Get all normalized statedots of Dynamic System
extern "C" DYNAMICS_API float* getAllNormalizedStateDot(DynamicSystem * DS, int* size_s);

// Get state dots of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getStateDot(DynamicSystem* DS, char* name, int* size_s);

// Get normalized state of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getNormalizedState(DynamicSystem * DS, char* name, int* size_s);

// Get normalized state dots of Dynamics System w.r.t its name
extern "C" DYNAMICS_API float* getNormalizedStateDot(DynamicSystem * DS, char* name, int* size_s);

// Get all observation of Dynamic System
extern "C" DYNAMICS_API float* getAllObservation(DynamicSystem* DS, int* size_s);

// Get all normalized observation of Dynamic System
extern "C" DYNAMICS_API float* getAllNormalizedObservation(DynamicSystem* DS, int* size_s);

// Get observation of Dynamic System
extern "C" DYNAMICS_API float* getObservation(DynamicSystem* DS, char* name, int* size_s);

// Get normalized observation of Dynamic System
extern "C" DYNAMICS_API float* getNormalizedObservation(DynamicSystem * DS, char* name, int* size_s);

// Getting value of yaml node from corresponding dynamics system w.r.t node name and variable name.
extern "C" DYNAMICS_API float getValueFromYamlNode(DynamicSystem* DS, char* node_name, char* var_name);

// Setting value of yaml node from corresponding dynamics system w.r.t node name and variable name.
extern "C" DYNAMICS_API void setValueInYamlNode(DynamicSystem * DS, char* node_name, char* var_name, float* value);

// Step Dynamic System
extern "C" DYNAMICS_API void step(DynamicSystem * DS, float* action);

// Reset Dynamic System
extern "C" DYNAMICS_API void reset(DynamicSystem* DS);

// Get number of obs
extern "C" DYNAMICS_API int getNumberOfObservations(DynamicSystem* DS);

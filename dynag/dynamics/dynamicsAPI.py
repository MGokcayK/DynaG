#
# DynaG Dynamics Python / Dynamics API
# 
# Dynamics API create connection between Python and C++
# for dynamics. 
#
# Author : @MGokcayK
# Author : @ugurcanozalp (uurcann94@gmail.com)
# C.Date : 21/09/2021
# Update : 21/09/2021 - Initialization & Implementation // @MGokcayK
#
# Last update information of API can find from `src/dyn_api.h`


import os
import sys
import ctypes
import numpy as np

"""
    Load shadered library w.r.t OS.
"""

if sys.platform == 'win32':
    lib = ctypes.cdll.LoadLibrary(os.environ['DYNAG_DYN_PYTHON_DIR'] + '/DynaG-dynamics.dll')
elif sys.platform == 'linux':
    lib = ctypes.cdll.LoadLibrary(os.environ['DYNAG_DYN_PYTHON_DIR'] + '/libDynaG-dynamics.so')

c_float_p = ctypes.POINTER(ctypes.c_float)
c_int_p = ctypes.POINTER(ctypes.c_int)

###################################################################################
def _to_encode(str_to_encode, encode_type = 'utf-8'):
    """
        Encoding the string into `encode_type` for char pointer of ctypes.
    """
    return str_to_encode.encode(encode_type)

###################################################################################
lib.ready.argtypes = []
lib.ready.restype = ctypes.c_bool

def ready():
    """
        Getting if system is ready to simulation.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the status for simulation
    """
    return lib.ready()

###################################################################################
lib.createHelicopterDynamics.argtypes = [ctypes.c_char_p, c_float_p]
lib.createHelicopterDynamics.restype = ctypes.c_void_p

def createHelicopterDynamics(yaml_path, dt):
    """
        Creating Helicopter Dynamics. 
        >>> yaml_path : Path of yaml file which stores helicopter & wind parameters.
        >>> dt        : Step size/time of helicopter calculations which calculated by 1/FPS.

        It returns the address of created HelicopterDynamics object.
    """
    return lib.createHelicopterDynamics(ctypes.c_char_p(_to_encode(yaml_path)), ctypes.c_float(dt) )

###################################################################################
lib.getAllAction.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllAction.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllAction(DynamicSystem):
    """
        Getting all action of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the action of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    action = lib.getAllAction(DynamicSystem, s)
    return np.copy(action[:s[0]])

###################################################################################
lib.getAllNormalizedAction.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllNormalizedAction.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllNormalizedAction(DynamicSystem):
    """
        Getting all normalized action of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the action of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    action = lib.getAllNormalizedAction(DynamicSystem, s)
    return np.copy(action[:s[0]])

###################################################################################
lib.getAction.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getAction.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getAction(DynamicSystem, name):
    """
        Getting dynamic action of DynamicsSystem w.r.t action's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic action.

        It returns the dynamic action vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    action = lib.getAction(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(action[:s[0]])

###################################################################################
lib.getNormalizedAction.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getNormalizedAction.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getNormalizedAction(DynamicSystem, name):
    """
        Getting dynamic action of DynamicsSystem w.r.t action's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic action.

        It returns the dynamic action vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    action = lib.getNormalizedAction(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(action[:s[0]])

###################################################################################
lib.getAllState.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllState.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllState(DynamicSystem):
    """
        Getting all state of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the state of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getAllState(DynamicSystem, s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getAllNormalizedState.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllNormalizedState.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllNormalizedState(DynamicSystem):
    """
        Getting all state of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the state of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getAllNormalizedState(DynamicSystem, s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getState.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getState.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getState(DynamicSystem, name):
    """
        Getting dynamic state of DynamicsSystem w.r.t state's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic state.

        It returns the dynamic state vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getState(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getNormalizedState.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getNormalizedState.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getNormalizedState(DynamicSystem, name):
    """
        Getting normalized dynamic state of DynamicsSystem w.r.t state's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic state.

        It returns the normalized dynamic state vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getNormalizedState(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getAllStateDot.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllStateDot.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllStateDot(DynamicSystem):
    """
        Getting all statedots of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the statedots of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getAllStateDot(DynamicSystem, s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getAllNormalizedStateDot.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllNormalizedStateDot.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllNormalizedStateDot(DynamicSystem):
    """
        Getting all statedots of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the statedots of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    state = lib.getAllNormalizedStateDot(DynamicSystem, s)
    return np.copy(state[:s[0]])

###################################################################################
lib.getStateDot.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getStateDot.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getStateDot(DynamicSystem, name):
    """
        Getting dynamic state derivative of DynamicsSystem w.r.t state's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic state derivative.

        It returns the dynamic state derivative vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    statedot = lib.getStateDot(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(statedot[:s[0]])

###################################################################################
lib.getNormalizedStateDot.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getNormalizedStateDot.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(10), flags='C_CONTIGUOUS')

def getNormalizedStateDot(DynamicSystem, name):
    """
        Getting normalized dynamic state derivative of DynamicsSystem w.r.t state's name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of dynamic state derivative.

        It returns the normalized dynamic state derivative vector as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    statedot = lib.getNormalizedStateDot(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(statedot[:s[0]])

###################################################################################
lib.getAllObservation.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllObservation.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllObservation(DynamicSystem):
    """
        Getting observation of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the observation of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    obs = lib.getAllObservation(DynamicSystem, s)
    return np.copy(obs[:s[0]])

###################################################################################
lib.getAllNormalizedObservation.argtypes = [ctypes.c_void_p, c_int_p]
lib.getAllNormalizedObservation.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getAllNormalizedObservation(DynamicSystem):
    """
        Getting observation of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the observation of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    obs = lib.getAllNormalizedObservation(DynamicSystem, s)
    return np.copy(obs[:s[0]])

###################################################################################
lib.getObservation.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getObservation.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getObservation(DynamicSystem, name):
    """
        Getting observation of DynamicsSystem w.r.t its name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of observation.

        It returns the observation of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    obs = lib.getObservation(DynamicSystem,  ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(obs[:s[0]])

###################################################################################
lib.getNormalizedObservation.argtypes = [ctypes.c_void_p, ctypes.c_char_p, c_int_p]
lib.getNormalizedObservation.restype = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(100), flags='C_CONTIGUOUS')

def getNormalizedObservation(DynamicSystem, name):
    """
        Getting normalized observation of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> name          : Name of normalized observation.

        It returns the normalized observation of DynamicsSystem as numpy vector.
    """
    s = (ctypes.c_int * 1)(*range(1)) # size parameters
    obs = lib.getNormalizedObservation(DynamicSystem, ctypes.c_char_p(_to_encode(name)), s)
    return np.copy(obs[:s[0]])

###################################################################################
lib.getValueFromYamlNode.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
lib.getValueFromYamlNode.restype = ctypes.c_float

def getValueFromYamlNode(DynamicSystem, node_name, variable_name):
    """
        Getting value of yaml node from corresponding dynamics system w.r.t node 
        name and variable name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> node_name     : Name of yaml node.
        >>> variable_name : Name of variable of node_name.
    """
    return lib.getValueFromYamlNode(DynamicSystem, ctypes.c_char_p(_to_encode(node_name)), ctypes.c_char_p(_to_encode(variable_name)))

###################################################################################
lib.setValueInYamlNode.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, c_float_p]
lib.setValueInYamlNode.restype = ctypes.c_void_p

def setValueInYamlNode(DynamicSystem, node_name, variable_name, value):
    """
        Setting value of yaml node from corresponding dynamics system w.r.t node 
        name and variable name.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> node_name     : Name of yaml node.
        >>> variable_name : Name of variable of node_name.
        >>> value         : New value of variable.
    """
    lib.setValueInYamlNode(DynamicSystem, ctypes.c_char_p(_to_encode(node_name)), ctypes.c_char_p(_to_encode(variable_name)), ctypes.c_float(value))

###################################################################################
lib.step.argtypes = [ctypes.c_void_p, np.ctypeslib.ndpointer(dtype=np.float32, flags='C_CONTIGUOUS')]
lib.step.restype = ctypes.c_void_p

def step(DynamicSystem, action):
    """
        Calculating one step of DynamicsSytem w.r.t its action.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
        >>> action        : Action input of DynamicSystem.

        Note :
        ------
        For HelicopterDynamics, action will be Collective, Longitudinal Cyclic, Lateral Cyclic and
        pedal inputs.\n
    """
    action = np.array(action, dtype=np.float32)
    lib.step(DynamicSystem, action)

###################################################################################
#lib.reset.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), np.ctypeslib.ndpointer(dtype=np.float32, flags='C_CONTIGUOUS'), c_int_p, c_int_p]
lib.reset.argtypes = [ctypes.c_void_p]
lib.reset.restype = ctypes.c_void_p

def reset(DynamicSystem):
    """
        Resetting the DynamicsSytem w.r.t trim_cond.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.
    """
    lib.reset(DynamicSystem)

###################################################################################
lib.getNumberOfObservations.argtypes = [ctypes.c_void_p]
lib.getNumberOfObservations.restype = ctypes.c_int

def getNumberOfObservations(DynamicSystem):
    """
        Getting number of observation of DynamicsSystem.
        >>> DynamicSystem : Address of DynamicSystem object like HelicopterDynamics.

        It returns the number of observation of DynamicsSystem as integer.
    """
    return lib.getNumberOfObservations(DynamicSystem)

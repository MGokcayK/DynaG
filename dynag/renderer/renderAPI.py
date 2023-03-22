#
# DynaG Rendering Python / Render API
# 
# Render API create connection between Python and C++
# for rendering. 
#
# Author : @MGokcayK
# C.Date : 04/05/2021
# Update : 04/05/2021 - Initialization & Implementation // @MGokcayK
# Update : 05/01/2022 - Adding new guiText methods // @MGokcayK
#
# Last update information of API can find from `src/ren_api.h`

import os
import sys
import ctypes
import numpy as np

"""
    Load shadered library w.r.t OS.
"""
if sys.platform == 'win32':
    lib = ctypes.cdll.LoadLibrary(os.environ['DYNAG_PYTHON_DIR'] + '/DynaG-renderer.dll')
elif sys.platform == 'linux':
    lib = ctypes.cdll.LoadLibrary(os.environ['DYNAG_PYTHON_DIR'] + '/libDynaG-renderer.so')

###################################################################################
def _to_encode(str_to_encode, encode_type = 'utf-8'):
    """
        Encoding the string into `encode_type` for char pointer of ctypes.
    """
    return str_to_encode.encode(encode_type)

###################################################################################
lib.createApplication.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_bool]
lib.createApplication.restype = ctypes.c_void_p

def createApplication(w, h, title, vulkan = False):
    """
        Create application with parameters which are
        >>> w      : Width of window
        >>> h      : Height of window
        >>> title  : Title of window
        >>> vulkan : Create application with Vulkan API.
    """
    return lib.createApplication(w, h, ctypes.c_char_p(_to_encode(title)), vulkan)  

###################################################################################
lib.render.argtypes = [ctypes.c_void_p]
lib.render.restype = ctypes.c_void_p

def render(application):
    """
        Render application.
        >>> application : Address of application which will be render.
    """
    lib.render(application)

###################################################################################
lib.close.argtypes = [ctypes.c_void_p]
lib.close.restype = ctypes.c_void_p

def close(application):
    """
        Close application.
        >>> application : Address of application which will be closed.
    """
    lib.close(application)

###################################################################################
lib.isClose.argtypes = [ctypes.c_void_p]
lib.isClose.restype = ctypes.c_bool

def isClose(application):
    """
        Check whether application is closed or not.
        >>> application : Address of application which will be checked.
    """
    return lib.isClose(application)  

###################################################################################
lib.terminateApplication.argtypes = []
lib.terminateApplication.restype = ctypes.c_void_p

def terminate():
    """
        Terminate the application.
    """
    lib.terminateApplication()

###################################################################################
lib.createModel.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
lib.createModel.restype = ctypes.c_void_p

def createModel(application, object_path, vertex_shader_path, fragment_shader_path):
    """
        Create model with its shaders.
        >>> application          : Application which model will be added.
        >>> object_path          : Path of `.obj` file which is the 3D model of object.
        >>> vertex_shader_path   : Path of vertex shader of object. 
        >>> fragment_shader_path : Path of fragment shader of object.
    """
    return lib.createModel(application,
                            ctypes.c_char_p(_to_encode(object_path)),
                            ctypes.c_char_p(_to_encode(vertex_shader_path)),
                            ctypes.c_char_p(_to_encode(fragment_shader_path)))

###################################################################################
lib.addPermanent2App.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
lib.addPermanent2App.restypes = ctypes.c_void_p

def addPermanent2App(application, model):
    """
        Add model as permanent drawables object to application.
        >>> application : Address of application which model will be added as permanent drawable object.
        >>> model       : Address of created drawable object. 
    """
    lib.addPermanent2App(application, model)

###################################################################################
lib.addInstantaneous2App.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
lib.addInstantaneous2App.restypes = ctypes.c_void_p

def addInstantaneous2App(application, model):
    """ 
        Add model as instantaneous drawables object to application.
        >>> application : Address of application which model will be added as instantaneous drawable object.
        >>> model       : Address of created drawable object. 
    """
    lib.addInstantaneous2App(application, model)
    
###################################################################################
lib.translateModel.argtypes = [ctypes.c_void_p, ctypes.c_float, 
                                ctypes.c_float, ctypes.c_float]
lib.translateModel.restype = ctypes.c_void_p

def translateModel(model, x, y, z):
    """
        Translate model to locations. Locations will be in OpenGL coordinates.
        >>> model : Translated model object adress.
        >>> x     : Translation of model in OpenGL x-axes.
        >>> y     : Translation of model in OpenGL y-axes.
        >>> z     : Translation of model in OpenGL z-axes.
    """
    lib.translateModel(model, x, y, z)

###################################################################################
lib.rotateModel.argtypes = [ctypes.c_void_p, ctypes.c_float, ctypes.c_float, 
                                ctypes.c_float, ctypes.c_float]
lib.rotateModel.restype = ctypes.c_void_p

def rotateModel(model, angle, x, y, z):
    """
        Rotate model to angle. From will be an rotation around arbitrary angle.
        >>> model : Rotated model object adress.
        >>> x     : Rotation of model around OpenGL x-axes.
        >>> y     : Rotation of model around OpenGL y-axes.
        >>> z     : Rotation of model around OpenGL z-axes.
    """
    lib.rotateModel(model, angle, x, y, z)

###################################################################################
lib.scaleModel.argtypes = [ctypes.c_void_p, ctypes.c_float, 
                                ctypes.c_float, ctypes.c_float]
lib.scaleModel.restype = ctypes.c_void_p

def scaleModel(model, x, y, z):
    """
        Scale model to the ratios for each axis.
        >>> model : Scaled model object adress.
        >>> x     : Scale ratio of model around OpenGL x-axes.
        >>> y     : Scale ratio of model around OpenGL y-axes.
        >>> z     : Scale ratio of model around OpenGL z-axes.
    """
    lib.scaleModel(model, x, y, z)

###################################################################################
lib.getFps.argtypes = [ctypes.c_void_p]
lib.getFps.restype = ctypes.c_float

def getFps(application):
    """
        Get FPS from the application.
        >>> application : Address of application which getting FPS.
    """
    return np.round(lib.getFps(application), 2)

###################################################################################
lib.setFps.argtypes = [ctypes.c_void_p, ctypes.c_float]
lib.setFps.restype = ctypes.c_void_p

def setFps(application, fps):
    """
        Set FPS of the application.
        >>> application : Address of application which setting FPS.
    """
    lib.setFps(application, fps)

###################################################################################
lib.getCamera.argtypes = [ctypes.c_void_p]
lib.getCamera.restype = ctypes.c_void_p

def getCamera(application):
    """
        Get Camera pointer of the application.
        >>> application : Address of application which camera connected to it.
    """
    return lib.getCamera(application)

###################################################################################
lib.setCameraPos.argtypes = [ctypes.c_void_p, ctypes.c_float,
                                ctypes.c_float, ctypes.c_float]
lib.setCameraPos.restype = ctypes.c_void_p

def setCameraPos(camera, x, y, z):
    """
        Set Camera position of each axis.
        >>> camera : Address of camera object.
        >>> x      : New location of camera object in OpenGL x-axes.
        >>> y      : New location of camera object in OpenGL y-axes.
        >>> z      : New location of camera object in OpenGL z-axes.
    """
    lib.setCameraPos(camera, x, y, z)

###################################################################################
lib.getCameraPos.argtypes = [ctypes.c_void_p]
lib.getCameraPos.restype = ctypes.POINTER(ctypes.c_float)

def getCameraPos(camera):
    """
        Get camera position as float array.
        >>> camera : Address of camera object.
    """
    return lib.getCameraPos(camera)

###################################################################################
lib.isVisible.argtypes = [ctypes.c_void_p]
lib.isVisible.restype = ctypes.c_bool

def isVisible(application):
    """
        Check whether the application is visible or not.
        >>> application : Address of application which checking it is visible or not.
    """
    return lib.isVisible(application)

###################################################################################
lib.hideWindow.argtypes = [ctypes.c_void_p]
lib.hideWindow.restype = ctypes.c_void_p

def hideWindow(application):
    """
        Hide the window.
        >>> application : Address of application which will be hided.
    """
    lib.hideWindow(application)

###################################################################################
lib.showWindow.argtypes = [ctypes.c_void_p]
lib.showWindow.restype = ctypes.c_void_p

def showWindow(application):
    """
        Show the application.
        >>> application : Address of application which will be showed.
    """
    lib.showWindow(application)

###################################################################################
lib.createGuiTextVector.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float]
lib.createGuiTextVector.restype = ctypes.c_int

def createGuiTextVector(application, title, pos, size):
    """
        Create guiText vector for guiTextSection.        
        >>> application : Address of application which guiText rendered.
        >>> title  : Title of guiText.
        >>> pos    : Position of guiText as 2D.
        >>> size   : Size of guiText as 2D.
    """
    assert type(pos) == tuple and len(pos) == 2, 'Make sure that position of guiText should be tuple and has 2 element.'
    assert type(size) == tuple and len(size) == 2, 'Make sure that size of guiText should be tuple and has 2 element.'
    return lib.createGuiTextVector(application, ctypes.c_char_p(_to_encode(title)), pos[0], pos[1], size[0], size[1])

###################################################################################
lib.addGuiTextAllLines.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), np.ctypeslib.ndpointer(dtype=np.float32, flags='C_CONTIGUOUS')]
lib.addGuiTextAllLines.restype = ctypes.c_void_p

def addGuiTextAllLines(application, guiText, strn, val):
    """
        Add the str and val to guiText on application.
        >>> application   : Address of application which guiText rendered.
        >>> guiText       : Address of guiText which strn will be added.
        >>> strn          : String of guiText row.
        >>> val           : Value of related guiText row.
    """
    assert type(strn) == list, 'Please make sure that `str` values in form of list.'
    for ind, item in enumerate(strn):
        strn[ind] = bytes(str(item), 'utf-8')
    strn_arr = (ctypes.c_char_p * (len(strn)))()    
    strn_arr[:] = strn

    if type(val) != np.ndarray:
        val = np.array(val)

    lib.addGuiTextAllLines(application, guiText, len(strn), strn_arr, val.astype(np.float32) )
###################################################################################
lib.addGuiTextLine.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, flags='C_CONTIGUOUS')]
lib.addGuiTextLine.restype = ctypes.c_void_p

def addGuiTextLine(application, guiText, text, val):
    """
        Add text and val to guiText on application. 

        >>> application   : Address of application which guiText rendered.
        >>> guiText       : Address of guiText which strn will be added.
        >>> text          : Text of related guiText row.
        >>> val           : Value of related guiText row.
    """
    lib.addGuiTextLine(application, guiText, ctypes.c_char_p(_to_encode(text)), np.array([val], np.float32))

###################################################################################
lib.setGuiTextAllValues.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, np.ctypeslib.ndpointer(dtype=np.float32, flags='C_CONTIGUOUS')]
lib.setGuiTextAllValues.restype = ctypes.c_void_p

def setGuiTextAllValues(application, guiText, val):
    """
        Set all val to guiText on application. Value vector order should be same as 
        string order.
        >>> application   : Address of application which guiText rendered.
        >>> guiText       : Address of guiText which strn will be added.
        >>> val           : Values of related guiText.
    """
    l = val.shape[0]
    lib.setGuiTextAllValues(application, guiText, l, val.astype(np.float32) )

###################################################################################
lib.setGuiTextAllTextLines.argtypes = [ctypes.c_void_p, ctypes.c_int,ctypes.c_int, ctypes.POINTER(ctypes.c_char_p),]
lib.setGuiTextAllTextLines.restype = ctypes.c_void_p

def setGuiTextAllTextLines(application, guiText, text):
    """
        Set all text to guiText on application.

        >>> application   : Address of application which guiText rendered.
        >>> guiText       : Address of guiText which strn will be added.
        >>> text          : Text of related guiText.
    """
    assert type(text) == list, 'Please make sure that `str` values in form of list.'
    for ind, item in enumerate(text):
        if (type(item) != bytes):
            text[ind] = bytes(str(item), 'utf-8')
    strn_arr = (ctypes.c_char_p * (len(text)))()    
    strn_arr[:] = text

    lib.setGuiTextAllTextLines(application, guiText, len(text), strn_arr)

###################################################################################
lib.setGuiTextLineValue.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, np.ctypeslib.ndpointer(dtype=np.float32, shape=(1), flags='C_CONTIGUOUS')]
lib.setGuiTextLineValue.restype = ctypes.c_void_p

def setGuiTextLineValue(application, guiText, lineNumber, val):
    """
        Set the val of guiText on application. 

        >>> application     : Address of application which guiText rendered.
        >>> guiText         : Address of guiText which strn will be added.
        >>> lineNumber      : Line number of guiText to set.
        >>> val             : Value of related guiText row.
    """
    val = np.array([val], np.float32)
    lib.setGuiTextLineValue(application, guiText, lineNumber, val)

###################################################################################
lib.setGuiTextLine.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
lib.setGuiTextLine.restype = ctypes.c_void_p

def setGuiTextLine(application, guiText, lineNumber, text):
    """
        Set the text of guiText on application. 

        >>> application     : Address of application which guiText rendered.
        >>> guiText         : Address of guiText which strn will be added.
        >>> lineNumber      : Line number of guiText to set.
        >>> text            : Value of related guiText row.
    """
    print(text, hex(id(text)))
    lib.setGuiTextLine(application, guiText, lineNumber, bytes(str(text), 'utf-8'))

###################################################################################
lib.setBool.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_bool]
lib.setBool.restype = ctypes.c_void_p

def setBool(model, name, value):
    """
        Set boolean uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setBool(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setInt.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
lib.setInt.restype = ctypes.c_void_p

def setInt(model, name, value):
    """
        Set integer uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setInt(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setFloat.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_float]
lib.setFloat.restype = ctypes.c_void_p

def setFloat(model, name, value):
    """
        Set float uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setFloat(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setVec2.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(2), flags='C_CONTIGUOUS')]
lib.setVec2.restype = ctypes.c_void_p

def setVec2(model, name, value):
    """
        Set vec2 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setVec2(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setVec3.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(3), flags='C_CONTIGUOUS')]
lib.setVec3.restype = ctypes.c_void_p

def setVec3(model, name, value):
    """
        Set vec3 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setVec3(model, ctypes.c_char_p(_to_encode(name)), value.astype(np.float32))

###################################################################################
lib.setVec4.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(4), flags='C_CONTIGUOUS')]
lib.setVec4.restype = ctypes.c_void_p

def setVec4(model, name, value):
    """
        Set vec4 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setVec4(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setMat2.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(2,2), flags='C_CONTIGUOUS')]
lib.setMat2.restype = ctypes.c_void_p

def setMat2(model, name, value):
    """
        Set mat2 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setMat2(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setMat3.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(3,3), flags='C_CONTIGUOUS')]
lib.setMat3.restype = ctypes.c_void_p

def setMat3(model, name, value):
    """
        Set mat3 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setMat3(model, ctypes.c_char_p(_to_encode(name)), value)

###################################################################################
lib.setMat4.argtypes = [ctypes.c_void_p, ctypes.c_char_p, np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, shape=(4,4), flags='C_CONTIGUOUS')]
lib.setMat4.restype = ctypes.c_void_p

def setMat4(model, name, value):
    """
        Set mat4 uniform of `model` shader. 
        >>> model  : Address of model. 
        >>> name   : Name of uniform.
        >>> value  : Value of uniform.
    """
    lib.setMat4(model, ctypes.c_char_p(_to_encode(name)), value)


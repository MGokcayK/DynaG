#
# DynaG Renderer Implementation Python / Renderer
# 
# Renderer class create manageable connection between Render API
# and user. It create a layer to prevent directly API methods usage.
#
# Author : @MGokcayK
# C.Date : 04/05/2021
# Update : 04/05/2021 - Initialization & Implementation // @MGokcayK
#

from typing import List

from dynag.dynamics import dynamicsAPI
from . import renderAPI
import os
import numpy as np

class Renderer(object):
    def __init__(self, w = 800, h = 600, title="dynag-renderer"):
        self.width = w
        self.height = h
        self.title = title
        self.app = renderAPI.createApplication(self.width, self.height, self.title, True)        
        self.camera = self.getCamera()
        self.guiTextList = []

    def render(self):
        """
            Render the `Renderer's` window.
        """
        renderAPI.render(self.app)

    def close(self):
        """
            Close the `Renderer's` window.
        """
        renderAPI.close(self.app)

    def isClose(self):
        """
            Check whether the `Renderer's` window is closed or not.
        """
        return renderAPI.isClose(self.app)
    
    def terminate(self):
        """
            Terminate OpenGL.
        """
        renderAPI.terminate()

    def createModel(self, rel_path = None, 
                vertex_shader_path = "/resources/shaders/vertex_base.vs", 
                fragment_shader_path = "/resources/shaders/frag_base.fs", 
                abs_path = None): 
        """
            Create model with shaders. `rel_path` means relative path of `obj` file. 
            `abs_path` means absolute path of `obj` file. 

            Note : If `abs_path` is used, shaders' path should be also absolute path.
        """
    
        if rel_path:
            return renderAPI.createModel(self.app, 
                                    os.path.dirname(os.path.realpath(__file__)) + rel_path, 
                                    os.path.dirname(os.path.realpath(__file__)) + vertex_shader_path,
                                    os.path.dirname(os.path.realpath(__file__)) + fragment_shader_path)
        else:
            return renderAPI.createModel(self.app,
                                    abs_path, 
                                    vertex_shader_path,
                                    fragment_shader_path)

    def addPermanentObject2App(self, model):
        """
            Add model to `Renderer's` application as permanent drawable objects.
        """
        renderAPI.addPermanent2App(self.app, model)

    def addInstantanousObject2App(self, model):
        """
            Add model to `Renderer's` application as instantaneous drawable objects.
        """
        renderAPI.addInstantaneous2App(self.app, model)

    def translateModel(self, model, x, y, z):
        """
            Translate model to x, y and z locations. Locations are in NED (North-East-Down)
            frame. 
        """
        # Because of the vector direction notation difference 
        # we set proper location to proper axis.
        # In Body-frame Y-axis is perpendicular to X-axis and Z-axis is cross
        # product of the axes. On the other hand, in OpenGL (or Computer Graphics)
        # Z-axis is Y-axis is altered. 
        renderAPI.translateModel(model, x, -z, y)

    def rotateModel(self, model, phi, theta, psi):
        """
            Rotate model w.r.t Euler angles. 
        """
        # In Body-frame Y-axis is perpendicular to X-axis and Z-axis is cross
        # product of the axes. On the other hand, in OpenGL (or Computer Graphics)
        # Z-axis is Y-axis is altered. 
        renderAPI.rotateModel(model, -psi   , 0, 1, 0)
        renderAPI.rotateModel(model,  theta , 0, 0, 1)
        renderAPI.rotateModel(model,  phi   , 1, 0, 0)

    def scaleModel(self, model, x, y, z):
        """
            Scale model to x, y and z axes. Axes are in NED (North-East-Down)
            frame. 
        """
        # Because of the vector direction notation difference 
        # we set proper angle to proper axis.
        # In Body-frame Y-axis is perpendicular to X-axis and Z-axis is cross
        # product of the axes. On the other hand, in OpenGL (or Computer Graphics)
        # Z-axis is Y-axis is altered. 
        renderAPI.scaleModel(model, x, -z, y)

    def getFPS(self):
        """
            Get FPS of `Renderer's` window.
            Return will be float.
        """
        return renderAPI.getFps(self.app)

    def setFPS(self, fps):
        """
            Set FPS of `Renderer's` window.
        """
        renderAPI.setFps(self.app, fps)

    def getCamera(self):
        """
            Get Camera of `Renderer's` window.
        """
        return renderAPI.getCamera(self.app)

    def setCameraPos(self, x, y, z):
        """
            Set Camera position of `Renderer's` window.
        """
        # Because of the vector direction notation difference 
        # we set proper angle to proper axis.
        # In Body-frame Y-axis is perpendicular to X-axis and Z-axis is cross
        # product of the axes. On the other hand, in OpenGL (or Computer Graphics)
        # Z-axis is Y-axis is altered. 
        renderAPI.setCameraPos(self.camera, x, -z, y)

    def getCameraPos(self):
        """
            Get Camera position of `Renderer's` window.
            Return will be float array.
        """
        return renderAPI.getCameraPos(self.camera)

    def coordFromGraphics2NED(self, x, y, z):
        """
            Convert OpenGL coordinates to North, East and Down (NED)
            frame coordinates.
        """
        return x, z, -y

    def isVisible(self):
        """
            Check whether `Renderer's` window is visible or not. 
            Return will be boolean.
        """
        return renderAPI.isVisible(self.app)

    def hideWindow(self):
        """
            Hide `Renderer's` window.
        """
        renderAPI.hideWindow(self.app)

    def showWindow(self):
        """
            Show `Renderer's` window.
        """
        renderAPI.showWindow(self.app)

    def createGuiText(self, title, pos_x, pos_y, size_x, size_y):
        """
            Create guiText for render text in gui. 
            
            It returns guiText object.
            
            Arguments:
            ----------

            >>> title    : Title of guiText.
            >>> type     : string 

            >>> pos_x    : Position of guiText in x.
            >>> type     : float

            >>> pos_y    : Position of guiText in y.
            >>> type     : float

            >>> size_x   : Size of guiText in x.
            >>> type     : float

            >>> size_y   : Size of guiText in y.
            >>> type     : float
        """
        return GuiText(self.app, title, pos_x, pos_y, size_x, size_y)

    def addGuiText(self, guiText, str, val):

        renderAPI.addGuiText(self.app, guiText, str, val)

    def setGuiText(self, guiText, str, val):
        """
            Set guiText.
            str and val are should be list.
        """
        renderAPI.setGuiText(self.app, guiText, str, val)

    def setUniformBool(self, model, name, value):
        """
            Set boolean uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setBool(model, name, value)

    def setUniformInt(self, model, name, value):
        """
            Set integer uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """        
        renderAPI.setInt(model, name, value)

    def setUniformFloat(self, model, name, value):
        """
            Set float uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setFloat(model, name, value)

    def setUniformVec2(self, model, name, value):
        """
            Set vec2 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setVec2(model, name, np.array([value], np.float32))

    def setUniformVec3(self, model, name, value):
        """
            Set vec3 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setVec3(model, name, value)
    
    def setUniformVec4(self, model, name, value):
        """
            Set vec4 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setVec4(model, name, np.array([value], np.float32))
    
    def setUniformMat2(self, model, name, value):
        """
            Set mat2 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setMat2(model, name, np.array([value], np.float32))

    def setUniformMat3(self, model, name, value):
        """
            Set mat3 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setMat3(model, name, np.array([value], np.float32))
    
    def setUniformMat4(self, model, name, value):
        """
            Set mat4 uniform of `model` shader. 
            >>> model  : Address of model. 
            >>> name   : Name of uniform.
            >>> value  : Value of uniform.
        """
        renderAPI.setMat4(model, name, np.array([value], np.float32))


class GuiText:
    """
        Arguments:
        ----------
        >>> window   : Address of window to add guiText.
        >>> type     : integer(pointer of window)

        >>> title    : Title of guiText.
        >>> type     : string 

        >>> pos_x    : Position of guiText in x.
        >>> type     : float

        >>> pos_y    : Position of guiText in y.
        >>> type     : float

        >>> size_x   : Size of guiText in x.
        >>> type     : float

        >>> size_y   : Size of guiText in y.
        >>> type     : float
    """
    def __init__(self, window, title, pos_x, pos_y, size_x, size_y) -> None:
        self.__window = window
        self.ID = renderAPI.createGuiTextVector(window, title, (pos_x, pos_y), (size_x, size_y))
        self.values = np.array([], dtype=np.float32)
        self.textLines = []
        
    def addAllTextLine2GuiText(self, text:str, value:np.array = None) -> None:
        """
            Add all text list and values array to guiText. 
            Format should be in printf-style format.
            \n * There should be 1 val for printing and vals should be numpy array.

            Arguments:
            ----------

            >>> text   : Line text list of guiText. 
            >>> type   : List(string)

            >>> value  : Values of lines with same order. If line has not value please add 0 as value at the same index.
            >>> type   : numpy.array(float32)
        """
        for line in text:
            self.textLines.append(line)
        if value != None:
            self.values = value
        else:
            self.values = np.zeros(len(self.textLines))
        renderAPI.addGuiTextAllLines(self.__window, self.ID, self.textLines, self.values)

    def addTextLine2GuiText(self, text:str, value = np.array([0], np.float32)) -> None:
        """
            Add line and corresponding value (if exists) of guiText.
            It add only one line of text. 
            Format should be in printf-style format.
            \n * There should be 1 val for printing and vals should be numpy array.

            Arguments:
            ----------

            >>> text   : Line text of guiText. 
            >>> type   : List(string)

            >>> value  : Value of line.
            >>> type   : numpy.array(float32)
        """
        self.textLines.append(text)
        self.values = np.append(self.values, value)
        renderAPI.addGuiTextLine(self.__window, self.ID, self.textLines[-1], self.values[-1])

    def setTextLine(self, lineNumber:int, newLine:str) -> None:
        """
            Set corresponding line of guiText. 

            Arguments:
            ----------

            >>> lineNumber   : Line number of text of guiText. 
            >>> type         : int

            >>> newLine      : New line string of guiText.
            >>> type         : str
        """
        self.textLines[lineNumber] = newLine
        renderAPI.setGuiTextLine(self.__window, self.ID, lineNumber, newLine)
    
    def setLineValue(self, lineNumber:int,  value = np.array([0], np.float32)) -> None:
        """
            Set corresponding value of line of guiText. 

            Arguments:
            ----------

            >>> lineNumber   : Line number of text of guiText. 
            >>> type         : int

            >>> value        : New value of line of guiText.
            >>> type         : numpy.array
        """
        self.values[lineNumber] = value
        renderAPI.setGuiTextLineValue(self.__window, self.ID, lineNumber, self.values[lineNumber])

    def setTextLineAndValue(self, lineNumber:int, newLine:str, value = np.array([0], np.float32)) -> None:
        """
            Set corresponding text and value of line of guiText. 

            Arguments:
            ----------

            >>> lineNumber   : Line number of text of guiText. 
            >>> type         : int

            >>> newLine      : New line string of guiText.
            >>> type         : str

            >>> value        : New value of line of guiText.
            >>> type         : numpy.array
        """
        self.setTextLine(lineNumber, newLine)
        self.setLineValue(lineNumber, value)
        
    def setAllTextLines(self, newLines:List) -> None:
        """
            Set all text list of guiText. 
            Format should be in printf-style format.
            \n * There should be 1 val for printing and vals should be numpy array.

            Arguments:
            ----------

            >>> newLines   : New Line text list of guiText. 
            >>> type       : List(string)            
        """
        assert len(newLines) == len(self.textLines), "Please make sure that `newLines` lenght should be equal to number of added lines in GuiText."
        self.textLines = newLines
        renderAPI.setGuiTextAllTextLines(self.__window, self.ID, self.textLines)

    def setAllLineValues(self, values:np.array) -> None:
        """
            Set all values of guiText. 
            
            Arguments:
            ----------

            >>> values  : Values of lines with same order. If line has not value please add 0 as value at the same index.
            >>> type    : numpy.array(float32)
        """
        assert values.shape == self.values.shape, "Please make sure that `newLines` lenght should be equal to number of added lines in GuiText."
        self.values = values.astype(np.float32)
        renderAPI.setGuiTextAllValues(self.__window, self.ID, self.values)
        
    def setAllTextLinesAndValues(self, newLines:List, values:np.array) -> None:
        """
            Set all text list and values array to guiText. 
            Format should be in printf-style format.
            \n * There should be 1 val for printing and vals should be numpy array.

            Arguments:
            ----------

            >>> newLines   : New Line text list of guiText. 
            >>> type       : List(string)

            >>> values     : New Values of lines with same order. If line has not value please add 0 as value at the same index.
            >>> type       : numpy.array(float32)
        """
        assert len(newLines) == len(self.textLines), "Please make sure that `newLines` lenght should be equal to number of added lines in GuiText."
        self.setAllTextLines(newLines)
        self.setAllLineValues(values)

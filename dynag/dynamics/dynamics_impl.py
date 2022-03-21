#
# DynaG Dynamics Implementation Python / Dynamics
# 
# In this file, user create dynamic object such as Helicopter. 
#
# Author : @MGokcayK
# C.Date : 22/12/2021
# Update : 22/12/2021 - Initialization & Implementation // @MGokcayK
#

from . import dynamicsAPI
import os
import math
import numpy as np

FTS2KNOT    = 0.5924838 # ft/s to knots conversion
EPS         = 1e-10 # small value for divison by zero
R2D         = 180/math.pi # Rad to deg
D2R         = 1/R2D
FT2MTR      = 0.3048 # ft to meter
TAU         = 2*math.pi

class Helicopter():
    def __init__(self, heliName:str, dt:float) -> None:
        self.heliName = heliName
        parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
        yaml_path = os.path.join(parent_dir, "helis", self.heliName + ".yaml")
        self.DT = dt
        self.heliDyn = dynamicsAPI.createHelicopterDynamics(yaml_path, self.DT)

    @property
    def getNumberOfDynamicsObservation(self):
        return dynamicsAPI.getNumberOfObservations(self.heliDyn)
    
    @property
    def heliDynObj(self):
        return self.heliDyn

    def createRendererObject(self, renderer):
        self.helicopterRenderObj = renderer.createModel('/resources/models/'+ self.heliName +'/'+ self.heliName +'.obj',
                                                          '/resources/shaders/'+ self.heliName +'_vertex.vs',
                                                          '/resources/shaders/'+ self.heliName +'_frag.fs')

        renderer.addPermanentObject2Window(self.helicopterRenderObj)

    ############################# HELICOPTER - API RELATION ################################
    def ready(self):
        return dynamicsAPI.ready()

    def getAllHelicopterAction(self):
        return dynamicsAPI.getAllAction(self.heliDyn)

    def getAllNormalizedHelicopterAction(self):
        return dynamicsAPI.getAllNormalizedAction(self.heliDyn)

    def getHelicopterAction(self, actionName:str):
        return dynamicsAPI.getAction(self.heliDyn, actionName)

    def getNormalizedHelicopterAction(self, actionName:str):
        return dynamicsAPI.getNormalizedAction(self.heliDyn, actionName)

    def getAllHelicopterState(self):
        return dynamicsAPI.getAllState(self.heliDyn)

    def getAllNormalizedHelicopterState(self):
        return dynamicsAPI.getAllNormalizedState(self.heliDyn)

    def getHelicopterState(self, stateName:str):
        return dynamicsAPI.getState(self.heliDyn, stateName)

    def getNormalizedHelicopterState(self, stateName:str):
        return dynamicsAPI.getNormalizedState(self.heliDyn, stateName)

    def getAllHelicopterStateDot(self):
        return dynamicsAPI.getAllStateDot(self.heliDyn)

    def getAllNormalizedHelicopterStateDot(self):
        return dynamicsAPI.getAllNormalizedStateDot(self.heliDyn)

    def getHelicopterStateDot(self, stateName:str):
        return dynamicsAPI.getStateDot(self.heliDyn, stateName)

    def getNormalizedHelicopterStateDot(self, stateName:str):
        return dynamicsAPI.getNormalizedStateDot(self.heliDyn, stateName)
    
    def getAllHelicopterObservation(self):
        return dynamicsAPI.getAllObservation(self.heliDyn) 

    def getAllNormalizedHelicopterObservation(self):
        return dynamicsAPI.getAllNormalizedObservation(self.heliDyn) 

    def getHelicopterObservation(self, observationName:str):
        return dynamicsAPI.getObservation(self.heliDyn, observationName)

    def getNormalizedHelicopterObservation(self, observationName:str):
        return dynamicsAPI.getNormalizedObservation(self.heliDyn, observationName)

    def setTrimValueInYamlFile(self, variableName:str, value):
        dynamicsAPI.setValueInYamlNode(self.heliDyn, "TRIM", variableName, value)
    
    def getValueFromYamlFile(self, nodeName:str, variableName:str):
        return dynamicsAPI.getValueFromYamlNode(self.heliDyn, nodeName, variableName)
    
    def setValueInYamlFile(self, nodeName:str, variableName:str, value):
        dynamicsAPI.setValueInYamlNode(self.heliDyn, nodeName, variableName, value)
    
    ##################################################################################
    def render(self, renderer):
        euler = self.getHelicopterObservation("eulerangles")
        xyz = self.getHelicopterState("xyz")
        betas = self.getHelicopterState("betas")
        psimr = self.getHelicopterState("psimr")
        psitr = self.getHelicopterState("psitr")
        # In Body-frame Y-axis is perpendicular to X-axis and Z-axis is cross
        # product of the axes. On the other hand, in OpenGL (or Computer Graphics)
        # Z-axis is Y-axis is altered.
        renderer.setUniformVec3(self.helicopterRenderObj, 
                                    "mainrotor", 
                                    np.array([betas[1], #betas1
                                            -psimr[0], #psi_mr
                                            betas[0]], #betas0 
                                            dtype="object"))

        renderer.setUniformVec3(self.helicopterRenderObj, 
                                    "tailrotor", 
                                    np.array([0, 
                                            0, 
                                            psitr[0]], #psi_tr
                                            dtype="object"))     

        renderer.translateModel(self.helicopterRenderObj, 
                                    xyz[0] * FT2MTR, #xyz0
                                    xyz[1] * FT2MTR, #xyz1
                                    xyz[2] * FT2MTR  #xyz2
                                    )

        renderer.rotateModel(self.helicopterRenderObj, 
                                    euler[0],
                                    euler[1],
                                    euler[2]
                                    )

    def step(self, actions):
        dynamicsAPI.step(self.heliDyn, actions)
        return self.getAllHelicopterObservation()

    def resetHelicopter(self):
        dynamicsAPI.reset(self.heliDyn)

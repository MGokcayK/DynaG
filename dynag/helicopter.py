"""
Base class of DynaG.

Author : @MGokcayK
Author : @ugurcanozalp
"""
import sys, math
import numpy as np
import copy

import gym
from gym import spaces
from gym.utils import EzPickle
from .renderer.renderer_impl import Renderer
from .dynamics.dynamics_impl import Helicopter

FPS         = 50.0
DT          = 1/FPS
FTS2KNOT    = 0.5924838 # ft/s to knots conversion
EPS         = 1e-10 # small value for divison by zero
R2D         = 180/math.pi # Rad to deg
D2R         = 1/R2D
FT2MTR      = 0.3048 # ft to meter
TAU         = 2*math.pi

class HelicopterGym(gym.Env, EzPickle):
    """
        Base `gym` environment class for helicopter. 

        Arguments:
        ----------

        >>> heli_name : Name of yaml file for helicopter parameters. 
        >>> dt        : Step time for helicopter dynamics.
    """
    metadata = {
        'render.modes': ['human', 'rgb_array'],
        'video.frames_per_second' : FPS
    }
    # Default maximum time for an episode
    default_max_time = 40.0
    # default task target
    default_task_target = {}

    max_reward = 10
    min_reward = -10

    def __init__(self, heli_name:str = "aw109", dt:float = DT):
        EzPickle.__init__(self)
        
        self.DT = dt
        self.helicopter = Helicopter(heli_name, self.DT)
        self.observation_space = spaces.Box(-np.inf, np.inf, shape=(self.helicopter.getNumberOfDynamicsObservation,), dtype=np.float32)
        self.action_space = spaces.Box(-1, +1, (4,), dtype=np.float32)
        self.successed_time = 0 # time counter for successing task through time.
        self.set_max_time()
        self.set_target()
        self.renderer = Renderer(w=1920, h=1080, title='DynaG')
        self.renderer.setFPS(FPS)
        self.helicopter.createRendererObject(self.renderer)        
        self.terrain = self.renderer.createModel('/resources/models/terrain/terrain.obj',
                                                 '/resources/shaders/terrain_vertex.vs',
                                                 '/resources/shaders/terrain_frag.fs')
        
        self.renderer.addPermanentObject2App(self.terrain)        
        self.sky = self.renderer.createModel('/resources/models/sky/sky.obj')
        self.renderer.addPermanentObject2App(self.sky)
        self._bGuiText = False

    # Setter functions for RL tasks
    def set_max_time(self, max_time=None):
        self.max_time = self.default_max_time if max_time is None else max_time  # [sec] Given time for episode
        self.success_duration = self.max_time/4 # [sec] Required successfull time for maneuver
        self.task_duration = self.max_time/4 # [sec] Allowed time for successing task

    def set_target(self, target={}):
        self.task_target = self.default_task_target
        self.task_target.update(target)

    def get_target(self):
        return copy.deepcopy(self.task_target)

    def _createGuiForInfo(self):
        # Base guiText for observations.
        self.guiObservations = self.renderer.createGuiText("Observations", 30.0, 30.0, 200.0, 0.0)
        self.guiObservations.addTextLine2GuiText("FPS        : %3.0f ")
        self.guiObservations.addTextLine2GuiText("POWER      : %8.2f hp" )
        self.guiObservations.addTextLine2GuiText("LON_AIR_VEL: %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("LAT_AIR_VEL: %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("DWN_AIR_VEL: %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("LON_VEL    : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("LAT_VEL    : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("DWN_VEL    : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("LON_ACC    : %7.2f ft/s^2")
        self.guiObservations.addTextLine2GuiText("LAT_ACC    : %7.2f ft/s^2")
        self.guiObservations.addTextLine2GuiText("DWN_ACC    : %7.2f ft/s^2")
        self.guiObservations.addTextLine2GuiText("N_VEL      : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("E_VEL      : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("DES_RATE   : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("ROLL       : %5.2f rad")
        self.guiObservations.addTextLine2GuiText("PITCH      : %5.2f rad")
        self.guiObservations.addTextLine2GuiText("YAW        : %5.2f rad")
        self.guiObservations.addTextLine2GuiText("ROLL_RATE  : %5.2f rad/s")
        self.guiObservations.addTextLine2GuiText("PITCH_RATE : %5.2f rad/s")
        self.guiObservations.addTextLine2GuiText("YAW_RATE   : %5.2f rad/s")
        self.guiObservations.addTextLine2GuiText("N_POS      : %8.2f ft")
        self.guiObservations.addTextLine2GuiText("E_POS      : %8.2f ft")
        self.guiObservations.addTextLine2GuiText("D_POS      : %8.2f ft")
        self.guiObservations.addTextLine2GuiText("GR_ALT     : %8.2f ft")
        self.guiObservations.addTextLine2GuiText("COLL_ANG   : %7.4f rad")
        self.guiObservations.addTextLine2GuiText("LON_ANG    : %7.4f rad")
        self.guiObservations.addTextLine2GuiText("LAT_ANG    : %7.4f rad")
        self.guiObservations.addTextLine2GuiText("PED_ANG    : %7.4f rad")
        self.guiObservations.addTextLine2GuiText("COLL_RATE  : %7.4f rad/s")
        self.guiObservations.addTextLine2GuiText("LON_RATE   : %7.4f rad/s")
        self.guiObservations.addTextLine2GuiText("LAT_RATE   : %7.4f rad/s")
        self.guiObservations.addTextLine2GuiText("PED_RATE   : %7.4f rad/s")
        self.guiObservations.addTextLine2GuiText("WIND_N_VEL : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("WIND_E_VEL : %7.2f ft/s")
        self.guiObservations.addTextLine2GuiText("WIND_D_VEL : %7.2f ft/s")

    def render(self, mode='human'):
        info_val = np.array(self.renderer.getFPS())
        info_val = np.append(info_val, self.helicopter.getAllHelicopterObservation())        
        self.guiObservations.setAllLineValues(info_val)

        self.helicopter.render(self.renderer)

        xyz = self.helicopter.getHelicopterState("xyz")

        self.renderer.translateModel(self.sky, 
                                    xyz[0] * FT2MTR,
                                    xyz[1] * FT2MTR,
                                    xyz[2] * FT2MTR + 500
                                    )

        self.renderer.setCameraPos(xyz[0] * FT2MTR,
                                     xyz[1] * FT2MTR + 30,
                                     xyz[2] * FT2MTR )

        if not self.renderer.isVisible():
            self.renderer.showWindow()
            
        self.renderer.render()
        
    def close(self):
        self.renderer.close()
        self.renderer.terminate()
    
    def exit(self):
        sys.exit()

    def step(self, actions):
        self.time_counter += self.DT
        _ = self.helicopter.step(actions)
        observation = self.helicopter.getAllNormalizedHelicopterObservation()
        reward, successed_step = self._calculate_reward()
        reward = np.clip(reward, self.min_reward, self.max_reward)
        info = self._get_info()
        done = info['failed'] or info['successed'] or info['time_up'] or info['sim_error'] or (reward == np.nan)
        self.successed_time += self.DT if successed_step else 0
        return np.copy(observation) , reward, done, info

    def reset(self):
        self.time_counter = 0
        self.successed_time = 0
        self.helicopter.resetHelicopter()
        if not self._bGuiText:
            self._createGuiForInfo()
            self._bGuiText = True
        return self.helicopter.getAllNormalizedHelicopterObservation()

    def _get_info(self):
        return {
            'failed': self._is_failed(), 
            'sim_error': self._sim_error(),
            'successed': self._is_successed(), 
            'time_up': self._is_time_up()
            }

    def _is_failed(self):
        xyz_normalized = self.helicopter.getNormalizedHelicopterObservation("xyz")
        uvw_normalized = self.helicopter.getNormalizedHelicopterObservation("uvw")
        ground_alt_normalized = self.helicopter.getNormalizedHelicopterObservation("gralt")
        eulerangles = self.helicopter.getNormalizedHelicopterObservation("eulerangles")
        cond1 = ground_alt_normalized[0] < 0.0015
        cond2 = np.abs(eulerangles[0]) > 0.33 # corresponds to 60 deg
        cond3 = np.abs(eulerangles[1]) > 0.33 # corresponds to 60 deg
        cond4 = np.linalg.norm(uvw_normalized) > 0.1
        cond5 = np.abs(xyz_normalized[0]) > 1.0 or np.abs(xyz_normalized[1]) > 1.0
        cond = ((cond1 and (cond2 or cond3 or cond4)) or cond5 ) 
        return cond

    def _sim_error(self):
        return (not self.helicopter.ready())

    def _is_successed(self):
        return self.successed_time >= self.success_duration  

    def _is_time_up(self):
        return self.time_counter > self.max_time

    def _calculate_reward(self):
        return 0.0, False
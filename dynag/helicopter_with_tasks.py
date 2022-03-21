import numpy as np
from gym import spaces
from .helicopter import HelicopterGym

# Lambda functions for reward calculation
normalize = lambda x: x / np.sqrt((x*x).sum())
norm = lambda x: np.sqrt((x*x).sum())

def randomStartPoint(env, start_point=(0.0, 0.0, 0.0), deviation = 200, alt_low = 0.0, alt_hight = 10.0):
    """
        Random start point generator method.

        Arguments:
        ----------

        >>> env         : Environment class. 
        >>> start_point : Start point declaration. If it is `None`, random point generated with normal distribution. 
            It represents North position, East position and Ground altitude in ft.
        >>> deviation   : Start point normal distribution devition magnitude.
        >>> alt_low     : Lowest altitude of start point in ft.
        >>> alt_height  : Heightest altitude of start point in ft.
    """
    if start_point == None:
        r = np.random.rand(2)
        if (r[1] < 0.33): r[0] *= 0.1 # the reason behind is generate more lower start point to handle more turbulence.
        gr_alt =  alt_low + (alt_hight - alt_low) * r[0]
        xy = np.random.randn(2) * deviation
    else:
        gr_alt = start_point[2]
        xy = [start_point[0],start_point[1]]

    if gr_alt < 0:
        gr_alt = 0
    
    gr_alt = np.round(gr_alt, 2)

    env.gr_alt = gr_alt
    env.n_pos = xy[0]
    env.e_pos = xy[1]

    env.helicopter.setTrimValueInYamlFile("GR_ALT", gr_alt)
    env.helicopter.setTrimValueInYamlFile("N_POS", xy[0])
    env.helicopter.setTrimValueInYamlFile("E_POS", xy[1])


def randomTargetPoint(env):
    """
        Random target point generator method.

        Arguments:
        ----------

        >>> env : Environment class. 
    """
    mt = env.max_time / 2
    start_gr_alt = env.helicopter.getValueFromYamlFile("TRIM", "GR_ALT")
    start_alt = start_gr_alt + 1328.29 # is offset of terrain from minimum altititude of terrain
    alt_low = 1700 # minimum target altitide
    alt_low = alt_low if (start_alt - 33*mt ) < alt_low else (start_alt - 33 * mt) # 33 * 80 is max climb rate times max time
    alt_high = start_alt + 33 * mt
    alt =  alt_low + (alt_high - alt_low) * np.random.rand()
    alt /= 3550 # normalizer for task_target
    xy = np.random.randn(2) * 100
    xy /= 3550 # normalizer for task_target

    task_target = {
            "xyz": [xy[0], xy[1], -alt], 
        }
    
    env.set_target(task_target)


def randomWeightAndCG(env):
    """
        Random weight and center of gravity generator. 

        Arguments:
        ----------

        >>> env : Environment class. 
    """
    env.weight = 3500 + np.random.rand() * 1901
    env.fs_cg = 128.7 + np.random.rand() * 8
    env.wl_cg = 34.5 + np.random.rand() * 8
    env.helicopter.setValueInYamlFile("HELI", "WT", env.weight)
    env.helicopter.setValueInYamlFile("HELI", "FS_CG", env.fs_cg )
    env.helicopter.setValueInYamlFile("HELI", "WL_CG", env.wl_cg )



class Hover(HelicopterGym):
    """
        Hover task class which aim to hover with robustness against random mass/weight and turbulence.

        Arguments:
        ----------

        >>> heli_name : Name of yaml file for helicopter parameters. 
    """
    def __init__(self, heli_name:str = "aw109"):
        HelicopterGym.__init__(self, heli_name=heli_name)
        self.observation_space = spaces.Box(-np.inf, np.inf, shape=(17,), dtype=np.float32)
        self.action_space = spaces.Box(-1, +1, (4,), dtype=np.float32)
        max_time = 60
        task_target = {
            "xyz": [0.0, 0.0, -0.5], 
        }
        self.set_max_time(max_time)

        self.score = 0.0

        self.weight = 0.0
        self.fs_cg = 0.0
        self.wl_cg = 0.0

        # Second guiText for hover task.
        self.guiText = self.renderer.createGuiText("Hover", self.renderer.width-200, 30, 0.0, 0.0)
        self.guiText.addTextLine2GuiText("Weight  : %9.4f")
        self.guiText.addTextLine2GuiText("FS_CG   : %9.4f")
        self.guiText.addTextLine2GuiText("WL_CG   : %9.4f")
        self.guiText.addTextLine2GuiText("-------------------")      
        self.guiText.addTextLine2GuiText("Reward  : %9.4f")
        self.guiText.addTextLine2GuiText("Score   : %12.4f")       
        self.guiText.addTextLine2GuiText("-------------------")      
        self.guiText.addTextLine2GuiText("Start  N_POS : %8.4f")        
        self.guiText.addTextLine2GuiText("Start  E_POS : %8.4f")       
        self.guiText.addTextLine2GuiText("Start  GR_ALT: %8.4f")
        self.guiText.addTextLine2GuiText("-------------------")      
        self.guiText.addTextLine2GuiText("Target N_POS : %8.4f")        
        self.guiText.addTextLine2GuiText("Target E_POS : %8.4f")       
        self.guiText.addTextLine2GuiText("Target D_POS : %8.4f")
        self.guiText.addTextLine2GuiText("-------------------")      
        self.guiText.addTextLine2GuiText("Remaining Time : %5.2f")

        self.gr_alt = 0.0
        self.n_pos = 0.0
        self.e_pos = 0.
        
        self.xyz_pos_weight = 0.03 / 0.5
        self.xyz_vel_weight = 0.0419 / 0.4# norm value for 30ft/s 0.0726 for 52ft/s
        self.pqr_weight = 8.592e-3 / 0.05 # for 0.20
        self.swash_weight = 0.0124 / 0.05 #for 0.25 
        
        self.reset()

    def __getOBSforModel(self, obs):      
        ind = [10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26]
        return obs[ind]        

    def get_coef(self):
        return "NO INFO"

    def _calculate_reward(self):
        xyz = self.helicopter.getNormalizedHelicopterObservation("xyz")
        nedvel = self.helicopter.getNormalizedHelicopterObservation("nedvel")
        pqr = self.helicopter.getNormalizedHelicopterObservation("pqr")
        swashrate = self.helicopter.getNormalizedHelicopterObservation("swashrate")

        fail_cost = 2*float(self._is_failed())

        xyz_error = self.task_target["xyz"] - xyz         
        e = normalize(xyz_error)
        vel_to_target = e.dot(nedvel)
        xyz_final =  norm(xyz_error) / self.xyz_pos_weight
        xyz_terminal = 1.0 - vel_to_target / (0.0419)
        xyz_terminal = abs(xyz_terminal)
        xyz_cost = min(xyz_final, xyz_terminal)
        vel_cost = norm(nedvel - e*vel_to_target) / self.xyz_vel_weight
        pqr_cost =  norm(pqr) / self.pqr_weight
        swash_cost =  norm(swashrate) / self.swash_weight
        
        reward = 1.0 - xyz_cost - pqr_cost - swash_cost - vel_cost - fail_cost
        reward *= self.DT

        pr = False
        #pr = True

        if pr:
            print(f"\nREWARD    :{reward}")
            print(f"XYZ REWARD :{xyz_cost} ")
            print(f"PQR REWARD :{pqr_cost}")   
            print(f"SWASH REWARD :{swash_cost}")          
            print(f"FAIL REWARD :{fail_cost}")          

        return reward, False

    def reset(self):        
        randomStartPoint(self, start_point=None, alt_low=100, alt_hight=4000)        
        randomTargetPoint(self)
        randomWeightAndCG(self)
        
        self.score = 0.0
        self.time_counter = 0
        self.successed_time = 0
        
        self.helicopter.resetHelicopter()
        
        if not self._bGuiText:
            self._createGuiForInfo()
            self._bGuiText = True
        
        obs = self.__getOBSforModel(self.helicopter.getAllNormalizedHelicopterObservation())

        return obs

    def step(self, actions):
        self.time_counter += self.DT
        _ = self.helicopter.step(actions)
        observation = self.helicopter.getAllNormalizedHelicopterObservation()
        reward, successed_step = self._calculate_reward()
        reward = np.clip(reward, self.min_reward, self.max_reward)
        self.score += reward
        info = self._get_info()
        done = info['failed'] or info['successed'] or info['time_up'] or info['sim_error'] or (reward == np.nan)
        self.successed_time += self.DT if successed_step else 0


        self.guiText.setAllLineValues(np.array([self.helicopter.getValueFromYamlFile("HELI","WT"), self.fs_cg, self.wl_cg, 0, 
                                            reward, self.score, 0, 
                                             self.n_pos, 
                                             self.e_pos,
                                             self.gr_alt, 
                                             0,
                                             self.task_target["xyz"][0] * 3550,
                                             self.task_target["xyz"][1] * 3550,
                                             self.task_target["xyz"][2] * 3550,
                                             0,
                                             self.max_time - self.time_counter], dtype='object'))
        
        obs = np.copy(observation)
        target = np.zeros_like(obs)
        target[19:22] = self.task_target['xyz']
        obs = obs - target
        return  self.__getOBSforModel(obs), reward, done, info

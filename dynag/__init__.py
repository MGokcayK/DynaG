from gym.envs.registration import register

from .helicopter import HelicopterGym
from .helicopter_with_tasks import Hover

register(
    id='HelicopterGym-v0',
    entry_point='dynag.HelicopterGym',
    max_episode_steps = 5000,
    reward_threshold  = 0.95,
    nondeterministic  = False
)

register(
    id='Hover-v0',
    entry_point='dynag.Hover',
    max_episode_steps = 5000,
    reward_threshold  = 0.95,
    nondeterministic  = False
)

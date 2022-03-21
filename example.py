import dynag
import numpy as np

env = dynag.Hover()

for _ in range(10):
    obs = env.reset()

    print(obs)
    action = env.helicopter.getAllHelicopterAction() # + np.array([-0.01, 0, 0, -0.01])
    print(action)
    
    for i in range(1000000):

        action = action #* 0
        obs, r, done, info = env.step(action)
        env.render()

        if done:
            print(info)
            env.close()
            print('Done!')
            break


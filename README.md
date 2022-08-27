# **DynaG**

Dynamic System solver for reinforcement learning environment. It solves ODE with $4^{th}$ order Runge-Kutta methods.

DynaG has

-   OpenAI GYM environment integration.
-   3D rendering with OpenGL.
-   Dynamics and rendering wrintten in C++

properties.

DynaG has two seperate APIs. For dynamic system calculation [DynaG-dynamcis](docs/dynamics.md) and for rendering [DynaG-renderer](docs/renderer.md) is used.

<br/>

---

<br/>

## **DynaG-Heli**

6-DOF Helicopter Environment with DynaG. Implementation structure can be found in [here](docs/helicopter.md).

This helicopter environment is written for flight tasks with reinforcement learning and has

-   Realistic Dynamic Model based on Minimum Complexity Helicopter Model ([Heffley and Mnich](https://robertheffley.com/docs/Sim_modeling/Heffley-Mnich--Minimum-Complexity%20Helicopter%20Simulation%20Math%20Model--NASA%20CR%20177476.pdf))
    In addition, inflow dynamics are added and model is adjusted so that it covers multiple flight conditions.
-   Dryden Turbulence model is added to make the environment stochastic.

properties.

![Caption](docs/ex.gif)

### **Action Space**

| Num | Act         | Unit | Min | Max |
| --- | ----------- | ---- | --- | --- |
| 0   | Collective  |      | -1  | 1   |
| 1   | Lon. Cyclic |      | -1  | 1   |
| 2   | Lat. Cyclic |      | -1  | 1   |
| 3   | Pedal       |      | -1  | 1   |

<br/>

### **Observation Space**

| Num | Obs                | Unit     | Min | Max |
| --- | ------------------ | -------- | --- | --- |
| 0   | power              | $hp $    | 0   | ∞   |
| 1   | lon. air speed     | $ft/s $  | -∞  | ∞   |
| 2   | lat. air speed     | $ft/s $  | -∞  | ∞   |
| 3   | down air speed     | $ft/s $  | -∞  | ∞   |
| 4   | lon. speed         | $ft/s $  | -∞  | ∞   |
| 5   | lat. speed         | $ft/s $  | -∞  | ∞   |
| 6   | down speed         | $ft/s $  | -∞  | ∞   |
| 7   | lon. acceleration  | $ft/s^2$ | -∞  | ∞   |
| 8   | lat. acceleration  | $ft/s^2$ | -∞  | ∞   |
| 9   | down acceleration  | $ft/s^2$ | -∞  | ∞   |
| 10  | north velocity     | $ft/s $  | -∞  | ∞   |
| 11  | east velocity      | $ft/s $  | -∞  | ∞   |
| 12  | descend rate       | $ft/s $  | -∞  | ∞   |
| 13  | roll angle         | $rad $   | -π  | π   |
| 14  | pitch angle        | $rad $   | -π  | π   |
| 15  | yaw angle          | $rad $   | -π  | π   |
| 16  | roll rate (body)   | $rad/s $ | -∞  | ∞   |
| 17  | pitch rate (body)  | $rad/s $ | -∞  | ∞   |
| 18  | yaw rate (body)    | $rad/s $ | -∞  | ∞   |
| 19  | x loc (earth)      | $ft $    | -∞  | ∞   |
| 20  | y loc (earth)      | $ft $    | -∞  | ∞   |
| 21  | sea altitude       | $ft $    | 0   | ∞   |
| 22  | ground altitude    | $ft $    | 0   | ∞   |
| 23  | coll. angle        | $rad $   | -∞  | ∞   |
| 24  | lon. angle         | $rad $   | -∞  | ∞   |
| 25  | lat. angle         | $rad $   | -∞  | ∞   |
| 26  | pedal angle        | $rad $   | -∞  | ∞   |
| 27  | coll. angle rate   | $rad/s $ | -∞  | ∞   |
| 28  | lon. angle rate    | $rad/s $ | -∞  | ∞   |
| 29  | lat. angle rate    | $rad/s $ | -∞  | ∞   |
| 30  | pedal angle rate   | $rad/s $ | -∞  | ∞   |
| 31  | turbulance u speed | $ft/s $  | -∞  | ∞   |
| 32  | turbulance v speed | $ft/s $  | -∞  | ∞   |
| 33  | turbulance w speed | $ft/s $  | -∞  | ∞   |

<br/>

### **Tasks**

For now only one task is available.

| Environment | Details | | 
| ----------- | ---------------------- | --- |
| Hover-v0    | [Hover](docs/hover.md) | [<img src="https://i.imgur.com/l6c1b3b.png" width="40"/>](https://drive.google.com/file/d/1Blu8M3lNC2bo0cO7vBqLDuoRY_dRFuZr/view?usp=sharing) |

<br/>
<br/>

---

## **Tested OS**

Environment tested on these OSs. If you have any problem, probably shared libraries for rendering
make it, please look at [renderer build.](docs/renderer_build.md)

Receiving max FPS with NVIDIA 1070-TI with Intel i7-8700K given in the table.

| Tested OS    | Max FPS |
| ------------ | ------- |
| Windows 10   | 1200    |
| Ubuntu 18.04 | 1300    |
| Ubuntu 16.04 | 1300    |

For the user who has lower than the version of Ubuntu 16.04 (like Ubuntu 14.04 etc.) or other Linux distros,
please re-compile dependent libraries. Re-compile instruction can be found at [renderer build.](docs/dynamics_build.md)
Also, dynamics should be re-compiled. Re-compile instruction can be found at [dynamics build.](docs/dynamics_build.md)

<br/>
<br/>

---

## **Setup**

Clone the repo.

```bash
git clone https://github.com/MGokcayK/DynaG.git
```

Run following command.

```bash
pip install .
```

or

```bash
pip install -e .
```

<br/>
<br/>

---

## **Usage**

Create environment by either,

```python
from dynag import Hover
env = Hover()
```

or

```python
import gym
import dnyag
env = gym.make("Hover-v0")
```

The rest is usual as of any GYM environment !

<br/>

---

```
@mastersthesis{MGK-Master-2022,
    author = {Mehmet Gökçay Kabataş},
    title = {{Pekiştirmeli Öğrenme İçin Bir Helikopter Ortamı Geliştirilmesi}},
    school = {Kocaeli Üniversitesi},
    address = {Türkiye},
    year = {2022}
}
```

---

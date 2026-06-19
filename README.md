# 🌊 Multi-Scene 2D Animation: Cox's Bazar & Sajek Valley ⛰️

An interactive, premium-tier 2D graphics application built in **C++** using the **OpenGL Utility Toolkit (GLUT)**. It showcases two iconic landscapes of Bangladesh—the sea breeze of **Cox's Bazar** and the cloud-capped highlands of **Sajek Valley**—layered with real-time controls, day/night cycles, ambient logic, and a collision-avoidance roadway traffic AI.

---

## 🖼️ Visual Preview & Gallery

Here is a side-by-side showcase of both scenes in their respective Day and Night modes:

### 🌊 Scene 1: Cox's Bazar Beach
| ☀️ Day Mode | 🌙 Night Mode |
|:---:|:---:|
| ![Cox's Bazar Day](coxs%20day.png) | ![Cox's Bazar Night](coxs%20night.png) |

### ⛰️ Scene 2: Sajek Valley
| ☀️ Day Mode | 🌙 Night Mode |
|:---:|:---:|
| ![Sajek Valley Day](sajek%20day.png) | ![Sajek Valley Night](sajek%20night.png) |

---

## 🕹️ Controls Reference

### ⌨️ Keyboard Hotkeys
| Key | Action |
| :---: | --- |
| **`Tab`** | Toggle between **Scene 1 (Cox's Bazar)** and **Scene 2 (Sajek Valley)** |
| **`N` / `n`** | Trigger **Night Mode** (activates building windows, stars, and vehicle headlights) |
| **`D` / `d`** | Trigger **Day Mode** (sun, light blue skies, and solid building shaders) |
| **`0`** | **Stop Speed** (stops windmills in Sajek, stops traffic in Cox's Bazar) |
| **`1`** | **Normal Speed** (standard simulation pace) |
| **`2`** | **Fast Speed** (accelerated traffic and windmill rotations) |
| **`ESC`** | Terminate program cleanly |

### 🖱️ Mouse Interactions
* **Cox's Bazar (Scene 1)**: Click anywhere on the screen to toggle realistic falling **Rain**.
* **Sajek Valley (Scene 2)**: Left-click on any of the three farmhouses to toggle their window lights on or off (Active in **Night Mode** only, calculated via exact coordinate bounding boxes).

---

## ⚙️ Key Technical Implementations

### 🚗 1. Collision-Avoidance Traffic AI (2-Pass Propagation)
To prevent vehicles in the same lane from clumping and overlapping, the update loop runs a **2-Pass Traffic AI**:
* **Pass 1 (Proximity Scan)**: Checks the distance between vehicles. If a tailing vehicle is within a safe distance ($130\text{px}$) of the leading vehicle, it adjusts its speed.
* **Pass 2 (Speed Propagation)**: Propagates adjusted velocities backward along the lane queue. This prevents cascading delays and preserves uniform spacing (using `min()` for right-bound and `max()` for left-bound traffic).

### 🛸 2. Advanced 2D Transformations (Local Matrix Stack)
* **Wing Flapping Animation**: Wing rotation is isolated from the bird's main body translation by pushing coordinates onto the matrix stack:
  ```cpp
  glPushMatrix();
  glTranslatef(0, 5, 0); // Translate to left wing joint
  glRotatef(wingAngle, 0, 0, 1); // Rotate wing locally
  // Draw wing triangle...
  glPopMatrix();
  ```
* **Depth Simulation (Perspective Scaling)**: As the soaring birds fly across the screen, they scale down dynamically:
  $$\text{scale}(x) = 1.0 - \left(\frac{x}{\text{SCR\_WIDTH}}\right) \times 0.6$$
  This creates a realistic 3D depth effect on a flat 2D projection.

### 🌊 3. Shaded Render Layering & Wave Equations
* **Painter's Algorithm**: Objects are drawn in a back-to-front stack sequence. Background layers (sky, mountains, celestial objects) are drawn first, followed by midground (houses, windmills, roadways), and foreground (river, waves, tree buffers) to ensure proper occlusion.
* **Fluid Wave Motion**: Water wave ripples are simulated using mathematical sine-wave offsets:
  $$y(x) = \text{base\_y} + \sin((x + \text{offset}) \times 0.04) \times 3.5$$

---

## 💻 Building and Compilation

This project is fully configured for **Code::Blocks** with the MinGW compiler suite.

### Linker Requirements
Make sure the following library flags are configured in your linker options:
```text
-lfreeglut
-lopengl32
-lglu32
-lwinmm
-lgdi32
```

### Setup Instructions
1. Open the project file [multi-scene-2d-animation.cbp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/multi-scene-2d-animation.cbp) in Code::Blocks.
2. Confirm your compiler directories contain the correct path to the GLUT headers (e.g., `C:/Program Files/CodeBlocks/MinGW/x86_64-w64-mingw32/include`).
3. Press **F9** to compile and launch the application.

---

## 📂 Code Files
* [main.cpp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/main.cpp) - Primary source containing the rendering pipelines, event handlers, and update loop.
* [multi-scene-2d-animation.cbp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/multi-scene-2d-animation.cbp) - Code::Blocks project file.
* [project_documentation.txt](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/project_documentation.txt) - Detailed glossary and Viva revision handbook.

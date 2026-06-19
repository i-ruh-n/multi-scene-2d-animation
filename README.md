# Multi-Scene 2D Animation (Cox's Bazar & Sajek Valley)

An interactive, multi-scene 2D graphical rendering and animation application built in **C++** using the **OpenGL Utility Toolkit (GLUT)**. The project showcases two of the most popular scenic destinations in Bangladesh: **Cox's Bazar Beach** and **Sajek Valley**, complete with dynamic environment transitions, user interactions, and custom traffic AI.

---

## 🎨 Visual Preview & Scenes

### 🌊 Scene 1: Cox's Bazar Beach
* **Background Elements**: Sun/Moon, drifting clouds, sea view with distant/foreground sailing ships, sandy beach, hotels, and a main roadway.
* **Ambient Cycle**: Supports real-time Day/Night toggle, transforming the sky, water coloring, hotel window lighting, stars, and vehicle headlights.
* **Interactive Rain**: Toggle rain on and off with a mouse click.

### ⛰️ Scene 2: Sajek Valley
* **Background Elements**: Mountain backdrop, elevated green plateaus, farm cottages, windmills, roadway, and a foreground river canal with sine-wave water ripples.
* **Dynamic Birds**: Flying birds that flap their wings using local rotation matrices and shrink as they fly further away (simulating depth scaling).
* **Interactive Cottages**: Bounding box collision checks that let you toggle individual cottage window lights on and off via mouse clicks when in Night Mode.

---

## 🕹️ Controls & Interaction Guide

| Input Method | Key/Event | Action |
|---|---|---|
| **Keyboard** | `Tab` | Switch between Cox's Bazar and Sajek Valley |
| | `N` / `n` | Switch to **Night Mode** (lights up buildings/headlights/stars) |
| | `D` / `d` | Switch to **Day Mode** |
| | `0` | **Stop Speed** (stops windmills in Scene 2, traffic in Scene 1) |
| | `1` | **Normal Speed** (default animation speed) |
| | `2` | **Fast Speed** (speeds up traffic and windmills) |
| | `ESC` | Exit application cleanly |
| **Mouse Left Click** | *Cox's Bazar* | Toggle dynamic falling **Rain** |
| | *Sajek Valley* | Click on a farmhouse to toggle its window light (Night Mode only) |

---

## ⚙️ Key Technical Features

### 1. Collision-Avoidance Traffic AI (2-Pass Propagation)
To prevent vehicles from overlapping and clumping on the two-way roadways, a custom **2-Pass Traffic AI** is implemented:
* **Pass 1 (Proximity Detection)**: Evaluates the distance between vehicles in the same lane. If a rear vehicle is within a safe threshold (130px) of a leading vehicle, it calculates slowing adjustments.
* **Pass 2 (Speed Propagation)**: Propagates velocity changes backward through the lane queue. In the leftward lane, speeds are adjusted using `max()`, while in the rightward lane, they are adjusted using `min()`, ensuring a uniform flow.

### 2. Depth Scaling & Local Transformations
* **Wing Flapping**: The birds fly and flap their wings using nested coordinate systems. The wing coordinates are isolated via `glPushMatrix()` and `glPopMatrix()`, translated to the body joints, and rotated using `glRotatef()`.
* **Depth Simulation**: Birds are scaled dynamically using `glScalef()` based on their horizontal coordinate ($X$), making them appear smaller as they approach the horizon.

### 3. Procedural Waves & Shading
* **Water Wave Shader**: The river canal's flowing water is simulated by computing dynamic offsets on a mathematical sine function:
  $$y(x) = \text{base\_y} + \sin((x + \text{offset}) \times 0.04) \times 3.5$$
* **Painter's Algorithm**: Strict layer rendering sequence (from background sky to foreground forest buffers) to ensure correct depth ordering without relying on a 3D Z-buffer.

---

## 💻 Installation & Compilation Setup (Windows & Code::Blocks)

This project is configured as a **Code::Blocks** project. Follow these steps to build and run it:

### Prerequisites
1. **Code::Blocks IDE** with the MinGW compiler suite.
2. **FreeGLUT** development libraries configured on your system.

### Linker Settings
Ensure the following libraries are linked in your compiler/linker settings:
* `freeglut`
* `opengl32`
* `glu32`
* `winmm`
* `gdi32`

### Building in Code::Blocks
1. Open the project file [hi.cbp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/hi.cbp) in Code::Blocks.
2. If necessary, update the compiler search directories:
   * **Compiler Search Dir**: `C:/Program Files/CodeBlocks/MinGW/x86_64-w64-mingw32/include`
   * **Linker Search Dir**: `C:/Program Files/CodeBlocks/MinGW/x86_64-w64-mingw32/lib`
3. Click **Build and Run** (or press `F9`).

---

## 📁 Repository Structure
* [main.cpp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/main.cpp) — Contains the core OpenGL rendering loops, animation, and logic.
* [hi.cbp](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/hi.cbp) — Code::Blocks project file.
* [project_documentation.txt](file:///e:/AIUB/Semester%2009/COMPUTER%20GRAPHICS%20%5BJ%5D/Lab%20Tasks/hi/project_documentation.txt) — Comprehensive Viva prep guide and code glossary.

# Jelly Cube Simulation
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Java](https://img.shields.io/badge/java-8%2B-orange)

---

## 📚 Table of Contents
- [About](#-about)
- [Features](#-features)
- [Installation](#-installation)
- [Usage](#-usage)
- [Configuration](#-configuration)
- [Project Structure](#-project-structure)
- [Contributing](#-contributing)
- [License](#-license)

---

## 📖 About
This project simulates a jelly cube made of elastic material using a **3D mass-spring system** with collision detection inside a bounding box. The simulation demonstrates realistic elastic dynamics and supports visualization with OpenGL (FreeGLUT 3.0.0).  

- **Cube dimensions**: 1m × 1m × 1m  
- **Bounding box dimensions**: 4m × 4m × 4m  
- **Discretization**: 512 mass points (8 × 8 × 8 grid) connected via structural, shear, and bend springs  
- **Forces considered**:  
  - Hooke’s law (spring forces)  
  - Damping forces  
  - Collisional forces (bounding box + inclined plane)  
  - External force fields (optional)  
- **Numerical integration methods**:  
  - Euler integration  
  - Runge-Kutta 4th Order (RK4) integration  

Two executables are included in `./Bin/Debug` (tested in Windows 11 64-bit arm):  
- `jello.exe` — runs the main jelly cube simulation.  
- `createWorld.exe` — generates input world files that specify initial conditions.  

World files are located in `./world` and define cube properties, environment settings, and collision objects.  

---

## ✨ Features
- 3D mass-spring network with structural, shear, and bend springs  
- Two integrators (Euler and RK4) for flexible simulation performance  
- Collision detection & response using the **penalty method**  
- Support for an **inclined plane** as an additional collision object  
- Configurable lighting and material properties:  
  - Sky blue background  
  - Red ambient global light  
  - Transparent, glossy orange jelly cube (with blending enabled)  
- Customizable elasticity and damping parameters through world files  
- Example world files demonstrating elasticity (`elastic.w`), rotation (`rotate.w`), behavior under gravity (`gravity.w`), etc.

---

## ⚙️ Installation
1. Install [FreeGLUT 3.0.0 (Win32)](http://freeglut.sourceforge.net/).  
2. Clone this repository:  
   ```bash
   git clone https://github.com/LeonardoZZN/Course_Projects.git
   cd cpp_jello_simulation
   ```
3. Build the project using your preferred C++ compiler with OpenGL and FreeGLUT linked. (Makefile is provided.)
4. The executables (`jello.exe`, `createWorld.exe`) will be generated.

---

## 🚀 Usage
1. Ensure your world files (*.w) are available in the `./world` directory.
2. To create a new world file:
```bash
[directory_of_the_executable]/createWorld.exe output.w
```
3. Experiment with parameters (spring constants, damping, timestep, external force field, etc.) by editing the world file directly or programmatically modifying the source code of `createWorld.cpp`.
4.  Run the simulation:
```bash
[directory_of_the_executable]/jello.exe [directory_of_the_world_file]/[.w file]
```

---

## 🔧 Configuration
- This project was developed and tested in Windows 11 64-bit arm.
- Simulation parameters are defined in world files (.w):
  - Cube properties: spring constants, damping coefficients, simulation timestep
  - Environment (required): bounding box size, collision properties
  - External forces (optional): force vector fields
  - Inclined plane (optional): defined by parameters (a, b, c, d)
- Example (elastic.w):
  - Hooke’s coefficient: 4500
  - Damping coefficient: 0.01
  - Timestep: 0.00025 (s)
  - Collision spring constants: 12000 (Hooke), 0.5 (damping)

---

## 📂 Project Structure
```pgsql
.
├── Bin/
│   └── Debug/
│       ├── jello.exe
│       └── createWorld.exe
├── world/
│   ├── elastic.w
│   ├── rotate.w
│   ├── gravity.w
│   └── other_world_files.w
├── createWorld.cpp
├── input.cpp
├── jello.cpp
├── physics.cpp
├── pic.cpp
├── ppm.cpp
├── showCube.cpp
└── README.md
```

---

## 🤝 Contributing
1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📜 License
Distributed under the MIT License. See LICENSE for more information.

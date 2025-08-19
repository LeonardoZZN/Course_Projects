# 🎥 Motion Capture Interpolation
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
This project implements multiple interpolation techniques to generate smooth and realistic human motion sequences from optical motion capture data.  

- **Input**:  
  - Skeleton structure (`.asf` files)  
  - Motion data (`.amc` files)  
- **Interpolation methods**:
  - Linear Interpolation (Euler angles)  
  - Bezier Interpolation (Euler angles)  
  - SLERP (quaternion)  
  - Bezier SLERP (quaternion)  

The skeleton is modeled hierarchically with joints represented as child nodes of a root node. Motions are recorded as Euler angles in `.amc` files. To create interpolation keyframes, `N` consecutive frames are removed from the motion sequence (where `N` is user-specified). The program then reconstructs the full sequence using the chosen interpolation method.  

Executables:  
- `./IDE-starter/VS2017/Debug/interpolate.exe` — Interpolation Framework: generates interpolated motion data. 
- `./IDE-starter/VS2017/Debug/mocapPlayer.exe` — GUI: visualizes skeletons and motion sequences.  

---

## ✨ Features
- Bezier interpolation for **Euler angles**.  
- SLERP and Bezier SLERP interpolation for **quaternions**.  
- Supports user-defined **keyframe removal parameter N**.  
- Visual comparison between original and interpolated motions.  
- Output: Graphs for analyzing joint angle curves, included in `./graphs`.  
- Output: Videos comparing interpolation quality across techniques.  

**Default Setup**:  
- Ground color changed to **fog blue** in `mocapPlayer`.  
- Custom **bone color cycling** (light green, light red, light blue) for clarity.  
- Performance measurement of interpolation algorithms (via `PerformanceCounter`):  
  - Linear Euler (~0.058s)  
  - Bezier Euler (~0.609s)  
  - SLERP quaternion (~0.747s)  
  - Bezier SLERP quaternion (~1.934s)  

---

## ⚙️ Installation
### Prerequisites
- [FLTK 1.3.8](https://www.fltk.org/) — must be downloaded in the **root directory**.  
- C++ compiler (Visual Studio 2017 recommended).  

### Steps
1. Clone this repository.  
2. Place **fltk-1.3.8** in the root directory.  
3. Open the Visual Studio solution and build the project.  

---

## ▶️ Usage
1. Navigate to executables:
   ```bash
   cd ./IDE-starter/VS2017/Debug/

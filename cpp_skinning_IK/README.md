# 🚶 Inverse Kinematics with Skinning
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Java](https://img.shields.io/badge/java-8%2B-orange)

---
## 📢 Acknowledgement
The starter code is provided for course CSCI 520 Computer Animation and Simulation (https://viterbi-web.usc.edu/~jbarbic/cs520-s25/), taught by Professor Jernej Barbič.
The project outline can be found on the course webpage: https://viterbi-web.usc.edu/~jbarbic/cs520-s25/assign3/.

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
This project implements **forward kinematics (FK)**, **inverse kinematics (IK)**, and **skinning** to enable interactive character deformation via skeleton manipulation.  

- **Character model**: OBJ mesh.  
- **Skeleton & skinning weights**: ASCII configuration files.  
- **Libraries used**:  
  - **OpenGL** — real-time rendering.  
  - **Eigen** — linear algebra operations.  
  - **ADOL-C** — automatic differentiation (required for IK).  

**Inverse Kinematics (IK)** is implemented using:  
- Tikhonov regularization (damped least squares).  
- Pseudoinverse method.  

**Skinning** is implemented using:  
- Linear Blend Skinning (LBS).  
- Dual Quaternion Skinning (DQS).  

---

## ✨ Features
### Core Accomplishments
1. **Forward Kinematics (FK)**  
   - Compute global transformations along the joint hierarchy from Euler angles.  
2. **Inverse Kinematics (IK)**  
   - Implemented with Tikhonov regularization (stable and smooth deformations).  
3. **Linear Blend Skinning (LBS)**  
   - Applied global transformation matrices from FK to deform the character mesh. 

### Additional Features
1. **Pseudoinverse IK Method**  
   - Compared to Tikhonov regularization.  
   - Test results across three demo models show:  
     - Comparable speed, but significantly less accurate.  
     - Prone to instability (model shivering, clipping issues) during large deformations.  
   - Images:  
     ```
     <project folder>/comparison/tikhonov_vs_pseudoinverse
     ```  
2. **Dual Quaternion Skinning (DQS)**  
   - Compared with LBS:  
     - Preserves volume better at joints during large rotations.  
     - Produces smoother, more realistic deformations.  
     - Slightly slower rendering speed compared to LBS.  
   - Images:  
     ```
     <project folder>/comparison/DQS_vs_LBS
     ```  

---

## ⚙️ Installation
### Prerequisites
- **ADOL-C** library  
- **Eigen** library  
- C++ compiler with OpenGL support  

### Steps
1. Clone this repository.
   ```bash
   git clone https://github.com/LeonardoZZN/Course_Projects.git
   cd cpp_skinning_IK
   ```
2. Place **ADOL-C** and **Eigen** in the same directory as the executable before compiling.  
3. Open the Visual Studio solution (`./VS2017/IK.sln`) and build the project.

---

## 🚀 Usage
1. Navigate to one of the three demo model directories:  
	```bash
	./armadillo
	./dragon
	./hand
	```
2. Run the executable:  
	```bash
	..\IK.exe <configFilename> <IKMethod> <SkinningMethod>
	```
	Parameters:
	- IKMethod:
		- t → Tikhonov Regularization Method
		- p → Pseudoinverse Method
	- SkinningMethod:
		- l → Linear Blend Skinning (LBS)
		- d → Dual Quaternion Skinning (DQS)
	Example:
	`..\IK.exe skin.config t l`
3. GUI Controls:
	- Right Mouse Drag: Rotate camera
	- Middle Mouse Drag: Zoom in/out
	- Left Mouse Drag: Select and manipulate IK handle
	- ESC: Exit
	- 0: Reset model to original shape
	- Tab: Toggle fullscreen
	- \: Reset camera view
	- =: Show skeleton hierarchy
	- w: Toggle wireframe visibility
	- e: Toggle model visibility
	- s: Toggle skeleton visibility

---

## 🛠 Configuration
- This project was developed and tested in Windows 11 64-bit arm.
- Source code is located in the root directory.
- The skin mesh data are located in `./armadillo/skin.config`, `./hand/skin.config`, and `./dragon/skin.config`.

---

## 📂 Project Structure
```pgsql
.
├── VS2017
│   └── IK.sln
├── armadillo/
│   └── skin.config                   # Config files for the armadillo model
├── dragon/
│   └── skin.config                   # Config files for the dragon model
├── hand/
│   └── skin.config                   # Config files for the hand model
├── comparison/
│   ├── tikhonov_vs_pseudoinverse/
│   └── DQS_vs_LBS/
├── IK.exe
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




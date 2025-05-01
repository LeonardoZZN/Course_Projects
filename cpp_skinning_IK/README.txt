CSCI 520 Assignment 3: Inverse Kinematics with Skinning
Author Name: Zining Zhu

|-----------Overview-----------|
This project implements forward kinematics (FK), inverse kinematics (IK), and skinning to allow users to deform a character by manipulating handles on a given skeleton structure.
The character itself is represented as an obj mesh, while the skeleton data and skinning weights come from given ASCII files.
The implementation utilizes OpenGL for rendering, Eigen for linear algebra, and ADOL-C for automatic differentiation required in IK.

|-----------Core Accomplishments-----------|
1. Implemented forward kinematics (FK) to compute global transformations along joint hierarchy based on input Euler angles of each joint.
2. Implemented inverse kinematics (IK) using Tikhonov regularization (damped least squares). 
3. Implemented Linear Blend Skinning (LBS) using the global transformation matrices output from FK.
The JPEGs for the animations of the three demo models are located at: <project folder>/animation

|-----------Additional Features-----------|
1. Implemented the pseudoinverse IK method and compared it with the Tikhonov regularization method:
	The comparison was executed on three demo models, in which linear blend skinning was applied and the same handle manipulations were used for each demo model.
	The three sets of comparison images are located at: <project folder>/comparison/tikhonov_vs_pseudoinverse
	The comparison of rendering speed (in release mode):
		For armadillo: 	Tikhonov: ~ 30.4 fps
				Pseudoinverse: ~ 26.5 fps
		For dragon: 	Tikhonov: ~ 4.6 fps
				Pseudoinverse: ~ 4.5 fps
		For hand: 	Tikhonov: ~ 75.1 fps
				Pseudoinverse: ~ 72.4 fps
	Based on the comparison result, though the pseudoinverse method is comparable to the Tikhonov method in rendering speed, its accuracy is significantly lower. 
	Specifically, the rendering output of the pseudoinverse method tends to be unstable (e.g., model shivering) and unrealistic (e.g., clipping issues), 
	particularly during large deformations. In contrast, the Tikhonov regularization method produces more stable and smoother deformations.

2. Implemented dual quaternion skinning (DQS) and compared it with linear blend skinning (LBS):
	The comparison was executed on three demo models, in which Tikhonov regularization was applied for IK and the same handle manipulations were used for each demo model.
	The three sets of comparison images are located at: <project folder>/comparison/DQS_vs_LBS
	The comparison of rendering speed (in release mode):
		For armadillo: 	LBS: ~ 30.4 fps
				DQS: ~ 29.2 fps
		For dragon: 	LBS: ~ 4.6 fps
				DQS: ~ 3.4 fps
		For hand: 	LBS: ~ 75.1 fps
				DQS: ~ 63.5 fps
	As shown in the comparison images, LBS results in volume loss at joints after large rotations (e.g., the skin unnaturally wrinkles). 
	In contrast, DQS encodes both rotation and translation using dual quaternions, allowing it to better preserve volume at bending joints and produce more realistic deformations. 
	In terms of rendering speed, DQS is slightly slower than LBS in release mode.

3. Modified the menu bar to display the applied IK method and skinning method during rendering.

|-----------Program Usage-----------|
1. How to run:
**Note: Please download the ADOL-C and Eigen libraries and place them in the same directory as the executable file before compiling the source code.
In powershell, go to one of the three directories: .\armadillo, .\dragon, or .\hand. 
Then, type: ..\IK.exe <configFilename> <IKMethod> <SkinningMethod>
Usage:
	IKMethod:
		t: Tikhonov Regularization Method
		p: Pseudo-Inverse Method
 	SkinningMethod:
		l: Linear Blend Skinning (LBS)
		d: Dual-Quaternion Skinning (DQS)
	Example: ..\IK.exe skin.config t l

2. GUI control: 
- Drag right mouse button to change camera angle.
- Hold middle mouse button and drag to zoom in or out.
- Left-mouse click to select an IK handle and drag to manipulate the mesh.
- 'ESC': Exit.
- '0': Reset model into the original shape.
- 'Tab': Full screen switch.
- '\': Reset camera view.
- '=': Show skeleton hierarchy.
- 'w': Change visibility of wireframe.
- 'e': Change visibility of model object.
- 's': Change visibility of skeleton.

|-----------Acknowledgement-----------|
The starter code is provided for course CSCI 520 Computer Animation and Simulation (https://viterbi-web.usc.edu/~jbarbic/cs520-s25/), taught by Professor Jernej Barbič.
The project outline can be found on the course webpage: https://viterbi-web.usc.edu/~jbarbic/cs520-s25/assign3/.


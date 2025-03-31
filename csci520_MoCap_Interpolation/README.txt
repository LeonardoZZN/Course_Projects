Author: Leonardo Zhu

Note: The source code along with all the AMC and ASF files used in this project are located at ./mocapPlayer-starter/. The two executable files (interpolate.exe and mocapPlayer.exe) are located in 
./IDE-starter/VS2017/Debug/.
This program uses fltk-1.3.8, which has to be downloaded in the root directory.

In this project, I implement Bezier interpolation for Euler angles, as well as SLERP and Bezier SLERP interpolations for quaternions to interpolate human motion data obtained
from an optical mocap system. The human model (skeleton, saved in .asf files) is structured hierarchically, with a root node and several child nodes representing different joints. The 
recorded motions are stored in .amc files, where joint rotations are encoded using Euler angles. To create keyframes for interpolation, N consecutive frames are removed from the
original motion, with N being a specified input parameter. The program generates a motion sequence of the same length as the original, based on the value of N and the chosen interpo-
lation method.

Author: Leonardo Zhu

Note: The executable files (jello.exe and createWorld.exe) are located in ./Bin/Debug. And the world files including parameters of initial conditions are located in ./world.

In this project, I simulated a jelly cube made of elastic material of dimensions 1 meter x 1 meter x 1 meter, constrained in the interior of a bounding box of dimensions 4 meters x 4 meters x 4 meters. To model the cube, I utilized a 3D mass-spring network that consists of structural, shear, and bend springs connected to 8 * 8 * 8 = 512 discrete mass points of equal mass. The dynamics of the cube is modeled by taking into account Hook's force and damping force exerted by the three types of springs listed above, collisional forces exerted by the bounding box, and an external non-homogeneous time-independent force field (if any). I implemented a function in physics.cpp to compute the net acceleration of a specific mass point. The resultant acceleration is used to numerically solve a system of ordinary differential equations based on Newton's second law to determine the position and velocity of every model point by each timestep. This program supports two time-stepping integrators that respectively apply Euler integration and Runge-Kutta 4th order (RK4) integration.
For collisions with the bounding box, I implemented collision detection and used the penalty method to model collision responses with imaginary springs embedded in walls of the bounding box.
To run the program, users have to provide an input world file that includes all necessary initial conditions of the jelly cube and the environment.

Beyond the basic function implementation, I also accomplished the following:
(1) Changed the lighting model.
Set background color to sky blue.
Modified global ambient light to red light.
Enabled full ambient reflection in the jelly material property.
Enabled blending for transparency of the jelly cube.
Modified the ambient, diffuse, specular, and emission coefficients in the jelly material property to make the jelly cube more transparent and appear glossy orange. 

(2) For the animation, I created a new world file: elastic.w with spring coefficients that enhances the jelly's elasticity.
Used the rotate.w as the starting point. 
Customized the Hook's coefficient for all elastic springs of the jelly cube to 4500 and the damping coefficient to 0.01, making the jelly cube more elastic.
I also reduced the timestep to 0.00025 to ensure the stability of the model. 
To further rigidify the walls of the bounding box, I increased the Hook's coefficient of collision springs to 12000 and the collision damping coefficient to 0.5.
Besides, I stretched one corner vertex of the jelly cube in the initial condition and assign the same initial velocity to all points, ensuring that the time evolution of the jelly's state is complex enough to fully demonstrate its elastic properties. 

(3) For extra credit, I implemented collision detection with an inclined plane whose parameters are defined by the input world file. For the collision response, I adopted the penalty method again with the same Hook's and damping coefficients as those of collision springs in the walls of the bounding box.
The contact point coordinates are determined as x = x_0 + a*t, y = y_0 + b*t, z = z_0 + c*t, where x_0, y_0, and z_0 are the coordinates of the current sampling point of interest in the jelly cube, a, b, and c are the input inclined plane parameters, while t = -(a*x_0 + b*y_0 + c*z_0 + d) / (a*a + b*b + c*c). 
To display the inclined plane, I implemented a function void showPlane(struct world* jello) in showCube.cpp in which I defined six clipping planes to cull the part of the inclined plane that is outside the bounding box.

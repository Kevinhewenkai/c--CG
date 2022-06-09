# Assignment 2 Justification
<!-- https://www.researchgate.net/figure/Quartic-trigonometric-Bezier-curves-and-the-other-curves_fig2_266501047 -->

If you think any of the questions within the sections are not applicable, please write "N/A".

## Section 1: Creation and Use of 3D Objects

In part 1, a,b and c, d are completed. The majority of the code for these is in the functions in the following list: `model::load` The structs xxxx and xxxx are also important for these functions.

### 1a: Show that you have created or imported at least one 3D object that is visible in your application. Include references to where this is implemented in your code.

There are 4 main kind of 3d object.
- aircraft(with size small, median, large)
  - line 76 - 114
- car
  - line 117 - 120
- plot light(cube)
  - line 128 - 139
- track(scene)
  - line 76 - 81

the path of the obj is at line **21-23** in the `main.cpp`

the load function is `model::load` which is in `model.cpp`, we load the objects in `main.cpp`

the struct of the objects is define in `scene::node_t` which is in `scene.hpp`

### 1b: Describe your scene graph and the hierarchy within the scene. Include references to where this is implemented in your code.

The scene is organised into a scene graph. The code structures are declared at line **71-74** in `main.cpp` and used from lines **76** to **139** to create the initial scene. Note how the smallAircraft is a child of medianAircraft, and medianAircraft is a child of largeAircraft.

### 1c: Describe how your objects are moving or animating in realtime. Include code references (line numbers and function names) to use of delta time and also how this affects the scene graph.

there are two object are moving or animating in realtime.

The delta time is define in line 146 in `main.cpp` by function `time_delta()` which is in line 218 - 225
- car
  - car keeps simple rotating in y axis
  - line 167
- aircraft
  - the aircraft is moving in a heart shape, the detail will show in 1d explaination

Moving the camera up from its starting position, you can clearly see the aircrafts in motion. Its child object median size aircraft is obviously attached to the large size aircraft and is also animating the same path relative to it. The code governing the changing relationship between the two starts at line 83 in file `main.cpp`.

### 1d: Describe how you have used parametric splines in your application. Include code references to the implementation of control point based curves in your project.

the moving track of aircraft is define by a bezier curve. the control point of the bezier curve is define at line **169-194**, the bezier curve function is defind at line **39-44** and be used at line **196-198**. The code are all in `main.cpp`

## Section 2: Camera

### 2a: Describe what controls the player should use to move the camera in your scene. Include code references to the implementation of delta time control of speed and use of user input in your project.

In this part, the user can use 
- w
  - front
- s 
  - back
- a
  - left
- d
  - right
- r
  - y axis up
- f
  - y axis down
- the mouse is used to control the camera angle

The functions are all in `euler_camera.cpp`, the `update_cam_angles` controls the camera angle, the `update_camera` change the postion of camera. line 51 define the camera speed,
the user input to control the movement of camera is defined at line **60 - 107**

### 2b: Describe how your camera interacts with the scene. Is it able to "walk" on the Z-X plane or follow terrain? Include code references.

If the user press `k`, the camera will change to walk mode, press `k` again will unable walk mode. In walk mode the y axis is fixed. Line **108 - 119** defined the user input, the walk mode will be implemented if this function is called. Line **63-66, 71-74, 79-82, 87-90, 95-98, 103-106** in `euler_camera.cpp` implement the functionality of the walk mode.

### 2c: Describe how the view of your scene can change from player controlled to application controlled (specify user input that makes this change happen). Include code references to use of parametric curves and delta time in the application controlled camera.

if the user press `u`, the user can get the view of the aircrafts.
the function `special_view` in the `main.cpp` is called if the user press `u`
the user input is defined at line **154-157**, `the special_view ` is called at Line **159 -152**

## Section 3: Lighting

### 3a: Describe where Diffuse Lighting is clear to see in your application. Include code references to your implemented lighting (including where this is implemented in your shaders).

car and the aircraft are colorful
for the directional light, the diffuse is defined at Line 52 at funtion `calc_dir_light` in `res/shaders/shader.frag` 
for the spot light the diffuse is defined at line 58-59 at function `calc_spot_light` in `res/shaders/shader.frag`

### 3b: Describe where Specular Lighting is clear to see in your application, including how the camera position affects the lighting. Include code references to your implemented lighting (including where this is implemented in your shaders).

if the user change the position of the plot light position by press `q` or `e`, the specular light(the white light) is easy to find
The calculation of the specular light is defined at line **60 - 63** at function `calc_spot_light` in `res/shaders/shader.frag`

### 3c: Describe which of your light(s) in your scene are point light(s) and how you have placed objects in the scene to clearly show the point lights functioning. Include code references to point light implementation.

there are two lighting cubics in the scene which are the plot lights.

one is white
- Line **127-132** (placed objects in the scene)

one is purple
- Line **134-139** (placed objects in the scene)

the plot light implementation is at `calc_spot_light` in `shader.frag`, the function is called at the main function of the `shader.frag` at line **85 - 90**
there are picture to show the influence of the lights (`withoutPlotLight.png` and `withPlotLight.png`)

### 3d: Describe all the lights in your scene and what type they are. Include code references to implementation of the capability to work with an arbitrary number of lights.

there is a direction light and two plot lights.
direction light calculation is defined at function `calc_dir_light` in `res/shaders/shader.frag` .
the color of direction light is yellow.
plot light calculation is defined at function `calc_spot_light` in `res/shaders/shader.frag`

Combining the lights together is at line **83-91** in res/shaders/shader.frag` .

### 3e: Explain what maps you have used and which objects they are on. Your application should clearly show the effect these maps are having on lighting. Include code references to maps on objects and vertices as well as lighting code that uses the maps.

there are two map
- diffuse map
  - start at line 8 at `shader.frag`
- specular map
  - start at line 11 at `shader.frag`

the difference you can try it at line 146-147 at `renderer.cpp`.

if you change to `glUniform1i(renderer.diffuse_map_loc, 1);` the scene will be black but the plot light is still working


## Section 4: Subjective Mark

### How have you fulfilled this requirement?

This project shows a reference to a scene from Justice League, the aircrafts are from this movie

### What is your vision for the scene you are showing?

I did not have a specific vision for this scene and do not need this to be marked.

### Are you referencing anything in this scene? Is this a quote or homage to any other art? Please describe it.

the aircrafts in Justice League

### Please give instructions for how a viewer/player should interact with this scene to see all the content you have created.

Write your answer here...

## Section 5 (optional): Code Style Feedback

### There are no marks assigned to code style in this assignment. However, if you would like to receive some feedback on your coding style, please point out a section of your code (up to 100 lines) and your tutor will give you some written feedback on the structure and readability of that code.

Write your answer here (lines numbers or function names that you'd like reviewed)

## Section 6 (optional): Use of External Libraries

### What does the library do?

no external library

### Why did you decide to use it?

Write your answer here...

### How does this integrate with the assignment project build system? Please include any instructions and requirements if a marker needs to integrate an external library.

Write your answer here...

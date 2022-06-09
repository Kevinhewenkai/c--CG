# Assignment 3 Justification

If you think any of the questions within the sections are not applicable, please write "N/A".
If you are answering a criteria in a different section, please describe that like: "4a. This criteria is met by the implementation described in 2b."

## Section 1: Design Pitch. Describe a possible setting and what kind of graphics technology would be necessary to implement this. These components do not have to be implemented in this project. Suggestion: 200 words should be more than enough for this.

Welcome to the 3421  Kevin's dynamic art show. What will be look like if our universe is split and then reorganized. What will remain, whether it is an existing object or an invisible spirit, or something we haven't discovered before, whether there is still light in this world, we don't know. In this work, the universe will have different spheres, and the audience can move the camera to enter each sphere to watch the reorganized world.

Our technology side will use the kernel to change the texture (e.g. make them blur) to decorate the items in the sphere. also cube map will be used to render the spheres. We'll need to make sure the engine can handle arbitrary reflections as well as temporal post processing effects. We need to use the bling light to make sure the lighting in some sphere is realistic, and we may use deferred shading to render a colorful world(have mant different color lighting resources), we will use hdr to make sure we can see the texture even we have so many light resources.

## Section 2: Advanced Lighting

### 2a: Blinn-Phong Lighting. Include code references to the implementation of lighting in your project.

In `shader.frag` from line 73 - 76

### 2b: High Dynamic Range. Show use of an intermediate frame buffer to store light values > 1.0 and how that is converted to Low Dynamic Range. Include code references.

In `framebuffer.cpp` I make a fucntion `make_hdr_framebuffer` to make framebuffer for hdr.
In `renderer.cpp` I make a function `tonemapping` to do the tone mapping

## Section 3: Reflections

### 3a: Environment Mapped Reflections. Show at least one object can reflect a cube map. Cube map can be a preprepared skybox and the object doesn't need to reflect other objects in the scene. Include code references to implementation.

There is a sphere in the scene which reflect a cube map, the setup code for the sphere is at `scene.cpp` line 52 - 52, the cube map is implemented by function `cubemap::make_cubemap();`, you can find this function at cubemap.cpp

### 3b: Realtime Cube Map. Describe how the cube map is created during the frame render and then used for a reflective object. Include code references to the generation of the cube map (3a should already describe the reflection process).

There is also a sphere pretends to do dynamic cubemap rendering. For creating the cubemap, I create a temporary camera (code is at `cubemap_camera.cpp`), the position of the camera is same as the position of the sphere, the camera take 6 picture(top, bot, front...)(`renderer.cpp` line 363 - 370) by looking at six angle and bind the picture to the framebuffer of each face of cubemap(line 389, function `renderscene`).

### 3c. (5%) In-world Camera/Magic Mirror. Render the scene from a camera to a texture that is then displayed somewhere else in the scene.
There is a portal which is pretend to show the view capture by another camera. The code is in `renderer.cpp`, line 402 - 430

### 3d: Planar Reflections. Describe the system used to generate a planar reflection, including any techniques for perspective and/or near plane correction. Include references to code.

Same as 3c, 3b

## Section 4: Post Processing

### 4a: Simple Colour Change. Describe how this project uses a framebuffer and what change is made to the colour data, including references to where this is implemented in code.

will explained in 4b

### 4b: Use of Kernels. Describe the kernel sampling technique in use, including code references.

I use two shader for two post processing. (`shader2d.frag`, `shader2d_2.frag`), when you running the code, please goes into the iterior, and press `U` and `I`, you will see the change made by kernel. The code implementing the framebuffer is at `main.cpp` line 193 - line 245

### 4c: Multiple Intermediate Framebuffers. Show use of more than one framebuffer for post processing (like chaining two post processing effects in a row). Include references to code.

please goes into the iterior, and press `U`, you will see the first post processing.
Then press `I`, you will see the second post processing

### 4d: Multiple Render Targets. Describe how this project uses multiple frame buffers to write information from the initial fragment shader pass and how it uses them in post processing. Include references to fragment shader output code as well as post processing code.

I have tried defer rendering, in `defer_renderer.cpp` but failed.

### 4e: Temporal Post Processing. Show storage of framebuffers from previous frame(s) and how they are being used in post processing. Include references to where this is happening in code.

I did not complete part 4e. (If you would like your 4e to be marked, you will need to replace this line)

## Section 7: Show Me What You Got. Implement any techniques that are not part of previous criteria for assignments in this course. This is open, so must be documented clearly with code references to show what has been created to add to the visual effects of the scene or objects in it. 

Anti-Aliaising
In `framebuffer.cpp`, the `make_framebuffer` and make `hdr_buffer` function
In `main.cpp`, from line 193 - line 245

## Section 8: Subjective Mark

### How have you fulfilled this requirement?



### What is your vision for the scene you are showing?

Depict the scenes in the universe where humans do not live, but exist in our universe, put them on a separate planet, and depict these places getting farther and farther away from us. 
### Are you referencing anything in this scene? Is this a quote or homage to any other art? Please describe it.

The inverse of "There are green, where there is life."

### Please give instructions for how a viewer/player should interact with this scene to see all the content you have created.

Go into the interior and press "U" and "I"

## Section 9 (optional): Code Style Feedback

### There are no marks assigned to code style in this assignment. However, if you would like to receive some feedback on your coding style, please point out a section of your code (up to 100 lines) and your tutor will give you some written feedback on the structure and readability of that code.

Write your answer here (lines numbers or function names that you'd like reviewed)

## Section 10 (optional): Use of External Libraries

### What does the library do?

Write your answer here...

### Why did you decide to use it?

Write your answer here...

### How does this integrate with the assignment project build system? Please include any instructions and requirements if a marker needs to integrate an external library.

Write your answer here...

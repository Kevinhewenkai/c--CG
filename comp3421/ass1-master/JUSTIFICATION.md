# Assignment 1 Justification

If you think any of the questions within the sections are not applicable, please write "N/A".

## Section 1: It runs

### How have you fulfilled this requirement?

This project completes this task and I have tested that the application builds with no errors and runs under the assignment environment provided by the course.

### When your application is run, what should the tutor look for or do to see that you have fulfilled this requirement? 

My project is based on the assignment project provided by the course. I have made changes to the following files `src/main.cpp`, `res/shader/frag.glsl`, `res/shader/vert.glsl`  and have added the following files `img/pokemon_ball.png`, `img/pokemon.png`, `img/background.png`.

### Where in the code should tutors look to see that you've fulfilled this requirement?
 I have made changes to the following files `src/main.cpp`, `res/shader/frag.glsl`, `res/shader/vert.glsl`  and have added the following files `img/pokemon_ball.png`, `img/pokemon.png`, `img/background.png`.


## Section 2: Visibility & Shaders

### How have you fulfilled this requirement?

This project completes this task and includes shaders that compile and function correctly.

### When your application is run, what should the tutor look for or do to see that you have fulfilled this requirement?

When running the built project, you should immediately see two squares with pictures of Pikachu and pokemon ball, the background is also a texture

### Where in the code should tutors look to see that you've fulfilled this requirement?

The shaders' file are at `res/shader`, and I create shader at line 169-170 at the main function,  and I bind the position and the texture position at line 120-124, uniform the shader at 190-191

## Section 3: Creation and Use of 2D Shapes

### How have you fulfilled this requirement?

This project completes this task and includes shaders that compile and function correctly.

### When your application is run, what should the tutor look for or do to see that you have fulfilled this requirement?

You can see when you run the application that there is a Pikachu and a pokemon ball near the centre of the view, and there is a grass background. 


### Where in the code should tutors look to see that you've fulfilled this requirement?

The code that relates to that object is in the file `src/main.cpp`. You can see the vertices and indices at lines 85-130, 164-166 and the code invoking the OpenGL calls at lines 164-166, 214-224, 227-237, 240-250 that make sure that this draws.

## Section 4: Use of Textures

### How have you fulfilled this requirement?

This project completes this task and includes shaders that compile and function correctly.

### When your application is run, what should the tutor look for or do to see that you have fulfilled this requirement?

There is a Pikachu and the pokemon ball which are textures. The pokemon ball is rotating and the pikachu is being small. 

### Where in the code should tutors look to see that you've fulfilled this requirement?

The texture file is called `res/img/pokemon.png`, `res/img/pokemon_ball.png`. It is applied to the object named `pokemon`, `pokemon ball`, `background` using the code at lines 178, 184, 187. Running the application, the square in the middle of the window should show the pokemon ball is shaking and the pikachu is being small and goes into pokemon ball, after the Pikachu entering the ball, the ball will stop shaking in seconds and the code invoking the OpenGL calls at lines 214-224, 227-237, 240-250 that make sure that this draws. 


### Section 5: Subjective Mark

### How have you fulfilled this requirement?

yes

### When your application is run, what should the tutor look for or do to see that you have fulfilled this requirement?

It shows a pikachu is caught by a pokemon ball. I search the picture from the google image. You can click the mouse to make Pikachu large to help him out of the ball.

### Where in the code should tutors look to see that you've fulfilled this requirement?
the shaking part is at line 258-267, the mouse click part is at line 151-156,198-199

## Use of External Libraries

### What does the library do?

I think I haven't use any other library

### Why did you decide to use it?


## Project title

Mango Moon

## Team members

Liam Wilbur and Weston Keller

## Project description

A 70s Arcade Lunar Lander game with a twist... players activate the thrusters of their Mango Lander using their voice! The custom controller allows precise rotation of the lander, which allows players to attempt landings on various plateaus, craters, and lunar surfaces. Hardware uses a rotary encoder and KY-038 sound sensor for I/O, and a vibrating disc motor for haptic feedback.

## Member contribution

Weston: Player physics/movement, Mango Lander / Moon / Explosion pixelart and animation, Bitmap reading functions, Start/Win/Crash/OOB/Fuel Empty State control flow & display screens. Modeled and 3D printed controller from scratch.

Liam: Trig tables, Rotary Encoder + Player rotation Logic, Mountain Drawing + Landing Zones, Framebuffer/Redraw Issues, Zoom In Screen when near Landing Zone, Aesthetics, Game State Control

## References

Weston:
1. Physics Resources - It was interesting to learn how game engines often tie player movement/animation to frame rates. After trying out a traditional Newtonian acceleration->velocity->position update pipeline, I found that in order to maintain the granularity of dx/dy needed for angular movement, while also updating the position every frame without moving too fast, it gave me more control to use a fixed time-step and scale positional outputs in more custom ways to create drag and responsive angular control. These resources were helpful:
https://linustechtips.com/blogs/entry/1503-why-are-physics-engines-tied-to-frame-rates/
https://www.gamedev.net/forums/topic/318184-accelerating-in-2d-space-to-maximum-velocity/
https://www.youtube.com/watch?v=hG9SzQxaCm8 (Thanks, Haven!)
https://medium.com/@brazmogu/physics-for-game-dev-a-platformer-physics-cheatsheet-f34b09064558
3. I referenced the font.c bitmap implementation when creating my sprite-drawing functions. I decided that storing bitmaps separately was the right choice, because I needed them to be of differing dimensions depending on which game object was being drawn.
4. Since C isn't an OOP language, we had to use structs to define game objects and create inheritances between things like our game manager and player. I found this blog extremely helpful when organizing the framework/control flow of our game:
https://cecilsunkure.blogspot.com/2012/04/object-oriented-c-class-like-structures.html

Liam:
1. Lunar Lander Online - HUGE inspiration and reason for the project. We based the project off the old Atari version of Lunar Lander, but being able to actually play a version of the game was incredibly helpful.
https://arcader.com/lunar-lander/
4. Reading rotary encoders as a state machine - Reading the rotary encoder was something I had worked on for the assignment 2 extension, but I wanted to incorporate it properly for this project. This forum post was helpful as a basic framework to translate into code using a better method than the one I had started with. Translating that input into the rotation of the actual sprite had a few issues involving debouncing or false inputs, but after those were resolved the rotary encoder blessed us throughout the project.
https://forum.arduino.cc/t/reading-rotary-encoders-as-a-state-machine/937388?_gl=1*1t6v9ut*_up*MQ..*_ga*MTIzMTYzMDMyLjE3NDIzNDk4OTQ.*_ga_NEXN8H46L5*MTc0MjM0OTg5NC4xLjAuMTc0MjM0OTg5NC4wLjAuNTkxNzEzNzM3
6. Xiaolin Wu's Line Algorithm Video - Incredibly helpful walkthrough of the logic of Xiaolin Wu's line algorithm which served me well for antialiasing. I originally attempted to use the Gupta Sproull algorithm based off its wikipedia page, but it failed when the slope of the line was greater than 1. After switching, it wasn't too difficult to implement.
https://www.youtube.com/watch?v=f3Rs20k-hcI

## Self-evaluation

We feel very happy with how many of our reach goals we were able to tackle! We feel that we struck a good balance in the movement system between being fun to maneuver, responsive, and appropriately challenging. We're proud of the aesthetics of the project, and the graphics performance choices we were able to implement to create smooth animation, minimize the redrawn portion of the screen, and transition quickly between scenes. We learned a lot about the pipeline for game state management, sprite instantiation / graphical redrawing, and creating satisfying interactions between hardware and gameplay. The end product is something we are so proud of and excited to show off.

## VIDEO DEMO

Full Demo
https://www.youtube.com/watch?v=Rgz2xvjeFv8

2X Speed
https://www.youtube.com/watch?v=WcCZ6NbiHEM 

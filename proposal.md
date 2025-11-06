## Title of your project
Mango Lander

## Team members
Liam Wilbur and Weston Keller

## Project goals
OVERALL GOAL
    You are landing a spaceship on Planet Pi, and you want to touch down softly! Using your comms (microphone input), you can activate your spaceship thrusters to slow your landing. You can rotate your ship using a potentiometer dial! Game interface is similar to the 70s arcade game Lunar Lander, running on our graphics library. We would like to design fun housings for our microphone and joystick/potentiometer based on the original Atari controllers, if time permits.

REACH GOAL
    Our dream is to have the spaceship respond with directional thrusting based on its rotation, graphically redrawn in response to our potentiometer position. Thrusters will activate and stay on for one second when audio input is greater than a certain threshold, using interrupts to activate thrusters only when the data line of our ADC microphone is above a certain line. However, we have adaptable sub-goals if this proves infeasible in the time frame:

FALLBACK SCOPE
    We will begin by implementing a library that translates analog volume/dB amplitude data into a digital signal, which we will later read to activate thrusters. We will need to create game states for start, playing, crashing, and winning, and spaceship/terrain game objects. Since smooth movement may be difficult for our framebuffer to handle, we will start by having one threshold where the thrusters will activate, and attempt to scale thrusting based on dB amplitude later if possible. Directional thrusting based on potentiometer rotation will also be a reach goal - we will start by thrusting in the up direction.

## Resources, budget
We will need either a Digital Sound Sensor Module such as KY-038, which will get a digital HIGH/LOW signal off the bat, or a MEMS Microphone with Threshold Detection in addition to an ADC that runs on I2C or SPI.

## Tasks, member responsibilities
Weston - Configuring decibel sensor to map to spaceship thrusting threshold, Graphics
Liam - Configuring joystick using potentiometer to map to character rotation, Physics

## Schedule, midpoint milestones
Buy and configure decibel sensors/joystick.
Build graphics of lunar lander game while parts are coming, implement physics of basic falling and thruster acceleration
Convert audio input to digital signal of decibel sensors
Rework lunar lander to map acceleration to a specified decibel threshold and map direction to the joystick
Polish game experience to have score, different landing targets, and icons
Design and 3D print stylish controller housing
Our one-week goal is to have ordered our parts and created a graphic of a square that falls across the screen and thrusts up when space is pressed (each space triggers a 1-second thrust).

## Additional resources, issues
The biggest risk is the game being unplayable because of slow redraws, given all the interrupts that are necessary. We would also appreciate guidance on making sure we have the right sensors to accurately and consistently read a digital threshold for audio dB. We are still unsure of whether it would be most advisable to use an SPI or I2C ADC to convert from a pure analog microphone amplifier, or whether digital dB sensors are preferable for our use case. Finally, we don’t have much experience with game building, so if there are resources that past students have found useful in jump-starting object oriented game/state design, we would love to see them! Thank you so much, this will be fun!

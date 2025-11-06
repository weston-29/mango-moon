/*
 * CS107E Winter 2025 | Liam Wilbur & Weston Keller
 * Mango Moon Arcade Game with Auditory Thrusters and Rotational Controls
 */
#include "assert.h"
#include "uart.h"
#include "mymodule.h"
#include "fb.h"
#include "gamegl.h"
#include "timer.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "trig.h"
#include "printf.h"
#include "malloc.h"
#include "strings.h"

#define GAMEWIDTH 800
#define GAMEHEIGHT 600

// GPIO Pins
const gpio_id_t BUZZER = GPIO_PB3;
const gpio_id_t MIC = GPIO_PB4;

// Graphics Message String Constants
const char *welcomemsg = "WELCOME TO MANGO MOON";
const char *instructmsg = "Use your COMMS to activate THRUSTERS";
const char *crashmsg = "SPLAT! MANGO LANDER WAS SMUSHED :(";
const char *playagainmsg = "THRUST TO RELAUNCH";
const char *newgamemsg = "Celebrate loudly to play again...";
const char *oobmsg = "COMMS OUT OF RANGE!";
const char *nofuelmsg = "OUT OF FUEL!";
const char *winmsg = "MANGO SAFE AND SOUND! WELL DONE, PILOT.";

// Starting Player States
static const int STARTX = 0;
static const int STARTY = 5;
static const int FULLFUEL = 5000;
static const int CRASHPENALTY = 100;

static int HIGHSCORE = 0;

// Physics Constants
static double XDRAG = 1.0;
static double YDRAG = 0.90;

// Change these to affect max speed
static const int dxMAX = 2; // Maximum positional displacements per frame.
static const int dyMAX = 3;

double GRAVITY = 2.0; // Force of Gravity
const int GRAV_DAMPER = 2; // Increase this value to make the effects of gravity weaker
const double THRUST = 10.0; // Force of Thrust
const double DTIME = 2.0; // After experimentation, using constant time made for smoother movement than
						  // FPS-based physics.
static int curFrame;

enum { FALSE, TRUE };
enum { STARTSTATE, GAMESTATE}; // used to tell certain graphics functions what state we're in

static int RESTART = 0; // delay for thrust upon start


static struct lander {
	int x, y; // position
	double vx, vy;
	double ax, ay;
	int rotation; // increments 15 degrees on each rotary encoder click
	int thrusting; // functions as bool
} lander;

static struct gamestate {
	struct lander player;
	int crashed; // out of bounds or failed landing game end state
	int won; // successful game end state
	int running; // main gameplay state
	int out_of_bounds; // mark if player is offscreen
	int mountainState; // 0 for default, #1-3 correspond to the mountainscape for zoom ins on landing spots
	
	long Fuel; // Display variables for stats
	long Score;
} gamestate;

static struct rotaryEncoder{
	int state; // tracks what current checks should be for clockwise/counter-clockwise
	int turnDirection; // 1 for clockwise, 0 for counter-clockwise, -1 for no turn
	int rotaryAngle; // -90 to 90 degrees
} rotaryEncoder;


void gamewindow_init(int gamewidth, int gameheight, color_t background) {
	gamegl_init(gamewidth, gameheight, GL_DOUBLEBUFFER); // init buffer
	gamegl_clear(background);
	gamegl_swap_buffer(); // display background window
}

/* Gives the initial state of all lander physics variables to instantiate at starting pos
   Movement is initialized in main() game loop when player is instantiated. */
void lander_init(struct lander *player) {
	player->x = STARTX; // Starting
	player->y = STARTY;	// Coordinates
	player->vx = 4; // This gives initial "wind" acceleration as player flies in from left
	player->vy = 0;
	player->ax = 0;
	player->ay = 0;
	player->rotation = 90;
	player->thrusting = FALSE;
}

// NUMBER PROCESSING FUNCTIONS: Rounding functions and String Conversions to Display Stats Onscreen

// Alternate rounding functions: rounds to nearest int rather than truncating
// Returns 1 in all cases of float being -0.5 < f < 0.5
int doubtoint(double num) {
	// by adding 0.5 before truncating, decimals < 0.5 round down & decimals >= 0.5 round up
	int retval;
	if (num < 0.5 && num > 0) {
		return 1;
	}
	else if (num > -0.5 && num < 0) {
		return -1;
	}
       	return (int)(num >= 0 ? num + 0.5 : num - 0.5);
}

// converts single digit long to char form (10-15 map to hex a-f)
char convtochar(unsigned long num) {
	char result;
	if (num < 10) {
		result = num + '0';
	}
	else if (num >= 10) {
	    result = 'a' + (num % 10);
	}
	return result;
}

void conv_num_to_string(unsigned long num, int base, char *outstr) {
	char tempbuf[20]; // buf size given by num chars needed to represent max possible unsigned long
	int i = 0;
	if (num == 0) {
		tempbuf[i] = '0'; // this is better than ASCII value 48
		i++;
	}
	while (num != 0) {
	    char digit = convtochar(num % base);
		tempbuf[i] = digit; // load digits into temporary buffer, processed in reverse order
		num /= base;
		i++;
	}
	for (int j = 0; j <= i; j++) {
		outstr[j] = tempbuf[i - 1 - j]; // read tempbuf digits into outstr in correct (reverse) order
	}
	outstr[i] = '\0'; // null-terminate
	return;
}

#define MAX_DIGITS 25
char *to_decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        conv_num_to_string(-val, 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        conv_num_to_string(val, 10, buf);
    }
    return buf;
}

void update_landerpos(struct lander *player) {
	player->vx *= XDRAG; // Residual velocities dampen over time: adjust Drag variables to change how much resistance the lander feels
	player->vy *= YDRAG;

// check if if statement causing issues, also always change v based on a 
	// player->thrusting = 1;
	// player->vx += (player->ax * DTIME);
	// player->vy += (player->ay * DTIME);
	if (player->thrusting) { // this will be determined by microphone threshold breached
		int theta = (player->rotation + 180) % 360; // Bound angle and invert to propel upwards
		player->ax = (double)((sin_table[theta] * THRUST) * -1.0);
		player->ay = (double)((cos_table[theta] * THRUST) * -1.0); // Calculate x and y acceleration by thrust angle
		
		// Bound x velocity at upper constant
		if (player->ax > dxMAX) {
			player->vx = dxMAX;
		} else if (player->ax < -dxMAX) {
			player->vx = -dxMAX;
		} else {
			player->vx = (player->ax);
		}

		// Bound y velocity at upper constant
		if (player->ay > dyMAX) {
			player->vy = dyMAX;
		} else if (player->ay < -dyMAX) {
			player->vy = -dyMAX;
		} else {
			player->vy = (player->ay);
		}
		
		// update onscreen position	
		int dx = (int)(player->vx / DTIME);	
		player->x += dx;
		
		int dy = (int)(player->vy / DTIME); 
		player->y -= dy;
	}

	// Apply gravity
	player->x += (int)(player->vx/2); 
	player->y -= (int)(player->vy/2); // residual from thrusting
	if (curFrame % (2 * GRAV_DAMPER)) { // 2 * is to prevent case of GRAV_DAMPER=1 which is always 0 when modulo'd 
		player->y += (int)(GRAVITY / DTIME);
	} // Modify strength of gravity
}

// rounds number to closest multiple of 15 for rotation graphics
int roundto15(int n) {
        int remainder = n % 15;
	int num = 0;

	// round up to higher 15
	if (remainder > 7) {
           num = n + (15 - remainder);
        }
	// round down to "lower" 15 in case of negatives
	else if (remainder < -7) {
           num = n - (15 + remainder);
        }
	// round to lower 15
	else {
	   num = n - remainder;
	}

	return num;
}


// compares previous values of pin A,B with current values to detect rotation of rotary encoder
// returns 1 for right turn, -1 for left turn, 0 if no turn detected
void rotation(void *auxdata) {
         // explicitly define currA, currB
         int currA = gpio_read(GPIO_PD14);
         int currB = gpio_read(GPIO_PD15);
         
         // set to 0 in case no turn is detected
         rotaryEncoder.turnDirection = 0;
         
         switch(rotaryEncoder.state) {
            // when no previous turn
            case 0:
           // clockwise check
               if (!currA) {
                  rotaryEncoder.state = 1;
           }
           // counter-clockwise check
           else if (!currB) {
                  rotaryEncoder.state = 4;
               }
           break;
            // continued clockwise rotation
            case 1:
           if (!currB) {
                  rotaryEncoder.state = 2;
               }
           break;
            // continued continued clockwise rotation
            case 2:
           if (currA) {
                  rotaryEncoder.state = 3;
               }
           break;
            case 3:
           if (currA && currB) {
                  rotaryEncoder.state = 0;
                  rotaryEncoder.turnDirection = -1; // make 1 for final clockwise movement to use for rotation
               }
           break;
            // counterclockwise rotation
            case 4:
           if (!currA) {
                  rotaryEncoder.state = 5;
               }
           break;
            // continued counterclockwise rotation
            case 5:
           if (currB) {
                  rotaryEncoder.state = 6;
               }
           break;
         // continued continued counterclockwise rotation
            case 6:
           if (currA && currB) {
                  rotaryEncoder.state = 0;
                  rotaryEncoder.turnDirection = 1; // return -1 for official counterclockwise movement to use for rotation
               }
           break;
         
         }
         gpio_interrupt_clear(GPIO_PD14); // explicitly clears interrupt for pinA
         gpio_interrupt_clear(GPIO_PD15); // explicitly clears interrupt for pinB
}

void configureRotary(gpio_id_t pinA, gpio_id_t pinB) {
         gpio_set_input(pinA);
         gpio_set_input(pinB);
         gpio_set_pullup(pinA);
         gpio_set_pullup(pinB);
         
         // set up interrupt for pin rotations
         gpio_interrupt_init();
         gpio_interrupt_config(pinA, GPIO_INTERRUPT_DOUBLE_EDGE, 1);
         gpio_interrupt_config(pinB, GPIO_INTERRUPT_DOUBLE_EDGE, 1);
         gpio_interrupt_register_handler(pinA, rotation, NULL); // last parameter is auxdata
         gpio_interrupt_register_handler(pinB, rotation, NULL);
         gpio_interrupt_enable(pinA);
         gpio_interrupt_enable(pinB);
}

void redrawMountains(int state) {
	 gamegl_clear(GL_BLACK);
	 gamegl_draw_mountains(state);
	 gamegl_swap_buffer();
	 gamegl_clear(GL_BLACK);
	 gamegl_draw_mountains(state);
         GRAVITY = 5;
}

void main(void) {    	
	// inline assembly to enable hardware floating-point calculations
	uint32_t mstatval = 0;
	// read current mstatus value into variable
	asm volatile ("csrr %0, mstatus" : "=r" (mstatval));
	// write desired mstatval and load into mstatus register
	mstatval |= (1 << 13);
	asm volatile ("csrw mstatus, %0" : : "r" (mstatval));
	
	interrupts_init();
	uart_init();
	generate_tables(); // populate cos/sin lookup tables

	// Setup for KY-038 dB Threshold Sensor and Buzzer
	gpio_init();
	gpio_set_function(MIC, GPIO_FN_INPUT);
	gpio_set_pullup(MIC);
	gpio_set_function(BUZZER, GPIO_FN_OUTPUT);

	// Setup for Rotary Encoder
        gpio_id_t pinA = GPIO_PD14;
        gpio_id_t pinB = GPIO_PD15;
        configureRotary(pinA, pinB);
        rotaryEncoder.state = 0;
        rotaryEncoder.turnDirection = 0;
	rotaryEncoder.rotaryAngle = 0;
	
	struct gamestate game; // Instantiate Game Manager
	curFrame = 0; // Frame Counter

	gamewindow_init(GAMEWIDTH, GAMEHEIGHT, GL_BLACK);
	
	
	// START SCREEN PHASE
start_screen:
	
	lander_init(&game.player);	
	
	int xcoord = 0;
	int soundmade = gpio_read(MIC);
	int thrustpast = 0;
	float speedmult = 1.0f;
        rotaryEncoder.turnDirection = 0;
	rotaryEncoder.rotaryAngle = 0;
	
	while (!thrustpast) {
		if (soundmade == 1) {
			speedmult += 0.2f;
		} // if thruster has been activated, continue accelerating

		if (xcoord > GAMEWIDTH && speedmult > 1.0f) {
			thrustpast = TRUE;
		} // if player has successfully tried thrust, set flag to enter gameplay on next pass

		// Draw Start Menu Graphics onscreen
		gamegl_clear_height(GL_BLACK, 350);
		gamegl_draw_constellations();
		gamegl_draw_stars(STARTSTATE);
		gamegl_draw_moon(150, 100, 0xff919191);
		gamegl_draw_string(100, 280, welcomemsg, GL_ORANGE);
		gamegl_draw_string(200, 320, instructmsg, GL_WHITE);

		gamegl_draw_string(300, 450, "HIGH SCORE: ", GL_SILVER);
		char* scorestr = to_decimal_string(HIGHSCORE);
		gamegl_draw_string((300 + 168), 450, scorestr, GL_SILVER);
		
		// Draw mountains and stars in silver
		gamegl_draw_lander(xcoord, 300, 90, GL_SILVER);
		if (soundmade) {
			gamegl_draw_effect(xcoord - 14, 300, 90, 1, 0, GL_SILVER);
		}
		xcoord = (xcoord > GAMEWIDTH) ? 0 : (xcoord + ((int)speedmult)); // loop around screen
		gamegl_swap_buffer();

		if (soundmade == 0) {
		        soundmade = gpio_read(MIC);
		}
	} // END OF START SCREEN PHASE
;
	
	// Set pre-gameplay values for Game Manager and Player
	game.running = TRUE;
	game.crashed = FALSE;
	game.out_of_bounds = FALSE;
	game.won = FALSE;
	//game.player.x = STARTX;
	//game.player.y = STARTY;
	game.mountainState = 0;
	//game.player.rotation = 90;
	rotaryEncoder.rotaryAngle = 90;
	game.Fuel = FULLFUEL;
	game.Score = 0;

	// draw initial mountain state to both buffers
	redrawMountains(0);
	GRAVITY = 2.0; // This is the setting for zoomed-out gameplay

	interrupts_global_enable();

	// MAIN GAME LOOP
	while (game.running) {
	    
		int readval = gpio_read(MIC);
		
		// Beautiful code to ensure the player is launched sufficiently upon restart
		if (RESTART) {
			game.player.thrusting = FALSE;
			RESTART--;
		} else {
			game.player.thrusting = (readval == 1) ? TRUE : FALSE; // set thrusters
		}
				
		if (game.player.thrusting) { gpio_write(BUZZER, 1); }
		
		// Operate under constant time interval / frame count rather than FPS dTime calculation for smoothness
		curFrame++;

               // ROTARY ENCODER ROTATION LOGIC:
               // rotaryEncoder.turnDirection 1 for clockwise, -1 for counter-clockwise, which is returned by interrupt function 'rotation'
		int rotationRate = 10; // rate at which the angle changes for each turn
		// right turn
		if (rotaryEncoder.turnDirection == 1) {
		   // keep angle within 90 bound
		   if (rotaryEncoder.rotaryAngle < 90) {
		      rotaryEncoder.rotaryAngle += rotationRate;
		   }
		}
		// left turn
		else if (rotaryEncoder.turnDirection == -1) {
	           // keep angle within -90 bound
		   if (rotaryEncoder.rotaryAngle > -90) {
		      rotaryEncoder.rotaryAngle -= rotationRate;
		   }
	        }
		rotaryEncoder.turnDirection = 0;

		game.player.rotation = roundto15(rotaryEncoder.rotaryAngle);
		// printf("\nCurrent Rotation: %d\n", game.player.rotation);

		// initialize previous location to draw black rectangle over
		int prevX = game.player.x;
		int prevY = game.player.y;

	   	// Update Player Position Behind the Scenes
		update_landerpos(&game.player);



		// DIRTY REDRAW METHOD FOR ZOOM IN STATES
		// draw over previous player sprite if in zoom in screen and not near any white pixels
		int quickClear = 0;
		int maxCheckX = prevX + 40;
		int maxCheckY = prevY + 40; // check 3 pixels of sprite to save space
                int startX = prevX - 20;
                int startY = prevY - 20;
                int rectWidth = 50;
                int rectHeight = 50;
                int endX = startX + rectWidth; 
                int endY = startY + rectHeight; 

                // Check if bounding box top row and bottom row contains white
                for (int j = startX; j < endX; j++) {
                    if (gamegl_read_pixel(j, startY) == GL_WHITE) {
                        quickClear = 1;
	                break;
                    }
                    if (gamegl_read_pixel(j, endY - 1) == GL_WHITE) {
                        quickClear = 1;
	                break;
                    }
                }
                
                // Check left column and right column
                for (int k = startY; k < endY; k++) {
                    if (gamegl_read_pixel(startX, k) == GL_WHITE) {
                        quickClear = 1;
	                break;
                    }
                    if (gamegl_read_pixel(endX - 1, k) == GL_WHITE) {
                        quickClear = 1;
	                break;
                    }
                }


                // for zoom in screens, quickClear boolean enables dirty method to draw over only player sprite
		if (!quickClear && (game.mountainState != 0)) {
		        // draw over previous player sprite
		        gamegl_draw_rect(prevX - 20, prevY - 20, rectWidth, rectHeight, GL_BLACK);
                        gamegl_clear_height(GL_BLACK, 100); // keep redrawing fuel and score
		} // END OF DIRTY REDRAW
		// Clear Drawing Buffer
		else if (game.player.y > 395 || game.mountainState != 0) {
                        gamegl_clear(GL_BLACK);
		        gamegl_draw_mountains(game.mountainState);
		}
	        else {
		     	gamegl_clear_height(GL_BLACK, 410); // clears top 2/3 unless player is not in that region
		}


		// Draw Mountains
		// Landing Pad 1
		if (game.player.y > 300 && game.player.x > 40  && game.player.x < 90 && game.mountainState == 0) {
		   game.mountainState = 1;
		   game.player.y = 0;
		   game.player.x *= 3.0;
		   redrawMountains(game.mountainState);
		}
		
		// Landing Pad 2
		if (game.player.y > 300 && game.player.x > 175 && game.player.x < 220 && game.mountainState == 0) {
		   game.mountainState = 2;
		   game.player.y = 0;
		   game.player.x *= 3.0;
		   redrawMountains(game.mountainState);
		}

		// Magic numbers for following landing pads are the offsetX used when scaling the drawn mountains
		// Landing Pad 3
		if (game.player.y > 325 && game.player.x > 250 && game.player.x < 355 && game.mountainState == 0) {
		   game.mountainState = 3;
		   game.player.y = 0;
		   game.player.x = (game.player.x * 3.0) - 500;
		   redrawMountains(game.mountainState);
		}

		// Landing Pad 4
		if (game.player.y > 300 && game.player.x > 585 && game.player.x < 635 && game.mountainState == 0) {
		   game.mountainState = 4;
		   game.player.y = 0;
		   game.player.x = (game.player.x * 3.0) - 1375;
		   redrawMountains(game.mountainState);
		}

		// Landing Pad 5
		if (game.player.y > 300 && game.player.x > 710 && game.player.x < 760 && game.mountainState == 0) {
		   game.mountainState = 5;
		   game.player.y = 0;
		   game.player.x = (game.player.x * 3.0) - 1600;
		   redrawMountains(game.mountainState);
		}

		// Draw Stars
		if (game.mountainState == 0) { gamegl_draw_stars(GAMESTATE); }

		// Draw Player
		gamegl_draw_lander(game.player.x, game.player.y, game.player.rotation, GL_WHITE);

		// Draw Thrust Animation and Decrement Fuel when Thrusting
		if (game.player.thrusting) {

			game.Fuel -= 10;

			if (game.player.rotation == 0) {
				gamegl_draw_effect(game.player.x, game.player.y + 14, game.player.rotation, 1, 0, GL_SILVER);
			} 
			else if (game.player.rotation == 90) {
				gamegl_draw_effect(game.player.x - 14, game.player.y, game.player.rotation, 1, 0, GL_SILVER);
			}
			else if (game.player.rotation == -90) {
				gamegl_draw_effect(game.player.x + 14, game.player.y, game.player.rotation, 1, 0, GL_SILVER);
			}
			else if (game.player.rotation > 0) {
				gamegl_draw_effect(game.player.x - 12, game.player.y + 12, game.player.rotation, 1, 0, GL_SILVER);
			} else {	
				gamegl_draw_effect(game.player.x + 12, game.player.y + 12, game.player.rotation, 1, 0, GL_SILVER);
			}
		}
		
		// Draw Stats
		gamegl_draw_string(5, 10, "SCORE ", GL_SILVER);
		char* scorestr = to_decimal_string(game.Score);
		gamegl_draw_string(103, 10, scorestr, GL_SILVER);
		
		color_t fuelColor;
		
		if (game.Fuel < 500) {
			fuelColor = GL_RED;			
		}
		else if (game.Fuel < 1000) {
			fuelColor = GL_ORANGE;
		}
		else if (game.Fuel < 1500) {
			fuelColor = 0xffba8602;
		}
		else {
			fuelColor = GL_SILVER;
		}

		gamegl_draw_string(5, 28, "FUEL  ", GL_SILVER);
		char* fuelval = to_decimal_string(game.Fuel);
		gamegl_draw_string(103, 28, fuelval, fuelColor);

		// Check in-bounds
		if (game.player.x < -20 || game.player.y < -20 || game.player.x > GAMEWIDTH + 10 || game.player.y > GAMEHEIGHT + 10) {
			game.out_of_bounds = TRUE;
		} // end of Out of Bounds Check

		// Check collision with mountains or edge of screen and set crash or win state accordingly
		maxCheckX = game.player.x + 14;
	        maxCheckY = game.player.y + 14; // check 3 pixels of sprite to save space
		for (int i = game.player.x; i < maxCheckX; i++) {
			// avoid multiple gamestates being triggered on end screen
			if (game.won) {
			   // game.player.y = game.player.y -maxCheckY;
			   game.crashed = FALSE;
                           break;
			}
			for (int j = maxCheckY; j > game.player.y; j--) { 
			      if (gamegl_read_pixel(i, j) == GL_GREEN) {
				      // check for speed and bad rotation upon landing
				      if (game.player.vy > 500 || (game.player.rotation > 35 || game.player.rotation < -35)) {
					      game.crashed = TRUE;
					      break;
			              }
				      else {
                                              game.won = TRUE;
					      break;
				      }
			      }
			      else if (gamegl_read_pixel(i, game.player.y + 14) == GL_WHITE) {
				      game.crashed = TRUE;
			      }
			}
		} // end of Collision Check
		 
		// Display New Frame
		gamegl_swap_buffer();

		gpio_write(BUZZER, 0); // Turn buzzer off for next frame
		
		if (game.crashed) {	
			
			// BEGIN EXPLOSION ANIMATION
			gamegl_clear(GL_BLACK);
			for (int frame = 1; frame < 4; frame++) {
				gamegl_draw_effect(game.player.x, game.player.y, game.player.rotation, 0, frame, GL_WHITE);
				gamegl_swap_buffer();
				timer_delay_ms(100);
				gamegl_clear(GL_BLACK);
			}
			// END EXPLOSION ANIMATION
			
			gamegl_swap_buffer();
			gamegl_clear(GL_BLACK);
			gamegl_draw_string(160, 200, crashmsg, GL_RED);
			gamegl_swap_buffer();
			gpio_write(BUZZER, 1);
		   	timer_delay(1);	
			gpio_write(BUZZER, 0);
			gamegl_draw_string(160, 200, crashmsg, GL_RED);
			gamegl_draw_string(260, 280, playagainmsg, GL_SILVER); 
			gamegl_swap_buffer();
			timer_delay(3);
			
			while (readval != 1) { 
				readval = gpio_read(MIC);
			}
			
			RESTART = 25;
			game.crashed = FALSE; // reset 
			game.Score -= CRASHPENALTY;
			game.mountainState = 0;
			game.player.x = STARTX;
			game.player.y = STARTY; 
			game.player.rotation = 90;
			rotaryEncoder.rotaryAngle = 90;
			game.player.vx = 4;
			redrawMountains(0);
			GRAVITY = 2.0;
		}

		if (game.out_of_bounds) {		
			gamegl_draw_string(255, 200, oobmsg, GL_AMBER);
			gamegl_draw_string(260, 280, playagainmsg, GL_SILVER); 
			gamegl_swap_buffer();
			timer_delay(3);
			
			while (readval != 1) { 
				readval = gpio_read(MIC);
			}

			RESTART = 25;
			game.out_of_bounds = FALSE; // reset
			game.player.x = STARTX;
			game.player.y = STARTY;
			game.player.rotation = 90;
			rotaryEncoder.rotaryAngle = 90;
			game.player.vx = 4;
			game.player.thrusting = FALSE;
			game.mountainState = 0;
			redrawMountains(0);
			GRAVITY = 2.0;
		}

		// Empty Fuel Tank
		if (game.Fuel <= 0) { 
			gamegl_clear(GL_BLACK);
			gamegl_swap_buffer();
			gamegl_clear(GL_BLACK);

			gamegl_draw_string(297, 200, nofuelmsg, GL_AMBER);
			gamegl_swap_buffer();
		   	timer_delay(2);	// Display Empty Fuel Message
			
			gamegl_draw_string(297, 200, nofuelmsg, GL_AMBER);
			gamegl_draw_string(250, 280, "YOUR HIGH SCORE: ", GL_SILVER); 
			gamegl_swap_buffer();
			timer_delay_ms(300); // Display Player's Score
			
			gamegl_draw_string(297, 200, nofuelmsg, GL_AMBER);
			gamegl_draw_string(250, 280, "YOUR HIGH SCORE: ", GL_SILVER);
			char* scorestr = to_decimal_string(game.Score);
			gamegl_draw_string(490, 280, scorestr, GL_SILVER);
			gamegl_swap_buffer();
			timer_delay(5); // Display Player's Score
			
			if (game.Score > HIGHSCORE) {
				HIGHSCORE = game.Score;
			} // update high score

			// Back to Start Screen (Chose to use goto to isolate start screen sequence from all other game states)
			goto start_screen; // I'm sorry, Esdger
		}

		if (game.won) {	
			// Erase old score
			gamegl_draw_string(5, 10, "SCORE ", GL_BLACK);
			char* scorestr = to_decimal_string(game.Score);
			gamegl_draw_string(103, 10, scorestr, GL_BLACK);

			// Update score: on first landing, simply add based on site. Use multiplier on subsequent landings
			switch (game.mountainState) {
				case 1:
					if (game.Score > 0) { game.Score *= 2; }
					game.Score = game.Score > 0 ? game.Score + 0 : game.Score + 200;
					break;
				case 2:	
					if (game.Score > 0) { game.Score *= 5; }
					game.Score = game.Score > 0 ? game.Score + 0 : game.Score + 500;
					break;
				case 3:
					if (game.Score > 0) { game.Score *= 3; }
					game.Score = game.Score > 0 ? game.Score + 0 : game.Score + 300;
					break;
				case 4:
					if (game.Score > 0) { game.Score *= 2; }
					game.Score = game.Score > 0 ? game.Score + 0 : game.Score + 200;
					break;
				case 5:
					if (game.Score > 0) { game.Score *= 5; }
					game.Score = game.Score > 0 ? game.Score + 0 : game.Score + 500;
					break;
			}
			
			// Draw New Score
			gamegl_draw_string(5, 10, "SCORE ", GL_SILVER);
			scorestr = to_decimal_string(game.Score);
			gamegl_draw_string(103, 10, scorestr, GL_SILVER);
			
			// Draw Win Messages
			//gamegl_draw_string(160, 200, winmsg, GL_GREEN);
			//gamegl_swap_buffer();
		   	//timer_delay(1);	
			gamegl_draw_string(120, 200, winmsg, GL_GREEN);
			gamegl_draw_string(160, 280, newgamemsg, GL_SILVER); 
			gamegl_swap_buffer();
			timer_delay(3);
			
			while (readval != 1) { 
				readval = gpio_read(MIC);
			}

			RESTART = 25;
			game.won = FALSE; // reset
			game.player.x = STARTX;
			game.player.y = STARTY;
			game.player.rotation = 90;
			rotaryEncoder.rotaryAngle = 90;
			game.player.vx = 4;
			game.mountainState = 0;
			redrawMountains(0);
			GRAVITY = 2.0;
		}
	} // END MAIN GAME LOOP
}


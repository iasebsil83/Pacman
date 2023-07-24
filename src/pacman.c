/*
******************************************************************************************

    LICENSE :

    C_Pacman_S2DE
    Copyright (C) 2020  Sebastien SILVANO
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.

    If not, see <https://www.gnu.org/licenses/>.
*/

// ---------------- IMPORTATIONS ----------------

//standard
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//calculations
#include <math.h>

//for the random
#include <time.h>

//graphics
#include "../lib/S2DE.h"












// ---------------- DECLARATIONS ----------------

//window
#define LargeurFenetre 800
#define HauteurFenetre 400

//game
#define chunkSize 25
#define playerMarge 4



//event variables
extern int S2DE_keyState; //keyboard
extern short S2DE_key;

//game matrix
static int ny = 16;
static int nx = 32;
static int walls[(int)(16*32)] = { // 0 = yellow ball, 1 is wall, 2 = orange ball, 3 = void
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
	1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,
	1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0,0,1,2,1,
	1,0,1,0,1,0,1,0,1,1,1,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,
	1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,
	1,0,1,0,1,0,1,0,1,1,1,1,0,1,0,1,2,1,0,0,0,2,0,0,0,1,0,1,0,0,0,1,
	1,2,1,0,1,2,1,0,1,2,0,0,0,1,0,1,0,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,
	1,0,1,0,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,0,1,1,1,1,0,1,0,1,1,1,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,2,1,
	1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,
	1,0,1,1,0,0,0,1,1,1,1,1,0,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};



//monsters
static int monstersNbr = 5;
static float monsters[5][2] = { //{x,y,direction}
	{29*chunkSize,chunkSize},
	{16*chunkSize,5*chunkSize},
	{3*chunkSize,8*chunkSize},
	{25*chunkSize,14*chunkSize},
	{7*chunkSize,12*chunkSize}
};
static int monstersDir[5] = {3,2,0,3,};
static int monstersWantedDir[5] = {3,2,0,3,};
static int monstersDirTimer = 80;
static int monstersDirTimerMax = 80;
static int monstersColor[5][3] = {
	{0  ,150,255},
	{255,0  ,0  },
	{0  ,255,40 },
	{255, 90,255},
	{0  ,255,255}
};
static const float monstersStep = 2.5;
static int monstersScore = 0;



//player
static float playerX = chunkSize;
static float playerY = chunkSize;
static const float playerStep = 2.5;
static int wantedDirection = 1; //right
static int direction = 1; //up



//player mouth
static float mouthDiv = 4; //4 = open; 2 = close;
static bool mouthSens = true;
static const float mouthStep = 0.2;



//mega mode (eat phantoms)
static int megaModeTimer = 0;
static const int megaModeTimerMax = 320;



//life
static short int life = 3;
static int lifeTimer = 0;
static const int lifeTimerMax = 90;



//other
static int score = 0;
static bool win = false;
static bool starter = true;












// ---------------- UTILITIES ----------------

//utilities
int rnd(float n){ //<=> round a float to an int
	if(n > (int)n){
		return (int)(n+1);
	}else{
		return (int)n;
	}
}

void circle(float x, float y, float R, int div, int nbr, short int strokes){
	switch(strokes){
		case 0: //borders
			for(int angle=0; angle < nbr; angle++){
				S2DE_line( x+R*cos(angle*2*M_PI/div)    , y+R*sin(angle*2*M_PI/div),
						   x+R*cos((angle+1)*2*M_PI/div), y+R*sin((angle+1)*2*M_PI/div) );
			}
		break;
		case 1: //center lines
			for(int angle=0; angle < nbr; angle++){
				S2DE_line(x,y, x+R*cos(angle*2*M_PI/div),y+R*sin(angle*2*M_PI/div));
			}
		break;
		case 2: //borders + center lines
			for(int angle=0; angle < nbr; angle++){
				S2DE_line(x,y, x+R*cos(angle*2*M_PI/div),y+R*sin(angle*2*M_PI/div));
				S2DE_line( x+R*cos(angle*2*M_PI/div)    , y+R*sin(angle*2*M_PI/div),
						   x+R*cos((angle+1)*2*M_PI/div), y+R*sin((angle+1)*2*M_PI/div) );
			}
		break;
	}
}



//monsters
void delMonster(int row){
	monstersNbr--;
	for(int a=row; a < monstersNbr; a++){
		monsters[a][0] = monsters[a+1][0];
		monsters[a][1] = monsters[a+1][1];
		monstersColor[a][0] = monstersColor[a+1][0];
		monstersColor[a][1] = monstersColor[a+1][1];
		monstersColor[a][2] = monstersColor[a+1][2];
		monstersDir[a] = monstersDir[a+1];
	}
}

bool getOk(float wallX, float wallY, float x, float y, int dir, float step){
	float tooMuch;
	if(y == wallY){ //if we are in the same Yline
		if(dir == 1){ //if right
			tooMuch = x+chunkSize+step;
			if(tooMuch > wallX && tooMuch < wallX+chunkSize){ //if xMax+1step is in the block
				return false;
			}
		}else if(dir == 3){ //if left
			tooMuch = x-step;
			if(tooMuch > wallX && tooMuch < wallX+chunkSize){ //if xMin-1step is in the block
				return false;
			}
		}
	}
	if(x == wallX){ //if we are in the same Xline
		if(dir == 0){ //if up
			tooMuch = y+chunkSize+step;
			if(tooMuch > wallY && tooMuch < wallY+chunkSize){ //if yMax+1step is in the block
				return false;
			}
		}else if(dir == 2){ //if down
			tooMuch = y-step;
			if(tooMuch > wallY && tooMuch < wallY+chunkSize){ //if yMin-1step is in the block
				return false;
			}
		}
	}
	return true;
}



//walls
bool wallsOk(){ //checkBalls() included
	float chunkX, chunkY;
	bool ok = true;
	bool tempOk = true;
	int alivedBalls = 0;
	for(int a=0; a < nx*ny; a++){
		if(walls[a] == 1){ //if this is a wall
			chunkY = chunkSize*(int)( a / (LargeurFenetre/chunkSize) );
			chunkX = chunkSize*(int)( a % (LargeurFenetre/chunkSize) );
			tempOk = getOk(chunkX,chunkY, playerX,playerY,direction,playerStep);
			if(!tempOk){
				ok = false;
			}
		}else if(walls[a] != 3){ //checkBalls()
			alivedBalls++;
			chunkY = chunkSize*(int)( a / (LargeurFenetre/chunkSize) );
			chunkX = chunkSize*(int)( a % (LargeurFenetre/chunkSize) );
			if(playerY == chunkY){ //if we are on the same Yline
				if( (playerX > chunkX && playerX < chunkX+chunkSize) || (playerX+chunkSize > chunkX && playerX < chunkX) ){ //if we share our Xspace with an other block (ball)
					if(walls[a] == 2){ //if this is an orange ball
						megaModeTimer = megaModeTimerMax;
					}
					walls[a] = 3; //set void
					score++;
				}
			}
			if(playerX == chunkX){ //if we are on the same Xline
				if( (playerY > chunkY && playerY < chunkY+chunkSize) || (playerY+chunkSize > chunkY && playerY < chunkY) ){ //if we share our Yspace with an other block (ball)
					if(walls[a] == 2){ //if this is an orange ball
						megaModeTimer = megaModeTimerMax;
					}
					walls[a] = 3; //set void
					score++;
				}
			}
		}
	}
	if(alivedBalls == 0){
		win = true;
	}
	return ok;
}

void checkMonsters(){
	float chunkX,chunkY;
	bool ok = true;
	bool tempOk = true;
	for(int a=0; a < monstersNbr; a++){
		if(
(
	playerY == monsters[a][1] &&
	((playerX > monsters[a][0] && playerX < monsters[a][0]+chunkSize) || (playerX+chunkSize > monsters[a][0] && playerX < monsters[a][0]))
) || (
	playerX == monsters[a][0] &&
	((playerY > monsters[a][1] && playerY < monsters[a][1]+chunkSize) || (playerY+chunkSize > monsters[a][1] && playerY < monsters[a][1]))
)
		){
			if(megaModeTimer != 0){
				delMonster(a);
				monstersScore += 100;
				printf("Eat monster : +100\n");
				break;
			}else if(lifeTimer == 0){
				life--;
				lifeTimer = lifeTimerMax;
				printf("Life : %d\n",life);
			}
		}
		if(rnd(monsters[a][0])%chunkSize == 0 && rnd(monsters[a][1])%chunkSize == 0){ //if we are in the perfect coo matrix
			monstersDir[a] = monstersWantedDir[a]; //update the direction
		}
		ok = true;
		tempOk = true;
		for(int b=0; b < nx*ny; b++){
			if(walls[b] == 1){
				chunkY = chunkSize*(int)( b / (LargeurFenetre/chunkSize) );
				chunkX = chunkSize*(int)( b % (LargeurFenetre/chunkSize) );
				tempOk = getOk(chunkX,chunkY, monsters[a][0],monsters[a][1],monstersDir[a],monstersStep);
				if(!tempOk){
					ok = false;
				}
			}
		}
		if(ok){
			switch(monstersDir[a]){
				case 0: //up
					monsters[a][1] += monstersStep;
				break;
				case 1: //right
					monsters[a][0] += monstersStep;
				break;
				case 2: //down
					monsters[a][1] -= monstersStep;
				break;
				case 3: //left
					monsters[a][0] -= monstersStep;
				break;
			}
		}
		if(monstersDirTimer == 0 || !ok){
			monstersWantedDir[a] += rnd( (rand() % 4)-2);
		}
		if(monstersWantedDir[a] > 3){
			monstersWantedDir[a] -= 4;
		}else if(monstersWantedDir[a] < 0){
			monstersWantedDir[a] += 4;
		}
	}
	if(monstersDirTimer == 0){
		monstersDirTimer = monstersDirTimerMax;
	}
	monstersDirTimer--;
}












// ---------------- EXECUTION ----------------

//event
void S2DE_event(int event){
	float chunkX,chunkY;
	switch(event){

		//display
		case S2DE_DISPLAY:
			S2DE_setColor(0,0,0);
			S2DE_rectangle(0,0, LargeurFenetre,HauteurFenetre, 1);

			//blocks
			for(int a=0; a < nx*ny; a++){
				chunkY = chunkSize*(int)( a / (LargeurFenetre/chunkSize) );
				chunkX = chunkSize*(int)( a % (LargeurFenetre/chunkSize) );
				switch(walls[a]){
					case 0: //if this is a yellow ball
						S2DE_setColor(255,255,0);
						S2DE_rectangle(chunkX+chunkSize/3,chunkY+chunkSize/3, chunkX+chunkSize-chunkSize/3,chunkY+chunkSize-chunkSize/3, 1);
					break;
					case 1:  //if this is a wall
						S2DE_setColor(0,0,255);
						S2DE_rectangle(chunkX,chunkY, chunkX+chunkSize,chunkY+chunkSize, 1);
					break;
					case 2: //if this is a orange ball
						S2DE_setColor(255,100,0);
						S2DE_rectangle(chunkX+chunkSize/4,chunkY+chunkSize/4, chunkX+chunkSize-chunkSize/4,chunkY+chunkSize-chunkSize/4, 1);
					break;
				}
			}
			//monsters
			for(int a=0; a < monstersNbr; a++){
				if(megaModeTimer != 0){
					S2DE_setColor(0,0,255);
				}else{
					S2DE_setColor(monstersColor[a][0],monstersColor[a][1],monstersColor[a][2]);
				}
				S2DE_rectangle(monsters[a][0]+chunkSize/6,monsters[a][1], monsters[a][0]+chunkSize-chunkSize/6,monsters[a][1]+chunkSize-chunkSize/6, 1);
				S2DE_setColor(255,255,255);
				S2DE_rectangle(monsters[a][0]+3*chunkSize/12,monsters[a][1]+2*chunkSize/3, monsters[a][0]+5*chunkSize/12,monsters[a][1]+5*chunkSize/12, 1);
				S2DE_rectangle(monsters[a][0]+7*chunkSize/12+1,monsters[a][1]+2*chunkSize/3, monsters[a][0]+9*chunkSize/12+1,monsters[a][1]+5*chunkSize/12, 1);
			}
			//player
			if(megaModeTimer != 0){
				S2DE_setColor(255,0,0);
			}else{
				S2DE_setColor(255,255,0);
			}
			if(lifeTimer == 0 || (lifeTimer > 8 && lifeTimer < 22) || (lifeTimer > 38 && lifeTimer < 52) || (lifeTimer > 68 && lifeTimer < 82) ){
				circle(playerX+chunkSize/2,playerY+chunkSize/2,chunkSize/2-1,70,70,1);
				S2DE_setColor(0,0,0);
				switch(direction){
					case 0: //up
						S2DE_triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize/mouthDiv,playerY+chunkSize-2, playerX+chunkSize-chunkSize/mouthDiv,playerY+chunkSize-2, 1);
					break;
					case 1: //right
						S2DE_triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize-2,playerY+chunkSize/mouthDiv, playerX+chunkSize-2,playerY+chunkSize-chunkSize/mouthDiv, 1);
					break;
					case 2: //down
						S2DE_triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize/mouthDiv,playerY+1, playerX+chunkSize-chunkSize/mouthDiv,playerY+1, 1);
					break;
					case 3: //left
						S2DE_triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+1,playerY+chunkSize/mouthDiv, playerX+1,playerY+chunkSize-chunkSize/mouthDiv, 1);
					break;
				}
			}
		break;



		//keyboard
		case S2DE_KEYBOARD:
			//get only keyPress events
			if(S2DE_keyState == S2DE_KEY_RELEASED)
				return;

			switch(S2DE_key){
				case S2DE_KEY_q:
					S2DE_stop();
				break;
				case S2DE_KEY_LEFT:
					wantedDirection = 3;
				break;
				case S2DE_KEY_RIGHT:
					wantedDirection = 1;
				break;
				case S2DE_KEY_DOWN:
					wantedDirection = 2;
				break;
				case S2DE_KEY_UP:
					wantedDirection = 0;
				break;
			}
		break;



		//mouse
		case S2DE_MOUSE_CLICK:
		break;



		//mouse move
		case S2DE_MOUSE_MOVE:
		break;



		//timed execution
		case S2DE_TIMER:
			if(rnd(playerX)%chunkSize == 0 && rnd(playerY)%chunkSize == 0){ //if we are in the perfect coo matrix
				direction = wantedDirection; //update the direction
			}
			if(win){
				if(starter){
					starter = false;
					printf("You win !\n");
					printf("Score : %d\nscore max possible : 724",(int)(score+monstersScore));
				}
			}else if(life == 0){
				if(starter){
					starter = false;
					printf("You lose !\n");
				}
			}else{
				if(megaModeTimer != 0){
					megaModeTimer--;
				}
				if(lifeTimer != 0){
					lifeTimer--;
				}
				checkMonsters();
				if(wallsOk()){ //checkBalls() included
					switch(direction){
						case 3: //left
							playerX -= playerStep;
						break;
						case 1: //right
							playerX += playerStep;
						break;
						case 2: //down
							playerY -= playerStep;
						break;
						case 0: //up
							playerY += playerStep;
						break;
					}
				}
				S2DE_refresh();
				if(mouthSens){
					mouthDiv += mouthStep;
				}else{
					mouthDiv -= mouthStep;
				}
				if(mouthDiv > 4 || mouthDiv < 2){
					mouthSens = !mouthSens;
				}
			}
		break;



		//resize
		case S2DE_RESIZE:
		break;
	}
}



// main
int main(int argc, char **argv){

	//init random
	srand(time(NULL));

	//init S2DE
	S2DE_init(argc,argv, "Pacman", LargeurFenetre, HauteurFenetre);
	S2DE_setTimer(17);

	//launch S2DE
	S2DE_start();

	return 0;
}

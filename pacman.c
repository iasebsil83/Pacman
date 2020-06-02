#include <stdlib.h> // Pour pouvoir utiliser exit()
#include <stdio.h> // Pour pouvoir utiliser printf()
#include <math.h> // Pour pouvoir utiliser sin() et cos()
#include "GfxLib.h" // Seul cet include est necessaire pour faire du graphique
#include "BmpLib.h" // Cet include permet de manipuler des fichiers BMP
#include "ESLib.h" // Pour utiliser valeurAleatoire()

#define LargeurFenetre 800
#define HauteurFenetre 400

#define chunkSize 25
#define playerMarge 4

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
//score
static int score = 0;
static const int scoreMax = 216;
//other
static bool win = false;
static bool starter = true;

int main(int argc, char **argv){
	initialiseGfx(argc, argv);
	prepareFenetreGraphique("Pacman", LargeurFenetre, HauteurFenetre);
	lanceBoucleEvenements();
	return 0;
}

int rnd(float n){ //<=> round a float to an int
	if(n > (int)n){
		return (int)(n+1);
	}else{
		return (int)n;
	}
}

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
			monstersWantedDir[a] += rnd(4*valeurAleatoire()-2);
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

void gestionEvenement(EvenementGfx evenement){
	static bool pleinEcran = false; // Pour savoir si on est en mode plein ecran ou pas
	//static DonneesImageRGB *image = NULL;
	float chunkX,chunkY;
	switch(evenement){
		case Initialisation:
			//init mobs
			demandeTemporisation(17); //<=> 60 fps
		break;
		case Temporisation: //<=> refresh(values)
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
				rafraichisFenetre();
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
		case Affichage: //<=> refresh(display)
			effaceFenetre(0,0,0);
			//blocks
			for(int a=0; a < nx*ny; a++){
				chunkY = chunkSize*(int)( a / (LargeurFenetre/chunkSize) );
				chunkX = chunkSize*(int)( a % (LargeurFenetre/chunkSize) );
				switch(walls[a]){
					case 0: //if this is a yellow ball
						couleurCourante(255,255,0);
						rectangle(chunkX+chunkSize/3,chunkY+chunkSize/3, chunkX+chunkSize-chunkSize/3,chunkY+chunkSize-chunkSize/3);
						//circle(chunkX+chunkSize/2,chunkY+chunkSize/2, chunkSize/3,6,6,1);
					break;
					case 1:  //if this is a wall
						couleurCourante(0,0,255);
						rectangle(chunkX,chunkY, chunkX+chunkSize,chunkY+chunkSize);
					break;
					case 2: //if this is a orange ball
						couleurCourante(255,100,0);
						rectangle(chunkX+chunkSize/4,chunkY+chunkSize/4, chunkX+chunkSize-chunkSize/4,chunkY+chunkSize-chunkSize/4);
						//circle(chunkX+chunkSize/2,chunkY+chunkSize/2, chunkSize/4,6,6,1);
					break;
				}
			}
			//monsters
			for(int a=0; a < monstersNbr; a++){
				if(megaModeTimer != 0){
					couleurCourante(0,0,255);
				}else{
					couleurCourante(monstersColor[a][0],monstersColor[a][1],monstersColor[a][2]);
				}
				rectangle(monsters[a][0]+chunkSize/6,monsters[a][1], monsters[a][0]+chunkSize-chunkSize/6,monsters[a][1]+chunkSize-chunkSize/6);
				couleurCourante(255,255,255);
				rectangle(monsters[a][0]+3*chunkSize/12,monsters[a][1]+2*chunkSize/3, monsters[a][0]+5*chunkSize/12,monsters[a][1]+5*chunkSize/12);
				rectangle(monsters[a][0]+7*chunkSize/12+1,monsters[a][1]+2*chunkSize/3, monsters[a][0]+9*chunkSize/12+1,monsters[a][1]+5*chunkSize/12);
			}
			//player
			if(megaModeTimer != 0){
				couleurCourante(255,0,0);
			}else{
				couleurCourante(255,255,0);
			}
			if(lifeTimer == 0 || (lifeTimer > 8 && lifeTimer < 22) || (lifeTimer > 38 && lifeTimer < 52) || (lifeTimer > 68 && lifeTimer < 82) ){
				circle(playerX+chunkSize/2,playerY+chunkSize/2,chunkSize/2-1,70,70,1);
				couleurCourante(0,0,0);
				switch(direction){
					case 0: //up
						triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize/mouthDiv,playerY+chunkSize-2, playerX+chunkSize-chunkSize/mouthDiv,playerY+chunkSize-2);
					break;
					case 1: //right
						triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize-2,playerY+chunkSize/mouthDiv, playerX+chunkSize-2,playerY+chunkSize-chunkSize/mouthDiv);
					break;
					case 2: //down
						triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+chunkSize/mouthDiv,playerY+1, playerX+chunkSize-chunkSize/mouthDiv,playerY+1);
					break;
					case 3: //left
						triangle(playerX+chunkSize/2,playerY+chunkSize/2, playerX+1,playerY+chunkSize/mouthDiv, playerX+1,playerY+chunkSize-chunkSize/mouthDiv);
					break;
				}
			}
		break;
		case Clavier:
			switch(caractereClavier()){
				case 'q':
					//libereDonneesImageRGB(&image[a]);
					//On libere la structure image, c'est plus propre, meme si on va sortir du programme juste apres
					termineBoucleEvenements();
				break;
				case 'f':
					pleinEcran = !pleinEcran; // Changement de mode plein ecran
					if(pleinEcran){
						modePleinEcran();
					}else{
						redimensionneFenetre(largeurFenetre(), hauteurFenetre());
					}
				break;
			}
			rafraichisFenetre();
		break;
		case ClavierSpecial:
			switch(toucheClavier()){
				case ToucheFlecheGauche:
					wantedDirection = 3; //left
				break;
				case ToucheFlecheDroite:
					wantedDirection = 1; //right
				break;
				case ToucheFlecheBas:
					wantedDirection = 2; //down
				break;
				case ToucheFlecheHaut:
					wantedDirection = 0; //up
				break;
			}
			rafraichisFenetre();
		break;
		case BoutonSouris:
		break;
		case Souris: // Si la souris est deplacee
		break;
		case Inactivite: // Quand aucun message n'est disponible
		break;
		case Redimensionnement: // La taille de la fenetre a ete modifie ou on est passe en plein ecran
		break;
	}
}

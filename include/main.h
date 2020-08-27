#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <gb/gb.h>
#include <rand.h>
#include <time.h>

#include "tiles.h"
#include "numbers.h"
#include "welcome_bkg.h"
#include "sound.h"

typedef struct {
    int birdY;
    int birdVelocity;
    int up;
    int animation;
    unsigned int frame;
    int pipe1Pos;
    int pipe1State;
    int pipe2Pos;
    int pipe2State;
    int nPipe;
    unsigned int points;
} Game;

int main();

void welcomeScreen();
void handleGame();

void setUpPipes(Game*);
void setUpBird(Game*);
void setUpPoints(Game*);

void setPoints(unsigned int);

void moveBird(int, int);
void updatePipe(int, int*, int*);

void handleJoypad(Game*);
int calcGravity(Game*);
void drawBird(Game*);
int handleHitbox(Game*);

int intersect(int,int,int,int,int,int,int,int);
#endif
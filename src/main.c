#include "../include/main.h"

const int BIRD_X        = 30;
const int GRAVITY       = -2;
const int JUMP_BOOST    = 7;

const int NUM_TILES     = 38;

int main() {
    welcomeScreen();

    // activate sound

    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;

    while(1) {
        delay(200);
        handleGame();

        playDeath();

        wait_vbl_done();
        waitpad(J_B);
    }

    return 0;
}


void welcomeScreen() {
    int seed = 0;
    set_bkg_data(0, NUM_TILES, tiles);
    set_bkg_tiles(0, 0, welcome_bkgWidth, welcome_bkgHeight, welcome_bkg);

    SHOW_BKG;
    DISPLAY_ON;

    while(joypad() != J_A) seed++;

    initrand(seed);

    HIDE_BKG;
}

void handleGame() {
    Game game = {75, 0, 0, 0, 0, 120, 0, 248, 0, 0, 0};

    // Set Sprite mode and load sprites
    SPRITES_8x16;
    set_sprite_data(0, NUM_TILES, tiles);
    set_sprite_data(50, 20, numbers);

    setUpPipes(&game);
    setUpBird(&game);
    setUpPoints(&game);

    SHOW_SPRITES;

    while(1) {
        handleJoypad(&game);

        if(calcGravity(&game)) break;
        
        game.frame++;
        if(game.frame >= 60) {
            game.frame = 0;
        }

        drawBird(&game);
        
        updatePipe(4, &(game.pipe1Pos), &(game.pipe1State));
        updatePipe(17, &(game.pipe2Pos), &(game.pipe2State));

        if(handleHitbox(&game)) break;

        wait_vbl_done();
    }
}


void setUpPipes(Game* game) {
    int i;
    game->pipe1State = rand();
    if(game->pipe1State < 0) game->pipe1State *= -1;
    game->pipe1State = game->pipe1State % 5;

    game->pipe2State = rand();
    if(game->pipe2State < 0) game->pipe2State *= -1;
    game->pipe2State = game->pipe2State % 5;

    // PIPE 1
    set_sprite_tile(4, 16);
    set_sprite_tile(5, 18);
    set_sprite_tile(6, 12);
    set_sprite_tile(7, 14);
    for(i = 0; i < 4; i++) {
        set_sprite_tile(8 + i, 32);
        set_sprite_tile(13 + i, 30);
    }

    // PIPE 2
    set_sprite_tile(17, 16);
    set_sprite_tile(18, 18);
    set_sprite_tile(19, 12);
    set_sprite_tile(20, 14);
    for(i = 0; i < 4; i++) {
        set_sprite_tile(21 + i, 32);
        set_sprite_tile(26 + i, 30);
    }
}

void setUpBird(Game* game) {
    set_sprite_tile(0, game->animation + 0);
    set_sprite_tile(2, game->animation + 2);
    moveBird(BIRD_X, game->birdY);
}

void setUpPoints(Game* game) {
    set_sprite_tile(31, 50);
    set_sprite_tile(32, 50);
    set_sprite_tile(33, 50);
    set_sprite_tile(34, 50);
    move_sprite(31, 160, 16);
    move_sprite(32, 152, 16);
    move_sprite(33, 144, 16);
    move_sprite(34, 136, 16);
    
    setPoints(game->points);
}


void setPoints(unsigned int points) {
    int i = 0;

    i = points % 10;
    points /= 10;
    set_sprite_tile(31, 50 + i * 2);
    i = points % 10;
    points /= 10;
    set_sprite_tile(32, 50 + i * 2);
    i = points % 10;
    points /= 10;
    set_sprite_tile(33, 50 + i * 2);
    i = points % 10;
    points /= 10;
    set_sprite_tile(34, 50 + i * 2);
}


void moveBird(int x, int y) {
    move_sprite(0, x, y); 
    move_sprite(2, x + 8, y);
}

void updatePipe(int spriteOff, int* pos, int* state) {
    int ti = 0;
    int i = 0;

    *pos = *pos - 1;

    if(*pos < -8) {
        *pos = 248;
        *state = rand();
        if(*state < 0) *state *= -1;
        *state = *state % 5;
    }

    for(i = 0; i < 9; i++) {
        if(i < *state) {
            move_sprite(spriteOff + 4 + ti, *pos, i * 16 + 16);
            move_sprite(spriteOff + 9 + ti, *pos + 8, i * 16 + 16);
            ti++;
        } else if(i == *state) {
            move_sprite(spriteOff + 0, *pos, i * 16 + 16);
            move_sprite(spriteOff + 1, *pos + 8, i * 16 + 16);
        } else if(i == *state + 4) {
            move_sprite(spriteOff + 2, *pos, i * 16 + 16);
            move_sprite(spriteOff + 3, *pos + 8, i * 16 + 16);
        } if(i > *state + 4) {
            move_sprite(spriteOff + 4 + ti, *pos, i * 16 + 16);
            move_sprite(spriteOff + 9 + ti, *pos + 8, i * 16 + 16);
            ti++;
        }
    }
}


void handleJoypad(Game* game) {
    switch(joypad()) {
        case J_A:
            if(game->up) break;
            playFlap();
            game->birdVelocity = JUMP_BOOST;
            game->up = 1;
            break;
        default:
            game->up = 0;
            break;
    }
}

int calcGravity(Game* game) {
    if(game->frame % 3 == 0) {
        game->birdY -= game->birdVelocity;
        if(game->birdY < 16) {
            game->birdY = 16;
        }
        if(game->birdY > 148) {
            game->birdY = 148;
            moveBird(BIRD_X, game->birdY);
            return 1;
        }
        
        game->birdVelocity += GRAVITY;
    }
    return 0;
}

void drawBird(Game* game) {
    if(game->frame % 10 == 0) {
        game->animation += 4;
        game->animation = game->animation % 16;
        if(game->animation == 12) {
            set_sprite_tile(0, game->animation - 8);
            set_sprite_tile(1, game->animation - 7);
            set_sprite_tile(2, game->animation - 6);
            set_sprite_tile(3, game->animation - 5);
        } else {
            set_sprite_tile(0, game->animation + 0);
            set_sprite_tile(1, game->animation + 1);
            set_sprite_tile(2, game->animation + 2);
            set_sprite_tile(3, game->animation + 3);
        }
    }

    moveBird(BIRD_X, game->birdY);
}

int handleHitbox(Game* game) {
    // Hitbox pipes
    if(intersect(BIRD_X, game->birdY, BIRD_X + 16, game->birdY + 12, game->pipe1Pos, 0, game->pipe1Pos + 16, (game->pipe1State + 2) * 16)) {
        return 1;
    }
    if(intersect(BIRD_X, game->birdY, BIRD_X + 16, game->birdY + 12, game->pipe1Pos, (game->pipe1State + 5) * 16, game->pipe1Pos + 16, 160)) {
        return 1;
    }
    if(intersect(BIRD_X, game->birdY, BIRD_X + 16, game->birdY + 12, game->pipe2Pos, 0, game->pipe2Pos + 16, (game->pipe2State + 2) * 16)) {
        return 1;
    }
    if(intersect(BIRD_X, game->birdY, BIRD_X + 16, game->birdY + 12, game->pipe2Pos, (game->pipe2State + 5) * 16, game->pipe2Pos + 16, 160)) {
        return 1;
    }

    // Hitbox get point
    if((BIRD_X > game->pipe1Pos && !game->nPipe) ||(BIRD_X > game->pipe2Pos && game->nPipe)) {
        game->points++;
        setPoints(game->points);
        game->nPipe = !game->nPipe;
        playPoint();
    }

    return 0;
}


int intersect(int l1, int t1, int r1, int b1, int l2, int t2, int r2, int b2) {
    return !(l2 > r1
            || r2 < l1
            || t2 > b1
            || b2 < t1
    );
}
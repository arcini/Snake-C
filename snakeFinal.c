#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>

typedef struct BodyPiece {
    short x;
    short y;
} BodyPiece;

typedef struct Board {
    int width;
    int height;
    short foodx;
    short foody;
    short headx;
    short heady;
    short* bodyx; //piece next to head is at beginning
    short* bodyy;
    short pieces;
    char* currentDirection;
    char* nextDirection;
} Board;

void printBoard(Board * b);
Board* newBoard();
int moveSnake(Board * b);
void checkDie(Board * b);
void generateFood(Board * b);
int checkAte(Board * b);

int main(void) {
    srand(time(NULL));
    initscr();
    Board * yeet = newBoard();
    generateFood(yeet);
    cbreak(); 
    start_color();
    init_pair(2,COLOR_GREEN,COLOR_WHITE);
    wattron(stdscr, COLOR_PAIR(2));
    wbkgd(stdscr, COLOR_PAIR(2));
    init_pair(3,COLOR_BLUE,COLOR_WHITE);
    init_pair(4,COLOR_RED,COLOR_WHITE);
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    nodelay(stdscr, true);
    printBoard(yeet);
    
    int pressedKey;
    for(;;) {
        pressedKey = moveSnake(yeet);        
        checkDie(yeet);
        checkAte(yeet);
        if(pressedKey == 0)
            usleep(80000 - yeet->pieces*100 > 40000? 80000-yeet->pieces*100:40000);
  
        printBoard(yeet);



    }
    return 0;
}

int checkAte(Board * b) {
    if(b->headx == b->foodx && b->heady == b->foody) {
        generateFood(b);
        //add body piece(s)
        //depends on current direction
        //int lastx = b->pieces > 0? b->bodyx[b->pieces-1] : b->headx;
        //int lasty = b->pieces > 0? b->bodyy[b->pieces-1] : b->heady;
        /*if (strcmp(b->currentDirection, "UP") == 0) {
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyx[j] = b->bodyx[lastx];
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyy[j] = b->bodyy[lasty] + (j-b->pieces) + 1;
        }
        if (strcmp(b->currentDirection, "DOWN") == 0) {
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyx[j] = b->bodyx[lastx];
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyy[j] = b->bodyy[lasty] - (j-b->pieces) - 1;
        }
        if (strcmp(b->currentDirection, "LEFT") == 0) {
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyx[j] = b->bodyx[lastx] + (j-b->pieces) + 1;
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyy[j] = b->bodyy[lasty];
        }
        if (strcmp(b->currentDirection, "RIGHT") == 0) {
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyx[j] = b->bodyx[lastx] - (j-b->pieces) - 1;
                for(short j = b->pieces; j < b->pieces + 4; ++j)
                    b->bodyy[j] = b->bodyy[lasty];
        }*/
        for(short j = b->pieces; j < b->pieces + 8; ++j) {
            b->bodyx[j] = -2;
            b->bodyy[j] = -2;
        }
        //update piece counter
        b->pieces += 8;
        return 1;
    } else {
        return 0; 
    }
}

void generateFood(Board * b) {
    int newx = random() % (b->width - 2);
    int newy = random() % (b->height - 2);
    b->foody = newy;
    b->foodx = newx;
}

void checkDie(Board * b) {
    if(b->headx == -1 || b->headx == b->width-2 || b->heady == -1 || b->heady == b->height-2) {
        sleep(2);
        endwin();
        exit(0);
    }
    for(int i = 0; i < b->pieces; ++i) {
        if(b->bodyx[i] == b->headx && b->bodyy[i] == b->heady) {
            sleep(2);
            endwin();    
            exit(0);
        }
    }
}

int moveSnake(Board * b) {
    //get input
    int keypress = getch();
    
    if(keypress != KEY_DOWN && keypress != KEY_UP && keypress != KEY_LEFT && keypress != KEY_RIGHT) {
        for(short i = b->pieces-1; i > 0; --i) {
            b->bodyx[i] = b->bodyx[i-1];
            b->bodyy[i] = b->bodyy[i-1]; //drop last piece, insert old head at beginning and shift all the rest down
        }

        if(b->pieces > 0) {
            b->bodyx[0] = b->headx;
            b->bodyy[0] = b->heady;
        }
            
        if(strcmp(b->nextDirection,"RIGHT")==0 && strcmp(b->currentDirection,"RIGHT")!=0 && strcmp(b->currentDirection,"LEFT")!=0) {
            b->headx++;//move head to the right
            b->currentDirection = "RIGHT";
        } else if(strcmp(b->nextDirection,"LEFT")==0 && strcmp(b->currentDirection, "RIGHT")!=0 && strcmp(b->currentDirection,"LEFT")!=0) {
            b->headx--;//move head to the left
            b->currentDirection = "LEFT";
        } else if(strcmp(b->nextDirection,"UP")==0 && strcmp(b->currentDirection, "DOWN")!=0 && strcmp(b->currentDirection,"UP")!=0) {
            b->heady--;//move head up
            b->currentDirection = "UP";
        } else if(strcmp(b->nextDirection,"DOWN")==0 && strcmp(b->currentDirection, "DOWN")!=0 && strcmp(b->currentDirection,"UP")!=0) {
            b->heady++;//move head down
            b->currentDirection = "DOWN";
        } else {
            if(strcmp(b->currentDirection, "RIGHT") == 0) {
                b->headx++;
            }
            if(strcmp(b->currentDirection, "LEFT") == 0) {
                b->headx--;
            }
            if(strcmp(b->currentDirection, "UP") == 0) {
                b->heady--;
            }
            if(strcmp(b->currentDirection, "DOWN") == 0) {
                b->heady++;
            }
            if(keypress==27) {
                sleep(2);
                endwin();
                exit(0);
            }
        }
        return 0;
    } else {
        switch(keypress) {   
            case KEY_UP:
                b->nextDirection = "UP";
                break;
            case KEY_DOWN:
                b->nextDirection = "DOWN";
                break;
            case KEY_RIGHT:
                b->nextDirection = "RIGHT";
                break;
            case KEY_LEFT:
                b->nextDirection = "LEFT";
                break;
            default:
                break;
        }
        return 1;
    }
    return 1;
}

void printBoard(Board * b) {
    erase(); 
    for(short i = 0; i < b->width; ++i) {
        mvaddch(0, i, 178 | COLOR_PAIR(2));
    }
    for(short i = 0; i < b->height-1; i++) {
        mvaddch(i, 0, 178 | COLOR_PAIR(2));
        mvaddch(i, b->width-1, 178 | COLOR_PAIR(2));
    }
    for(short i = 0; i < b->width; ++i) {
        mvaddch(b->height-1, i, 178 | COLOR_PAIR(2));
    }
    if(b->pieces > 0) {
        for(int j = 0;j < b->pieces; ++j) {
            mvaddch(b->bodyy[j]+1, b->bodyx[j]+1, 178 | COLOR_PAIR(3));
        }
    }
    mvprintw(0,0,"%d",b->pieces/4);
    mvaddch(b->heady+1, b->headx+1, 178 | COLOR_PAIR(3)); //put head of snake
    mvaddch(b->foody+1, b->foodx+1, 48 | COLOR_PAIR(4)); //put food
    refresh();
}

Board* newBoard() {
    Board * out = malloc(sizeof(Board) + sizeof(COLS*LINES + LINES+1));
    out->width = COLS;
    out->height = LINES;
    out->headx = 4;
    out->heady = 4;
    out->bodyx = calloc(255, sizeof(short));
    out->bodyy = calloc(255, sizeof(short));
    out->currentDirection = "RIGHT";
    out->nextDirection = "RIGHT";
    out->pieces = 0;
    return out;
}   



#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <6502.h>
#include <conio.h>

#define MAX_COLS         40
#define MAX_ROWS         25

#define WALL_CODE        0x00A6
#define SPACE_CODE       0x0020
#define SNAKE_CODE       0x8020

#define CHR_MASK         0x00FF
#define CHR_SHIFT        0
#define REV_MASK         0x8000
#define REV_SHIFT        15
#define COL_MASK         0x0F00
#define COL_SHIFT        8


#define SNAKE_MAX_LENGTH MAX_COLS*MAX_ROWS


typedef enum {
    NORTH,
    WEST,
    SOUTH,
    EAST
} direction_t;


typedef struct {
    uint8_t head_x;
    uint8_t head_y;
    uint8_t tail_x;
    uint8_t tail_y;
    uint8_t grow;
    uint16_t speed;
    direction_t direction;
    uint8_t history[SNAKE_MAX_LENGTH];
    uint8_t history_front;
    uint8_t history_rear;
} snake_t;


snake_t snake;


void putchxy(uint8_t x, uint8_t y, uint16_t code)
{
    textcolor((code & COL_MASK) >> COL_SHIFT);
    revers((code & REV_MASK) >> REV_SHIFT);
    cputcxy(x, y, (code & CHR_MASK) >> CHR_SHIFT);
}


void initSnake()
{
    snake.head_x = MAX_COLS / 2;
    snake.head_y = MAX_ROWS / 2;
    snake.tail_x = snake.head_x;
    snake.tail_y = snake.head_y;
    snake.grow = 5;
    snake.speed = 500;
    snake.direction = EAST;
    snake.history_front = 0;
    snake.history_rear = 0;
}


void updatePosition(uint8_t dir, uint8_t* x, uint8_t* y)
{
    switch (dir) {
    case NORTH:
        *y -= 1;
        break;
    case SOUTH:
        *y += 1;
        break;
    case EAST:
        *x += 1;
        break;
    case WEST:
        *x -= 1;
        break;
    default:
        break;
    }
}


void updateHead()
{
    updatePosition(snake.direction, &snake.head_x, &snake.head_y);
    
    snake.history[snake.history_front] = snake.direction;
    snake.history_front = (snake.history_front + 1) % SNAKE_MAX_LENGTH;
}


void updateTail()
{
    uint8_t dir = snake.history[snake.history_rear];
    snake.history_rear = (snake.history_rear + 1) % SNAKE_MAX_LENGTH;

    updatePosition(dir, &snake.tail_x, &snake.tail_y);
}


void updateSnake()
{
    updateHead();
    putchxy(snake.head_x, snake.head_y, SNAKE_CODE);

    if (snake.grow == 0) {

        updateTail();
        putchxy(snake.tail_x, snake.tail_y, SPACE_CODE);
        
    } else {
        --snake.grow;
    }
}


void initScreen()
{
    clrscr();
    bgcolor(COLOR_GREEN);
    bordercolor(COLOR_GREEN);
    textcolor(COLOR_BLACK);
}


void resetScreen()
{
    struct regs r;
    
    /* SCINIT. 
     * Initialize VIC; restore default input/output to keyboard/screen; clear screen; 
     * set PAL/NTSC switch and interrupt timer. */
    r.pc = 0xFF81;

    /* call SCINIT */
    _sys(&r);

    clrscr();
    cputs("goodbye!!\n\r");
}


void initGameField()
{   
    uint8_t x = 0;
    uint8_t y = 0;

    for (x=0; x<MAX_COLS; x++) {
        putchxy(x, 0, WALL_CODE);
        putchxy(x, MAX_ROWS-1, WALL_CODE);
    }

    for (y=0; y<MAX_ROWS; y++) {
        putchxy(0, y, WALL_CODE);
        putchxy(MAX_COLS-1, y, WALL_CODE);
    }
}


void checkKeyboard()
{
    char ch = 0;

    if (!kbhit()) {
        return;
    }
        
    ch = toupper(cgetc());
    
    switch (ch) {
    case 'Q':
    case 145:
        snake.direction = NORTH;
        break;
    case 'A':
    case  17:
        snake.direction = SOUTH;
        break;
    case 'O':
    case 157:
        snake.direction = WEST;
        break;
    case 'P':
    case  29:
        snake.direction = EAST;
        break;
    default:
        break;
    }
}


void gameLoop()
{
    uint16_t delay = 0;

    while (true) {
        if (delay == 0) {
            updateSnake();
            delay = snake.speed;
        } else {

            --delay;
        }

       checkKeyboard();

    }
}


int main()
{
    initScreen();
    initSnake();

    initGameField();

    gameLoop();

    resetScreen();

    return 0;
}
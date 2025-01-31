#include <string.h>
#include "pico/stdlib.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <sys/time.h>

// Display libraries
#include <hagl_hal.h>
#include <hagl.h>
#include <fps.h>
#include <font6x9.h>

// Buttons
#include "buttons.h"

// Colors
#include "colors.h"

// fps counter
volatile bool fps_flag = false;
static fps_instance_t fps;
static const uint64_t US_PER_FRAME_30_FPS = 1000000 / 30;

// message
wchar_t message[32];
// text
wchar_t text[10];

// display
static hagl_backend_t *display;

// board
int board[12][12];
int showed[12][12];

bool show_timer_callback(struct repeating_timer *t)
{
    fps_flag = true;
    return true;
}

void static inline show_fps() {
    hagl_color_t green = hagl_color(display, 0, 255, 0);

    fps_flag = 0;

    /* Print the message on lower left corner. */
    swprintf(message, sizeof(message), L"%.*f FPS  ", 0, fps.current);
    hagl_put_text(display, message, 80, 0, green, font6x9);
}

void reveal_neighboor(int pos_x, int pos_y) {
    for(int i=pos_x-1; i<=pos_x+1; i++) {
        for(int j=pos_y-1; j<=pos_y+1; j++) {

            if(j<0 || j >=12 || i<0 || i>=12 || showed[i][j] == 1 || (j == pos_x && i == pos_y)) continue;
            showed[i][j] = 1;
            hagl_fill_rectangle_xywh(display, i*20, j*20, 20, 20, color_darkgray);
            hagl_draw_rectangle_xywh(display, i*20, j*20, 20, 20, color_black);
            if(board[i][j] > 0) {
                swprintf(text, sizeof(text), L"%u", board[i][j]);
                if(board[i][j] <= 2) {
                    hagl_put_text(display, text, i*20+7, j*20+5, color_lightgreen, font6x9);
                } else if(board[i][j] <= 4) {
                    hagl_put_text(display, text, i*20+7, j*20+5, color_yellow, font6x9);
                } else {
                    hagl_put_text(display, text, i*20+7, j*20+5, color_lightorange, font6x9);
                }
            } else if (board[i][j] == 0) {
                reveal_neighboor(i, j);
            }
        }
    }
    return;
}

int main() {
    // Timer for the FPS counter
    struct repeating_timer show_timer;
    // Init I/O
    stdio_init_all();
    // Init FPS
    fps_init(&fps);
    // Init display
    display = hagl_init();
    // Clear the display
    hagl_clear(display);
    /* Update displayed FPS counter every 250 ms. */
    add_repeating_timer_ms(250, show_timer_callback, NULL, &show_timer);
    // Init buttons
    buttons_init();

    // VARIABLES
    // mines
    int8_t mines;
    // mine coordinates
    int8_t mine_x, mine_y;
    // cursor
    int8_t cursor_x, cursor_y;
    // buttons
    bool key_a, key_b, joy_up, joy_down, joy_left, joy_right; 
    // ending
    bool end;

    // GAME
    while(true) {
        // ################################################# Game logic ##############################################################
        uint64_t start = time_us_64();
        hagl_flush(display);
        busy_wait_until(start + 1000000);

        // initialise variables
        mines = 20;
        mine_x = 0;
        mine_y = 0;
        cursor_x = 0;
        cursor_y = 0;
        key_a = key_b = joy_up = joy_down = joy_left = joy_right = false;
        end = false;
        srand(start);
        for(int i=0; i<12; i++) {
            for(int j=0; j<12; j++) {
                board[i][j] = 0;
                showed[i][j] = 0;
            }
        }

        // place mines
        for(int i=0; i<mines; i++) {
            do {
                mine_x = rand() % 12;
                mine_y = rand() % 12;
            } while (board[mine_x][mine_y] < 0);
            
            for(int i=mine_x-1; i<=mine_x+1; i++) {
                for(int j=mine_y-1; j<=mine_y+1; j++) {
                    if(j<0 || j >=12 || i<0 || i>=12) continue;

                    if(i==mine_x && j==mine_y) board[i][j] = -mines;
                    else board[i][j]++;
                }
            }
        }

        // draw the minefield
        for(int i=0; i<12; i++) {
            for(int j=0; j<12; j++) {
                hagl_fill_rectangle_xywh(display, i*20, j*20, 20, 20, color_green);
                hagl_draw_rectangle_xywh(display, i*20, j*20, 20, 20, color_black);
            }
        }

        // draw the cursor
        hagl_draw_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_red);

        while (true) {
            // Game logic
            // start frame timer
            uint64_t start = time_us_64();

            // delete cursor
            hagl_draw_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_black);

            // read the input
            if(end) {
                if(!gpio_get(KEY_Y)) {
                    if(end) break;
                }
            } else {
                if(!gpio_get(KEY_A)) {
                    key_b = joy_up = joy_down = joy_left = joy_right = false;
                    if(showed[cursor_x][cursor_y] == 1 || showed[cursor_x][cursor_y] == 2) continue;
                    showed[cursor_x][cursor_y] = 1;
                    if(board[cursor_x][cursor_y] < 0) {
                        hagl_fill_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_red);
                        hagl_draw_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_black);
                        end = true;
                        swprintf(text, sizeof(text), L"GAME OVER");
                        hagl_put_text(display, text, 88, 120, color_red, font6x9);
                    } else {
                        hagl_fill_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_darkgray);
                        hagl_draw_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_black);
                        if(board[cursor_x][cursor_y] > 0) {
                            swprintf(text, sizeof(text), L"%u", board[cursor_x][cursor_y]);
                            if(board[i][j] <= 2) {
                                hagl_put_text(display, text, i*20+7, j*20+5, color_lightgreen, font6x9);
                            } else if(board[i][j] <= 4) {
                                hagl_put_text(display, text, i*20+7, j*20+5, color_yellow, font6x9);
                            } else {
                                hagl_put_text(display, text, i*20+7, j*20+5, color_lightorange, font6x9);
                            }
                        } else {
                            reveal_neighboor(cursor_x, cursor_y);
                        }
                    }
                } else if(!gpio_get(KEY_B)) {
                    key_a = joy_up = joy_down = joy_left = joy_right = false;
                    if(!key_b) {
                        if(showed[cursor_x][cursor_y] == 2) {
                            showed[cursor_x][cursor_y] = 0;
                            hagl_fill_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_green);
                        } else if(showed[cursor_x][cursor_y] == 0) {
                            showed[cursor_x][cursor_y] = 2;
                            hagl_fill_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_yellow);
                        }
                        key_b = true;
                    }
                } else if(!gpio_get(JOY_UP)) {
                    key_a = key_b = joy_down = joy_left = joy_right = false;
                    if(!joy_up) {
                        if(cursor_y > 0) cursor_y--;  
                        joy_up = true;
                    }
                } else if(!gpio_get(JOY_DOWN)) {
                    key_a = key_b = joy_up = joy_left = joy_right = false;
                    if(!joy_down) {
                        if(cursor_y < 11) cursor_y++;
                        joy_down = true;
                    }
                } else if(!gpio_get(JOY_LEFT)) {
                    key_a = key_b = joy_up = joy_down = joy_right = false;
                    if(!joy_left) {
                        if(cursor_x > 0) cursor_x--;  
                        joy_left = true;
                    }
                } else if(!gpio_get(JOY_RIGHT)) {
                    key_a = key_b = joy_up = joy_down = joy_left = false;
                    if(!joy_right) {
                        if(cursor_x < 11) cursor_x++;  
                        joy_right = true;
                    }
                } else {
                    key_a = key_b = joy_up = joy_down = joy_left = joy_right = false;
                }
            }

            // draw the cursor
            hagl_draw_rectangle_xywh(display, cursor_x*20, cursor_y*20, 20, 20, color_red);

            // see win
            int squares = 12*12;
            for(int i=0; i<12; i++) {
                for(int j=0; j<12; j++) {
                    if(showed[i][j] == 1) squares--;
                }
            }
            if(squares == mines) {
                end = true;
                swprintf(text, sizeof(text), L"YOU WIN");
                hagl_put_text(display, text, 95, 120, color_lightgreen, font6x9);
            }

            // ################################################################################################################
            // Update the displayed fps if requested
            /*if (fps_flag) {
                show_fps();
            }*/

            // Flush back buffer contents to display
            hagl_flush(display);

            // Update FPS
            //fps_update(&fps);

            // Cap to 60 fps
            busy_wait_until(start + US_PER_FRAME_30_FPS);
        }

        start = time_us_64();
        busy_wait_until(start + 500000);
    }
    
    return 0;
}
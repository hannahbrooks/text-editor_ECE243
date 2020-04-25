#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// interrupt functions
void enable_A9_interrupts();
void config_GIC();
void config_interrupt (int N, int CPU_target);
void config_PS2s();
void set_A9_IRQ_stack();
void PS2_ISR();

// helper functions
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int color);
void swap(int* val1, int* val2);
void clear_screen();
void plot_string(int x, int y, char *text_ptr);
void clear_characters();

volatile int pixel_buffer_start;
volatile char *character_buffer;

// global variable declaration
int where_you_are_x, where_you_are_y;
int ctrl_ptr_x, ctrl_ptr_y;
int seen_flag = 0, shift = 0, shift_flag = 0, key_flag = 0, bs_flag = 0, ctrl_flag = 0;
int start_y = 0, end_y = 0, start_x = 80; end_x = 80;
int size = 0;
int all_lines[60] = {0};
char temp_char;
bool ctrl = false; 
char *copy;

// converts PS/2 input data to ASCII for printing 
int ps2_to_ascii (int ps2, int shift) {
    if (ps2 == 0x8014) return 1; 
    else if (shift) {        
        if (ps2 == 0x801C) return 0x41; // A
        else if (ps2 == 0x8032) return 0x42; // B
        else if (ps2 == 0x8021) return 0x43; // C
        else if (ps2 == 0x8023) return 0x44; // D
        else if (ps2 == 0x8024) return 0x45; // E
        else if (ps2 == 0x802B) return 0x46; // F
        else if (ps2 == 0x8034) return 0x47; // G
        else if (ps2 == 0x8033) return 0x48; // H
        else if (ps2 == 0x8043) return 0x49; // I
        else if (ps2 == 0x803B) return 0x4A; // J
        else if (ps2 == 0x8042) return 0x4B; // K
        else if (ps2 == 0x804B) return 0x4C; // L
        else if (ps2 == 0x803A) return 0x4D; // M
        else if (ps2 == 0x8031) return 0x4E; // N
        else if (ps2 == 0x8044) return 0x4F; // O
        else if (ps2 == 0x804D) return 0x50; // P
        else if (ps2 == 0x8015) return 0x51; // Q
        else if (ps2 == 0x802D) return 0x52; // R
        else if (ps2 == 0x801B) return 0x53; // S
        else if (ps2 == 0x802C) return 0x54; // T
        else if (ps2 == 0x803C) return 0x55; // U
        else if (ps2 == 0x802A) return 0x56; // V
        else if (ps2 == 0x801D) return 0x57; // W
        else if (ps2 == 0x8022) return 0x58; // X
        else if (ps2 == 0x8035) return 0x59; // Y
        else if (ps2 == 0x801A) return 0x5A; // Z
        else if (ps2 == 0x8016) return 0x21; // !
        else if (ps2 == 0x8052) return 0x22; // 
        else if (ps2 == 0x8026) return 0x23; // #
        else if (ps2 == 0x8025) return 0x24; // $
        else if (ps2 == 0x802E) return 0x25; // %
        else if (ps2 == 0x803D) return 0x26; // &              
        else if (ps2 == 0x8046) return 0x28; // (
        else if (ps2 == 0x8045) return 0x29; // )
        else if (ps2 == 0x803E) return 0x2A; // *
        else if (ps2 == 0x8055) return 0x2B; // +
        else if (ps2 == 0x804C) return 0x3A; // :
        else if (ps2 == 0x8049) return 0x3E; // >
        else if (ps2 == 0x804A) return 0x3F; // ?
        else if (ps2 == 0x801E) return 0x40; // @
        else if (ps2 == 0x8036) return 0x5E; // ^
        else if (ps2 == 0x804E) return 0x5F; // _
        else if (ps2 == 0x8054) return 0x7B; // {
        else if (ps2 == 0x805D) return 0x7C; // |
        else if (ps2 == 0x805B) return 0x7D; // }
        else if (ps2 == 0x800E) return 0x7E; // ~
        else if (ps2 == 0x8029) return 0x20; // space
        else if (ps2 == 0x8066) return 0x08; // backspace 
        else if (ps2 == 0x800D) return 0x09; // tab 
        else if (ps2 == 0x805A) return 0x0D; // enter 
        else if (ps2 == 0x8076) return 0x1B; // escape
        else return 0;
    }
    else {
        if (ps2 == 0x801C) return 0x61; // a 
        else if (ps2 == 0x8032) return 0x62; // b 
        else if (ps2 == 0x8021) return 0x63; // c
        else if (ps2 == 0x8023) return 0x64; // d
        else if (ps2 == 0x8024) return 0x65; // e
        else if (ps2 == 0x802B) return 0x66; // f
        else if (ps2 == 0x8034) return 0x67; // g
        else if (ps2 == 0x8033) return 0x68; // h
        else if (ps2 == 0x8043) return 0x69; // i
        else if (ps2 == 0x803B) return 0x6A; // j
        else if (ps2 == 0x8042) return 0x6B; // k
        else if (ps2 == 0x804B) return 0x6C; // l
        else if (ps2 == 0x803A) return 0x6D; // m
        else if (ps2 == 0x8031) return 0x6E; // n
        else if (ps2 == 0x8044) return 0x6F; // o
        else if (ps2 == 0x804D) return 0x70; // p
        else if (ps2 == 0x8015) return 0x71; // q
        else if (ps2 == 0x802D) return 0x72; // r
        else if (ps2 == 0x801B) return 0x73; // s
        else if (ps2 == 0x802C) return 0x74; // t
        else if (ps2 == 0x803C) return 0x75; // u
        else if (ps2 == 0x802A) return 0x76; // v
        else if (ps2 == 0x801D) return 0x77; // w
        else if (ps2 == 0x8022) return 0x78; // x
        else if (ps2 == 0x8035) return 0x79; // y
        else if (ps2 == 0x801A) return 0x7A; // z
        else if (ps2 == 0x8045) return 0x30; // 0
        else if (ps2 == 0x8016) return 0x31; // 1
        else if (ps2 == 0x801E) return 0x32; // 2
        else if (ps2 == 0x8026) return 0x33; // 3
        else if (ps2 == 0x8025) return 0x34; // 4
        else if (ps2 == 0x802E) return 0x35; // 5
        else if (ps2 == 0x8036) return 0x36; // 6
        else if (ps2 == 0x803D) return 0x37; // 7
        else if (ps2 == 0x803E) return 0x38; // 8
        else if (ps2 == 0x8046) return 0x39; // 9
        else if (ps2 == 0x8052) return 0x27; // '
        else if (ps2 == 0x8041) return 0x2C; // ,
        else if (ps2 == 0x804E) return 0x2D; // -
        else if (ps2 == 0x8049) return 0x2E; // .
        else if (ps2 == 0x804A) return 0x2F; // /
        else if (ps2 == 0x804C) return 0x3B; // ;
        else if (ps2 == 0x8055) return 0x3D; // =
        else if (ps2 == 0x8054) return 0x5B; // [
        else if (ps2 == 0x805D) return 0x5C; // 
        else if (ps2 == 0x805B) return 0x5D; // ]
        else if (ps2 == 0x800E) return 0x60; // `
        else if (ps2 == 0x8029) return 0x20; // space
        else if (ps2 == 0x8066) return 0x08; // backspace 
        else if (ps2 == 0x800D) return 0x09; // tab 
        else if (ps2 == 0x805A) return 0x0D; // enter 
        else if (ps2 == 0x8076) return 0x1B; // escape
        //else if (ps2 == 0x806B) return 0x4B; // left
        //else if (ps2 == 0x8074) return 0x4D; // right
        //else if (ps2 == 0x8075) return 0x48; // up
        //else if (ps2 == 0x8072) return 0x50; // down
        else return 0;
    }
}

// handler for all PS/2 signals
void PS2_ISR() {
    volatile int * PS2_ptr = (int *) 0xFF200100;
    int PS2_data;
    PS2_data = *(PS2_ptr);

    // actual ascii char code from PS/2
    int ascii_data = ps2_to_ascii(PS2_data, shift);

    /***********************************************************/
    /*******   MANIPULATING CTRL   *****************************/
    /***********************************************************/
    if (PS2_data == 0x8014 && ctrl == false) {
        if (ctrl_flag == -1)
            ctrl_flag = 0;
        else
            ctrl = 1;       
        ctrl_ptr_x = where_you_are_x;
        ctrl_ptr_y = where_you_are_y;
        end_x = ctrl_ptr_x;
        start_x = ctrl_ptr_x; 
    }
    else if (PS2_data == 0x8014 && ctrl == true) {
        if (ctrl_flag == 0) 
            ctrl_flag = 1;
        else {
            ctrl = 0;
            ctrl_flag = -1;
        }

        for (int i = start_y; i < end_y + 4; i++)
            draw_line(80 *4, i *4, 0 *4, i*4, 0x0000);
    }

    /***********************************************************/
    /*******   BACKSPACE FUNCTIONALITY   ***********************/
    /***********************************************************/
    if (ascii_data == 0x08 && !ctrl) {
        // reset seen_flag as to nto confuse flags later on
        seen_flag = 0;
        if (where_you_are_x > 0) { // if you're not at the start of a line
            if (bs_flag == 0) {
                // erase the cursor
                int offset = (where_you_are_y << 7) + where_you_are_x;
                *(character_buffer + offset) = 0;

                // corner case for when you're at the end
                if (where_you_are_x == 79)
                    *(character_buffer + offset + 1) = 0;

                // shift all characters on the line to the left
                for (int i = 0; i < all_lines[where_you_are_y] + 1; i++) {
                    offset = (where_you_are_y << 7) + where_you_are_x + i;
                    *(character_buffer + offset - 1) = *(character_buffer + offset);
                }

                // erase the last character on the line
                offset = (where_you_are_y << 7) + where_you_are_x + all_lines[where_you_are_y];
                *(character_buffer + offset) = 0;

                // update position
                where_you_are_x--;
                all_lines[where_you_are_y] = all_lines[where_you_are_y] - 1;

                // show that you have already performed the backspace operation
                bs_flag = 1;
            } else
                bs_flag = 0;
        }
        else if (!(all_lines[where_you_are_y] > 0)) { // if the line is full
            if (where_you_are_y > 0) { // if you're not the first line
                if (bs_flag == 0) {
                    // erase the cursor
                    int offset = (where_you_are_y << 7) + where_you_are_x;
                    *(character_buffer + offset) = 0;

                    // update position
                    where_you_are_y--;
                    where_you_are_x = all_lines[where_you_are_y];

                    bs_flag = 1;
                } else
                    bs_flag = 0;
            }
        }
        if (bs_flag) {
            // perform the backspace
            int offset = (where_you_are_y << 7) + where_you_are_x;
            *(character_buffer + offset) = 0;

            // reprint the cursor
            char data[2];
            data[0] = 0x3C;
            data[1] = '\0';
            plot_string(where_you_are_x, where_you_are_y, data);
        }
        return;
    }


    /***********************************************************/
    /*******   COPY FUNCTIONALITY  *****************************/
    /***********************************************************/
    if (ctrl && PS2_data == 0x8021) {
        int lower_bound, upper_bound;
        if (key_flag == 0) {
            key_flag = 1; 
            if (size > 0) {
                memset(copy, 0, size * (sizeof copy[0]));
                free(copy);
            }
            int k = 0; 
            int diff = where_you_are_y - ctrl_ptr_y;
            if (diff > 0)
                size = (80 - ctrl_ptr_x) + where_you_are_x + (80*(diff-1));
            else 
                size = where_you_are_x - ctrl_ptr_x; 

            copy = (char *)malloc(size * sizeof(char));
            for (int i = ctrl_ptr_y; i < where_you_are_y + 1; i++) {
                if (ctrl_ptr_y == where_you_are_y) {lower_bound = ctrl_ptr_x; upper_bound = where_you_are_x;}
                else if (i == start_y) {lower_bound = ctrl_ptr_x; upper_bound = 80;}
                else if (i == where_you_are_y) {lower_bound = 0; upper_bound = where_you_are_x;}
                else {lower_bound = 0; upper_bound = 80;}
                for (int j = lower_bound; j < upper_bound; j++) {
                    int offset = (i << 7) + j;
                    if (*(character_buffer + offset) != '<')
                        copy[k] = *(character_buffer + offset); 
                    else 
                        copy[k] = ' ';
                    k++;
                }
            }
        } else key_flag = 0;
    }

    /***********************************************************/
    /*******   CUT FUNCTIONALITY  ******************************/
    /***********************************************************/
    if (ctrl && PS2_data == 0x8022) {
        int lower_bound, upper_bound;
        if (key_flag == 0) {
            key_flag = 1; 
            if (size > 0) {
                memset(copy, 0, size * (sizeof copy[0]));
                free(copy);
            }
            int k = 0; 
            int diff = where_you_are_y - ctrl_ptr_y;
            if (diff > 0)
                size = (80 - ctrl_ptr_x) + where_you_are_x + (80*(diff-1));
            else 
                size = where_you_are_x - ctrl_ptr_x; 

            copy = (char *)malloc(size * sizeof(char));
            for (int i = ctrl_ptr_y; i < where_you_are_y + 1; i++) {
                if (ctrl_ptr_y == where_you_are_y) {lower_bound = ctrl_ptr_x; upper_bound = where_you_are_x;}
                else if (i == ctrl_ptr_y) {lower_bound = ctrl_ptr_x; upper_bound = 80;}
                else if (i == where_you_are_y) {lower_bound = 0; upper_bound = where_you_are_x-1;}
                else {lower_bound = 0; upper_bound = 80;}
                for (int j = lower_bound; j <= upper_bound; j++) {
                    int offset = (i << 7) + j;
                    if (*(character_buffer + offset) != '<')
                        copy[k] = *(character_buffer + offset); 
                    else 
                        copy[k] = ' ';
                    *(character_buffer + offset) = 0;
                    k++;
                }
            }
            int offset = (where_you_are_y << 7) + where_you_are_x;
            *(character_buffer + offset) = 0;
            
            where_you_are_x = ctrl_ptr_x;
            where_you_are_y = ctrl_ptr_y; 
            char data[2];
            data[0] = 0x3C;
            data[1] = '\0';
            plot_string(where_you_are_x, where_you_are_y, data);
        } else key_flag = 0; 
    }

    /***********************************************************/
    /*******   PASTE FUNCTIONALITY   ***************************/
    /***********************************************************/
    if (ctrl && PS2_data == 0x802A) {
        if (key_flag == 0) {
            int k = 0;
            key_flag = 1; 
            int j = where_you_are_x; 
            for (int i = 0; i < size; i++){
                if (j > 79) {
                    k++; 
                    j = 0; 
                }
                char data [1];
                data [0] = copy[i];
                if (data[0] != '<')
                    plot_string(j, where_you_are_y + k, data);
                j++; 
            }
            where_you_are_x = j;
            where_you_are_y = where_you_are_y + k; 
            char data[2];
            data[0] = 0x3C;
            data[1] = '\0';
            plot_string(where_you_are_x, where_you_are_y, data);
        } else key_flag = 0; 
    }

    /***********************************************************/
    /*******   ARROW KEY FUNCTIONALITY   ***********************/
    /***********************************************************/
    if (PS2_data == 0x806B) { // left
        if (!ctrl) {
            if (where_you_are_x > 0) { // you're allowed to move left if you're not at the edge
                if (key_flag == 0) {
                    // put the old char back where cursor was and store new char
                    int offset = (where_you_are_y << 7) + where_you_are_x;
                    *(character_buffer + offset) = temp_char;
                    temp_char = *(character_buffer + offset - 1);

                    // update youre position and show that you've done your move
                    where_you_are_x--;
                    key_flag = 1;

                    // reprint cursor
                    char data[2];
                    data[0] = 0x3C;
                    data[1] = '\0';
                    plot_string(where_you_are_x, where_you_are_y, data);
                } else
                    key_flag = 0;
            }
            else if (where_you_are_y > 0) { // if you're not on the first line
                if (key_flag == 0) {
                    // put back the old temp char where the cursor was
                    int offset = (where_you_are_y << 7) + where_you_are_x;
                    *(character_buffer + offset) = temp_char;

                    // update y position
                    where_you_are_y--;

                    // get the new temp char
                    offset = (where_you_are_y << 7) + where_you_are_x;
                    temp_char = *(character_buffer + offset);

                    // update x position
                    where_you_are_x = all_lines[where_you_are_y];
                    key_flag = 1;

                    // put cursor back
                    char data[2];
                    data[0] = 0x3C;
                    data[1] = '\0';
                    plot_string(where_you_are_x, where_you_are_y, data);
                } else
                    key_flag = 0;
            }
        }
        else { // highlight
            if (ctrl_ptr_x > 0) {
                if (key_flag == 0) {
                    ctrl_ptr_x--;
                    key_flag = 1;
                    if (ctrl_ptr_y == where_you_are_y)
                        draw_line(ctrl_ptr_x *4, ctrl_ptr_y *4, where_you_are_x *4, ctrl_ptr_y*4, 0xF81F);
                    else if (ctrl_ptr_x == 0 && ctrl_ptr_y == 0) return;
                    else draw_line(ctrl_ptr_x *4, ctrl_ptr_y *4, (ctrl_ptr_x-1) *4, ctrl_ptr_y*4, 0xF81F);
                } else key_flag = 0;
            }
            else if (ctrl_ptr_y > 0) {
                if (key_flag == 0) {
                    ctrl_ptr_y--;
                    ctrl_ptr_x = all_lines[ctrl_ptr_y];
                    key_flag = 1;
                    draw_line(ctrl_ptr_x *4, ctrl_ptr_y *4, (ctrl_ptr_x-1) *4, ctrl_ptr_y*4, 0xF81F);
                } else key_flag = 0;
            }
            if (ctrl_ptr_y <= start_y) {
                start_y = ctrl_ptr_y;
                if (ctrl_ptr_x < start_x)
                    start_x = ctrl_ptr_x;
            }
            else if (ctrl_ptr_y >= end_y) {
                end_y = ctrl_ptr_y;
                if (ctrl_ptr_x < end_x)
                    start_x = ctrl_ptr_x;
            }
        }
    }
    else if (PS2_data == 0x8074) { // right
        // essentially the same as the left, but moving right instead of left
        if (!ctrl) {
            if (where_you_are_x < 80) {
                if (key_flag == 0) {

                    int offset = (where_you_are_y << 7) + where_you_are_x;
                    *(character_buffer + offset) = temp_char;
                    temp_char = *(character_buffer + offset + 1);

                    where_you_are_x++;
                    key_flag = 1;

                    char data[2];
                    data[0] = 0x3C;
                    data[1] = '\0';
                    plot_string(where_you_are_x, where_you_are_y, data);
                } else
                    key_flag = 0;
            }
            else {
                if (where_you_are_y < 59) {
                    if (key_flag == 0) {
                        int offset = (where_you_are_y << 7) + where_you_are_x;
                        *(character_buffer + offset) = temp_char;
                        where_you_are_y++;

                        offset = (where_you_are_y << 7) + where_you_are_x;
                        temp_char = *(character_buffer + offset);

                        where_you_are_x = all_lines[where_you_are_y];
                        key_flag = 1;

                        char data[2];
                        data[0] = 0x3C;
                        data[1] = '\0';
                        plot_string(where_you_are_x, where_you_are_y, data);
                    } else
                        key_flag = 0;
                }
            }
        }
    }
    else if (PS2_data == 0x8075) { // up
         if (where_you_are_y > 0 && !ctrl) { // if you can move up
            if (key_flag == 0) {
                // put temp char back where cursor was
                int offset = (where_you_are_y << 7) + where_you_are_x;
                *(character_buffer + offset) = temp_char;

                // update y position
                where_you_are_y--;

                // hold a new temp char
                offset = (where_you_are_y << 7) + where_you_are_x;
                temp_char = *(character_buffer + offset);

                key_flag = 1;

                // put cursor back
                char data[2];
                data[0] = 0x3C;
                data[1] = '\0';
                plot_string(where_you_are_x, where_you_are_y, data);
            } else
                 key_flag = 0;
        }
    }
    else if (PS2_data == 0x8072) { // down
        // essentially the same as up but moving down instead of up
        if (where_you_are_y < 59 && !ctrl) {
            if (key_flag == 0) {

                int offset = (where_you_are_y << 7) + where_you_are_x;
                *(character_buffer + offset) = temp_char;

                where_you_are_y++;

                offset = (where_you_are_y << 7) + where_you_are_x;
                temp_char = *(character_buffer + offset);

                key_flag = 1;

                char data[2];
                data[0] = 0x3C;
                data[1] = '\0';
                plot_string(where_you_are_x, where_you_are_y, data);
            } else
                key_flag = 0;
        }
    }
    
    /***********************************************************/
    /*******   MANIPULATING SHIFT   ****************************/
    /***********************************************************/
    if (PS2_data == 0x8012 && shift == 0) {
        if (shift_flag == -1) // if third flag is shown, reset
            shift_flag = 0;
        else
            shift = 1; // if we see a 0, allow shift   
    } 
    else if (PS2_data == 0x8012 && shift == 1) { // if flag has already been seen
        if (shift_flag == 0) // if still false, set to true
            shift_flag = 1;
        else {
            shift = 0; // disallow shift
            shift_flag = -1; // set the flag to tertiary value, to prevent bouncing
        }
    }
    
    /***********************************************************/
    /*******   CHECKING FOR BREAK CODES  ***********************/
    /***********************************************************/
    if ((ascii_data == 0) && (PS2_data != 0x8012) // if you get a break and data is not shift or arrows, seen_flag is true
        && (PS2_data != 0x8074)
        && (PS2_data != 0x8072)
        && (PS2_data != 0x8075)
        && (PS2_data != 0x806B)) {
        seen_flag = 1;
        return;
    }
    
    /***********************************************************/
    /*******   DRAW CHARACTERS TO VGA   ************************/
    /***********************************************************/
    if ((seen_flag == 0) && (PS2_data != 0x8012) && ascii_data != 1 && !ctrl) {
        // prep cursor
        char data[3];
        data[0] = ascii_data;
        data[1] = 0x3C;
        data[2] = '\0';
        
        /***********************************************************/
        /*******   TAB FUNCTIONALITY   *****************************/
        /***********************************************************/
        if (ascii_data == 0x09) {
            // if you have enough space to tab
            if ((where_you_are_x + 4) < 79) {
                // put temp char back where cursor was
                int offset = (where_you_are_y << 7) + where_you_are_x;
                *(character_buffer + offset) = temp_char;

                // shift all your characters
                for (int i = all_lines[where_you_are_y]; i > where_you_are_x; i--) {
                    int offset = (where_you_are_y << 7) + i;
                    *(character_buffer + offset + 3) = *(character_buffer + offset - 1);
                    *(character_buffer + offset - 1) = 0;
                }

                // update position
                where_you_are_x+=4;
                all_lines[where_you_are_y] = all_lines[where_you_are_y] + 4;

                // redraw cursor
                data[0] = 0x3C;
                data[1] = '\0';
                plot_string(where_you_are_x, where_you_are_y, data);
            }
            return;
        }
        
        /***********************************************************/
        /*******   ENTER FUNCTIONALITY   ***************************/
        /***********************************************************/
        if (ascii_data == 0x0D) {
            // if you can move down
            if (where_you_are_y < 59) {
                // put temp char back where cursor was
                int offset = (where_you_are_y << 7) + where_you_are_x;
                *(character_buffer + offset) = temp_char;
                
                // update position
                where_you_are_y++;
                where_you_are_x = 0;

                // get a new temp char
                offset = (where_you_are_y << 7) + where_you_are_x;
                temp_char = *(character_buffer + offset);

                data[0] = 0x3C;
                data[1] = '\0';
                plot_string(where_you_are_x, where_you_are_y, data);
            }
            return;
        }
        
        /***********************************************************/
        /*******   ALL OTHER CHARACTERS  ***************************/
        /***********************************************************/
        if (all_lines[where_you_are_y] < 80) {
            // if you're not at the end of a line
            if (where_you_are_x < 79) {
                // move all characters on the line over by one
                for (int i = all_lines[where_you_are_y] + 1; i > where_you_are_x; i--) {
                    int offset = (where_you_are_y << 7) + i;
                    *(character_buffer + offset) = *(character_buffer + offset - 1);
                }

                // put the new character in 
                plot_string(where_you_are_x, where_you_are_y, data);

                // update position
                where_you_are_x++;
                all_lines[where_you_are_y] = all_lines[where_you_are_y] + 1;
            }
            // if you're at the end of a line
            else if ((where_you_are_x == 79) && (where_you_are_y < 59)) {
                // put the new character in
                plot_string(where_you_are_x, where_you_are_y, data);

                // update position
                where_you_are_x = 0;
                where_you_are_y++;

                // print new cursor
                data[0] = 0x3C;
                data[1] = '\0';
                plot_string(where_you_are_x, where_you_are_y, data);
            }
        }
    } else // reset the character flag
        seen_flag = 0;
}

// initiate the stack pointer for IRQ mode
// from DE1-SOC ARM Cortex A9 Document
void set_A9_IRQ_stack() {
    int stack, mode;
    stack = 0xFFFFFFFF - 7;

    mode = 0b11010010;
    asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
    asm("mov sp, %[ps]" : : [ps] "r" (stack));

    mode = 0b11010011;
    asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}

// configure the Generic Interrupt Controller GIC
// code taken from DE1-SOC ARM Cortex A9 Document
void config_GIC() {
    config_interrupt(79, 1);
    *((int *) 0xFFFEC104) = 0xFFFF;
    *((int *) 0xFFFEC100) = 1;
    *((int *) 0xFFFED000) = 1;
}

// configure PS2 to reset & generate interrupts
// code taken from DE1-SOC ARM Cortex A9 Document
void config_PS2s() {
    volatile int* PS2_ptr_interrupt = (int*)0xFF200104;
    *(PS2_ptr_interrupt) = 0x1;
}

// turn on interrupts
// code taken from DE1-SOC ARM Cortex A9 Document
void enable_A9_interrupts() {
    int status = 0b01010011;
    asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

// IRQ exception handler
// code taken from DE1-SOC ARM Cortex A9 Document
void __attribute__ ((interrupt)) __cs3_isr_irq () {
    int interrupt_ID = *((int *) 0xFFFEC10C);
    if (interrupt_ID == 79) {
        PS2_ISR();
    } else
        while (1);

    *((int *) 0xFFFEC110) = interrupt_ID;
}

// configure ICDISERn and ICDIPTRn
// code taken from Using the ARM GIC document
void config_interrupt (int N, int CPU_target) {
    int reg_offset, index, value, address;

    reg_offset = (N >> 3) & 0xFFFFFFFC;
    index = N & 0x1F;
    value = 0x1 << index;
    address = 0xFFFED100 + reg_offset;

    *(int *)address |= value;

    reg_offset = (N & 0xFFFFFFFC);
    index = N & 0x3;
    address = 0xFFFED800 + reg_offset + index;

    *(char *)address = (char) CPU_target;
}

// plots pixel on screen
void plot_pixel(int x, int y, short int line_color) {
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// draws a straight line to screen
void draw_line(int x0, int y0, int x1, int y1, short int color) {
    bool is_steep = abs(y1-y0) > abs(x1-x0);

    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    
    int diff_x = abs(x1 - x0);
    int diff_y = abs(y1-y0);
    int error = -(diff_x / 2);
    int y_step = 0;
    int y = y0;
    
    if (y0 < y1) {
        y_step = 1;
    }
    else {
        y_step = -1;
    }
    
    for (int x = x0; x < x1; x++) {
        if (is_steep) {
            plot_pixel(y, x, color);
            plot_pixel(y, x, color);
        }
        else {
            for (int i = 0; i < 5; i++)
                plot_pixel(x, y + i, color);
        }
        error+=diff_y;
        if (error >= 0) {
            y = y + y_step;
            error-=diff_x;
        }
    }
}

//helper function for draw line
void swap(int* val1, int* val2) {
    int temp = *val1;
    *val1 = *val2;
    *val2 = temp;
}

// plots a string to the VGA using char buffer
void plot_string(int x, int y, char *text_ptr) {
    int offset = (y << 7) + x;
        while (*(text_ptr)) {
            *(character_buffer + offset) = *(text_ptr);
            text_ptr++;
            offset++;
        }
}

// clear screen of any pixels
void clear_screen() {
    for (int x = 0; x < 320; x++) {
        for (int y = 0; y < 240; y++) {
            plot_pixel(x, y, 0x0000);
        }
    }
}

// clear screen of any characters
void clear_characters() {
    for(int y = 0; y < 60; y++){
        for(int x = 0; x < 80; x++){
            int offset = (y << 7) + x;
            *(character_buffer + offset) = 0;
            offset++;
        }
    }
}

int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    pixel_buffer_start = *pixel_ctrl_ptr;
    character_buffer = (char *) 0xC9000000;
    
    // clear the screen of pixels and characters
    clear_screen();
    clear_characters();

    // set up interrupts
    set_A9_IRQ_stack();
    config_GIC();
    config_PS2s();
    enable_A9_interrupts();
    
    // initialize character tracking
    where_you_are_x = 0;
    where_you_are_y = 0;

    // put the cursor in top left corner
    char data[2];
    data[0] = 0x3C;
    data[1] = '\0';
    plot_string(where_you_are_x, where_you_are_y, data);

    // have a loop run continuously
    while (1) {}
    
    return 0;
}

// monitor.c -- Defines functions for writing to the monitor.
//             heavily based on Bran's kernel development tutorials,
//             but rewritten for JamesM's kernel tutorials.

#include "io-x86.h"

// The VGA framebuffer starts at 0xB8000.
// Virtual address is offsetted with 0xc0000000
static uint16_t * const video_memory = (uint16_t *)(0xB8000 + 0xc0000000);
// Stores the cursor position.
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Updates the hardware cursor.
static void move_cursor() {
    // The screen is 80 characters wide...
    uint16_t cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

// Scrolls the text on the screen up by one line.
static void scroll() {

    // Get a space character with the default colour attributes.
    uint8_t attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    uint16_t blank = 0x20 /* space */ | (attributeByte << 8);

    // Row 25 is the end, this means we need to scroll up
    if (cursor_y >= 25) {
        // Move the current text chunk that makes up the screen
        // back in the buffer by a line
        int i;
        for (i = 0 * 80; i < 24 * 80; i++) {
            video_memory[i] = video_memory[i + 80];
        }

        // The last line should now be blank. Do this by writing
        // 80 spaces to it.
        for (i = 24 * 80; i < 25 * 80; i++) {
            video_memory[i] = blank;
        }
        // The cursor should now be on the last line.
        cursor_y = 24;
    }
}

// Writes a single character out to the screen.
void monitor_put(char c) {
    // The background colour is black (0), the foreground is white (15).
    uint8_t backColour = 0;
    uint8_t foreColour = 15;

    // The attribute byte is made up of two nibbles - the lower being the
    // foreground colour, and the upper the background colour.
    uint8_t attributeByte = (backColour << 4) | (foreColour & 0x0F);
    // The attribute byte is the top 8 bits of the word we have to send to the
    // VGA board.
    uint16_t attribute = attributeByte << 8;
    uint16_t *location;

    // Handle a backspace, by moving the cursor back one space
    if (c == 0x08 && cursor_x) {
        cursor_x--;
    }

    // Handle a tab by increasing the cursor's X, but only to a point
    // where it is divisible by 8.
    else if (c == 0x09) {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }

    // Handle carriage return
    else if (c == '\r') {
        cursor_x = 0;
    }

    // Handle newline by moving cursor back to left and increasing the row
    else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    }
    // Handle any other printable character.
    else if (c >= ' ') {
        location = video_memory + (cursor_y * 80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    // Check if we need to insert a new line because we have reached the end
    // of the screen.
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll the screen if needed.
    scroll();
    // Move the hardware cursor.
    move_cursor();
}

// Clears the screen, by copying lots of spaces to the framebuffer.
void monitor_clear() {
    // Make an attribute byte for the default colours
    uint8_t attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    uint16_t blank = 0x20 /* space */ | (attributeByte << 8);

    int i;
    for (i = 0; i < 80 * 25; i+=8) {
        video_memory[i] = blank;
        video_memory[i+1] = blank;
        video_memory[i+2] = blank;
        video_memory[i+3] = blank;
        video_memory[i+4] = blank;
        video_memory[i+5] = blank;
        video_memory[i+6] = blank;
        video_memory[i+7] = blank;
    }

    // Move the hardware cursor back to the start.
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

// Outputs a null-terminated ASCII string to the monitor.
void monitor_write(char *c) {
    int i = 0;
    while (c[i]) {
        monitor_put(c[i++]);
    }
}

void monitor_write_dec(uint32_t value) {
    static const char num_2_digit[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    };
    /* Maxiumum number is 0xffffffff = 4 294 967 295
     * which is exactly ten digits long */
    char printout[10 + 1] = "          ";
    int index;
    for (index = 0; 0 != value; ++index, value /= 10) {
        printout[sizeof(printout) - index - 1] = num_2_digit[value % 10];
    }
    monitor_write(printout);
}

void monitor_write_hex(uint32_t value) {
    static const char hex_num_2_hex_digit[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                               '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    uint32_t hex_num;
    char printout[] = "00000000";
    char *pos = &printout[0];
    /* Write down 8 characters */
    hex_num = (value >> 28) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 24) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 20) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 16) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 12) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 8) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = (value >> 4) & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    hex_num = value & 0x0f;
    *pos++ = hex_num_2_hex_digit[hex_num];

    monitor_write(printout);
}

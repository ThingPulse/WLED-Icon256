#pragma once

#include "wled.h"

#define FX_MODE_ICON256_WORD_CLOCK 218

byte frame[32];

// Helper to set a pixel in the frame buffer
void setFrame(byte row, byte col, bool on)
{
    if (row >= 16 || col >= 16) return;
    byte byteNum = 2 * row + (col / 8);
    byte mask = (byte)0x80 >> (col % 8);
    if (on)
        frame[byteNum] |= mask;
    else
        frame[byteNum] &= ~mask;
}

// Add a word to the frame buffer from PROGMEM
void addWordToFrame(const byte word[3])
{
    byte row = pgm_read_byte(word + 0);
    byte col = pgm_read_byte(word + 1);
    byte len = pgm_read_byte(word + 2);
    for (byte i = 0; i < len; ++i)
    {
        setFrame(row, col + i, true);
    }
}

#include "lang_en.h" // Include the English language definitions for the word clock

const byte w_en_seconds[16][3] PROGMEM = {
  {15,  0, 1}, {15,  1, 1}, {15,  2, 1}, {15,  3, 1}, {15,  4, 1}, {15,  5, 1},
  {15,  6, 1}, {15,  7, 1}, {15,  8, 1}, {15,  9, 1}, {15, 10, 1}, {15, 11, 1},
  {15, 12, 1}, {15, 13, 1}, {15, 14, 1}, {15, 15, 1}
};

// Helper to get a pixel from the frame buffer
bool getFrame(byte row, byte col)
{
    if (row >= 16 || col >= 16) return false;
    byte byteNum = 2 * row + (col / 8);
    byte mask = (byte)0x80 >> (col % 8);
    return (frame[byteNum] & mask) != 0;
}

// Clear the frame buffer
void clearFrame()
{
    for (int i = 0; i < 32; i++) {
        frame[i] = 0;
    }
}

// Display seconds on the bottom row
void showSeconds(byte s)
{
    if (s > 59) return;
    // Tens (0..5) in w_en_seconds[0..5], units (0..9) in w_en_seconds[6..15]
    byte tens = s / 10;
    byte unit = s % 10;
    addWordToFrame(w_en_seconds[tens]);
    addWordToFrame(w_en_seconds[6 + unit]);
}

/*
 * Word Clock effect
 */
uint16_t mode_icon256_word_clock()
{
    if (!strip.isMatrix) return 0; // Not for strips

    clearFrame();

    addWordToFrame(w_en_the);
    addWordToFrame(w_en_time_);
    addWordToFrame(w_en_is);

    byte h = hour(localTime);
    byte m = minute(localTime);
    byte s = second(localTime);

    showTimeWords(h, m, s);
    showSeconds(s);

    uint8_t palette_index = 0;
    // Render frame to the strip
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {   
            CRGB color = CRGB::Black;
            if (getFrame(y, x)) {
                // Use primary color for the text
                color = SEGMENT.color_from_palette(palette_index, false, true, 0);
            } 
            SEGMENT.setPixelColor(y * 16 + x, color);
            palette_index++;
        }
    }
    
    return FRAMETIME;
}

static const char _data_FX_MODE_WORD_CLOCK[] PROGMEM = "Icon256@!,Spark rate,Diffusion Speed,Turbulence,,Use palette;;Color;;2;pal=35";

class UsermodIcon256WordClock : public Usermod
{
private:
    // Frame buffer for the 16x16 matrix (2 bytes per row)

public:
    void setup() override
    {
        // Add the effect to the list
        strip.addEffect(FX_MODE_ICON256_WORD_CLOCK, mode_icon256_word_clock, _data_FX_MODE_WORD_CLOCK);
    }

    void loop() override
    {
        // Nothing to do in the main loop
    }

    uint16_t getId() override
    {
        return USERMOD_ID_WORDCLOCK;
    }
};

static UsermodIcon256WordClock icon256_wordclock;
REGISTER_USERMOD(icon256_wordclock);

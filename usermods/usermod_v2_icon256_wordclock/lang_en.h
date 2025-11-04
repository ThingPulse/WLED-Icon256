#ifndef LANG_EN_H
#define LANG_EN_H

#include <Arduino.h>
#include <pgmspace.h>

// =================== ENGLISH WORD MASK COORDINATES ===================
const byte w_en_the[3]       PROGMEM = { 0,  0, 3 };
const byte w_en_time_[3]     PROGMEM = { 0,  4, 4 }; // named w_time_ to avoid clash with <time.h>
const byte w_en_is[3]        PROGMEM = { 0,  9, 2 };
const byte w_en_half[3]      PROGMEM = { 0, 12, 4 };

const byte w_en_a[3]         PROGMEM = { 0, 13, 1 };

const byte w_en_minute[3]    PROGMEM = { 7,  0, 6 };
const byte w_en_s[3]         PROGMEM = { 7,  6, 1 };
const byte w_en_past[3]      PROGMEM = { 7,  8, 4 };
const byte w_en_to[3]        PROGMEM = { 7, 11, 2 };

const byte w_en_oclock[3]    PROGMEM = { 11, 0, 6 };
const byte w_en_in[3]        PROGMEM = { 11, 7, 2 };
const byte w_en_at[3]        PROGMEM = { 11, 9, 2 };

const byte w_en_night_ampm[3]    PROGMEM = { 12, 0, 5 };
const byte w_en_the_ampm[3]      PROGMEM = { 12, 4, 3 };
const byte w_en_morning_ampm[3]  PROGMEM = { 12, 8, 7 };

const byte w_en_evening_ampm[3]  PROGMEM = { 13, 0, 7 };
const byte w_en_afternoon_ampm[3]PROGMEM = { 13, 7, 9 };
const byte w_en_noon_ampm[3]     PROGMEM = { 13,12, 4 };

const byte w_en_pir[3]       PROGMEM = { 14, 0, 3 };
const byte w_en_on[3]        PROGMEM = { 14, 4, 2 };
const byte w_en_off[3]       PROGMEM = { 14, 7, 3 };
const byte w_en_sync_sync[3] PROGMEM = { 14,12, 4 };

const byte w_en_minutes[20][3] PROGMEM = {
  { 2, 12, 3 }, // one
  { 2, 10, 3 }, // two
  { 6, 11, 5 }, // three
  { 5,  0, 4 }, // four
  { 3,  8, 4 }, // five
  { 2,  3, 3 }, // six
  { 4,  0, 5 }, // seven
  { 3,  0, 5 }, // eight
  { 4,  8, 4 }, // nine
  { 2,  0, 3 }, // ten
  { 6,  5, 6 }, // eleven
  { 6,  0, 6 }, // twelve
  { 5,  8, 8 }, // thirteen
  { 5,  0, 8 }, // fourteen
  { 1,  0, 7 }, // quarter
  { 2,  3, 7 }, // sixteen
  { 4,  0, 9 }, // seventeen
  { 3,  0, 8 }, // eighteen
  { 4,  8, 8 }, // nineteen
  { 1,  7, 6 }  // twenty
};
const byte w_en_hours[12][3] PROGMEM = {
  { 8,  2, 3 }, // one
  { 8,  0, 3 }, // two
  { 9,  5, 5 }, // three
  {10,  0, 4 }, // four
  {10,  5, 4 }, // five
  { 8, 13, 3 }, // six
  { 9,  0, 5 }, // seven
  {10,  8, 5 }, // eight
  { 8,  9, 4 }, // nine
  {10, 12, 3 }, // ten
  { 8,  4, 6 }, // eleven
  { 9, 10, 6 }  // twelve
};

void showTimeWords(byte h, byte m, byte s) {
  byte h2 = h; // possibly incremented for "TO"

  // Minutes
  if (m == 0) {
    if (h == 0) {
      addWordToFrame(w_en_hours[11]); // twelve
      addWordToFrame(w_en_oclock);
      addWordToFrame(w_en_at);
      addWordToFrame(w_en_night_ampm);
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    } else if (h == 12) {
      addWordToFrame(w_en_hours[11]); // twelve
      addWordToFrame(w_en_oclock);
      addWordToFrame(w_en_in);
      addWordToFrame(w_en_the_ampm);
      addWordToFrame(w_en_afternoon_ampm);
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    } else {
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
      addWordToFrame(w_en_oclock);
    }
  } else {
    if (m <= 20) {
      addWordToFrame(w_en_minutes[m - 1]);
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    } else if (m < 30) {
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
      addWordToFrame(w_en_minutes[19]);           // twenty
      addWordToFrame(w_en_minutes[m - 21]);       // one..nine
    } else if (m == 30) {
      addWordToFrame(w_en_half);
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    } else if (m < 40) {
      addWordToFrame(w_en_minutes[19]);           // twenty
      addWordToFrame(w_en_minutes[60 - m - 21]);  // (to) nine..one
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    } else {
      addWordToFrame(w_en_minutes[60 - m - 1]);   // nineteen..one
      addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
    }

    if (m <= 30) {
      addWordToFrame(w_en_past);
    } else {
      addWordToFrame(w_en_to);
      ++h2;
    }
  }

  // Hours (unless exactly midnight/noon case handled above)
  if (!(m == 0 && (h == 0 || h == 12))) {
    if (h2 == 0)           addWordToFrame(w_en_hours[11]);          // twelve
    else if (h2 <= 12)     addWordToFrame(w_en_hours[h2 - 1]);
    else                   addWordToFrame(w_en_hours[h2 - 13]);
  }

  // Singular "minute" for 1 or 59; otherwise "minutes" via 'minute' + 's'
  if ((m == 1) || (m == 59)) {
    addWordToFrame(w_en_minute);
  }
  if (( (m > 1 && m < 5) || (m > 5 && m < 10) || (m > 10 && m < 15) ||
        (m > 15 && m < 20) || (m > 20 && m < 25) || (m > 25 && m < 30) ||
        (m > 30 && m < 35) || (m > 35 && m < 40) || (m > 40 && m < 45) ||
        (m > 45 && m < 50) || (m > 50 && m < 55) || (m > 55 && m < 59) )) {
    addWordToFrame(w_en_minute);
    addWordToFrame(w_en_s);
  }

  // Day-part
  if (h < 1) {
    addWordToFrame(w_en_at);
    addWordToFrame(w_en_night_ampm);
    addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
  } else if (h < 12) {
    addWordToFrame(w_en_in);
    addWordToFrame(w_en_the_ampm);
    addWordToFrame(w_en_morning_ampm);
    addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
  } else if (h < 18) {
    addWordToFrame(w_en_in);
    addWordToFrame(w_en_the_ampm);
    addWordToFrame(w_en_afternoon_ampm);
    addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
  } else if (h >= 18 && h < 21) {
    addWordToFrame(w_en_in);
    addWordToFrame(w_en_the_ampm);
    addWordToFrame(w_en_evening_ampm);
    addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
  } else {
    addWordToFrame(w_en_at);
    addWordToFrame(w_en_night_ampm);
    addWordToFrame(w_en_the); addWordToFrame(w_en_time_); addWordToFrame(w_en_is);
  }

  // “a” for quarter past/to
  if (m == 15 || m == 45) addWordToFrame(w_en_a);
}

#endif // LANG_EN_H
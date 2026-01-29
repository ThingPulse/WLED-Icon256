#ifndef LANG_CH_H
#define LANG_CH_H

#include <Arduino.h>
#include <pgmspace.h>

// =================== SWISS GERMAN WORD MASK COORDINATES ===================
// Fixed words
const byte w_ch_ziit[3] PROGMEM = { 0, 0, 4 };      // ZIIT
const byte w_ch_isch[3] PROGMEM = { 0, 5, 4 };      // ISCH
const byte w_ch_ab[3] PROGMEM = { 6, 0, 2 };        // AB
const byte w_ch_vor[3] PROGMEM = { 6, 2, 3 };        // VOR
const byte w_ch_punkt[3] PROGMEM = { 6, 5, 5 };      // PUNKT
const byte w_ch_halbi[3] PROGMEM = { 6, 11, 5 };     // HALBI
const byte w_ch_viertel[3] PROGMEM = { 4, 9, 7 };    // VIERTEL
const byte w_ch_e[3] PROGMEM = { 1, 6, 1 };          // E (as in ZWÄNZG E)

// Time of day words
const byte w_ch_am[3] PROGMEM = { 10, 11, 2 };        // AM
const byte w_ch_ide[3] PROGMEM = { 10, 13, 3 };       // IDE
const byte w_ch_morge[3] PROGMEM = { 12, 0, 5 };     // MORGE
const byte w_ch_vormittag[3] PROGMEM = { 11, 7, 9 }; // VORMITTAG
const byte w_ch_mittag[3] PROGMEM = { 11, 10, 6 }; // MITTAG
const byte w_ch_namittag[3] PROGMEM = { 12, 6, 8 };  // NAMITTAG
const byte w_ch_abig[3] PROGMEM = { 13, 0, 4 };      // ABIG
const byte w_ch_nacht[3] PROGMEM = { 11, 0, 5 };     // NACHT

// Hour words
const byte w_ch_hours[12][3] PROGMEM = {
    { 7, 0, 3 },    // EIS
    { 7, 7, 4 },    // ZWEI
    { 7, 3, 3 },   // DRÜ
    { 7, 11, 5 },   // VIERI
    { 8, 6, 5 },    // FOIFI
    { 8, 0, 6 },    // SECHSI
    { 8, 11, 5 },   // SIBNI
    { 9, 0, 5 },    // ACHTI
    { 9, 6, 4 },    // NÜNI
    { 10, 0, 5 },   // ZÄHNI
    { 10, 5, 4 },   // ELFI
    { 9, 10, 6 }    // ZWÖLFI
};

// Minute words
const byte w_ch_min_1_to_12[12][3] PROGMEM = {
    { 1, 0, 3 },    // EIS
    { 1, 7, 4 },    // ZWEI
    { 1, 12, 3 },   // DRÜ
    { 2, 0, 4 },    // VIER
    { 2, 5, 4 },    // FOIF
    { 0, 10, 5 },   // SECHS
    { 3, 9, 5 },    // SIEBE
    { 2, 11, 4 },   // ACHT
    { 4, 4, 3 },    // NÜN
    { 5, 11, 3 },   // ZÄH
    { 4, 0, 3 },    // ELF
    { 3, 4, 5 }     // ZWÖLF
};

const byte w_ch_min_13_to_19[7][3] PROGMEM = {
    { 5, 8, 5 },    // DRI(ZÄH)
    { 2, 0, 4 },   // VIER(ZÄH)
    { 4, 9, 7 },    // VIERTEL
    { 0, 10, 4 },    // SECH(ZÄH)
    { 3, 9, 4 },    // SIEB(ZÄH)
    { 2, 11, 4 },   // ACHT(ZÄH)
    { 4, 4, 3 }     // NÜN(ZÄH)
};


const byte w_ch_zwanzg[3] PROGMEM = { 5, 0, 6 }; // ZWÄNZG
const byte w_ch_eine[3] PROGMEM = { 1, 3, 4 };   // EINE
const byte w_ch_zweie[3] PROGMEM = { 1, 7, 5 };   // ZWEIE
const byte w_ch_drue[3] PROGMEM = { 1, 12, 4 };   // DRÜE
const byte w_ch_viere[3] PROGMEM = { 2, 0, 5 };    // VIERE


void showTimeWordsSwissGerman(byte h, byte m, byte s, tm t) {
  addWordToFrame(w_ch_ziit);
  addWordToFrame(w_ch_isch);

  h %= 12;

  byte next_h = (h + 1) % 12;

  if (m == 0) {
    addWordToFrame(w_ch_punkt);
    addWordToFrame(w_ch_hours[h]);
  } else if (m >= 1 && m <= 20) {
    if (m <= 12) {
         addWordToFrame(w_ch_min_1_to_12[m - 1]);
    } else if (m == 15) {
        addWordToFrame(w_ch_viertel);
    } else if (m <= 19) {
        addWordToFrame(w_ch_min_13_to_19[m - 13]);
        addWordToFrame(w_ch_min_1_to_12[9]);
    } else if (m == 20) {
        addWordToFrame(w_ch_zwanzg);
    }
    addWordToFrame(w_ch_ab);
    addWordToFrame(w_ch_hours[h]);
  } else if (m >= 21 && m <= 24) {
    if (m == 21) addWordToFrame(w_ch_eine);
    if (m == 22) addWordToFrame(w_ch_zweie); // ZWEI
    if (m == 23) addWordToFrame(w_ch_drue); // DRÜE
    if (m == 24) addWordToFrame(w_ch_viere); // VIERE
    addWordToFrame(w_ch_zwanzg);
    addWordToFrame(w_ch_ab);
    addWordToFrame(w_ch_hours[h]);
  } else if (m >= 25 && m <= 29) {
    byte min_rem = 30 - m; // 5, 4, 3, 2, 1
    addWordToFrame(w_ch_min_1_to_12[min_rem - 1]);
    addWordToFrame(w_ch_vor);
    addWordToFrame(w_ch_halbi);
    addWordToFrame(w_ch_hours[next_h]);
  } else if (m == 30) {
    addWordToFrame(w_ch_halbi);
    addWordToFrame(w_ch_hours[next_h]);
  } else if (m >= 31 && m <= 39) {
    byte min_rem = m - 30; // 1 to 9
    addWordToFrame(w_ch_min_1_to_12[min_rem - 1]);
    addWordToFrame(w_ch_ab);
    addWordToFrame(w_ch_halbi);
    addWordToFrame(w_ch_hours[next_h]);
  } else if (m >= 40 && m < 60) {
    byte min_rem = 60 - m;
    if (min_rem <= 12) {
         addWordToFrame(w_ch_min_1_to_12[min_rem - 1]);
    } else if (min_rem == 15) {
        addWordToFrame(w_ch_viertel);
    } else if (min_rem <= 19) {
        addWordToFrame(w_ch_min_13_to_19[min_rem - 13]);
        addWordToFrame(w_ch_min_1_to_12[9]);
    } else if (min_rem == 20) {
        addWordToFrame(w_ch_zwanzg);
    }

    addWordToFrame(w_ch_vor);
    addWordToFrame(w_ch_hours[next_h]);
  }

  // Time of day
  byte current_hour = h; // h is already 12-hour format from main.cpp
  if (t.tm_hour >= 12) current_hour += 12; // Reconstruct 24h for logic

  if (current_hour >= 6 && current_hour < 10) {
    // 6-10: Am Morge
    addWordToFrame(w_ch_am);
    addWordToFrame(w_ch_morge);
  } else if (current_hour >= 10 && current_hour < 12) {
    // 10-12: Am Vormittag
    addWordToFrame(w_ch_am);
    addWordToFrame(w_ch_vormittag);
  } else if (current_hour == 12) {
    addWordToFrame(w_ch_am);
    addWordToFrame(w_ch_mittag);
  } else if (current_hour >= 12 && current_hour < 17) {
    // 12-17: Am Namittag
    addWordToFrame(w_ch_am);
    addWordToFrame(w_ch_namittag);
  } else if (current_hour >= 17 && current_hour < 20) {
    // 17-20: Am Abig
    addWordToFrame(w_ch_am);
    addWordToFrame(w_ch_abig);
  } else { // 20-6
    // 20-6: I De Nacht
    addWordToFrame(w_ch_ide);
    addWordToFrame(w_ch_nacht);
  }
}

#endif // LANG_CH_H
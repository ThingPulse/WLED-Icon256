#ifndef LANG_DE_H
#define LANG_DE_H

#include <Arduino.h>
#include <pgmspace.h>

// Zero-based cell coordinates in the German stencil; umlauts occupy one cell.
const byte w_de_es[3] PROGMEM = { 0, 0, 2 }; // ES
const byte w_de_ist[3] PROGMEM = { 0, 3, 3 }; // IST
const byte w_de_eine[3] PROGMEM = { 0, 7, 4 }; // EINE
const byte w_de_ein_min[3] PROGMEM = { 1, 0, 3 }; // EIN
const byte w_de_sech[3] PROGMEM = { 2, 4, 4 }; // SECH
const byte w_de_sieb[3] PROGMEM = { 2, 9, 4 }; // SIEB
const byte w_de_zehn[3] PROGMEM = { 4, 0, 4 }; // ZEHN
const byte w_de_und[3] PROGMEM = { 4, 5, 3 }; // UND
const byte w_de_zwanzig[3] PROGMEM = { 4, 9, 7 }; // ZWANZIG
const byte w_de_minute[3] PROGMEM = { 5, 0, 6 }; // MINUTE
const byte w_de_minuten[3] PROGMEM = { 5, 0, 7 }; // MINUTEN
const byte w_de_viertel[3] PROGMEM = { 5, 8, 7 }; // VIERTEL
const byte w_de_vor[3] PROGMEM = { 6, 0, 3 }; // VOR
const byte w_de_nach[3] PROGMEM = { 6, 4, 4 }; // NACH
const byte w_de_halb[3] PROGMEM = { 6, 9, 4 }; // HALB
const byte w_de_ein_hour[3] PROGMEM = { 7, 0, 3 }; // EIN
const byte w_de_uhr[3] PROGMEM = { 10, 6, 3 }; // UHR
const byte w_de_morgens[3] PROGMEM = { 11, 0, 7 }; // MORGENS
const byte w_de_abends[3] PROGMEM = { 11, 8, 6 }; // ABENDS
const byte w_de_vormittags[3] PROGMEM = { 12, 0, 10 }; // VORMITTAGS
const byte w_de_nachts[3] PROGMEM = { 12, 10, 6 }; // NACHTS
const byte w_de_nachmittags[3] PROGMEM = { 13, 0, 11 }; // NACHMITTAGS
const byte w_de_sync[3] PROGMEM = { 14, 7, 4 }; // SYNC

const byte w_de_minutes[12][3] PROGMEM = {
    { 1, 0, 4 }, // EINS
    { 1, 4, 4 }, // ZWEI
    { 1, 8, 4 }, // DREI
    { 1, 12, 4 }, // VIER
    { 2, 0, 4 }, // FÜNF
    { 2, 4, 5 }, // SECHS
    { 2, 9, 6 }, // SIEBEN
    { 3, 0, 4 }, // ACHT
    { 3, 4, 4 }, // NEUN
    { 4, 0, 4 }, // ZEHN
    { 3, 8, 3 }, // ELF
    { 3, 11, 5 }, // ZWÖLF
};

const byte w_de_hours[12][3] PROGMEM = {
    { 7, 0, 4 }, // EINS
    { 7, 4, 4 }, // ZWEI
    { 7, 8, 4 }, // DREI
    { 7, 12, 4 }, // VIER
    { 8, 0, 4 }, // FÜNF
    { 8, 4, 5 }, // SECHS
    { 8, 9, 6 }, // SIEBEN
    { 9, 0, 4 }, // ACHT
    { 9, 4, 4 }, // NEUN
    { 9, 8, 4 }, // ZEHN
    { 9, 12, 3 }, // ELF
    { 10, 0, 5 }, // ZWÖLF
};

// Minute differences are 1..29; quarter hours are handled by the caller.
void showGermanMinuteNumber(byte minutes) {
    if (minutes == 1) {
        addWordToFrame(w_de_eine);
    } else if (minutes <= 12) {
        addWordToFrame(w_de_minutes[minutes - 1]);
    } else if (minutes < 20) {
        if (minutes == 16) addWordToFrame(w_de_sech);
        else if (minutes == 17) addWordToFrame(w_de_sieb);
        else addWordToFrame(w_de_minutes[minutes - 11]);
        addWordToFrame(w_de_zehn);
    } else {
        if (minutes > 20) {
            if (minutes == 21) addWordToFrame(w_de_ein_min);
            else addWordToFrame(w_de_minutes[minutes - 21]);
            addWordToFrame(w_de_und);
        }
        addWordToFrame(w_de_zwanzig);
    }
}

void showTimeWordsGerman(byte h, byte m, bool regionalQuarters,
                         bool halfHourRelative, bool timeOfDay) {
    addWordToFrame(w_de_es);
    addWordToFrame(w_de_ist);

    byte referencedHour = h;
    if (m >= 30 || (regionalQuarters && m == 15) || (halfHourRelative && m == 25)) {
        referencedHour = (h + 1) % 24;
    }

    if (m == 30) {
        addWordToFrame(w_de_halb);
    } else if (halfHourRelative && (m == 25 || m == 35)) {
        addWordToFrame(w_de_minutes[4]); // FÜNF
        addWordToFrame(m == 25 ? w_de_vor : w_de_nach);
        addWordToFrame(w_de_halb);
    } else if (m == 15 || m == 45) {
        if (regionalQuarters && m == 45) addWordToFrame(w_de_minutes[2]); // DREI
        addWordToFrame(w_de_viertel);
        if (!regionalQuarters) addWordToFrame(m == 15 ? w_de_nach : w_de_vor);
    } else if (m != 0) {
        byte difference = m < 30 ? m : 60 - m;
        showGermanMinuteNumber(difference);
        addWordToFrame(difference == 1 ? w_de_minute : w_de_minuten);
        addWordToFrame(m < 30 ? w_de_nach : w_de_vor);
    }

    byte hourIndex = (referencedHour + 11) % 12;
    if (m == 0 && hourIndex == 0) addWordToFrame(w_de_ein_hour);
    else addWordToFrame(w_de_hours[hourIndex]);
    if (m == 0) addWordToFrame(w_de_uhr);

    // Product convention uses the actual local hour, not the referenced hour.
    if (timeOfDay) {
        if (h < 5 || h >= 22) addWordToFrame(w_de_nachts);
        else if (h < 10) addWordToFrame(w_de_morgens);
        else if (h < 12) addWordToFrame(w_de_vormittags);
        else if (h < 18) addWordToFrame(w_de_nachmittags);
        else addWordToFrame(w_de_abends);
    }
}

#endif // LANG_DE_H

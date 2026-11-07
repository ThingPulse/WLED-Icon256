"""Host checks against stencil text; run with python3 test/icon256_wordclock/test_german.py."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
MOD = ROOT / 'usermods/usermod_v2_icon256_wordclock'
MATRIX = '''ESXISTXEINESANPR
EINSZWEIDREIVIER
FÜNFSECHSSIEBENX
ACHTNEUNELFZWÖLF
ZEHNXUNDXZWANZIG
MINUTENXVIERTELX
VORXNACHXHALBXER
EINSZWEIDREIVIER
FÜNFSECHSSIEBENX
ACHTNEUNZEHNELFX
ZWÖLFXUHRRLAMIDE
MORGENSXABENDSAG
VORMITTAGSNACHTS
NACHMITTAGSRBINS
EINAUSXSYNCLWIFI
0123450123456789'''.splitlines()
NUMBERS = 'EINS ZWEI DREI VIER FÜNF SECHS SIEBEN ACHT NEUN ZEHN ELF ZWÖLF'.split()
MINUTES = ['EINE'] + NUMBERS[1:] + [
    'DREI ZEHN', 'VIER ZEHN', 'FÜNF ZEHN', 'SECH ZEHN', 'SIEB ZEHN',
    'ACHT ZEHN', 'NEUN ZEHN', 'ZWANZIG', 'EIN UND ZWANZIG',
] + [n + ' UND ZWANZIG' for n in NUMBERS[1:9]]


def expected(h, m, options):
    regional, half, suffix = options & 1, options & 2, options & 4
    hour = h
    words = [('ES', [0]), ('IST', [0])]
    if m == 0:
        pass
    elif m == 30:
        words += [('HALB', [6])]
        hour += 1
    elif half and m in (25, 35):
        words += [('FÜNF', [2]), ('VOR' if m == 25 else 'NACH', [6]), ('HALB', [6])]
        hour += 1
    elif m in (15, 45):
        if regional:
            if m == 45:
                words += [('DREI', [1])]
            words += [('VIERTEL', [5])]
            hour += 1
        else:
            words += [('VIERTEL', [5]), ('NACH' if m == 15 else 'VOR', [6])]
            hour += m == 45
    else:
        difference = min(m, 60 - m)
        words += [(w, [0] if w == 'EINE' else range(1, 5)) for w in MINUTES[difference - 1].split()]
        words += [('MINUTE' if difference == 1 else 'MINUTEN', [5]),
                  ('NACH' if m < 30 else 'VOR', [6])]
        hour += m > 30
    name = NUMBERS[(hour - 1) % 12]
    words += [('EIN' if name == 'EINS' and m == 0 else name, range(7, 11))]
    if m == 0:
        words += [('UHR', [10])]
    if suffix:
        label = next(label for end, label in [(5, 'NACHTS'), (10, 'MORGENS'),
                     (12, 'VORMITTAGS'), (18, 'NACHMITTAGS'), (22, 'ABENDS'), (24, 'NACHTS')] if h < end)
        words += [(label, range(11, 14))]
    cells = set()
    last = -1
    for word, rows in words:
        row = next(r for r in rows if word in MATRIX[r])
        start = row * 16 + MATRIX[row].index(word)
        assert start > last, (h, m, words)
        cells.update(range(start, start + len(word)))
        last = start
    return ''.join('1' if i in cells else '0' for i in range(256))


def main():
    source = (MOD / 'usermod_v2_icon256_wordclock.cpp').read_text()
    # Exercise the production frame buffer and seconds code as well as the German header.
    helpers = source[source.index('void setFrame'):source.index('#include "lang_en.h"')]
    seconds = source[source.index('const byte w_en_seconds'):source.index('/*\n * Word Clock effect')]
    harness = '''#include <cstdio>
#include <cassert>
#include <Arduino.h>
#include <pgmspace.h>
byte frame[32];
''' + helpers + '\n#include "lang_de.h"\n' + seconds + '''
void output() {
    for (int r=0;r<16;++r) for(int c=0;c<16;++c) putchar(getFrame(r,c)?'1':'0');
    putchar('\\n');
}
int main() {
    for(int o=0;o<8;++o) for(int h=0;h<24;++h) for(int m=0;m<60;++m) {
        clearFrame(); showTimeWordsGerman(h,m,o&1,o&2,o&4); output();
    }
    for(int s=0;s<60;++s) { clearFrame(); showSeconds(s); output(); }
    clearFrame(); addWordToFrame(w_de_sync); output();
}
'''
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        (tmp / 'Arduino.h').write_text('#pragma once\nusing byte = unsigned char;\n')
        (tmp / 'pgmspace.h').write_text('#pragma once\n#define PROGMEM\n#define pgm_read_byte(p) (*(p))\n')
        (tmp / 'check.cpp').write_text(harness)
        subprocess.run(['c++', '-std=c++11', '-Wall', '-Wextra', '-Werror', '-I', str(tmp),
                        '-I', str(MOD), str(tmp / 'check.cpp'), '-o', str(tmp / 'check')], check=True)
        lines = iter(subprocess.check_output([str(tmp / 'check')], text=True).splitlines())
        for options in range(8):
            for h in range(24):
                for m in range(60):
                    assert next(lines) == expected(h, m, options), (h, m, options)
        for s in range(60):
            cells = {240 + s // 10, 246 + s % 10}
            assert next(lines) == ''.join('1' if i in cells else '0' for i in range(256)), s
        assert next(lines) == '0' * 231 + '1111' + '0' * 21
        assert next(lines, None) is None
    print('Passed: 11,520 complete sentence frames, all 60 seconds, and German SYNC.')


if __name__ == '__main__':
    main()

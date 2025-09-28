#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

/* reverse a string in place */
static void reverse_str(char *s) {
    size_t i = 0, j = strlen(s);
    if (j == 0) return;
    j--;
    while (i < j) {
        char c = s[i];
        s[i] = s[j];
        s[j] = c;
        i++; j--;
    }
}

/* turn a digit into 0–9 or A–F */
static char digit_to_char(int d) {
    return (d < 10) ? (char)('0' + d) : (char)('A' + (d - 10));
}

/* make a 32-bit number into a 32-char binary string */
static void u32_to_bin(uint32_t v, char *out32) {
    for (int i = 31; i >= 0; --i) {
        out32[31 - i] = ((v >> i) & 1u) ? '1' : '0';
    }
    out32[32] = '\0';
}

/* A1 stuff */

/* division/remainder algorithm */
void div_convert(uint32_t n, int base, char *out) {
    if (!out || base < 2 || base > 16) return;
    if (n == 0) { strcpy(out, "0"); return; }

    char tmp[65];
    int pos = 0;

    while (n > 0) {
        uint32_t q = n / (uint32_t)base;
        uint32_t r = n % (uint32_t)base;
        tmp[pos++] = digit_to_char((int)r);
        n = q;
    }
    tmp[pos] = '\0';
    reverse_str(tmp);
    strcpy(out, tmp);
}

/* subtraction-of-powers algorithm */
void sub_convert(uint32_t n, int base, char *out) {
    if (!out || base < 2 || base > 16) return;
    if (n == 0) { strcpy(out, "0"); return; }

    uint64_t p = 1;
    int k = 0;
    while (1) {
        uint64_t next = p * (uint64_t)base;
        if (next > (uint64_t)n) break;
        p = next; k++;
    }

    char buf[65];
    int pos = 0, started = 0;
    for (int i = k; i >= 0; --i) {
        int digit = (int)(n / p);
        if (digit != 0 || started) {
            buf[pos++] = digit_to_char(digit);
            started = 1;
        }
        n -= (uint32_t)(digit * p);
        p /= (uint64_t)base;
    }
    if (!started) buf[pos++] = '0';
    buf[pos] = '\0';
    strcpy(out, buf);
}

/* helper for print_tables */
static void print_all_bases_line(const char *label, uint32_t value) {
    char bin[65], oct[24], hex[16];
    div_convert(value, 2,  bin);
    div_convert(value, 8,  oct);
    div_convert(value, 16, hex);
    printf("%s: Binary=%s Octal=%s Decimal=%u Hex=%s\n", label, bin, oct, value, hex);
}

/* three-line table: original, shifted, masked */
void print_tables(uint32_t n) {
    print_all_bases_line("Original", n);
    uint32_t shifted = (uint32_t)(n << 3);
    print_all_bases_line("Left Shift by 3", shifted);
    uint32_t masked = (uint32_t)(n & 0xFFu);
    print_all_bases_line("AND with 0xFF", masked);
}

/* A2 stuff */

/* octal -> binary, 3 bits per digit */
void oct_to_bin(const char *oct, char *out) {
    if (!oct || !out) return;
    char *pout = out;
    for (const char *p = oct; *p; ++p) {
        if (*p < '0' || *p > '7') { *out = '\0'; return; }
        int d = *p - '0';
        *pout++ = '0' + ((d >> 2) & 1);
        *pout++ = '0' + ((d >> 1) & 1);
        *pout++ = '0' + ((d >> 0) & 1);
    }
    *pout = '\0';
}

/* octal -> hex (go through binary) */
void oct_to_hex(const char *oct, char *out) {
    if (!oct || !out) return;

    char binbuf[1024];
    oct_to_bin(oct, binbuf);
    size_t L = strlen(binbuf);
    if (L == 0) { strcpy(out, ""); return; }

    int pad = (int)((4 - (L % 4)) % 4);
    char padded[1040];
    int pos = 0;
    for (int i = 0; i < pad; ++i) padded[pos++] = '0';
    memcpy(padded + pos, binbuf, L);
    pos += (int)L;
    padded[pos] = '\0';

    char hexbuf[1040];
    int hpos = 0;
    for (int i = 0; i < pos; i += 4) {
        int nib = (padded[i]-'0')*8 + (padded[i+1]-'0')*4 +
                  (padded[i+2]-'0')*2 + (padded[i+3]-'0');
        hexbuf[hpos++] = digit_to_char(nib);
    }
    hexbuf[hpos] = '\0';

    char *q = hexbuf;
    while (*q == '0' && *(q+1) != '\0') q++;
    strcpy(out, q);
}

/* hex -> binary, 4 bits per digit */
void hex_to_bin(const char *hex, char *out) {
    if (!hex || !out) return;
    char *pout = out;
    for (const char *p = hex; *p; ++p) {
        char c = *p;
        int val;
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = 10 + (c - 'A');
        else if (c >= 'a' && c <= 'f') val = 10 + (c - 'a');
        else { *out = '\0'; return; }
        *pout++ = '0' + ((val >> 3) & 1);
        *pout++ = '0' + ((val >> 2) & 1);
        *pout++ = '0' + ((val >> 1) & 1);
        *pout++ = '0' + ((val >> 0) & 1);
    }
    *pout = '\0';
}

/* signed integer -> sign magnitude */
void to_sign_magnitude(int32_t n, char *out) {
    if (!out) return;
    if (n == 0) { memset(out, '0', 32); out[32] = '\0'; return; }
    if (n > 0) { u32_to_bin((uint32_t)n, out); return; }

    uint32_t mag;
    if (n == INT32_MIN) mag = 0x7FFFFFFF;  /* can't do -INT_MIN exactly */
    else mag = (uint32_t)(-n) & 0x7FFFFFFF;

    out[0] = '1';
    for (int i = 30; i >= 0; --i) {
        out[31 - i] = ((mag >> i) & 1u) ? '1' : '0';
    }
    out[32] = '\0';
}

/* one's complement */
void to_ones_complement(int32_t n, char *out) {
    if (!out) return;
    if (n == 0) { memset(out, '0', 32); out[32] = '\0'; return; }
    if (n > 0) { u32_to_bin((uint32_t)n, out); return; }

    uint32_t mag = (uint32_t)(-(int64_t)n);
    uint32_t flipped = ~mag;
    u32_to_bin(flipped, out);
}

/* two's complement */
void to_twos_complement(int32_t n, char *out) {
    if (!out) return;
    u32_to_bin((uint32_t)n, out);
}

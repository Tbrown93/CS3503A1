#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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

static char digit_to_char(int d) {
    return (d < 10) ? (char)('0' + d) : (char)('A' + (d - 10));
}

/* Division/remainder algorithm */
void div_convert(uint32_t n, int base, char *out) {
    if (base < 2 || base > 16 || out == NULL) return;
    if (n == 0) { strcpy(out, "0"); return; }

    char tmp[65]; /* enough for base-2 of 64-bit, we only need 33 for 32-bit but this is safe */
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

/* Subtraction-of-powers algorithm */
void sub_convert(uint32_t n, int base, char *out) {
    if (base < 2 || base > 16 || out == NULL) return;
    if (n == 0) { strcpy(out, "0"); return; }

    /* find highest power base^k <= n, using 64-bit to avoid overflow while stepping up */
    uint64_t p = 1;
    int k = 0;
    while (1) {
        uint64_t next = p * (uint64_t)base;
        if (next > (uint64_t)n) break;
        p = next;
        k++;
    }

    char buf[65];
    int pos = 0;
    int started = 0;
    for (int i = k; i >= 0; --i) {
        int digit = (int)(n / p);
        if (digit != 0 || started) {
            buf[pos++] = digit_to_char(digit);
            started = 1;
        }
        n -= (uint32_t)(digit * p);
        p /= (uint64_t)base;
    }
    if (!started) { /* n was 0, but we handled earlier; defensive */
        buf[pos++] = '0';
    }
    buf[pos] = '\0';
    strcpy(out, buf);
}

/* Helper to print one line with all bases */
static void print_all_bases_line(const char *label, uint32_t value) {
    char bin[65], oct[24], hex[16];
    div_convert(value, 2,  bin);
    div_convert(value, 8,  oct);
    div_convert(value, 16, hex);
    /* EXACT required format from the spec (no extra spaces, uppercase hex). */
    printf("%s: Binary=%s Octal=%s Decimal=%u Hex=%s\n", label, bin, oct, value, hex);
}

/* Required by spec: three lines, exact labels and equals signs. */
void print_tables(uint32_t n) {
    /* Original */
    print_all_bases_line("Original", n);

    /* Left Shift by 3 (unsigned 32-bit semantics) */
    uint32_t shifted = (uint32_t)(n << 3);
    print_all_bases_line("Left Shift by 3", shifted);

    /* AND with 0xFF */
    uint32_t masked = (uint32_t)(n & 0xFFu);
    print_all_bases_line("AND with 0xFF", masked);
}

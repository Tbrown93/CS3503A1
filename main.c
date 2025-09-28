#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* A1 functions */
void div_convert(uint32_t n, int base, char *out);
void sub_convert(uint32_t n, int base, char *out);
void print_tables(uint32_t n);

/* A2 functions */
void oct_to_bin(const char *oct, char *out);
void oct_to_hex(const char *oct, char *out);
void hex_to_bin(const char *hex, char *out);
void to_sign_magnitude(int32_t n, char *out);
void to_ones_complement(int32_t n, char *out);
void to_twos_complement(int32_t n, char *out);

/* trim right-side whitespace/newlines */
static void rstrip(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || isspace((unsigned char)s[len-1]))) {
        s[--len] = '\0';
    }
}

/* this chooses default test file: prefer A2 */
static const char* pick_default_file(void) {
    /* tries A2, then A1, then common alt names */
    const char *candidates[] = {
        "A2_tests.txt",
        "A1_tests.txt",
        "a2_test.txt",
        "a1_test_file.txt",
        NULL
    };
    for (int i = 0; candidates[i]; ++i) {
        FILE *f = fopen(candidates[i], "r");
        if (f) { fclose(f); return candidates[i]; }
    }
    return "A2_tests.txt"; /* last resort */
}

int main(int argc, char **argv) {
    const char *infile = (argc >= 2) ? argv[1] : pick_default_file();
    FILE *fin = fopen(infile, "r");
    if (!fin) {
        fprintf(stderr, "Error: could not open test file (tried %s)\n", infile);
        return 1;
    }

    FILE *fout = fopen("output.txt", "w");
    if (!fout) {
        fprintf(stderr, "Warning: could not open output.txt for writing. Printing to stdout only.\n");
    }

    char line[1024];
    unsigned total = 0, passed = 0;

    /* big scratch buffers for outputs */
    char got[4096];

    while (fgets(line, sizeof(line), fin)) {
        rstrip(line);
        if (line[0] == '\0') continue;  /* blank */
        if (line[0] == '#')  continue;  /* comment */

        /* up to 4 tokens works for A1 and A2 cases */
        char cmd[128] = {0};
        char arg1[1024] = {0};
        char arg2[128] = {0};
        char expected[4096] = {0};

        int n_tokens = sscanf(line, "%127s %1023s %127s %4095s", cmd, arg1, arg2, expected);
        if (n_tokens < 2) continue;

        /* A1: div_convert/sub_convert use: <func> <n> <base> <expected> */
        if (strcmp(cmd, "div_convert") == 0 || strcmp(cmd, "sub_convert") == 0) {
            if (n_tokens != 4) {
                if (fout) fprintf(fout, "Malformed test line: %s\n", line);
                else      fprintf(stdout,"Malformed test line: %s\n", line);
                continue;
            }
            uint32_t n = (uint32_t)strtoul(arg1, NULL, 10);
            int base   = (int)strtol(arg2, NULL, 10);

            if (strcmp(cmd, "div_convert") == 0) {
                div_convert(n, base, got);
            } else {
                sub_convert(n, base, got);
            }

            int ok = (strcmp(got, expected) == 0);
            total += 1;
            if (ok) passed += 1;

            if (fout) fprintf(fout, "Test: %s (%u, %d) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, n, base, expected, got, ok ? "PASS" : "FAIL");
            printf(        "Test: %s (%u, %d) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, n, base, expected, got, ok ? "PASS" : "FAIL");
            continue;
        }

        /* A1: print_tables <n> FORMATTED_OUTPUT (we just run it) */
        if (strcmp(cmd, "print_tables") == 0) {
            if (n_tokens < 2) {
                if (fout) fprintf(fout, "Malformed test line: %s\n", line);
                else      fprintf(stdout,"Malformed test line: %s\n", line);
                continue;
            }
            uint32_t n = (uint32_t)strtoul(arg1, NULL, 10);
            if (fout) fprintf(fout, "print_tables(%u) -> [FORMATTED OUTPUT]\n", n);
            printf("print_tables(%u) -> [FORMATTED OUTPUT]\n", n);
            print_tables(n);
            /* no PASS/FAIL for formatted output */
            continue;
        }

        /* A2: mapping/representation tests use: <func> <input> <expected> */
        if (strcmp(cmd, "oct_to_bin") == 0) {
            if (n_tokens != 3) { if (fout) fprintf(fout,"Malformed: %s\n", line); else printf("Malformed: %s\n", line); continue; }
            oct_to_bin(arg1, got);
            int ok = (strcmp(got, arg2) == 0);
            total++; if (ok) passed++;
            if (fout) fprintf(fout, "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            printf(        "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            continue;
        }

        if (strcmp(cmd, "oct_to_hex") == 0) {
            if (n_tokens != 3) { if (fout) fprintf(fout,"Malformed: %s\n", line); else printf("Malformed: %s\n", line); continue; }
            oct_to_hex(arg1, got);
            int ok = (strcmp(got, arg2) == 0);
            total++; if (ok) passed++;
            if (fout) fprintf(fout, "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            printf(        "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            continue;
        }

        if (strcmp(cmd, "hex_to_bin") == 0) {
            if (n_tokens != 3) { if (fout) fprintf(fout,"Malformed: %s\n", line); else printf("Malformed: %s\n", line); continue; }
            hex_to_bin(arg1, got);
            int ok = (strcmp(got, arg2) == 0);
            total++; if (ok) passed++;
            if (fout) fprintf(fout, "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            printf(        "Test: %s (%s) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, arg1, arg2, got, ok ? "PASS" : "FAIL");
            continue;
        }

        if (strcmp(cmd, "to_sign_magnitude") == 0 ||
            strcmp(cmd, "to_ones_complement") == 0 ||
            strcmp(cmd, "to_twos_complement") == 0) {
            if (n_tokens != 3) { if (fout) fprintf(fout,"Malformed: %s\n", line); else printf("Malformed: %s\n", line); continue; }

            long long tmp = strtoll(arg1, NULL, 10);
            int32_t n = (int32_t)tmp;

            if (strcmp(cmd, "to_sign_magnitude") == 0) {
                to_sign_magnitude(n, got);
            } else if (strcmp(cmd, "to_ones_complement") == 0) {
                to_ones_complement(n, got);
            } else {
                to_twos_complement(n, got);
            }

            int ok = (strcmp(got, arg2) == 0);
            total++; if (ok) passed++;
            if (fout) fprintf(fout, "Test: %s (%d) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, n, arg2, got, ok ? "PASS" : "FAIL");
            printf(        "Test: %s (%d) -> Expected: \"%s\", Got: \"%s\" [%s]\n",
                              cmd, n, arg2, got, ok ? "PASS" : "FAIL");
            continue;
        }

         /* unknown directive
           If a line in the test file has a command we don’t recognize, I set it up to just skip it instead of crashing. 
           This keeps the runner flexible in case the file has typos or extra lines. */
        if (fout) fprintf(fout, "Skipping unknown directive: %s\n", line);
        else      fprintf(stdout,"Skipping unknown directive: %s\n", line);
    }

    if (fout) {
        fprintf(fout, "Summary: %u/%u numeric tests passed\n", passed, total);
        fclose(fout);
    }
    printf("Summary: %u/%u numeric tests passed\n", passed, total);

    fclose(fin);
    return 0;
}

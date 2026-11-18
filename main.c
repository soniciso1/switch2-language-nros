// One language per NRO.
//
// Controller input never worked under kaiHEN and the SD target file could not
// be read, so a menu inside one program is not reachable. hbmenu is the menu
// instead: build this source once per language, with the tag in a generated
// header, and pick the language by launching the matching NRO.
//
// build-all.sh regenerates include/lang-tag.h and builds one NRO per tag.

#include <stdio.h>
#include <string.h>
#include <switch.h>

#include "lang-tag.h"

#define P(...) do { printf(__VA_ARGS__); consoleUpdate(NULL); } while (0)

#ifndef LANG_TAG
#define LANG_TAG "en-US"
#endif

static u64 packTag(const char *tag, size_t len) {
    u64 code = 0;
    for (size_t i = 0; i < len && i < 8; i++) {
        code |= ((u64)(unsigned char)tag[i]) << (8 * i);
    }
    return code;
}

static void tagText(u64 code, char *out, size_t outSize) {
    size_t n = 0;
    for (int i = 0; i < 8 && n + 1 < outSize; i++) {
        char c = (char)((code >> (8 * i)) & 0xff);
        if (c == 0) break;
        out[n++] = (c >= 0x20 && c < 0x7f) ? c : '?';
    }
    out[n] = '\0';
}

static u64 readLanguage(void) {
    u64 out = 0;
    if (R_FAILED(setInitialize())) return 0;
    setGetLanguageCode(&out);
    setExit();
    return out;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    const u64 target = packTag(LANG_TAG, strlen(LANG_TAG));
    char before[16], after[16];

    consoleInit(NULL);

    {
        u64 now = readLanguage();
        tagText(now, before, sizeof(before));
        P("set language \"%s\"\n\n", LANG_TAG);
        P("before     \"%s\"\n", before);
    }

    if (R_FAILED(setsysInitialize())) {
        P("set:sys unreachable\n");
        goto hold;
    }
    {
        Result rw = setsysSetLanguageCode(target);
        P("write      0x%08x\n", rw);
    }
    setsysExit();

    {
        u64 now = readLanguage();
        tagText(now, after, sizeof(after));
        P("after      \"%s\"\n", after);
        P("result     %s\n", now == target ? "APPLIED" : "NOT APPLIED");
    }

hold:
    P("\nReboot to confirm it stays.\n");
    for (int i = 20; i > 0; i--) {
        P("%d seconds until this closes itself\n", i);
        for (int s = 0; s < 10; s++) svcSleepThread(100000000ULL);
    }

    consoleExit(NULL);
    return 0;
}

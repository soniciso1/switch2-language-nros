# Switch 2 language NROs

Eighteen NROs, one per system language, plus the source they are built from.

Each NRO writes one language through `set:sys` and reads it back:

```
before     "ja"
write      0x00000000
after      "ko"
result     APPLIED
```

No controller input, no SD file, no menu. The NRO you launch is the selection.
`Japanese.nro` reverts to Japanese. Every build writes immediately and exits on
its own after 20 seconds.

## Install

Unzip into `sdmc:/switch/`. hbmenu lists them alphabetically.


## How the write works

- Reads use the `set` service: `setGetLanguageCode`, `setGetAvailableLanguageCodes`.
- The write is `setsysSetLanguageCode(u64)` on `set:sys`, from an application-class
  process. That permission was measured, not assumed: a same-value write returned
  `0` while the language read reported `0x616a` ("ja" packed little-endian).
- The language code is the tag's bytes packed little-endian: `ja` is `0x616a`,
  `ko` is `0x6f6b`, `en-US` is `0x000053552d6e65`.

The console's own Settings app can show no Language row at all, on some region
configurations, and the write still applies. Do not judge success by that menu.

## Build

Requires the Switch 2 devkit from [ouncebrew/ounce-devkit](https://github.com/ouncebrew/ounce-devkit),
which provides `libounce`. **A plain libnx build is rejected by kaiHEN**: it
identifies a Switch 2 NRO by an `OUN0` MOD0 extension with platform word `2`, and
libnx emits only `LNY0`-`LNY2`.

```sh
docker build -t ounce-devkit /path/to/ounce-devkit
docker run --rm -v "$PWD:/build/pack" -w /build/pack ounce-devkit sh build-all.sh
```

`build-all.sh` regenerates `include/lang-tag.h` per tag and builds one NRO per
language. To build a single language by hand:

```sh
printf '#define LANG_TAG "ko"\n' > include/lang-tag.h
make TARGET=Korean BUILD=build-ko
```

The Makefile includes `$(DEVKITPRO)/libounce/switch2_rules` and links with
libounce's own `switch.specs`; that is what emits the `OUN0` block.

## Files

| Path | Contents |
| --- | --- |
| `*.nro` | one program per language |
| `source/main.c` | the program |
| `build-all.sh` | builds all eighteen |
| `include/lang-tag.h` | generated tag header, defaults to `en-US` |
| `Makefile` | standard Switch build, libounce rules |

## Credits

Idea and reference implementation: [hrvtuirvw/switch2-language-experiment](https://github.com/hrvtuirvw/switch2-language-experiment).
Its console runs, the MakeLanguageCode table, and the region/language readbacks are what this
project builds on. The language codes here were confirmed against that reference.

Entry chain: [pepsipu/ns2jb](https://github.com/pepsipu/ns2jb).
Switch 2 devkit: [ouncebrew/ounce-devkit](https://github.com/ouncebrew/ounce-devkit) and [ouncebrew/libounce](https://github.com/ouncebrew/libounce).

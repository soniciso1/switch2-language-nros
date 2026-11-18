#!/bin/sh
# Builds one NRO per language tag. Run inside the ounce-devkit image:
#   docker run --rm -v "<this dir>:/build/switch2-lang-one" \
#     -w /build/switch2-lang-one ounce-devkit sh build-all.sh
#
# The tag goes in include/lang-tag.h rather than on the compiler command line,
# because a -D value with quotes and hyphens is mangled by the shell twice.
set -eu

for tag in ja en-US fr de it es ko nl pt ru en-GB fr-CA es-419 zh-Hans zh-Hant pt-BR pl th; do
    printf '#define LANG_TAG "%s"\n' "$tag" > include/lang-tag.h
    rm -rf "build-$tag"
    make TARGET="switch2-lang-$tag" BUILD="build-$tag" >/dev/null
    echo "built switch2-lang-$tag.nro"
done

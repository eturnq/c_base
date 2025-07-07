#!/bin/sh

LIBSRC=$(find . -mindepth 2 -name "*.c" | paste -s -d' ' -)
TESTSRC=test.c

LIBOBJ=$(for f in $LIBSRC; do echo $f.o; done | paste -s -d' ' -)

LIB=base.a
CC=gcc
AR=ar

LDFLAGS="-L. -l:base.a"

# Rules
echo rule cc
echo "  command = $CC -c -o \$out \$in"

echo rule mklib
echo "  command = $AR -r -c -s \$out \$in"

echo rule linktest
echo "  command = $CC -o \$out $TESTSRC $LDFLAGS"

# Build targets
echo "build $LIB: mklib $LIBOBJ"
echo "build test: linktest $LIB $TESTSRC"

for f in $LIBSRC; do
	echo "build $f.o: cc $f"
done


#!/bin/sh

if [ ! -f build.ninja ]; then ./gen.sh > build.ninja; fi
ninja

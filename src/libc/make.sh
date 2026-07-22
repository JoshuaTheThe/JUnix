#!/bin/bash
clang -m32 stdio.c -o stdio.o -nostdlib -nostartfiles -c -I .

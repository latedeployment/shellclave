#!/usr/bin/env bash
# read if needed https://astyle.sourceforge.net/astyle.html
astyle --style=bsd \
    --indent-switches \
    --indent-cases \
    --max-code-length=80 \
    --add-braces \
    --convert-tabs \
    --suffix=none \
    --align-pointer=type \
    --align-reference=type \
    *.c *.h

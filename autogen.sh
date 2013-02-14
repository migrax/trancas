#! /bin/sh

autoreconf --force --install
aclocal -I m4
./configure $@


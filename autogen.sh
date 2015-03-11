#!/bin/sh
mkdir -p m4 build-aux || exit $?
autoreconf -fi

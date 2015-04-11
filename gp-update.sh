#!/usr/bin/env bash
#
# Run this script from root source directory and provide path to Geany-Plugins
# root source tree as the only argument.
#

SRCDIR=`pwd`
DSTDIR=$1

cp -rv $SRCDIR/geany-plugins/* "$DSTDIR"
cp -v $SRCDIR/overview/*.[ch] "$DSTDIR/overview/overview/" || exit $?
cp -v $SRCDIR/data/prefs.ui "$DSTDIR/overview/data/" || exit $?

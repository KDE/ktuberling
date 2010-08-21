#! /usr/bin/env bash
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.cpp pics/layout.i18n -o $podir/ktuberling.pot

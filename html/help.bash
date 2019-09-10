#!/bin/bash

HELPCOMP=C:/Qt/5.13.1/msvc2017_64/bin/qhelpgenerator
HELPPROJ=xeropath.qhp
HELPCOLL=xeropath.qhcp
HELPPROJOUT=xeropath.qch
HELPCOLLOUT=xeropath.qhc

FILES=index.html

$HELPCOMP $HELPPROJ -o $HELPPROJOUT
$HELPCOMP $HELPCOLL -o $HELPCOLLOUT

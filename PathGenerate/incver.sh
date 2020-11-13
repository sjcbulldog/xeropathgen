#!/bin/bash

PATH=/bin:/usr/bin

major=`grep XERO_MAJOR_VERSION build.h | awk '{ print $3 }'`
minor=`grep XERO_MINOR_VERSION build.h | awk '{ print $3 }'`
micro=`grep XERO_MICRO_VERSION build.h | awk '{ print $3 }'`
build=`grep XERO_BUILD_VERSION build.h | awk '{ print $3 }'`

build=$((build + 1))

echo "#define XERO_MAJOR_VERSION $major" > build.h
echo "#define XERO_MINOR_VERSION $minor" >> build.h
echo "#define XERO_MICRO_VERSION $micro" >> build.h
echo "#define XERO_BUILD_VERSION $build" >> build.h


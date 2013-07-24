#!/bin/sh

ARCHCODE="arm"
CCPREFIX='arm-linux-gnueabihf'

[ -n "${CROSS_COMPILE}" ] && { exit; }

echo "Type 'exit' to return to non-crosscompile environment"
export ARCH="$ARCHCODE"
export CROSS_COMPILE="$CCPREFIX-"
export CFLAGS=""
echo "NOW in crosscompile environment for $ARCH ($CROSS_COMPILE)"
/bin/bash
echo 'Back in non-crosscompile environment'

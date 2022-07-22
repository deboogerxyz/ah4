#!/bin/sh

pid=$(pgrep -n "csgo_linux64")
lib="$(pwd)/libah4.so"

if [ $(id -u) -ne 0 ]
then
	echo "$0 requires root privileges"
	exit 1
fi

if [ -z "$pid" ]
then
	echo "Couldn't find the target process"
	exit 1
fi

if [ ! -f "$lib" ]
then
	echo "Couldn't find the shared object"
	exit 1
fi

if grep -q "$lib" "/proc/$pid/maps"
then
	unload="call \$dlclose(\$lib)"
fi

gdb -n -q \
    -ex "attach $pid" \
    -ex "set \$dlopen = (void *(*)(char *, int))dlopen" \
    -ex "set \$dlclose = (int (*)(void *))dlclose" \
    -ex "set \$lib = \$dlopen(\"$lib\", 6)" \
    -ex "$unload" \
    -ex "$unload" \
    -ex "call \$dlopen(\"$lib\", 1)"

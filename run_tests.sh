#!/bin/sh
set -e
if [ ! -d "b" ]; then
	mkdir b; cd b
	cmake ..
	cmake --build .
	cd ..
fi
for f in test/*.lua; do
	echo "==== $f ===="
	lua -e 'package.cpath=package.cpath..";b/src/?.so"' $f
done

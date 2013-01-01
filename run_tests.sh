#!/bin/sh

#set -x
# /opt/luadist/bin/luadist make .
for f in test/*.lua; do
	echo "==== $f ===="
	/opt/luadist/bin/lua -e 'package.cpath=package.cpath..";b/src/?.so"' $f
done

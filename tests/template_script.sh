#!/bin/sh

srcfile=file.c
cat - > $srcfile <<EOF

EOF

ret=0
. $srcdir/tester.sh
rm $srcfile
exit $ret

#!/bin/sh
#
# Merging nam layout file with original nam trace and dump to stdout
#
# $Header: /home/narwhal/u0/soohyunc/CVS_SERV/USN/nam-1.11/bin/merge-layout.sh,v 1.1.1.1 2005/06/16 23:35:02 soohyunc Exp $

if [ $# -ne 2 ] ; then 
	echo "Usage: merge-layout.sh <orig_trace> <layout>"
	echo "  Merged trace file will be dumped to stdout."
	exit 1
fi

egrep -E '^[nl][[:blank:]]-t[[:blank:]]\*' $2
egrep -v -E '^[nl][[:blank:]]-t[[:blank:]]\*' $1
exit 0

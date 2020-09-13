#!/bin/sh

echo "#define CASE_MSG_LIST \\" > defmsg.h
cat defmsg.txt | awk '{print "CASE_MSG ("$2") \\"}' >> defmsg.h
echo "" >> defmsg.h

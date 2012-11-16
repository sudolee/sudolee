#!/usr/bin/python

val = 1

#val = (val and [2] or [3])[0]
val = 3 if val else 2
# i.e-> val = val ? 3 : 2

print "the result is:", val

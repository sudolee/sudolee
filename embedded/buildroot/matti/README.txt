// -*- mode:doc; -*-
// vim: set syntax=asciidoc:

This is an example for +adding-packages-generic+ of
a local source code directory.

How to group packages:
create buildroot/package/groupname/groupname.mk with code below:
include $(sort $(wildcard package/groupname/*/*.mk))

;; $Id$

oink-scripts contains wrapper scripts and configuration files for running oink and oink/qual.

The entry points are called oinkx and qualx.

Example usage:

    oinkx -lc file1.c file2.i

        cpp file1.c, then run oink on the cpped file1.c, file2.i, and libc.oz.

    qualx -c file1.i
    qualx -c file1.i -o file1.qz

        produces file1.qz

    qualx -lc file1.qz file2.qz

        run qual on file1.qz, file2.qz, libc.qz.

Flags to oinkx and qualx:

Flags to qualx:

     --variant {A,B,C}    choose flag variant.  Default: B


Qualx variants:
    A: basic
    B: A + flow down from pointer and compount
    C: B + flow from array index


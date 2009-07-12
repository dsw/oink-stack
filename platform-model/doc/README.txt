-*- text -*-

CONTENTS
========

This project contains taint annotations for GNU libc from glibc-2.3.5.

It also contains non-header portions of GNU libstdc++-v3 from gcc-3.4.3.
These are straight from the sources as we don't require annotations but we
need to create our own library for linking.

Platform-model currently requires:
- Oink and qual
    - Recommended to be built with ARCHIVE_SRZ_ZIP support.
- Oink-scripts
- Gcc-3.4 (this requirement is due to Elsa; see README-gcc.txt)

"Configure" will complain if you are lacking these.

To compile, run:

    ./configure && make

To use the libc model, run, e.g.

    oink-scripts/qualx -lc example.c

    oink-scripts/oinkx -lc example.c



TODO
----

All annotations are currently all conflated; it is planned to separate them
into:

    0. Vanilla annotations
    1. Generic taint data flow
    2. Taint sources (file system, network, getenv, etc) (grep $tainted)
    3. Format string sinks (grep $untainted)

A unification tool will be required to merge them and an optional bijection
checker to check for missing annotations.


BUILDING
========

'make'

You need oink (including qual), gcc-3.4.


AUTHORS/CONTRIBUTORS
====================

Karl Chen <quarl@cs.berkeley.edu>
David Wagner <daw@cs.berkeley.edu>
Daniel Wilkerson <dsw@cs.berkeley.edu>


LICENSE
=======

The contents of this repository outside the libc_* and libstdc++_* directories
are released under the Berkeley Software Distribution license (BSD).

    Copyright (C) 2005, 2006 Regents of the University of California.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the
        distribution.

        Neither the name of the University of California, Berkeley nor the
        names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written
        permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



*** NOTE ***

    MOST LIKELY YOU INTEND TO USE THIS CODE AS DATA.  AS SUCH, YOU ARE NOT
    LINKING AGAINST IT AND THE LICENSE OF THE DATA DOES NOT AFFECT YOUR OWN
    CODE.

    For example, suppose foo.cc is released under the GPL.  Suppose you write
    ultragrep, a commercial program that looks in a file for lines matching a
    given regular expression, which you want to have nothing to do with the
    GPL.  However, you want to run ultragrep on foo.cc looking for certain
    lines.  Because you run ultragrep on foo.cc, treating foo.cc AS DATA, NOT
    AS CODE, you are not linking, and this action does not affect ultragrep.


The directory libc_glibc-2.3.5 contains files which were loosely based on
glibc-2.3.5, which is released under LGPL v2.1+.  See
libc_glibc-2.3.5/upstream-doc/ for details.

The directory libstdc++_gcc-3.4 contains files from gcc-3.4.3, which is
released under GPL v2+.  See libstdc++_gcc-3.4/upstream-doc/ for details.


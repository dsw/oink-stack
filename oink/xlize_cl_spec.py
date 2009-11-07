#!/usr/bin/python

# make the module class definition spec:
#
# foreach xformspec, insert the source filename into each line after
# the mangled name; concatenate them; sort that file

import sys
import re

outlines = []                   # output lines

spec_re = re.compile \
(r'^localize: alnum-name:([a-zA-Z0-9_]+), size:([a-z]+), c-type-name:(.*)$')

xlize_filename_re = re.compile(r'^(.*)\.xlize_xf_spec$')

def xform_line(line, srcfilename):
    m = spec_re.match(line)
    if not m:
        raise "line does not match: %s" % line
    (alnum_name, size, c_type_name) = m.groups()
    outline ='localize: alnum-name:%s, srcfile:"%s", size:%s, c-type-name:%s'%(
        alnum_name, srcfilename, size, c_type_name)
    outlines.append(outline)

def populate_outlines(argv):
    for xformspec in argv:
        m = xlize_filename_re.match(xformspec)
        if not m:
            raise Exception, "illegal xformspec filename: %s" % xformspec
        (srcfilename,) = m.groups()
        for line in open(xformspec):
            xform_line(line, srcfilename)

def print_string_list(l):
    for x in l:
        print x

def main():
    populate_outlines(sys.argv[1:])
    outlines.sort()
    print_string_list(outlines)

if __name__ == "__main__":
    main()

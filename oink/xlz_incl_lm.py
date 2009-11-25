#!/usr/bin/python

# insert includes of instantiated local_malloc.h to patched src

import sys
import re
from xlz_lib import *

def write_include_lines(specs, outfile):
    for spec in specs:
        outfile.write \
            ('#include "local_malloc_%s.h" // inserted by xlz_incl_lm.py\n' % \
                 spec.get_alnum_id())

def xform_input(specs, infile, outfile):
    include_re = re.compile(r'^\s*\#\s*include')
    insertion_done = False
    for line in infile:
        # insert class include lines before the first #include line
        if not insertion_done and include_re.match(line):
            insertion_done = True
            write_include_lines(specs, outfile)
        outfile.write(line)

def main():
    specs = check_const_module(parse_spec(sys.argv[1]))
    xform_input(specs, sys.stdin, sys.stdout)

if __name__ == "__main__":
    main()

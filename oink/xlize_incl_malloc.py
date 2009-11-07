#!/usr/bin/python

# insert includes of instantiated local_malloc.h to patched src

import sys
import re

alnum_names = []

spec_re = re.compile \
(r'^localize: alnum-name:([a-zA-Z0-9_]+), size:([a-z]+), c-type-name:(.*)$')

include_re = re.compile(r'^\s*\#\s*include')

def get_alnum_names(spec_file):
    for line in spec_file:
        m = spec_re.match(line)
        if not m:
            raise "line does not match: %s" % line
        (alnum_name, size, c_type_name) = m.groups()
#         print 'xlize: alnum-name:%s, size:%s, c-type-name:%s' % \
#             (alnum_name, size, c_type_name)
        alnum_names.append(alnum_name)

def print_include_lines():
    for name in alnum_names:
        print '#include "local_malloc__%s.h" // inserted by localize' % name

def xform_input(input_file):
    incl_done = None
    for line in input_file:
        if not incl_done and include_re.match(line):
            print_include_lines()
            incl_done = True
        else:
            sys.stdout.write(line)

def main():
    get_alnum_names(open(sys.argv[1]))
    xform_input(sys.stdin)

if __name__ == "__main__":
    main()

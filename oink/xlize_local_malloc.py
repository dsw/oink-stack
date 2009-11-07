#!/usr/bin/python

# instantiate local malloc: for each line in malloc spec, if the class
# is statically-sized, then expand local_malloc_HOMgld.c to
# local_malloc_<alnum_name>.c; for dynamically-size classes print a
# warning

import sys
import re

local_malloc = None          # where to find local_malloc
ml_spec = None               # module local malloc instantiation specs

spec_re = re.compile \
(r'^localize: alnum-name:([a-zA-Z0-9_]+), size:([a-z]+), c-type-name:(.*)$')

# expand a copy of local_malloc into this directory using these names
# in the identifiers
def expand(alnum_name, src_HOMMgld):
    target = src_HOMMgld.replace("HOMgld", alnum_name)
    out = open(target, "w")
    for line in open("%s/%s" % (local_malloc, src_HOMMgld)):
        line = line.replace("HOMgld", alnum_name)
        out.write(line)
    out.close()

def check_size(size, alnum_name):
    if size == "dyn":
        raise Exception, "%s: cannot handle dynamically-sized class" % \
            alnum_name
    if size != "sta":
        raise Exception, "%s: bad size specification value: %s" % \
            (alnum_name, size)

def instantiate(spec_file):
    for line in open(spec_file):
        m = spec_re.match(line)
        if not m:
            raise Exception, "line does not match: %s" % line
        (alnum_name, size, c_type_name) = m.groups()
        check_size(size, alnum_name)
        expand(alnum_name, "local_malloc_HOMgld.c")
        expand(alnum_name, "local_malloc_HOMgld.h")

def parse_command_line(argv):
    global local_malloc
    global ml_spec
    argv = argv[1:]
    while argv != []:
        if argv[0].find("--local-malloc") == 0:
            argv = argv[1:]
            local_malloc = argv[0]
            argv = argv[1:]
        elif argv[0].find("--ml-spec") == 0:
            argv = argv[1:]
            ml_spec = argv[0]
            argv = argv[1:]
        else:
            raise Exception, "illegal argument: %s" % argv[0]

def main():
    parse_command_line(sys.argv)
    instantiate(ml_spec)

if __name__ == "__main__":
    main()

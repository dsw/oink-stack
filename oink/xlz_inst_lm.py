#!/usr/bin/python

# instantiate local malloc: for each line in malloc spec, if the class
# is statically-sized, then expand local_malloc_HOMgld.c to
# local_malloc_<alnum_id>.c; for dynamically-size classes print a
# warning

import sys
from xlz_lib import *

# instantiate local_malloc into this directory
def instantiate(cmd, spec, orig):
    alnum_id = spec.get_alnum_id()
    target = orig.replace("HOMgld", alnum_id)
    out = open(target, "w")
    infile = open("%s/%s" % (cmd.local_malloc, orig))
    for line in infile:
        line = line.replace("HOModule", cmd.module)
        line = line.replace("HOMgld", alnum_id)
        out.write(line)
    infile.close()
    out.close()

def main():
    cmd = parse_command_line_Inst(sys.argv)
    for spec in check_module_gen(cmd.module, parse_spec(cmd.ml_spec)):
        instantiate(cmd, spec, "local_malloc_HOMgld.c")
        instantiate(cmd, spec, "local_malloc_HOMgld.h")

if __name__ == "__main__":
    main()

#!/usr/bin/python

# make the enum of module class ids for this module

import sys
from xlz_lib import *

# emit the actual individual enum members per class
def emit_alnum_ids(alnum_ids, out):
    for alnum_id in alnum_ids:
        out.write("  class_id_%s," % alnum_id)

# instantiate enum ClassId_HOModule header into this directory
def instantiate(module, local_malloc, alnum_ids):
    orig = "local_malloc_HOModule_clsids.h"
    target = orig.replace("HOModule", module)
    out = open(target, "w")
    infile = open("%s/%s" % (local_malloc, orig))
    replace_re = re.compile(r".*REPLACED during instantiation\s*")
    replacements_done = 0
    for line in infile:
        # find the place to replace the singleton example with a list
        if replace_re.match(line):
            emit_alnum_ids(alnum_ids, out)
            replacements_done += 1
            # do not write the line
            continue
        # also instantiate the module name where it occurs
        line = line.replace("HOModule", module)
        # write the line
        out.write(line)
    if replacements_done != 1:
        raise Exception, "wrong number replacements: %d" % replacements_done
    infile.close()
    out.close()

def main():
    cmd = parse_command_line_Inst(sys.argv)
    alnum_ids = [spec.get_alnum_id() for spec
                 in check_module_gen(cmd.module,
                                     parse_spec(cmd.ml_spec))]
    instantiate(cmd.module, cmd.local_malloc, alnum_ids)

if __name__ == "__main__":
    main()

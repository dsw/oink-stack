#!/usr/bin/python

# make the local_malloc_clsdef_HOMgld.h file for each class: scan for
# the first mention of the class in the module class definition spec,
# if the class is statically-sized, then grep out the #-lines from
# that source file and instantiate the clsdef header file; for
# dynamically-size classes print a warning
#
# Note that the local_malloc_clsdef_HOMgld.h file is the file
# consisting of the #-lines from a client file that uses the class;
# the theory is that these lines should #include enough client headers
# to get the class definition in there somewhere.

import sys
import re
from xlz_lib import *

# get the lines from a file that start with a '#', the '#-lines'
def get_hash_lines(filename):
    hash_re = re.compile(r'^\s*#')
    hash_lines = []
    infile = open(filename)
    for line in infile:
        if hash_re.match(line):
            hash_lines.append(line)
    infile.close()
    return hash_lines

# emit the class definition file: the instantiated
# local_malloc_clsdef_HOMgld.h file.
def emit_clsdef(hash_lines, spec):
    alnum_id = spec.get_alnum_id()
    clsdef_filename = "local_malloc_clsdef_%s.h" % alnum_id
    out = open(clsdef_filename, "w")
    out.write("/*Generated header file for mangled name %s for class %s.*/\n" \
                  % (alnum_id, spec.c_type))
    out.write("#ifndef LOCAL_MALLOC_CLSDEF_%s_H\n" % alnum_id)
    out.write("#define LOCAL_MALLOC_CLSDEF_%s_H\n" % alnum_id)
    out.write("\n")
    out.write("/* These are the #-lines grepped from source file %s */\n" % \
              spec.client_file)
    for line in hash_lines:
        out.write(line)
    out.write("/* End lines grepped from sourcefile */\n")
    out.write("\n")
    out.write("#define HOClass %s\n" % spec.c_type)
    out.write("\n")
    out.write("#endif /* LOCAL_MALLOC_CLSDEF_%s_H */\n" % alnum_id)
    out.close()                 # there will be lots of these

def main():
    last_alnum_class = None
    for spec in check_const_module(parse_spec(sys.argv[1])):
        # use only one client file for each class
        if spec.alnum_class != last_alnum_class:
            hash_lines = get_hash_lines(spec.client_file)
            emit_clsdef(hash_lines, spec)
            last_alnum_class = spec.alnum_class

if __name__ == "__main__":
    main()

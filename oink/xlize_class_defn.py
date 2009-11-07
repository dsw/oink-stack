#!/usr/bin/python

# make the local_malloc_clsdef_HOMgld.h file for each class:
#
# scan for the first mention of the class in the module class
# definition spec, if the class is statically-sized, then grep out the
# #-lines from that source file and instantiate the clsdef header
# file; for dynamically-size classes print a warning

import sys
import re

srcspec_re = re.compile \
(r'^localize: alnum-name:([a-zA-Z0-9_]+), srcfile:"(.*)", size:([a-z]+), c-type-name:(.*)$')

hash_re = re.compile(r'^\s*#')

def check_size(size, alnum_name):
    if size == "dyn":
        raise Exception, "%s: cannot handle dynamically-sized class" % \
            alnum_name
    if size != "sta":
        raise Exception, "%s: bad size specification value: %s" % \
            (alnum_name, size)

def get_hash_lines(file):
    hash_lines = []
    for line in open(file):
        if hash_re.match(line):
            hash_lines.append(line)
    return hash_lines

def write_clsdef(hash_lines, alnum_name, srcfile, c_type_name):
    clsdef_filename = "local_malloc_clsdef_%s.h" % alnum_name
    out = open(clsdef_filename, "w")
    out.write("/*Generated header file for mangled name %s for class %s.*/\n" \
                  % (alnum_name, c_type_name))
    out.write("#ifndef LOCAL_MALLOC_CLSDEF_%s_H\n" % alnum_name)
    out.write("#define LOCAL_MALLOC_CLSDEF_%s_H\n" % alnum_name)
    out.write("\n")
    out.write("/* These are the #-lines grepped from source file %s */\n" % \
              srcfile)
    for line in hash_lines:
        out.write(line)
    out.write("/* End lines grepped from sourcefile */\n")
    out.write("\n")
    out.write("#define HOClass %s\n" % c_type_name)
    out.write("\n")
    out.write("#endif /* LOCAL_MALLOC_CLSDEF_%s_H */\n" % alnum_name)
    out.close()                 # there will be lots of these

def generate(alnum_name, srcfile, size, c_type_name):
    check_size(size, alnum_name)
    hash_lines = get_hash_lines(srcfile)
    write_clsdef(hash_lines, alnum_name, srcfile, c_type_name)

def main():
    last_alnum_name = None
    for line in open(sys.argv[1]):
        m = srcspec_re.match(line)
        if not m:
            raise Exception, "line does not match: %s" % line
        (alnum_name, srcfile, size, c_type_name) = m.groups()
        if alnum_name != last_alnum_name:
            generate(alnum_name, srcfile, size, c_type_name)

if __name__ == "__main__":
    main()

#!/usr/bin/python

# make the module class definition spec: foreach xformspec, insert the
# source filename into each line after the mangled name; concatenate;
# sort; print

import sys
import re
from xlz_lib import *

def make_classdef_specs(argv):
    classdef_specs = []
    xlz_xf_spec_filename_re = re.compile(r'^(.*)\.xlz_xf_spec$')
    for xf_spec_filename in argv:
        m = xlz_xf_spec_filename_re.match(xf_spec_filename)
        if not m:
            raise Exception, "illegal xformspec filename: %s" % xf_spec_filename
        (client_file,) = m.groups()
        for spec in check_const_module(parse_spec(xf_spec_filename)):
            spec.client_file = client_file
            classdef_spec = serialize_spec(spec)
            classdef_specs.append(classdef_spec)
    return classdef_specs

def main():
    classdef_specs = make_classdef_specs(sys.argv[1:])
    classdef_specs.sort()
    for outline in classdef_specs:
        print outline

if __name__ == "__main__":
    main()

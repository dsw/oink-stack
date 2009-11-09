# a library factoring out the commonality in xlz commands

import sys
import re

# **** transform/localize specification

# parse the general string
xlz_spec_line_re = re.compile(r"""^
  localize:\s*
  module:(?P<module>[a-zA-Z0-9_]+),\s*
  alnum-class:(?P<alnum_class>[a-zA-Z0-9_]+),\s*
  (?:client-file:"(?P<client_file>.*)",\s*)?
  size:(?P<size>[a-z]+),\s*
  c-type:(?P<c_type>.*)
$""", re.VERBOSE)

class XlzSpec:
    def __init__(self, dict):
        self.module = dict["module"]
        self.alnum_class = dict["alnum_class"]
        self.size = dict["size"]
        self.c_type = dict["c_type"]
        self.client_file = dict["client_file"]
        # FIX: temporary until we can handle variable-sized objects
        self.check_size()

    def check_size(self):
        if self.size == "dyn":
            raise Exception, "%s: cannot handle dynamically-sized class" % \
                self.alnum_class
        if self.size != "sta":
            raise Exception, "%s: bad size specification value: %s" % \
                (self.alnum_class, self.size)

    def get_alnum_id(self):
        return "_%s__%s" % (self.alnum_class, self.module)

def serialize_spec(spec):
    client_file_str = None
    if spec.client_file:
        client_file_str = ' client-file:"%s",' % spec.client_file
    else:
        client_file_str = ''
    return \
        'localize: module:%s, alnum-class:%s,%s size:%s, c-type:%s' % \
        (spec.module, spec.alnum_class, \
         client_file_str, \
         spec.size, spec.c_type)

def parse_spec_line(line):
    sys.stderr.write("parse_spec_line: %s\n" % line)
    m = xlz_spec_line_re.match(line)
    if not m:
        raise Exception, "line does not match: %s" % line
    return XlzSpec(m.groupdict())

def parse_spec_gen(specfile):
    specin = open(specfile)
    for line in specin:
        yield parse_spec_line(line)
    specin.close()

def parse_spec(specfile):
    return [spec for spec in parse_spec_gen(specfile)]

def check_const_module_gen(specs):
    module = None
    for spec in specs:
        if not module:
            module = spec.module
        elif module != spec.module:
            raise Exception, "module should not change in spec sequence: "+ \
                "old '%s', new '%s'" % (module, spec.module)
        yield spec

def check_const_module(specs):
    return [spec for spec in check_const_module_gen(specs)]

# **** command line for instantiation tools

class CmdLine_Inst:
    def __init__(self):
        self.module = None       # the module we are instantiating
        self.local_malloc = None # where to find local_malloc
        self.ml_spec = None      # local malloc instantiation specs

def parse_command_line_Inst(argv):
    ret = CmdLine_Inst()
    argv = argv[1:]
    while argv != []:
        if False: pass          # orthogonality
        elif argv[0].find("--module") == 0:
            argv = argv[1:]
            ret.module = argv[0]
            argv = argv[1:]
        elif argv[0].find("--local-malloc") == 0:
            argv = argv[1:]
            ret.local_malloc = argv[0]
            argv = argv[1:]
        elif argv[0].find("--ml-spec") == 0:
            argv = argv[1:]
            ret.ml_spec = argv[0]
            argv = argv[1:]
        else:
            raise Exception, "illegal argument: %s" % argv[0]
    return ret

def check_module_gen(module, specs):
    for spec in specs:
        if module != spec.module:
            raise Exception, "command line module '%s' != spec module '%s'" % \
                (module, spec.module)
        yield spec

#!/usr/bin/env python2.4

# $Id$

# runtest input_file.i [additional args]

#   - runs gcc input_file.i
#   - runs qualx input_file.i additional args
#   - prints PASS or FAIL to stdout and returns 0, else test is malformed

import sys
import os
import subprocess
import random
# import atexit
import re
import time

script_path = os.path.abspath(__file__)
platform_model_dir = os.path.join(os.path.dirname(script_path), '..')
stack_dir = os.path.join(platform_model_dir, '..')
oink_scripts_dir = os.path.join(stack_dir, 'oink-scripts')

if not os.path.isdir(oink_scripts_dir):
    raise SystemExit("Can't find oink-scripts at %s"%oink_scripts_dir)

QUALX_PATH = os.path.join(oink_scripts_dir, 'bin', 'qualx')
if not os.path.exists(QUALX_PATH):
    raise SystemExit("Can't find qualx at %s"%QUALX_PATH)
# TODO: use path found in configure

LOGFILENAME = 'test.log'

have_tdel = 0

if have_tdel and os.path.exists(LOGFILENAME):
    subprocess.call(['tdel', LOGFILENAME])
logfile = open(LOGFILENAME, 'w')

class Stats: pass
stats = Stats()
stats.total_subtests = 0
stats.total_subtests_passed = 0
stats.total_subtests_failed = 0
stats.total_subtests_malformed = 0

def timestamp():
    return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())

class Context:
    def __init__(self, name):
        self.name = name
        Context.contexts.append(self.name)

    def __del__(self):
        removed = Context.contexts.pop(-1)
        assert removed == self.name

    def msg(cls, str):
        return '%s: %s' %(': '.join(cls.contexts), str)
    msg = classmethod(msg)

    def wmsg(cls, str):
        if str == None: return
        if str == '':
            print
            return
        cls.log(str)
        print cls.msg(str)
    wmsg = classmethod(wmsg)

    def log(cls, str):
        if not str: return
        print >> logfile, '[%s] %s'%(timestamp(), cls.msg(str))
    log = classmethod(log)

    contexts = [ sys.argv[0] ]

def call(*args, **kwargs):
    Context.log("Executing: %s" %(' '.join(args[0])))
    xstdout = kwargs.get('stdout')
    if xstdout and isinstance(xstdout,file):
        xstdout.flush()
    return subprocess.call(*args, **kwargs)

class TmpDir:
    def __init__(self, file):
        self.tmpdir = '/tmp/%s.%d.%d' %( os.path.basename(file),
                                         os.getpid(),
                                         random.randrange(1000,10000) )

        Context.log("mkdir %s" %(self.tmpdir))
        os.mkdir(self.tmpdir)

    def __del__(self):
        ret = call(['rm', '-rf', self.tmpdir])
        if ret:
            Context.wmsg('Warning: cleanup of temporary directory %s failed' %(self.tmpdir))

# atexit.register(cleanup)

# def cleanup():
#     ret = subprocess.call(['rm', '-rf', tmpdir])
#     if ret:
#         wmsg('Warning: cleanup of temporary directory %s failed' %(tmpdir))

class TestException:
    def __init__(self, pfx, str, quiet):
        if not quiet:
            if str:
                Context.wmsg('%s: %s' %(pfx,str))
            else:
                Context.log('%s' %(pfx))
        self.str = str

class TestFailed(TestException):
    def __init__(self, str=None, quiet=False):
        TestException.__init__(self, 'TEST FAILED', str, quiet)

class TestMalformed(TestException):
    def __init__(self, str=None, quiet=False):
        TestException.__init__(self, 'TEST MALFORMED', str, quiet)

def check_gcc(input_file, lang, tmpdir, ldflags):
    # sanity check using gcc
    output_file = os.path.join(tmpdir.tmpdir,
                               os.path.basename(input_file)+'.out')

    ret = call(['g++',
                '-x', lang.lower(),
                '-o', output_file, input_file] + ldflags,
               stdout=logfile, stderr=logfile)
    if ret or not os.path.exists(output_file):
        raise TestMalformed("gcc failed")
        # ABORT("gcc failed")
    # could also try with gcc -x c++
    os.unlink(output_file)
    Context.log("check_gcc '%s' succeeded" %input_file)

def find_expected_result(input_file):
    data = open(input_file).read()
    expect_taint_warnings = ('EXPECT_TAINT_WARNINGS' in data)
    expect_no_warnings = ('EXPECT_NO_WARNINGS' in data)

    if expect_taint_warnings and expect_no_warnings:
        raise TestMalformed("too many expected results specified")
    elif expect_taint_warnings:
        return 32
    elif expect_no_warnings:
        return 0
    else:
        raise TestMalformed("no expected result specified")

def check_qual(input_file, qual_args):
    expected_ret = find_expected_result(input_file)
    cmd = [ QUALX_PATH, input_file ] + qual_args

    Context.log("Executing: %s" %(' '.join(cmd)))

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    logoutput = proc.communicate()[0]
    ret = proc.returncode
    assert(ret is not None)
    logfile.write(logoutput)

    if ret != expected_ret:
        raise TestFailed("qual returned exit code %s (expected %s)" %(ret, expected_ret))

    found_unsoundness_warning = ('Unsoundness warning' in logoutput)

    if 'EXPECT_UNSOUNDNESS_WARNINGS' in open(input_file).read():
        if found_unsoundness_warning:
            Context.log("Got unsoundness warning as expected")
        else:
            raise TestFailed("did not get expected unsoundness warning")
    else:
        if found_unsoundness_warning:
            raise TestFailed("got unexpected unsoundness warning")

    Context.log("check_qual '%s' succeeded" %input_file)

def runtest1(input_file, qual_args, gcc_ldflags, lang, tmpdir=None):
    context = Context(input_file)

    try:
        tmpdir = tmpdir or TmpDir(input_file)
        check_gcc(input_file, lang, tmpdir, gcc_ldflags)
        check_qual(input_file, qual_args)
        Context.log("runtest1 '%s' succeeded" %input_file)
    finally:
        del context

def get_filters_and_flags(master_input_file):
    filters = []
    flags = {}
    for line in open(master_input_file):
        m=re.match('^//@@ TESTS: (.*)', line)
        if m:
            filters.append(m.group(1).split())
            continue
        m=re.match('^//@@ LDFLAGS: (.*)', line)
        if m:
            flags['LDFLAGS'] = m.group(1)

    return filters, flags

# iterate_choices([['a','b'], ['c','d']])
# =>
#   ['a', 'c']
#   ['a', 'd']
#   ['b', 'c']
#   ['b', 'd']
def iterate_choices(lists):
    if len(lists) == 0:
        yield []

    elif len(lists) == 1:                           # optimization
        for x in lists[0]:
            yield [x]

    else:
        rest = lists[1:]
        for x in lists[0]:
            for r in iterate_choices(rest):
                yield [x] + r

def count_choices(lists):
    n = 1
    for list in lists:
        n *= len(list)
    return n

def preprocess(master_input_file, filter_choice, lang, tmpdir):
    if lang == 'C':
        ext = 'i'
    elif lang == 'C++':
        ext = 'ii'
    else:
        assert False
    i_name = os.path.join(tmpdir.tmpdir,
                          '%s.%s.%s' %(os.path.basename(master_input_file),
                                       ','.join(filter_choice), ext))
    args = (['cpp', '-x', lang.lower()] +
            #'-I/home/quarl/proj/debfsv/glibc-fs/include',
            [ '-D'+x+'=1' for x in filter_choice] +
            [ '-o', i_name,
              master_input_file ])

    ret = call(args, stdout=logfile, stderr=logfile)
    if ret:
        raise TestMalformed("cpp failed:"+' '.join(args))
    return i_name

def get_xfail(master_input_file):
    xfail = []
    for line in open(master_input_file):
        m = re.match('^XFAIL: (.*)', line)
        if m:
            xfail += m.groups(1).split(' ')
    return xfail

def runtest(master_input_file):
    master_input_file = re.sub('^[.]/', '', master_input_file)
    context = Context(master_input_file)

    try:

        if not os.path.exists(master_input_file):
            raise TestMalformed('file does not exist')

        tmpdir = TmpDir(master_input_file)

        filters, flags = get_filters_and_flags(master_input_file)
        # xfail = get_xfail(master_input_file)
        context.log("filters: %s"%(filters))

        passed = []
        failed = []
        malformed = []
        count = 0
        total_subtests = count_choices(filters) * 2 # *2 for language
        Context.wmsg('Running %d subtests...' %(total_subtests))
        for filter_choice in iterate_choices(filters):
            for lang in ['C','C++']:
                count += 1
                fdescr = ','.join(filter_choice)+','+lang
                print "  Running sub test %d of %d: %s" %(count, total_subtests, fdescr)
                stats.total_subtests += 1
                tmp_i = preprocess(master_input_file, filter_choice, lang, tmpdir)

                try:
                    qual_flags = ['-fo-no-report-link-errors', '--variant', 'C']
                    if lang == 'C++':
                        qual_flags.append('-fo-no-instance-sensitive')

                    runtest1(tmp_i, ['-I../include',
                                     '-lc',
                                     # '../libc_glibc-2.3.5/libc.qz',
                                     # '/home/quarl/proj/debfsv/glibc-fs/src/stdio.c',
                                     # '/home/quarl/proj/debfsv/glibc-fs/src/ctype.c',
                                     # '/home/quarl/proj/debfsv/glibc-fs/src/stdlib.c',
                                     # '/home/quarl/proj/debfsv/glibc-fs/src/string.c',
                                     # '/home/quarl/proj/debfsv/glibc-fs/src/_main.c',
                                     ] + qual_flags,
                             flags.get('LDFLAGS','').split(),
                             lang,
                             tmpdir)
                except TestFailed:
                    stats.total_subtests_failed += 1
                    failed.append(fdescr)
                except TestMalformed:
                    stats.total_subtests_malformed += 1
                    malformed.append(fdescr)
                else:
                    stats.total_subtests_passed += 1
                    passed.append(fdescr)

                os.unlink(tmp_i)

        if len(passed) == count:
            Context.wmsg('TEST PASSED (all %d/%d subtests)' %(len(passed), count))
        else:
            Context.wmsg('TEST FAILED.')
            Context.wmsg('  Passed %d/%d subtests: %s' %(len(passed), count,
                                                         ' '.join(passed)))
            if failed:
                Context.wmsg('  Failed %d/%d subtests: %s' %(len(failed), count,
                                                             ' '.join(failed)))
            if malformed:
                Context.wmsg('  Malformed %d/%d subtests: %s' %(len(malformed), count,
                                                                ' '.join(malformed)))

            Context.wmsg('')

            if failed:
                raise TestFailed(' '.join(failed),quiet=1)

            if malformed:
                raise TestMalformed(' '.join(malformed),quiet=1)

            raise Exception("Internal error: shouldn't have reached here")

    finally:
        del context

def runtests(master_input_files):
    passed = []
    failed = []
    malformed = []
    count = len(master_input_files)
    for master_input_file in master_input_files:
        try:
            runtest(master_input_file)
        except TestFailed, e:
            failed.append('%s: %s'%(master_input_file, e.str))
        except TestMalformed, e:
            malformed.append('%s: %s'%(master_input_file, e.str))
        else:
            passed.append(master_input_file)

    Context.wmsg('')
    if len(passed) == count:
        Context.wmsg('ALL %d/%d MASTER TESTS PASSED' %(len(passed), count))

    else:
        Context.wmsg('SOME TEST(S) FAILED.')
        Context.wmsg('  Passed %d/%d: %s' %(len(passed), count,
                                            (' '.join(passed) or '(none)')))
        if failed:
            Context.wmsg('  Failed %d/%d:' %(len(failed), count))
            for x in failed:
                Context.wmsg('      '+x)
        if malformed:
            Context.wmsg('  Malformed %d/%d:' %(len(malformed), count))
            for x in malformed:
                Context.wmsg('      '+x)

    Context.wmsg('')
    Context.wmsg('Total subtests:')
    Context.wmsg('    Passed %d/%d' %(stats.total_subtests_passed, stats.total_subtests))
    Context.wmsg('    Failed %d/%d' %(stats.total_subtests_failed, stats.total_subtests))
    Context.wmsg('    Malformed %d/%d' %(stats.total_subtests_malformed, stats.total_subtests))

    if len(passed) == count:
        return

    if failed:
        raise TestFailed

    if malformed:
        raise TestMalformed

    raise Exception("Internal error: shouldn't have reached here")

Context.log('Beginning testing...')

#!/usr/bin/perl -w
# see License.txt for copyright and terms of use
# Modified from elsa/configure.pl by Scott McPeak

# Converts its command line flags into name/value pairs with which
# files named FILE.in will have '@name@' substituted by 'value' to
# produce FILE.  Generates config.status which will actually do the
# substitution.

use strict;
use warnings;

my @files_to_configure = qw(
  Makefile
  Build.incl.mk
);

# files that should be marked executable
my %xable = ();
# end of line comment in the language of the file
my %eolcomment = (
   'Makefile'      => '#',
   'Build.incl.mk' => '#',
  );

my @config_vars = qw(
  SMBASE_DIR
  AST_DIR
  EHD_DIR
  ELSA_DIR
  LIBQUAL_DIR
  LIBREGION_DIR
  LLVM_DIR
  TMP_DIR
  CFLAGS_EXTRA
  USE_ZIPIOS
  USE_LLVM
);

# not used for now
#  LDFLAGS

# defaults
#  my $BASE_FLAGS = "-g -Wall -Wno-deprecated -D__UNIX__";
my @CFLAGS_EXTRA = ();

# true if we want to link with zipios
my $USE_ZIPIOS = 1;

# true if we want to link with llvm
my $USE_LLVM = 1;

# These are implied by NDEBUG
#push @CFLAGS_EXTRA, "-DDO_SELFCHECK=0", "-DDO_TRACE=0";
#push @CFLAGS_EXTRA, "-DNDEBUG";

#  my @LDFLAGS = ("-g", "-Wall");
#my @LDFLAGS = ();
my $debug = 0;
my $use_dash_g = 1;
my $allow_dash_O2 = 1;

my $SMBASE_DIR  = "../smbase";
my $AST_DIR     = "../ast";
my $EHD_DIR     = "../elkhound";
my $ELSA_DIR    = "../elsa";
my $LIBQUAL_DIR = "../libqual";
my $LIBREGION_DIR = "../libregion";
my $LLVM_DIR    = "../../llvm";

my $TMP_DIR = ".";
if (-d '/dev/shm' && -w '/dev/shm') {
    # quarl 2006-06-03: use a tmpfs if possible; it's incredibly fast!
    $TMP_DIR = '/dev/shm/oink.buildtmp';
} elsif (-d '/tmp') {
    $TMP_DIR = '/tmp/oink.buildtmp';
}

sub usage {
  print(<<"EOF");
usage: ./configure [options]
options:
  -h:                print this message
  -debug,-nodebug:   enable/disable debugging options [disabled]
  -no-dash-g         disable -g
  -no-dash-O2        disable -O2
  -prof              enable profiling

  --enable-archive-srz-zip=yes  enable zip archive serialization
  --enable-archive-srz-zip=no   disable zip archive serialization
  --enable-archive-srz-zip=auto enable zip archive serialization if available

  --enable-llvm=yes  enable llvm
  --enable-llvm=no   disable llvm
  --enable-llvm=auto enable llvm if available

  -smbase=<dir>:     specify where the smbase library is [$SMBASE_DIR]
  -ast=<dir>:        specify where the ast system is [$AST_DIR]
  -elkhound=<dir>:   specify where the elkhound system is [$EHD_DIR]
  -elsa=<dir>:       specify where the elsa system is [$ELSA_DIR]
  -libqual=<dir>:    specify where the libqual library is [$LIBQUAL_DIR]
  -libregion=<dir>:  specify where the libregion library is [$LIBREGION_DIR]
  -llvm=<dir>:       specify where the llvm library is [$LLVM_DIR]
EOF
}
#    -devel             add options useful while developing
#    <op>:              add a given option to the gcc command line,
#                         including forms: -W*, -D*, -O*, -m*

my $enable_archive_srz_zip = 'auto';
my $enable_llvm = 'auto';

# process command-line arguments
my $originalArgs = join(' ', @ARGV);
while (@ARGV) {
  my $tmp;
  my $arg = $ARGV[0];
  shift @ARGV;

  # treat leading "--" uniformly with leading "-"
  $arg =~ s/^--/-/;

  if ($arg eq "-h" ||
      $arg eq "-help") {
    usage();
    exit(0);
  }

  # things that look like options to gcc should just
  # be added to CCFLAGS
  elsif ($arg =~ m/^(-W|-pg$|-D|-O|-m)/) {
    push @CFLAGS_EXTRA, $arg;
  }
  elsif ($arg eq "-ccflag") {
    push @CFLAGS_EXTRA, $ARGV[0];
    shift @ARGV;
  }

  elsif ($arg eq "-d" ||
         $arg eq "-debug") {
    $debug = 1;
  } elsif ($arg eq "-nodebug") {
    $debug = 0;
  }
  elsif ($arg eq "-no-dash-g") {
    $use_dash_g = 0;
  }
  elsif ($arg eq "-no-dash-O2") {
    $allow_dash_O2 = 0;
  }


  elsif ($arg eq "-prof") {
    push @CFLAGS_EXTRA, "-pg";
#    push @LDFLAGS, "-pg";
  }

#    elsif ($arg eq "-gcov") {
#      push @CFLAGS_EXTRA, "-fprofile-arcs -ftest-coverage";
#      push @LDFLAGS, "-fprofile-arcs -ftest-coverage";
#    }

  elsif ($arg eq "-devel") {
    push @CFLAGS_EXTRA, "-Werror";
  }

  elsif (($tmp) = ($arg =~ m/^-smbase=(.*)$/)) {
    $SMBASE_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-ast=(.*)$/)) {
    $AST_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-elkhound=(.*)$/)) {
    $EHD_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-elsa=(.*)$/)) {
    $ELSA_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-libqual=(.*)$/)) {
    $LIBQUAL_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-libregion=(.*)$/)) {
    $LIBREGION_DIR = $tmp;
  } elsif (($tmp) = ($arg =~ m/^-llvm=(.*)$/)) {
    $LLVM_DIR = $tmp;
  }

  elsif ($arg eq "-useSerialNumbers") {
    push @CFLAGS_EXTRA, "-DUSE_SERIAL_NUMBERS=1";
  }

  elsif ($arg eq '-enable-archive-srz-zip') {
      $enable_archive_srz_zip = 'yes';
  } elsif ($arg =~ m,^-enable-archive-srz-zip=(yes|no|auto)$,) {
      $enable_archive_srz_zip = $1;
  } elsif ($arg eq "-require-archive-srz-zip") {
      $enable_archive_srz_zip = 'yes';
  }
  elsif ($arg eq "-require-no-archive-srz-zip") {
      $enable_archive_srz_zip = 'no';
  }

  elsif ($arg eq '-enable-llvm') {
      $enable_llvm = 'yes';
  } elsif ($arg =~ m,^-enable-llvm=(yes|no|auto)$,) {
      $enable_llvm = $1;
  } elsif ($arg eq "-require-llvm") {
      $enable_llvm = 'yes';
  }
  elsif ($arg eq "-require-no-llvm") {
      $enable_llvm = 'no';
  }

  else {
    die "unknown option: $arg\n";
  }
}

push(@CFLAGS_EXTRA, "-DARCHIVE_SRZ_DIR=1");

die unless $enable_archive_srz_zip;
if ($enable_archive_srz_zip eq 'no') {
    print "Note: ARCHIVE_SRZ_ZIP disabled.  No support for .oz/.qz files.\n";
} elsif ($enable_archive_srz_zip eq 'yes') {
    my $have_archive_srz_zip = (0==system("./configure-check-libzipios"));
    if (!$have_archive_srz_zip) {
        print STDERR <<EOF
You wanted to enable ARCHIVE_SRZ_ZIP, but you don't have libzipios++ or
I couldn't compile with it.

    You can get it at http://zipios.sourceforge.net/, or
      'apt-get install libzipios++-dev zlib1g-dev' on Debian

    Note that you also need zlib (the Debian package libzipios++-dev
    is missing the dependency on zlib1g-dev).

Stopping because you specified --enable-archive-srz-zip=yes.

EOF
;
        exit 1;
    }
    print "ARCHIVE_SRZ_ZIP enabled and seems to work.\n";
} elsif ($enable_archive_srz_zip eq 'auto') {
    my $have_archive_srz_zip = (0==system("./configure-check-libzipios"));
    if ($have_archive_srz_zip) {
        print "ARCHIVE_SRZ_ZIP seems to work so enabling it.\n";
        $enable_archive_srz_zip = 'yes';
    } else {
        print <<EOF

ARCHIVE_SRZ_ZIP disabled because you don't have libzipios++ or
I couldn't compile with it.

    You can get it at http://zipios.sourceforge.net/, or
      'apt-get install libzipios++-dev zlib1g-dev' on Debian

    Note that you also need zlib (the Debian package libzipios++-dev
    is missing the dependency on zlib1g-dev).

EOF
;
        $enable_archive_srz_zip = 'no';
    }
} else {
    die "67600920-e937-48f5-a118-342cea1d4671 $enable_archive_srz_zip";
}

if ($enable_archive_srz_zip eq 'yes') {
    push(@CFLAGS_EXTRA, "-DARCHIVE_SRZ_ZIP=1");
}
$USE_ZIPIOS = ($enable_archive_srz_zip eq 'yes' ? 1 : 0);

die unless $enable_llvm;
my $llvm_failure_message = <<EOF
You wanted to enable LLVM, but you don't have llvm or
I couldn't compile with it.

    You can get it at http://llvm.org/

EOF
;
if ($enable_llvm eq 'no') {
    print "Note: LLVM disabled.  No support for generating executables.\n";
} elsif ($enable_llvm eq 'yes') {
    my $have_llvm = (0==system("./configure-check-llvm"));
    if (!$have_llvm) {
        print STDERR $llvm_failure_message;
        print STDERR <<EOF
Stopping because you specified --enable-llvm=yes.
EOF
;
        exit 1;
    }
    print "LLVM enabled and seems to work.\n\n";
} elsif ($enable_llvm eq 'auto') {
#     my $have_llvm = (0==system("./configure-check-llvm"));
    my $have_llvm = -d $LLVM_DIR;
    if ($have_llvm) {
        print "LLVM seems to work so enabling it.\n\n";
        $enable_llvm = 'yes';
    } else {
        print STDERR $llvm_failure_message;
        $enable_llvm = 'no';
    }
} else {
    die "Can't get here.";
}

if ($enable_llvm eq 'yes') {
#    push(@CFLAGS_EXTRA, "-DLLVM=1");
}
$USE_LLVM = ($enable_llvm eq 'yes' ? 1 : 0);

if (!$debug) {
  if ($allow_dash_O2) {
     push @CFLAGS_EXTRA, "-O2";
    # push @CFLAGS_EXTRA, "-O3";
  }
  push @CFLAGS_EXTRA, "-DNDEBUG";
}

if ($use_dash_g) {
  push @CFLAGS_EXTRA, "-g";
}

my $os = `uname -s`;
chomp($os);
if ($os eq "Linux") {
  push @CFLAGS_EXTRA, "-D__LINUX__";
}

# smash the list together to make a string
my $CFLAGS_EXTRA = join(' ', @CFLAGS_EXTRA);
#my $LDFLAGS = join(' ', @LDFLAGS);


# ------------------ check for needed components ----------------
# smbase
if (! -f "$SMBASE_DIR/nonport.h") {
  die "I cannot find nonport.h in `$SMBASE_DIR'.\n" .
      "The smbase library is required for oink.\n" .
      "If it's in a different location, use the -smbase=<dir> option.\n";
}

# ast
if (! -f "$AST_DIR/asthelp.h") {
  die "I cannot find asthelp.h in `$AST_DIR'.\n" .
      "The ast system is required for oink.\n" .
      "If it's in a different location, use the -ast=<dir> option.\n";
}

# elkhound
if (! -f "$EHD_DIR/glr.h") {
  die "I cannot find glr.h in `$EHD_DIR'.\n" .
      "The elkhound system is required for oink.\n" .
      "If it's in a different location, use the -elkhound=<dir> option.\n";
}

# elsa
if (! -f "$ELSA_DIR/cc_type.h") {
  die "I cannot find cc_type.h in `$ELSA_DIR'.\n" .
      "The elsa system is required for oink.\n" .
      "If it's in a different location, use the -elsa=<dir> option.\n";
}

# libqual
if (! -f "$LIBQUAL_DIR/quals.h") {
  die "I cannot find quals.h in `$LIBQUAL_DIR'.\n" .
      "The libqual library is required for oink.\n" .
      "If it's in a different location, use the -libqual=<dir> option.\n";
}

# libregion
if (! -f "$LIBREGION_DIR/regions.h") {
  die "I cannot find regions.h in `$LIBREGION_DIR'.\n" .
      "The libregion library is required for oink.\n" .
      "If it's in a different location, use the -libregion=<dir> option.\n";
}

# llvm; FIX: add something here to check (1) the version of llvm and
# (2) that it is built
if ($USE_LLVM &&
    ! -f "$LLVM_DIR/include/llvm/AbstractTypeUser.h")
{
  die "I cannot find include/llvm/AbstractTypeUser.h in `$LLVM_DIR'.\n" .
      "You enabled llvm but you didn't tell me where to find it.\n" .
      "If it's in a different location, use the -llvm=<dir> option.\n";
}

#  # use smbase's $BASE_FLAGS if I can find them
#  $smbase_flags = `$SMBASE_DIR/config.summary 2>/dev/null | grep BASE_FLAGS`;
#  if (defined($smbase_flags)) {
#    ($BASE_FLAGS = $smbase_flags) =~ s|^.*: *||;
#    chomp($BASE_FLAGS);
#  }


# ------------------ config.summary -----------------
# create a program to summarize the configuration
my $configSummary = "config.summary";
unlink $configSummary;
open(OUT, ">$configSummary") or die("can't make config.summary");
print OUT (<<"OUTER_EOF");
#!/bin/sh
# config.summary

cat <<EOF
./configure command:
  $0 $originalArgs

Oink configuration summary:

Location flags:
  SMBASE_DIR:    $SMBASE_DIR
  AST_DIR:       $AST_DIR
  EHD_DIR:       $EHD_DIR
  ELSA_DIR:      $ELSA_DIR
  LIBQUAL_DIR:   $LIBQUAL_DIR
  LIBREGION_DIR: $LIBREGION_DIR
  LLVM_DIR:      $LLVM_DIR

  TMP_DIR:       $TMP_DIR

Compile flags:
  debug:        $debug
  enable_archive_srz_zip: $enable_archive_srz_zip
  enable_llvm: $enable_llvm
  CFLAGS_EXTRA:      $CFLAGS_EXTRA
EOF

OUTER_EOF
# ' " EOF "OUTER_EOF"

#  Compile flags:
#    debug:        $debug
#    BASE_FLAGS:   $BASE_FLAGS
#    CFLAGS_EXTRA:      $CFLAGS_EXTRA
#    LDFLAGS:      $LDFLAGS

close(OUT) or die;
chmod 0555, "config.summary";


# ------------------- config.status ------------------
# from here on, combine BASE_FLAGS and CFLAGS_EXTRA
#  $CFLAGS_EXTRA = "$BASE_FLAGS $CFLAGS_EXTRA";

my $substitutions = "";
for my $varname (@config_vars) {
  my $value = eval "\$${varname}";
  $substitutions .= "    -e \"s|\\\@${varname}\\\@|${value}|g; \" \\\n";
}

# create a program which will create the Makefile
my $configStatus = "config.status";
unlink $configStatus;
open(OUT, ">$configStatus") or die("can't make config.status");
print OUT (<<"OUTER_EOF");
#!/bin/sh
# config.status
# this file was created by ./configure

# report on configuration
./config.summary

OUTER_EOF

for my $file (@files_to_configure) {
  print OUT (<<"OUTER_EOF");
echo "creating $file ..."
# overcome my chmod below
rm -f $file
OUTER_EOF
;
  die unless defined ${eolcomment{$file}};
  my $eolcmnt = ${eolcomment{$file}};
  my $configure_splash = <<"OUTER_EOF";
$eolcmnt $file for oink\\n\\
$eolcmnt NOTE: do not edit; generated by:\\n\\
$eolcmnt   $0 $originalArgs\\
OUTER_EOF
  ;
  print OUT (<<"OUTER_EOF");
# make the configure autogenerated message
# and substitute variables
perl -p \\
-e \"s|\\\@CONFIGURE_SPLASH\\\@|${configure_splash}|g; \" \\
$substitutions <$file.in >>$file || exit
# discourage editing
chmod a-w $file
OUTER_EOF
  ;
  if ($xable{$file}) {
  print OUT (<<"OUTER_EOF");
chmod u+x $file
OUTER_EOF
  }
}

close(OUT) or die;
chmod 0555, "config.status";


# ----------------- final actions -----------------
# run the output file generator
my $code = system("./config.status");
if ($code != 0) {
  # hopefully ./config.status has already printed a message,
  # I'll just relay the status code
  if ($code >> 8) {
    exit($code >> 8);
  }
  else {
    exit($code & 127);
  }
}

print("\nYou can now run make, usually called 'make' or 'gmake'.\n");

exit(0);

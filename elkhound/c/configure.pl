#!/usr/bin/perl -w
# configure script for C parser

use strict 'subs';

# default location of smbase relative to this package
$SMBASE = "../../smbase";
$req_smcv = 1.03;            # required sm_config version number
$thisPackage = "elkhound/c";

# -------------- BEGIN common block ---------------
# do an initial argument scan to find if smbase is somewhere else
for (my $i=0; $i < @ARGV; $i++) {
  my ($d) = ($ARGV[$i] =~ m/-*smbase=(.*)/);
  if (defined($d)) {
    $SMBASE = $d;
  }
}

# try to load the sm_config module
eval {
  push @INC, ($SMBASE);
  require sm_config;
};
if ($@) {
  die("$@" .     # ends with newline, usually
      "\n" .
      "I looked for smbase in \"$SMBASE\".\n" .
      "\n" .
      "You can explicitly specify the location of smbase with the -smbase=<dir>\n" .
      "command-line argument.\n");
}

# check version number
$smcv = get_sm_config_version();
if ($smcv < $req_smcv) {
  die("This package requires version $req_smcv of sm_config, but found\n" .
      "only version $smcv.\n");
}
# -------------- END common block ---------------


# defaults
$AST = "../../ast";
$ELKHOUND = "..";


sub usage {
  standardUsage();

  print(<<"EOF");
package options:
  -ast=<dir>:        specify where the ast system is [$AST]
  -elkhound=<dir>:   specify where the elkhound system is [$ELKHOUND]
EOF
}


# -------------- BEGIN common block 2 -------------
# global variables holding information about the current command-line
# option being processed
$option = "";
$value = "";

# process command-line arguments
foreach $optionAndValue (@ARGV) {
  # ignore leading '-' characters, and split at first '=' (if any)
  ($option, $value) =
    ($optionAndValue =~ m/^-*([^-][^=]*)=?(.*)$/);
                      #      option     = value

  my $arg = $option;

  if (handleStandardOption()) {
    # handled by sm_config.pm
  }
  # -------------- END common block 2 -------------

  elsif ($arg eq "ast") {
    $AST = $tmp;
  }
  elsif ($arg eq "elkhound") {
    $ELKHOUND = $tmp;
  }

  else {
    die "unknown option: $arg\n";
  }
}

finishedOptionProcessing();


# ------------------ check for needed components ----------------
test_smbase_presence();

test_CXX_compiler();

# ast
if (! -f "$AST/asthelp.h") {
  die "I cannot find asthelp.h in `$AST'.\n" .
      "The ast system is required for elsa.\n" .
      "If it's in a different location, use the -ast=<dir> option.\n";
}

# elkhound
if (! -f "$ELKHOUND/glr.h") {
  die "I cannot find glr.h in `$ELKHOUND'.\n" .
      "The elkhound system is required for elsa.\n" .
      "If it's in a different location, use the -elkhound=<dir> option.\n";
}


# ------------------ config.summary -----------------
$summary = getStandardConfigSummary();

$summary .= <<"OUTER_EOF";
cat <<EOF
  AST:         $AST
  ELKHOUND:    $ELKHOUND
EOF
OUTER_EOF

writeConfigSummary($summary);


# ------------------- config.status ------------------
writeConfigStatus("AST" => "$AST",
                  "ELKHOUND" => "$ELKHOUND");


# ----------------- final actions -----------------
# run the output file generator
run("./config.status");

print("\nYou can now run make, usually called 'make' or 'gmake'.\n");

exit(0);


# silence warnings
pretendUsed($thisPackage);

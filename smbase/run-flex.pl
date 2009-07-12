#!/usr/bin/perl -w
# wrapper around flex

# The purpose of this script is basically to "fix" flex's output
# in a number of ways.
#
# 1. It changes a forward declaration of istream into a proper
# #include directive.  In current C++ libraries, istream is not
# a class but rather a template specialization.
#
# 2. Fix the "yyclass" option output so that the resulting module
# can be linked with other flex-generated lexers:
#
#   2a. Wrap all yyFlexLexer methods with
#         #ifdef WANT_YYFLEXLEXER_METHODS
#         #endif
#   so that from the Makefile I can control whether the object file
#   contains those methods.
#
#   2b. Make copies of two yyFlexLexer methods and rename them to
#   be methods of the derived lexer class, as these methods are
#   different in each generated lexer.  (This is optional.)
#
# 3. Redhat's flex-2.5.4a-29 (actually somewhere between -20 and -23)
# includes a "fix" so its generated output file includes the line:
#
#   using namespace std;
#
# This causes a clash for the name 'string'.  Long term, I'd like
# to have a good, general way to reconcile this clash.  But for the
# short term, this script replaces that line with these two:
#
#   using std::istream;
#   using std::ostream;
#
# and that solves the problem nicely, as Flex does not use any
# library classes other than those two.
#
# 4. Emit proper #line directives for merged extensions.

# Given that I make so many changes, and they are so dependent on
# the details of the generated file, it might seem easier to just
# hack my own copy of flex and distribute that.  I may end up
# doing that, but for now this provides a little portability (I
# will try to make this work with both 2.5.4 and 2.5.31), and
# avoids the need for me to repackage things like flex's configure
# script.

use strict 'subs';

# defaults
$nobackup = 0;           # if true, require the scanner to be backup-free
$makeMethodCopies = 0;   # if true, do step 2b above
$outputFile = "";        # name of eventual output file
$inputFile = "";         # name if input file
@flexArgs = ("flex");    # arguments to pass to flex

if (@ARGV == 0) {
  print(<<"EOF");
usage: $0 [-nobackup] [-copies] -o<fname> [flex-options] input.lex
  -nobackup: fail if the scanner can jam
  -copies:   make copies of methods that use per-lexer state
  -o<fname>: specify output file name
For details on other flex options, consult "man flex".
EOF
  exit(0);
}

# process command-line arguments; the syntax is basically a superset
# of what flex itself accepts
for (; @ARGV; shift @ARGV) {
  # interestingly, both my choices of options are deprecated NOPs
  # to flex in their single-letter forms (-n and -c), so there is
  # little chance of confusion

  if ($ARGV[0] eq "-nobackup") {
    $nobackup = 1;
    push @flexArgs, ("-b");
    next;
  }

  if ($ARGV[0] eq "-copies") {
    $makeMethodCopies = 1;
    next;
  }

  my ($s) = ($ARGV[0] =~ m/^-o(.+)/);
  if (defined($s)) {
    diagnostic("saw output file: $s\n");
    $outputFile = $s;
  }

  my ($if) = ($ARGV[0] =~ m/([^-].+)/);
  $inputFile = $if if defined($if);

  push @flexArgs, ($ARGV[0]);
}

if (!$inputFile) {
  die("please specify an input file\n");
}

if (!$outputFile) {
  die("please specify an output file with -o<file>\n");
}

# run flex
print(join(' ', @flexArgs) . "\n");
if (0!=system(@flexArgs)) {
  print("flex failed, so removing output file $outputFile\n");
  system("rm -f $outputFile");
  exit(2);
}

# check for backing up
if ($nobackup) {
  if (0==system("grep non-accepting lex.backup")) {
    print("(see lex.backup for details)\n");
    print("removing output file $outputFile\n");
    system("rm -f $outputFile");
    exit(2);
  }
  else {
    unlink("lex.backup");
  }
}

# don't want to give the impression that the only thing that was
# done was running flex, as I echoed that command line
print("modifying $outputFile\n");

# read the flex-generated output into memory
open(IN, "<$outputFile") or die("cannot read $outputFile: $!\n");
@lines = <IN>;
close(IN);

# start writing it again
open(OUT, ">$outputFile") or die("cannot write $outputFile: $!\n");

# keep track of what we've done
$state = 1;

# name of derived lexer class (if any)
$derivedClassName = "";

# text (lines) of methods to copy
@methodCopies = ();

# additional lines to move past the end of the methodCopies
@movedLines = ();

# within extension
my $extfilename;
my $extlineno = -1;

# filename as specified by last #line directive (including quotes)
my $linefile = "\"$outputFile\"";
my $lineno = 0;       # current line number of processed output

# begin translating the generated file, making the changes outlined above
for ($i=0; $i < @lines; $i++) {
  $line = $lines[$i];

  # this is stateless for no good reason
  my ($s) = ($line =~ m/^\#define YY_DECL int (.*)::yylex/);
  if (defined($s) && $s ne "yyFlexLexer") {
    $derivedClassName = $s;
  }

  # this is stateless because it does not occur in the cygwin
  # flex output (they have a different fix)
  if ($line =~ m/class istream;/) {
    $lineno++;
    print OUT ("#include <iostream.h>      // class istream\n");
    next;
  }

  # also stateless for similar reasons
  if ($line =~ m/using namespace std;/) {
    $lineno++;
    print OUT ("using std::istream;\n");
    $lineno++;
    print OUT ("using std::ostream;\n");
    next;
  }

  # this 'undef' must be moved to after the copied methods because
  # those methods refer to 'yytext_ptr', which is just a #define
  # for 'yytext'
  if ($makeMethodCopies &&
      $line =~ m/\#undef yytext_ptr/) {
    push @movedLines, ($line);
    next;
  }

  # this is stateless because I don't understand the state stuff
  # start of lexical extension
  if ($line =~ /\/\* start of (?:extension|base) ([^(]*)\((\d+)\) \*\//) {
    $extfilename = $1;
    $extlineno = $2;
    next;
  }

  # this is stateless because I don't understand the state stuff
  # adjust #line directives for proper pointing into extensions
  if ($extlineno != -1 && $line =~ /#line (\d+)\s+("[^"]*"|)/) {
    $linefile = $2 unless $2 eq "";
    my $filename = $linefile;
    my $number = $lineno + 1;
    if ($filename eq "\"$inputFile\"") {
      $filename = "\"$extfilename\"";
      $number = $1 - $extlineno;
    } elsif ($filename ne "\"$outputFile\"") {
      # if not output file, consider it preprocessed and leave as is
      $number = $1;
    }
    $line = "#line $number $filename\n";
  }

  # this is stateless because I don't understand the state stuff
  # end of lexical extension
  if ($line =~ /\/\* end of extension (.*) \*\//) {
    $extlineno = -1;
    next;
  }

  if ($state == 1) {
    if ($lines[$i+1] =~ m/^(static )?void \*(yy_flex_alloc|yyalloc)/) {
      $state++;
      $lineno++;
      print OUT ("#ifndef NO_YYFLEXLEXER_METHODS\n");
      next;
    }
  }

  elsif ($state == 2) {
    if ($line =~ m/^(static )?void (yy_flex_free|yyfree)/) {
      $state++;
      $i++;
      $lineno++;
      $lineno++;
      print OUT ($line .
                 "#endif // yyflexlexer methods\n");
      next;
    }
  }

  elsif ($state == 3) {
    if ($line =~ m/^\#include <FlexLexer.h>/) {
      $state++;
      $lineno++;
      print OUT ("#include \"sm_flexlexer.h\"\n");
      next;
    }
  }

  elsif ($state == 4) {
    if ($line =~ m/^int yyFlexLexer::yylex/) {
      $state++;
      $i++;       # skip the '{' line, to keep #line numbers in sync
      chomp($line);
      $lineno += 3;
      print OUT ("#ifndef NO_YYFLEXLEXER_METHODS\n" .
                 $line . " {\n");
      next;
    }
  }

  elsif ($state == 5) {
    if ($line =~ m/^\s*\}\s*$/) {
      $state++;
      $i++;       # skip subsequent blank line
      $lineno++;
      $lineno++;
      print OUT ($line .
                 "#endif // yyflexlexer methods\n");
      next;
    }
  }

  elsif ($state == 6) {
    if ($lines[$i+1] =~ m/^yyFlexLexer::yyFlexLexer/) {
      $state++;
      $lineno++;
      print OUT ("#ifndef NO_YYFLEXLEXER_METHODS\n");
      next;
    }
  }

  elsif ($state == 7) {
    if ($lines[$i+1] =~ m/yyFlexLexer::yy_get_previous_state/) {
      $state++;
    }
  }

  elsif ($state == 8) {
    push @methodCopies, ($line);
    if ($line =~ m/yyFlexLexer::yy_try_NUL_trans/) {
      $state++;

      # need to see how much this brace is indented, so we can
      # find the matching one in the next state
      ($ind) = ($lines[$i+1] =~ m/^(\s*)\{/);
      if (!defined($ind)) {
        die("yy_try_NUL_trans not followed by line with left brace\n");
      }
    }
  }

  elsif ($state == 9) {
    push @methodCopies, ($line);
    if ($line =~ m/^$ind\}\s*$/) {
      $state++;
    }
  }

  elsif ($state == 10) {
    if ($line =~ m/^\#line/) {    # #line directive just before section 3
      $state++;

      # NOTE: This if-endif encloses the redefinition of 'yynext'
      # that is appropriate for section 3.  I never use yynext in
      # section 3 (in fact I rarely use section 3 at all), but if
      # someone does then this will need to be adjusted.
      $lineno++;
      print OUT ("#endif // yyflexlexer methods\n");

      if ($makeMethodCopies) {
        # emit the copied method text again, this time substituting
        # the name of the derived lexer class
        if (!$derivedClassName) {
          die("$0: did not see a derived lexer class name\n");
        }

        $lineno++;
        print OUT ("// BEGIN method copies for $derivedClassName\n");
        foreach $copyLine (@methodCopies) {
          $copyLine =~ s/yyFlexLexer/$derivedClassName/;
          $lineno++;
          print OUT ($copyLine);
        }
        $lineno++;
        print OUT ("// END method copies for $derivedClassName\n");

        $lineno += $#movedLines + 1;
        print OUT (@movedLines);
      }
    }
  }

  elsif ($state == 11) {
    # this state prevails until the end of the file; just check
    # that the yynext breakage isn't being exposed
    if ($line =~ m/yynext/) {
      die("$0: unimplemented: 'yynext' used in section 3\n");
    }
  }

  $lineno++;
  print OUT ($line);
}

$lastState = 11;
if ($state != $lastState) {
  $lineno++;
  print OUT ("#error please rebuild $outputFile\n");    # in case not deleted
  close(OUT);    # flush
  die("failed to reach state $lastState; got stuck in state " . $state);
}

close(OUT);
exit(0);


sub diagnostic {
  #print(@_);
}


# EOF

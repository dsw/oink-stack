#!/usr/bin/perl -w
# token.pl            see license.txt for copyright and terms of use

use strict;

# Think of this perl script as compressed data: the set of tokens for
# the xml for the type system.  This file generates three different
# files that have to agree with one another.

# defines the token names for purposes of printing out
my $tokenNamesFile = "xml_name_1.gen.cc";
# the lexer file
my $tokenLexerFile = "xml_lex_1.gen.lex";
# the file that goes into the definition of enum XmlToken
my $tokenEnumFile = "xml_enum_1.gen.h";

open(NAMES, ">$tokenNamesFile") or die $!;
open(LEXER, ">$tokenLexerFile") or die $!;
open(ENUM,  ">$tokenEnumFile") or die $!;

my %data_lines;                 # data lines should be idempotent

sub renderFiles {
  while (<>) {
    chomp;                      # trim trailing newline
    s/^\s*//;                   # trim leading whitespace
    s/\s*$//;                   # trim trailnig whitespace

    if (/^$/) {
      # blank line
      print NAMES "\n";
      print LEXER "\n";
      print ENUM  "\n";
      next;
    }

    my $comment;

    if (/^\w+$/) {
      # data line
      if (defined $data_lines{$_}) {
        # if we have already seen it, comment it out
        $comment = "redundant token: $_";
      } else {
        $data_lines{$_}++;
        undef $comment;         # redundant
        print NAMES "  \"XTOK_$_\",\n";
        print LEXER "\"$_\" return tok(XTOK_$_);\n";
        print ENUM  "  XTOK_$_, // \"$_\"\n";
        next;
      }
    }

    if (m/^\#(.*)$/) {
      die "this line is both data and comment?!: $_" if $comment;
      $comment = $1;
    }

    if ($comment) {
      # comment line
      print NAMES "  // $comment\n";
      print LEXER "  /*${comment}*/\n";
      print ENUM  "  // $comment\n";
      next;
    }

    # illegal line
    die "illegal line: $_\n";
  }
}

eval {
  renderFiles();
};
if ($@) {
  print "$@";
  unlink $tokenNamesFile;
  unlink $tokenLexerFile;
  unlink $tokenEnumFile;
  exit 1;
}

close(NAMES) or die $!;
close(LEXER) or die $!;
close(ENUM)  or die $!;

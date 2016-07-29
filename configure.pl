#!/usr/bin/perl -w
# -*-perl-*-
use strict;

use FindBin;

# This is a meta-configure file that runs configure in the appropriate
# subdirectories.

# state
my $stackdir = $FindBin::Bin;   # the top-level stack directory
my $mode = 'fastdebug';
my %subdir2extraargs;           # map dir to configure args for it
my @SERIAL_NO = ();             # do we want serial numbers?
my @ELSA_OINK_CONF = ();        # flags common to elsa and oink
my $AUTOMAKE_CFLAGS = "";       # equivalent flags for automake projects

sub parse_command_line {
    while(1) {
        my $arg = shift @ARGV;
        last unless defined $arg;
        if (0) {                # orthogonality
        } elsif ($arg =~ m/^-h$/ || $arg =~ m/^--help$/) {
            print_usage();
            exit(1);
        } elsif ($arg =~ m/^[+](\S+?):(\S+)$/) {
            my ($subdirs, $extraarg) = ($1, $2);
            my @subdirslist = split ',', $subdirs;
            for my $d (@subdirslist) {
                push @{$subdir2extraargs{$d}}, $extraarg;
            }
        } else {
            $mode = $arg;
        }
    }
}

sub print_usage {
    print << "END0";

Oink-Stack configuration usage:

    $0 [MODE] [+DIR,...,DIR:ARG]...
       configure all the subdirectories consistent with MODE as below;
         if omitted the default is fastdebug.
       for any +DIR,...,DIR:ARG, add ARG do the configure command arguments
         for each directory DIR given
    $0 -h|--help
       print this message

Where MODE must be exactly one of the below:

debug: Make every effort to find bugs and allow debugging; leaves on
expensive assertions and other debugging support: serial numbers, -g,
etc.  You will probably never want this.

fastdebug: Still allow debugging but don't be gratuitously slow so we
can get lots of tests through; turn off expensive assertions but don't
turn on -O2; turn on serial numbers, -g, etc.  Use this one for
development.

profile: Find performance bugs using gprof; turn on profiling; turn
off expensive assertions but prevent -g (I have had experiance with it
interfering with -pg a long time ago) and prevent optimization
(otherwise call graph and times are messed up).

profile-debug: Profile but also with -g.

perform: Go as fast as we can; leaves on cheap assertions and -O2.
Use this one for running.
END0
    ;
}

sub translate_modes_to_flags {
    if (0) {                    # orthogonality
    } elsif ($mode eq 'debug') {
        push @SERIAL_NO, '-useSerialNumbers';
        push @ELSA_OINK_CONF, '-debug';
	$AUTOMAKE_CFLAGS .= '-g';
    } elsif ($mode eq 'fastdebug') {
        push @SERIAL_NO, '-useSerialNumbers';
        push @ELSA_OINK_CONF, '-no-dash-O2';
	$AUTOMAKE_CFLAGS .= '-g';
    } elsif ($mode eq 'devel') {
        push @SERIAL_NO, '-useSerialNumbers';
        push @ELSA_OINK_CONF, '-no-dash-O2';
        push @ELSA_OINK_CONF, '-devel';
	$AUTOMAKE_CFLAGS .= '-g';
    } elsif ($mode eq 'profile') {
        push @ELSA_OINK_CONF, '-prof';
        push @ELSA_OINK_CONF, '-no-dash-g';
        push @ELSA_OINK_CONF, '-no-dash-O2';
	$AUTOMAKE_CFLAGS .= '-g';
    } elsif ($mode eq 'profile-debug') {
        push @ELSA_OINK_CONF, '-prof';
        push @ELSA_OINK_CONF, '-no-dash-O2';
	$AUTOMAKE_CFLAGS .= '-g';
    } elsif ($mode eq 'perform') {
	$AUTOMAKE_CFLAGS .= '-O2';
    } else {
        die "illegal mode: $mode\n";
    }
}

sub configure_dir {
    my ($repo, $flags_list_ref) = @_;
    my @flags = @{$flags_list_ref};
    if (defined $subdir2extraargs{$repo} && @{$subdir2extraargs{$repo}}) {
        @flags = (@flags, @{$subdir2extraargs{$repo}});
    }
    my $fulldir = "$stackdir/$repo";
    if (!-d $fulldir) {
        print "warning: can't find $fulldir (skipped)\n";
        return;
    }
    print "\n**** Configuring in $repo\n";
    my $curdir = `pwd`; chomp $curdir;
    chdir $fulldir;
    my @cmd = ("./configure", @flags);
    print(join (' ', @cmd), "\n"); # NOTE: looses information if there are spaces in args
    die "configure failed" if system(@cmd);
}

sub do_configure {
    configure_dir('smbase', \@ELSA_OINK_CONF);
    configure_dir('ast', \@ELSA_OINK_CONF);
    configure_dir('elkhound', [ '-endloc' ]);
    #configure_dir('mcpp', [ '--enable-replace-cpp', '--enable-modular-cpp', 'CFLAGS='.$AUTOMAKE_CFLAGS ]);
    configure_dir('elsa', [ '-gnu=1', '-kandr=1', @ELSA_OINK_CONF, @SERIAL_NO ]);
    configure_dir('libregion', [ ]);
    configure_dir('libqual', [ ]);
    configure_dir('oink', [ @ELSA_OINK_CONF, @SERIAL_NO ]);
    configure_dir('oink-scripts', [ ]);

# uncomment this line and the corresponding line in
# StackRepositories.list in order to use platform model
#    configure_dir('platform-model', [ ]);

}

# main ****

eval {
    parse_command_line();
    translate_modes_to_flags();
    do_configure();
};
if ($@) {
    print "$@";
    print "to get a usage message type: $0 -h\n";
    exit(1);
} else {
    print "configure DONE\n";
}

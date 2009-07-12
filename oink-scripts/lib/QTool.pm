
# $Id: QtoolUtil.pm 3814 2006-07-17 21:59:07Z quarl $

# TODO: --no-filelist to avoid using filelist.
# TODO: --gdb to invoke gdb with arguments.

package QTool;

use strict;
use warnings;

use File::Spec;
use File::Temp qw/ tempfile tempdir /;
use FindBin;
use Carp;
use Cwd;
use Config;

use Stopwatch;

use base qw(Exporter);

our @EXPORT_OK = qw(
     $ORIG_0
     $QUAL
     $OINK
     $CONFIG
);

our %EXPORT_TAGS = (all => [@EXPORT_OK]);

our $ORIG_0 = $0;
$0 =~ s,.*/,,;

my $STACK_DIR = "$FindBin::RealBin/../..";

if (!(-f "$STACK_DIR/.stack-dir.cqual++-stack" ||
      -f "$STACK_DIR/.stack-dir.oink-stack"))
{
    die "$ORIG_0: can't find stack directory - looking for $STACK_DIR/.stack-dir.cqual++-stack or $STACK_DIR/.stack-dir.oink-stack\n";
}

my $PLATFORM_MODEL_DIR = "$STACK_DIR/platform-model";
my $OINK_DIR = "$STACK_DIR/oink";

# TODO: refactor these
our $QUAL = "$OINK_DIR/qual";
our $OINK = "$OINK_DIR/oink";
our $CONFIG = "$STACK_DIR/oink-scripts/config/lattice.taint";

my $signames;
setup_signames();
sub sig_name { return $signames->{$_[0]} || "NUM$_[0]" }

sub new {
    my ($class) = @_;
    my $self = bless {}, $class;

    # configuration to be set by child (some optional)
    $self->{TOOL} = undef;                          # must be set by child
    $self->{CPP_CMD} = 'cpp';
    $self->{EXT_ZIP} = undef;                       # must be set by child
    $self->{OPT_SRZ} = '-o-srz';
    $self->{LIBC} = undef;
    $self->{OPT_NOSRZ} = '';

    $self->{MIN_STACKSIZE} = 0;

    $self->{DEFAULT_VARIANT} = undef;
    $self->{ALLOWED_VARIANTS} = [];

    # options
    $self->{twopass} = 0;
    $self->{outputfile} = undef;
    $self->{verbose} = 0;
    $self->{print_times} = 0;

    $self->Init();

    Carp::confess unless $self->{TOOL};
    Carp::confess unless $self->{OPT_SRZ};
    Carp::confess unless $self->{ARCHIVE_SRZ_EXT_PFX};
    $self->{EXT_ZIP} = $self->{ARCHIVE_SRZ_EXT_PFX} . 'z';
    $self->{EXT_DIR} = $self->{ARCHIVE_SRZ_EXT_PFX} . 'dir';

    # quarl 2006-10-11
    #     support both qz and qdir, preferring qz.
    if (!$self->{LIBC}) {
        my $l = "$PLATFORM_MODEL_DIR/libc_glibc-2.3.5/libc";
        if (-e "$l.$self->{EXT_ZIP}") {
            $self->{LIBC} = "$l.$self->{EXT_ZIP}";
        } elsif (-e "$l.$self->{EXT_DIR}") {
            $self->{LIBC} = "$l.$self->{EXT_DIR}";
        } else {
            # don't die here since we might not need it
        }
    }

    if (!$self->{LIBSTDCXX}) {
        my $l = "$PLATFORM_MODEL_DIR/libstdc++_gcc-3.4/libstdc++";
        if (-e "$l.$self->{EXT_ZIP}") {
            $self->{LIBSTDCXX} = "$l.$self->{EXT_ZIP}";
        } elsif (-e "$l.$self->{EXT_DIR}") {
            $self->{LIBSTDCXX} = "$l.$self->{EXT_DIR}";
        } else {
            # don't die here since we might not need it
        }
    }

    $self->CanonicalizePaths();
    # $self->CheckPaths();

    return $self;
}

# virtual method
sub Init {
    my ($self) = @_;
}

sub _canonicalize_path {
    my ($rf) = @_;
    Carp::confess unless $rf && ref($rf) eq 'SCALAR';
    $$rf = Cwd::realpath($$rf) if $$rf;
}

sub CanonicalizePaths {
    my ($self) = @_;

    _canonicalize_path(\ $self->{TOOL});
    _canonicalize_path(\ $self->{LIBC});
    _canonicalize_path(\ $self->{LIBSTDCXX});
    _canonicalize_path(\$CONFIG);
}

sub _checkpath_file {
    my ($f) = @_;
    if (! -e $f) {
        die "$0: $f does not exist\n";
    }
    if (! -r $f) {
        die "$0: $f is not readable\n";
    }
}

sub _checkpath_tool {
    my ($f) = @_;
    if (! -e $f) {
        die "$0: $f does not exist\n";
    }
    if (! -x $f || -d $f) {
        die "$0: $f is not executable\n";
    }
}

sub CheckPaths {
    my ($self) = @_;
    _checkpath_tool($self->{TOOL});
    _checkpath_file($self->{LIBC}) if $self->{need_libc};
    _checkpath_file($self->{LIBSTDCXX}) if $self->{need_libstdcxx};
}

# virtual method
sub ApplyVariant {
    my ($self) = @_;
}

sub System {
    my ($self, $cmd) = @_;
    if ($self->{verbose}) {
        print STDERR "Executing: $cmd\n";
    }
    if (system($cmd)) {
        my $e = $? >> 8;
        if ($e) {
            print STDERR "$0: exit code $e from executing $cmd\n";
        } else {
            print STDERR "$0: exit signal $? from executing $cmd\n";
        }
        exit $e;
    }
    # exit $? >> 8 || -$?;
    # if ($?) { die; }
}

my $tmpdir;

sub tmpi {
    my ($self, $suffix) = @_;
    $tmpdir ||= tempdir( "qtool-preproc.XXXXXXXX", TMPDIR => 1, CLEANUP => 1 );
    my ($fh, $filename) = tempfile( DIR => $tmpdir, SUFFIX => $suffix );
    return $filename;
}

sub cpp {
    my ($self, $input, $output) = @_;
    $self->System("$self->{CPP_CMD} -o $output $input");
}

sub getqzext {
    my ($self, $f) = @_;
    die unless $f;
    $f =~ /[.]([^.]+)$/;
    return $1 || $self->{EXT_ZIP};
}

sub noext {
    my ($f) = @_;
    die unless $f;
    $f =~ /^(.+)[.][^.]+$/;
    return $1 || $f;
}

sub removeq {
    my ($f) = @_;
    if ($f =~ /[.].dir$/ && -d $f) {
        system("rm", "-r", $f);
    } else {
        unlink($f);
    }
}

sub Analyze_i {
    my ($self) = @_;
    $self->{stopwatch} = new Stopwatch;
    my @i = @{$self->{files}};
    # my $todel = [];

    my $filelist = new File::Temp(TEMPLATE => "qtool-filelist.XXXXXXXX",
                                  DIR => File::Spec->tmpdir);
    # my $inputfiles = "";
    for my $i (@i) {
        Carp::confess "7423557d-f7b3-4b4d-ac6b-e30bb02ef149" unless $i;
        if (!-e $i) { die "$0: can't find $i\n"; }
        if ($i =~ /[.]c$/) {
            my $t = $self->tmpi('.i');
            $self->cpp($i, $t);
            $i = $t;
        } elsif ($i =~ /[.]cc$/) {
            my $t = $self->tmpi('.ii');
            $self->cpp($i, $t);
            $i = $t;
        }

        # $inputfiles = "$inputfiles $i";
        print $filelist "$i\n";
    }
    $filelist->close();
    my $inputfiles = "-o-program-files " . $filelist->filename;
    if ($self->{verbose}) {
        print STDERR "Contents of " . $filelist->filename . ": \n";
        system("cat ".$filelist->filename);
    }

    if ($self->{twopass} && $self->{outputfile}) {
        my $ext = $self->getqzext($self->{outputfile});
        my $tmpfile1 = "$self->{outputfile}.tmp_pass1.$ext";
        my $tmpfile2 = "$self->{outputfile}.tmp_pass2.$ext";

        my $cmd = "$self->{CMD} $inputfiles $self->{OPT_SRZ} $tmpfile1";
        if ($self->{verbose}) {
            print STDERR "Executing: $cmd\n";
        }
        system($cmd);
        my $r1 = $?;
        my $e1 = $r1 >> 8;
        # if ($e1) {
        #     print STDERR "$0: first pass returned exit code $e1\n";
        # }
        if ($r1 && !$e1) {
            my $signame = sig_name($r1);
            die "$0: first pass died with SIG$signame (signal exit code $r1)\n";
        }
        if (! -e $tmpfile1) {
            if ($e1) {
                exit $e1;
            } else {
                print STDERR "$0: first pass did not output $tmpfile1\n";
                die;
            }
        }

        $cmd = "$self->{CMD} $tmpfile1 $self->{OPT_SRZ} $tmpfile2";
        if ($self->{verbose}) {
            print STDERR "Executing: $cmd\n";
        }
        # system($cmd);
        my $output2 = `$cmd 2>&1` || '';
        my $r2 = $?;
        my $e2 = $r2 >> 8;
        # if ($tmpfile1 =~ /[.]qdir$/ && -d $tmpfile1) {
        # }
        removeq($tmpfile1);
        if ($r2 && !$e2) {
            my $signame = sig_name($r2);
            print STDERR "$0: second pass died with SIG$signame (signal exit code $r2) vs first pass exit code $e1\n";
            print STDERR $output2;
            die;
        }
        if ($e1 != $e2) {
            print STDERR "$0: second pass exited with exit code $e2 vs first pass exit code $e1\n";
            print STDERR $output2;
            die;
        }
        if (! -e $tmpfile2) {
            print STDERR "$0: second pass did not output $tmpfile2\n";
            die;
        }
        removeq($self->{outputfile});
        rename($tmpfile2, $self->{outputfile});
        $self->PrintTimes();
        exit $e2;
    } else {
        my $cmd = "$self->{CMD} $inputfiles";
        my $tmpfile1;
        if ($self->{outputfile}) {
            my $ext = $self->getqzext($self->{outputfile});
            $tmpfile1 = "$self->{outputfile}.tmp_pass1.$ext";
            $cmd .= " $self->{OPT_SRZ} $tmpfile1";
        } else {
            $cmd .= " $self->{OPT_NOSRZ}";
        }
        if ($self->{verbose}) {
            print STDERR "Executing: $cmd\n";
        }
        system($cmd);
        my $r1 = $?;
        my $e1 = $r1 >> 8;
        if ($r1 && !$e1) {
            my $signame = sig_name($r1);
            die "$0: died with SIG$signame (signal exit code $r1)\n";
        }
        if ($tmpfile1) {
            if (! -e $tmpfile1) {
                if ($e1) {
                    exit $e1;
                } else {
                    print STDERR "$0: first pass did not output $tmpfile1\n";
                    die;
                }
            }

            removeq($self->{outputfile});
            rename($tmpfile1, $self->{outputfile});
        }
        $self->PrintTimes();
        exit $e1;
    }
}


sub PrintTimes {
    my ($self) = @_;
    $self->{stopwatch}->stop();
    if ($self->{print_times}) {
        my $wallclock = $self->{stopwatch}->getWallclock();
        my $cpu = $self->{stopwatch}->getCPUtime();
        my $sys = $self->{stopwatch}->getSystime();
        print STDERR "qtool times: wallclock:$wallclock cpu:$cpu sys:$sys\n";
    }
}

sub syntax {
    my ($self) = @_;
    die "syntax: $0 [options] files.i files.c...\n";
}

sub ParseCmdLine() {
    my ($self) = @_;

    if (!@ARGV) {
        $self->syntax();
    }

    $self->{CMD} = $self->{TOOL};
    $self->{variant} = $self->{DEFAULT_VARIANT};

    my $specified_qsrz = 0;
    my $compile_mode = 0;
    my $libc = 0;
    my $libstdcxx = 0;
    my $report_link_errors = 1;
    my @files;
    for (my $i=0; $i < @ARGV; ++$i)
    {
        local $_ = $ARGV[$i];
        if (/[.]ii?|[.]cc?|[.]h|[.][qo]z|[.][qo]dir$/) {
            push(@files, $_);
        } elsif (/^-o-program-files$/) {
            ++$i;
            my $f = $ARGV[$i];
            local *F;
            open(F,$f) || die "$!";
            local $_;
            while (<F>) {
                chomp;
                push(@files, $_);
            }
            close(F);
        } elsif (/^--two-pass$/) {
            $self->{twopass} = 1;
        } elsif (/^--qq-verbose$/) {
            $self->{verbose} = 1;
        } elsif (/^-lc$/) {
            $libc = 1;
        } elsif (/^-lstdc[+][+]$/) {
            $libstdcxx = 1;
        } elsif (/^-c$/) {
            $compile_mode = 1;
            $report_link_errors = 0;
        } elsif (/^-fo-report-link-errors$/) {
            $report_link_errors = 1;
        } elsif (/^-fo-no-report-link-errors$/) {
            $report_link_errors = 0;
        } elsif (/^--variant$/) {
            ++$i;
            $self->{variant} = $ARGV[$i];
        } elsif (/^--print-times$/) {
            $self->{print_times} = 1;
        } elsif (/^-/) {
            if (/^-I/) {
                $self->{CPP_CMD} .= " $_";
            } elsif (/^-o-lang$/ || /^-tr$/ || /^-o-func-filter$/) {
                ++$i;
                $self->{CMD} .= " $_ $ARGV[$i]";
            } elsif (/^-[oq]-srz$/) {
                if ($specified_qsrz++) { die }
                ++$i;
                $self->{outputfile} = $ARGV[$i];
            } else {
                # pass-through
                $self->{CMD} .= " $_";
            }
        } else {
            syntax();
        }
    }

    if ($self->{variant}) {
        $self->{variant} = uc($self->{variant});
        if (!grep { $_ eq $self->{variant} } @{$self->{ALLOWED_VARIANTS}}) {
            die "$0: invalid variant '$self->{variant}'; allowed variants are: @{$self->{ALLOWED_VARIANTS}}\n";
        }
    }

    $self->{CMD} .= " -o-lang SUFFIX";

    # no_writeSerialNo is now obsolete since the default is on; to turn
    # serialno on you would do -tr serialno-write -tr serialno-announce

    # $self->{CMD} .= " -tr no_writeSerialNo";

    # TODO: only use this if input files are .i, not .c!
    $self->{CMD} .= " -tr nohashline";

    if ($report_link_errors) {
        $self->{CMD} .= " -fo-report-link-errors";
    } else {
        $self->{CMD} .= " -fo-no-report-link-errors";
    }

    if ($compile_mode && !$specified_qsrz) {
        if (scalar(@files) == 1) {
            $self->{outputfile} = noext($files[0]) . '.' . $self->{EXT_ZIP};
        } else {
            die "$0: in compile mode, must specify output file if not exactly 1 input file (you specified: @files)\n";
        }
    }

    if ($libc) {
        my $LIBC_CONTROL = "$self->{LIBC}.ctl";
        push(@files, $self->{LIBC});
        $self->{CMD} .= " -o-control $LIBC_CONTROL";
        $self->{need_libc} = 1;
    }

    if ($libstdcxx) {
        my $LIBSTDCXX_CONTROL = "$self->{LIBSTDCXX}.ctl";
        push(@files, $self->{LIBSTDCXX});
        $self->{CMD} .= " -o-control $LIBSTDCXX_CONTROL";
        $self->{need_libstdcxx} = 1;
    }

    $self->{files} = [@files];
}

sub Run {
    my ($self) = @_;
    increase_stack_ulimit($self->{MIN_STACKSIZE});
    $self->ParseCmdLine();
    $self->CheckPaths();
    $self->ApplyVariant();
    $self->Analyze_i();
}

sub increase_stack_ulimit {
    # increase the maximum stack size
    my ($stack_mb) = @_;
    return unless $stack_mb;

    if ($ARGV[0] && $ARGV[0] eq '--ulimited') {
        shift @ARGV;
    } else {
        my $ULIMIT = $stack_mb*1024;
        exec "ulimit -s $ULIMIT; exec $ORIG_0 --ulimited @ARGV"
            || die "exec failed";
    }
}

sub setup_signames {
    my $sig_num = $Config::Config{'sig_num'} or die;
    my $sig_name = $Config::Config{'sig_name'} or die;
    $sig_num = [ split /[ ,]+/, $sig_num ];
    $sig_name = [ split /[ ,]+/, $sig_name ];

    die unless scalar(@$sig_num) == scalar(@$sig_name);

    $signames = {};
    for my $i (0..$#$sig_num) {
        $signames->{$sig_num->[$i]} = $sig_name->[$i];
    }
}

1;

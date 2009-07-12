# $Id: Stopwatch.pm 3949 2006-08-01 05:35:30Z quarl $

# quarl 2006-07-26
#    new module

use strict;
use warnings;

package Stopwatch;

use Time::HiRes qw(time);
use Carp;

sub new {
    my ($class) = @_;
    my $self = {};
    bless $self, $class;
    $self->{start} = gettimes();
    return $self;
}

sub stop {
    my ($self) = @_;
    Carp::confess unless ref($self);
    $self->{end} = gettimes();
    $self->{diff} = difftimes($self->{end}, $self->{start});
}

sub getWallclock {
    my ($self) = @_;
    Carp::confess unless $self->{diff};
    return roundtotenth($self->{diff}->[0]);
}

sub getCPUtime {
    my ($self) = @_;
    Carp::confess unless $self->{diff};
    return roundtotenth($self->{diff}->[3]);
}

sub getSystime {
    my ($self) = @_;
    Carp::confess unless $self->{diff};
    return roundtotenth($self->{diff}->[4]);
}

sub gettimes() {
    # returns [wallclock, cpu, sys, c_cpu, c_sys]
    #   c_cpu is the cpu time used by children
    #   c_sys is the system time used by children
    return [time(), times()];
}

sub roundtotenth() {
    my ($x) = @_;
    return int(10*$x)/10.;
}

sub difftimes() {
    my ($t1, $t2) = @_;
    Carp::confess unless scalar(@$t1) == 5 && scalar(@$t2) == 5;
    my $r = [];
    for my $i (0..4) {
        push(@$r, ($t1->[$i] - $t2->[$i]));
    }
    return $r;
}

1;

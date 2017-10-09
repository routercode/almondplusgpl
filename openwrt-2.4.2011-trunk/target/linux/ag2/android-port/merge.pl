#!/usr/bin/env perl
#
use strict;
use FileHandle;

my $merge_file = $ARGV[0];
my $into_file  = $ARGV[1];

my @merge = ();
my @into  = ();

my $merge = {};
my $into  = {};

parse( $merge_file, \@merge, $merge );
# parse( $into_file,  \@into,  $into  );

my $fd = new FileHandle "<$into_file";
while( <$fd> ) {
    chomp;
    my $line = $_;
    my ($var, $val);
    if ( $line =~ /^\# (CONFIG_\S+) is not set/ ) {
	$var = $1;
    }
    elsif ( $line =~ /^(CONFIG_\w+)=(.*)/ ) {
	$var = $1;
	$val = $2;
    }
    else {
	print $line, "\n";
	next;
    }

    if ( defined( $merge->{$var} ) ) {
	if ( defined( $merge->{$var} ) ) {
	    print "$var=$merge->{$var}\n";
	}
	else {
	    print "# $var is not set\n";
	}
	$merge->{$var} = 'used';
    }
    else {
	print "$line\n";
    }
}

print "\n#\n# Android Config\n#\n";
foreach my $key ( keys( %$merge ) ) {
    next if ( $merge->{$key} eq 'used' );
    print "$key=$merge->{$key}\n";
}

sub parse {
    my( $filename, $db, $hash ) = @_;

    my $fd = new FileHandle "<$filename";
    while( <$fd> ) {
	chomp;
	my( $var, $val ) = split( /=/, $_ );
	unless ( $var =~ /^CONFIG_/ ) {
	    $var = "CONFIG_" . $var;
	}
	unless( $val ) {
	    $val = 'y';
	}
	push( @$db, { name => $var, value => $val } );
	$hash->{$var} = $val;
    }
    close $fd;
}


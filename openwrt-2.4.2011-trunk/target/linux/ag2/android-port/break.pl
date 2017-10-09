#!/usr/bin/env perl
use strict;
use FileHandle;

my $base = shift @ARGV;
my $pnum = 1;
my $fd;

my $working_id = '';

while(<>) {
    my $line = $_;
    if ( $line =~ /^diff -Naur $base/ ) {
	my $id = get_id( $line );
	if ( $id eq 'notfound' ) {
	    die "id not found in: $line";
	}
	if ( $id ne $working_id ) {
	    $working_id = $id;
	    close( $fd ) if ( $fd );
	    my $filename = sprintf( "%04d_android_%s.patch", $pnum, $id );
	    $pnum += 1;
	    $fd = new FileHandle ">$filename";
	}
    }
    print $fd $line;
}
close $fd;

sub get_id {
    if ( /\s+$base\/(\w+)\// ) {
	return $1;
    }
    else {
	return 'notfound';
    }
}

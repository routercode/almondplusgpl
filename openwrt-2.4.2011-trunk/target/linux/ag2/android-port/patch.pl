#!/usr/bin/env perl
use strict;
use File::Basename;

my $src = <>;
chop $src;

while (<>) {
    chop;
    my ( $op, $file ) = split;
    if ( $op eq 'D' ) {
	system( "cp -f $src/$file $file" ) &&
	    die "Failed to copy $src/$file to $file: $!";
    }
    elsif ( $op eq 'A' ) {
	system( "mkdir -p " . dirname( $file ) ) &&
	    die "Failed to mkdir " . dirname( $file), "$!";
	cvs_add_dir( dirname( $file ) );
	system( "cp -f $src/$file $file" ) &&
	    die "Failed to copy $src/File to $file: $!";
	system( "cvs add $file" ) &&
	    die "Failed to cvs add $file: $!";
    }
    elsif ( $op eq 'R' ) {
	unlink( $file );
	system( "cvs rm $file" ) &&
	    die "Failed to remove $file: $!";
    }
}

sub cvs_add_dir {
    my $dir = shift;
    return if ( -d "$dir/CVS" );
    cvs_add_dir( dirname( $dir ) );
    system( "cvs add $dir" ) &&
	die "Failed to cvs add $dir: $!";
}

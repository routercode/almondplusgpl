#!/usr/bin/perl
use strict;
use FileHandle;
use File::Basename;
use File::Copy;
use File::Compare;

# A conditional copy script.  Acts just like 'cp -fR', except it
# will not do a copy of the timestamp on the target is greater
# than the timestamp on the source.  
#
# Also will not recurse into CVS or .svn directories.
#
# If passed in a -report <file> option, will print the files it
# ended up copying into the <file>, which can then be used after
# the conditional copy to do things only to copied files.
#
my $echo = 0;
my @args = ();
my $rfile;

while( my $arg = shift( @ARGV ) ) {
    if ( $arg eq '-report' ) {
	my $report_file = shift @ARGV;
	$rfile = new FileHandle ">$report_file" ;
	unless( $rfile ) {
	    die "Cannot open $report_file to write: $!";
	}
	next;
    }
    if ( $arg eq '-debug' ) {
	$echo = 1;
	next;
    }
    push( @args, $arg );
}

my @source_files = @args;
my $target_file  = pop @source_files;
my $prepared = -f "$target_file/.prepared";

foreach my $source_file ( @source_files ) {
    next if ( -d $source_file && basename( $source_file ) eq 'CVS' );
    next if ( -d $source_file && basename( $source_file ) eq '.svn' );
    rcopy( $source_file, $target_file );
}
close $rfile if ( $rfile );
exit 0;

sub rcopy {
    my( $src, $tar ) = @_;

    if ( -f $src ) {
	my $bn = basename( $src );
	if ( -d $tar ) {
	    $tar = "$tar/$bn";
	}
	if ( -f $tar ) {
	    if ( -M $src < -M $tar ) {
		chmod( 0644, $tar );
		print "ccp (time): copy $src to $tar\n" if ( $echo );
		copy( $src, $tar ) or die $!;
		print $rfile "$tar\n" if ( $rfile );
	    }
	    elsif ( (! $prepared) && (compare($src,$tar) != 0) ) {
		chmod( 0644, $tar );
		print "ccp (mod): copy $src to $tar\n" if ( $echo );
		copy( $src, $tar ) or die $!;
		print $rfile "$tar\n" if ( $rfile );
	    }
	}
	else {
	    my $dir = dirname( $tar );
	    if ( ! -d $dir ) {
		mkdir( $dir ) || die "cpp: failed to mkdir $dir: $!";
	    }
	    print "ccp (new): copy $src to $tar\n" if ( $echo );
	    copy( $src, $tar ) or die $!;
	    print $rfile "$tar\n" if ( $rfile );
	}
    }
    else {
	my $bn = basename( $src );
	if ( ! -d $tar ) {
	    mkdir( "$tar" ) || die "cpp: failed to mkdir $tar: $!";
	}
	if ( ! -d "$tar/$bn" ) {
	    mkdir( "$tar/$bn" ) || die "cpp: failed to mkdir $tar/$bn: $!";
	}
	my $D;
	opendir( $D, $src ) or die "ccp: Cannot read $src.";
	my @dir = readdir( $D );
	closedir( $D );
	foreach my $e ( @dir ) {
	    next if ( $e eq '.' );
	    next if ( $e eq '..' );
	    next if ( $e eq 'CVS' );
	    next if ( $e eq '.svn' );
	    rcopy( "$src/$e", "$tar/$bn" );
	}
    }
}


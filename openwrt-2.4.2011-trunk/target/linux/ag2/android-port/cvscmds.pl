#!/usr/bin/env perl
use strict;
use FileHandle;
use File::Find;
use Cwd;
use File::Basename;

my $dir1 = $ARGV[0];
my $parent = $ARGV[1];

my $top = cwd();

my @added = ();
my @removed = ();
my @diff = ();

my $DIR = $dir1;
chdir( $dir1 );
find({wanted=>\&snoop}, '.');

chdir( ".." );
chdir( $parent );
find({wanted=>\&snoop2}, '.');

print "$top/$DIR\n";
print "A $_\n" foreach @added;
print "D $_\n" foreach @diff;
print "R $_\n" foreach @removed;

sub snoop {

    my $fullname = $File::Find::name;
    my $dirname  = $File::Find::dir;
    my $filename = $_;

    return if ( -d "$top/$parent/$fullname" );
    return if ( -d "$top/$DIR/$fullname" );
    return if ( $_ eq '.' );

    $fullname =~ s/^\.\///g;
    $dirname =~ s/^\.\///g;

#    print $fullname, "\n";
#    print "  ", $dirname, "\n";
#    print "  ", $filename, "\n";

    if ( -f "$top/$parent/$fullname" ) {
	# present, different?
	if ( system( "cmp -s $top/$parent/$fullname $top/$DIR/$fullname" ) ) {
	    push( @diff, $fullname );
	}
    }
    else {
	# added 
	push( @added, $fullname );
    }
}

sub snoop2 {

    my $fullname = $File::Find::name;
    my $dirname  = $File::Find::dir;
    my $filename = $_;

    return if ( -d "$top/$parent/$fullname" );
    return if ( -d "$top/$DIR/$fullname" );
    return if ( $_ eq '.' );

    $fullname =~ s/^\.\///g;
    $dirname =~ s/^\.\///g;

#    print $fullname, "\n";
#    print "  ", $dirname, "\n";
#    print "  ", $filename, "\n";

    if ( ! -f "$top/$DIR/$fullname" ) {
	push( @removed, $fullname );
    }
}

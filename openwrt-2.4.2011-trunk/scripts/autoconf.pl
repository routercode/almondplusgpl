#!/usr/bin/perl
#
# Script to generate
#   CFLAGS
#   Makefile include
#   C header
#
# from CONFIG_XXX_ variables found in $(TOPDIR)/.config.
#
# Can be used in package Makefiles to leverage configuration variables
# for conditional builds.
#
# To make this all work the way it should, you need the following in
# your package Makefile:
#
# ifeq ($(DUMP),)
#   STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep '^CONFIG_XXX_' $(TOPDIR)/.config | md5s)
# endif
#
# define Build/Configure
#   rm -f $(PKG_BUILD_DIR)/.configured*'
#   $(AUTOCONF) --config $(TOPDIR)/.config --pattern XXX --header   $(PKG_BUILD_DIR)/autoconf.h
#   $(AUTOCONF) --config $(TOPDIR)/.config --pattern XXX --makefile $(PKG_BUILD_DIR)/autoconf.mk
# endef
#
# EXTRA_CFLAGS := $(shell $(AUTOCONF) --config $(TOPDIR)/.config --pattern XXX --cflags)
#
use strict;
use FileHandle;

my( $conf, $pat, $mode, $outfile );
my @vars = ();

while( my $arg = shift( @ARGV ) ) {
    if ( $arg eq '--config' ) {
	$conf = new FileHandle "<" . shift( @ARGV );
	die "Cannot read --config file: $!"
	    unless( $conf );
	next;
    }
    if ( $arg eq '--pattern' ) {
	$pat = shift @ARGV;
	next;
    }
    if ( $arg eq '--header' ) {
	$mode = 'header';
	$outfile = new FileHandle ">" . shift( @ARGV );
	die "Cannot write --header file: $!"
	    unless( $outfile );
	next;
    }
    if ( $arg eq '--makefile' ) {
	$mode = 'makefile';
	$outfile = new FileHandle ">" . shift( @ARGV );
	die "Cannot write --makefile file: $!"
	    unless( $outfile );
	next;
    }
    if ( $arg eq '--cflags' ) {
	$mode = 'cflags';
	next;
    }
}

while( <$conf> ) {
    chomp;
    next if ( /^$/ );
    next if ( /^\#/ );
    if ( /^CONFIG_${pat}_(\S+)=(.+)/ ) {
      push( @vars, { name => $1, val => $2 } );
    }
}
close $conf;

{
    if ( $mode eq 'cflags' ) {
	my @flags = ();
	foreach my $var ( @vars ) {
	    if ( $var->{val} eq 'y' ) {
		push( @flags, "-D" . $var->{name} );
	    }
	    else {
		push( @flags, "-D" . $var->{name} . '=' . $var->{val} );
	    }
	}
	print join ' ', @flags, "\n";
    }
    elsif ( $mode eq 'header' ) {
	print $outfile "#ifndef __AUTOCONFIG_H__\n";
	print $outfile "#define __AUTOCONFIG_H__\n";
	foreach my $var ( @vars ) {
	    if ( $var->{val} eq 'y' ) {
		print $outfile "#define $var->{name} 1\n";
	    }
	    else {
		print $outfile "#define $var->{name} $var->{val}\n";
	    }
	}
	print $outfile "#endif\n";
	close $outfile;
    }
    elsif ( $mode eq 'makefile' ) {
	foreach my $var ( @vars ) {
	    print $outfile "$var->{name}=$var->{val}\n";
	}
	close $outfile;
    }
}
exit 0;


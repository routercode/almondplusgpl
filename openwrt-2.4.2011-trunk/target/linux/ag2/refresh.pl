#!/usr/local/bin/perl
#
# refresh.pl - A script to keep ag2/files up-to-date with any changes made in
# g2/files since the last run.
#
use strict;
use YAML;
use File::Basename;
use Cwd;
use File::Find;
use FileHandle;

my $die_if_not_match = 1;
my $dont_doit = 0;

my $old_tree = "linux-2.6.36";
my $new_tree = "linux-2.6.36-new";

if ( $ARGV[0] eq '-n' ) {
    $dont_doit = 1;
}

my $top = cwd();
my $reference = "$top/../g2";
#my $reference = "/work/bahamut/peebles/g2-port-reference/openwrt-2.4.2011-trunk/target/linux/g2";

my $owrt = dirname( dirname( dirname( $top ) ) );
unless( -f "$owrt/dl/linux-2.6.36.tar.bz2" ) {
    die "$owrt/dl/linux-2.6.36.tar.bz2 is missing.  You need to build first!";
}

my $db = YAML::LoadFile( "port-db.yml" );
unless( $db ) {
    die "Failed to read port-db.yml: Are you in the right directory?";
}

# From our database, construct the list of all known files in the
# g2 tree.  We will use this to make sure nothing was added or
# deleted.
#
# old_drivers + unique + patches
#
my @all_old_files = ( @{$db->{old_drivers}}, @{$db->{unique}} );
foreach my $key ( keys( %{$db->{patches}} ) ) {
    @all_old_files = ( @all_old_files, @{$db->{patches}->{$key}} );
}
my @all_patch_files = ();
foreach my $pf ( keys( %{$db->{patches}} ) ) {
    foreach my $file ( @{$db->{patches}->{$pf}} ) {
	push( @all_patch_files, { file => $file, 
				  version => $db->{patch_versions}->{$file}, 
				  pf => $pf } );
    }
}

# See if our count matches the actual
#
print "Checking basic database consistentcy ...\n";
my $not_found = 0;
foreach my $file ( @all_old_files ) {
    if ( ! -f "$reference/files/$file" ) {
	print "In db, but missing from G2: $file\n";

	# Suggestions?
	# This file has been removed.  If its in 'unique' we need to do a cvs-remove.
	# If its in the driver map, it needs to be cvs-removed.
	# If its in a patch file, the database needs to be updated.
	# 
	my @unique = @{$db->{unique}};
	my @old = @{$db->{old_drivers}};

	if ( in_list( $file, @unique ) ) {
	    print "  suggestion: rm -f files/$file; cvs rm files/$file\n";
	    print "    and remove from unique: in database.\n";
	}
	elsif ( in_list( $file, @old ) ) {
	    foreach my $key ( keys( %{$db->{mapping}} ) ) {
		if ( $db->{mapping}->{$key} eq $file ) {
		    print "  suggestion: rm -f files/$key; cvs rm files/$key\n";
		    print "    and modify corresponding Makefile/Kconfig\n";
		    print "    and remove from mapping: in database.\n";
		}
	    }
	}
	else {
	    print "  suggestion: this file is in a patch set.  remove it from db and regen the patch set.\n";
	}

	$not_found = 1;
    }
}
die if ( $not_found && $die_if_not_match );

# Check the other direction
#
$not_found = 0;
my @all_g2_files = find_files_in( "$reference/files" );
foreach my $file ( @all_g2_files ) {
    if ( ! in_list( $file, @all_old_files ) ) {
	print "In G2, but missing from db: $file\n";

	# Suggestions?
	# It might be a new unique.
	# It might be a new driver file, so a mapping needs to be created.
	# It might be a new overlap, so it needs to be added to patch files.
	#
	# We need a reference kernel to know if its unique or an overlap.
	#
	if ( ! -d $old_tree ) {
	    print "  Creating a reference linux kernel tree ...\n";
	    create_old_tree( $old_tree );
	}
	if ( -f "$old_tree/$file" ) {
	    print "  This file is part of the kernel.  It needs to be added to a patch file and the patch file regenerated.\n";
	}
	else {
	    print "  This file is missing from the kernel.  Its either unique, or a new driver file.\n";
	    print "  If its unique, then\n";
	    print "    cp -f $reference/files/$file files/$file; cvs add files/$file\n";
	    print "  and create a new unique: line in the database.\n";
	    print "  If its part of a G2 driver, copy and cvs-add it to the new G2 driver location\n";
	    print "  and create a new mapping in the database.\n";
	}
	$not_found = 1;
    }
}
die if ( $not_found && $die_if_not_match );

# Go through the mappings (new driver to old) and see if there are any
# inconsistencies or diffs.
#
print "Checking driver mapping consistency, and files to update ...\n";
$not_found = 0;
my $diffs = {};
my $maybe = {};
foreach my $file ( keys( %{$db->{mapping}} ) ) {
    my $orig = $db->{mapping}->{$file};
    if ( ! -f "files/$file" ) {
	print "mapping table: new driver in db not found: $file\n";
	$not_found = 1;
	next;
    }
    if ( ! -f "$reference/files/$orig" ) {
	print "mapping table: old file ($orig) not found for new: $file\n";
	$not_found = 1;
    }
    my $bn = basename( $file );
    if ( $bn eq 'Kconfig' || $bn eq 'Makefile' ) {
	unless ( diffem( "files/$file", "$reference/files/$orig" ) ) {
	    $maybe->{$file} = $orig;
	}
	next;
    }
    unless ( diffem( "files/$file", "$reference/files/$orig" ) ) {
	$diffs->{$file} = $orig;
    }
}
die if ( $not_found && $die_if_not_match );

#
# Here we know what driver files have changed ($diffs->{ag2} = g2)
#
foreach my $file ( keys( %$diffs ) ) {
    # print "cp -f $reference/files/$diffs->{$file} files/$file\n";
}

#
# $maybe contains all of the Makefile and Kconfig files which
# are mostly guarenteed to be different.  The only way to know
# for sure if we need to address the change is to track original
# versions and see if the new original version is different than
# the old (know) version.  Yuk.
#
print "Checking versions for Kconfig and Makefile changes ...\n";
$not_found = 0;
foreach my $file ( keys( %$maybe ) ) {
    my $version = get_version( "$reference/files/$maybe->{$file}" );
    if ( $version ne $db->{versions}->{$maybe->{$file}} ) {
	print "  kconfig/makefile version check: a newer version is available for $reference/files/$maybe->{$file}\n";
	print "    new version: $version, old version: $db->{versions}->{$maybe->{$file}}\n";
	print "      resolve the difference in files/$file and update the version in the database.\n";
	$not_found = 1;
    }
}
die if ( $not_found && $die_if_not_match );

# Checkpoint.  We have not yet done anything.  We have checked some database
# consistency, and we have identified any driver code that needs to be updated.
#
# Now lets look though the unique files and see if any of that needs to be
# copied/updated, then we'll look at patch files.
#
print "Checking for unique files to update ...\n";
foreach my $file ( @{$db->{unique}} ) {
    unless( diffem( "files/$file", "$reference/files/$file" ) ) {
	$diffs->{$file} = $file;
    }
}

# For patch files, we have nothing to compare with, so we need to do
# a version compare, like we did with kconfig/makefiles.
print "Checking for patch files to regenerate ...\n";
my $regen = {};
foreach my $file ( @all_patch_files ) {
    my $version = get_version( "$reference/files/$file->{file}" );
    if ( $version ne $file->{version} ) {
	$regen->{$file->{pf}} = 1;
    }
}

# This loop will cause ALL patch files to be regenerated.
# This is probably the safest this we could do but its
# very expensive.  You could comment out the following
# loop if you know that the loop above is sufficient.
foreach my $pregen ( keys( %{$db->{patches}} ) ) {
    $regen->{$pregen} = 1;
}

printf "Work that needs to be done:\n";
foreach my $file ( keys( %$diffs ) ) {
    # print "cp -f $reference/files/$diffs->{$file} files/$file\n";
    my $source = "$reference/files/$diffs->{$file}";
    my $target = "files/$file";
    print "Need to update $target (with $source)\n";
}

foreach my $pf ( keys( %$regen ) ) {
    print "Need to regenerate patch file $pf\n";
}

if ( $dont_doit ) {
    exit 0;
}

# Lets doit!
print "Updating files that need updating ...\n";
foreach my $file ( keys( %$diffs ) ) {
    # print "cp -f $reference/files/$diffs->{$file} files/$file\n";
    my $source = "$reference/files/$diffs->{$file}";
    my $target = "files/$file";
    if ( system( "cp -f $source $target" ) ) {
	die "Failed to: cp -f $source $target: $!";
    }
}

my @patch_files = keys( %$regen ); 
if ( $#patch_files == -1 ) {
    if ( -d $new_tree ) {
	system( "rm -rf $new_tree" );
    }
    if ( -d $old_tree ) {
	system( "rm -rf $old_tree" );
    }
    exit 0;
}

# Patching.  This is a time consuming operation!
#
# First we need a reference tree to serve as the
# "old" tree for the diff.  Once created, this
# doesn't need to change.  This needs to be the
# base linux tree, plus all changes applied up to
# when we need to make our changes.
#
# Then for each patch set, we need to copy the
# reference tree to "new", copy the files involved
# in the patch set to "new" and then do the diff.
#
print "Creating $old_tree for patches ...\n";
create_old_tree( $old_tree );

foreach my $pf ( @patch_files ) {
    print "Regenerating $pf ...\n";
    copy_old_to_new( $old_tree, $new_tree );
    my @files = @{$db->{patches}->{$pf}};
    my $p_filename = "patches/${pf}.patch";
    if ( system( "rm -f $p_filename" ) ) {
	die "Failed to: rm -f $p_filename";
    }
    foreach my $file ( @files ) {
	if ( system( "cp -f $reference/files/$file $new_tree/$file" ) ) {
	    die "Failed to: cp -f $reference/files/$file $new_tree/$file: $!";
	}
    }
    system( "diff -Naur $old_tree $new_tree > $p_filename" );
}

# Clean up
#
if ( -d $new_tree ) {
    system( "rm -rf $new_tree" );
}
if ( -d $old_tree ) {
    system( "rm -rf $old_tree" );
}

############################################################################

sub in_list {
    my $file = shift;
    my @list = @_;
    foreach my $lf ( @list ) {
	if ( $lf eq $file ) {
	    return 1;
	}
    }
    return 0;
}

sub find_files_in {
    my $root = shift;
    my @files = ();
    find( sub {
	my $fullname = $File::Find::name;
	my $dirname  = $File::Find::dir;
	my $filename = $_;
	return if ( -d $fullname );
	return if ( $filename eq '.' );
	return if ( $fullname =~ /CVS/ );
	return if ( $filename =~ /^\./ );
	$fullname =~ s/$root\///g;
	push( @files, $fullname );
	  }, $root );
    return @files;
}

sub diffem {
    my( $src_file, $tar_file ) = @_;
    my $src = new FileHandle "<$src_file";
    my $tar = new FileHandle "<$tar_file";

    my $status = 1;

    while( <$src> ) {
        my $sline = $_;
        my $tline = <$tar>;
        unless( $tline ) {
            $status = 0;
            last;
        }

        $sline = fixup( $sline );
        $tline = fixup( $tline );

        unless( $sline eq $tline ) {
            $status = 0;
            last;
        }
    }

    close $src;
    close $tar;

    return $status;
}

sub fixup {
    my $line = shift;
    
    # remove cvs keyword expansion
    #
    $line =~ s/\$\S+:.+\$//g;

    return $line;
}

sub get_version {
    my $path = shift;
    my $dir  = dirname $path;
    my $file = basename $path;
    my $cmd = "( cd $dir; cvs status $file)";
    my $pipe = new FileHandle( "$cmd|" );
    unless( $pipe ) {
	die "Cannot open pipe: $!: $cmd";
    }
    my $version = 'unknown';
    while( <$pipe> ) {
	if ( /Working revision:\s+(\S+)/ ) {
	    $version = $1;
	    last;
	}
    }
    close $pipe;
    return $version;
}

sub create_old_tree {
    my $location = shift;
    return if ( -d $location );
    my $owrt = dirname( dirname( dirname( $top ) ) );
    unless( -f "$owrt/dl/linux-2.6.36.tar.bz2" ) {
	die "$owrt/dl/linux-2.6.36.tar.bz2 is missing.  You need to build first!";
    }
    do_or_die( "tar jxf $owrt/dl/linux-2.6.36.tar.bz2" );
    chdir( $location );
    do_or_die( "patch -p1 < $top/arm-patches/100-arm.patch >/dev/null" );
    do_or_die( "cp -rf $owrt/target/linux/generic/files/* ." );
    # Our patches are applied before openwrt's are, so this line below
    # should not be executed:
    # do_or_die( "$top/android-port/apply.sh $owrt/target/linux/generic/patches-2.6.36 >/dev/null" );

    chdir( $top );
}

sub copy_old_to_new {
    my ( $old_tree, $new_tree ) = @_;
    if ( -d $new_tree ) {
	if ( system( "rm -rf $new_tree" ) ) {
	    die "Failed to: rm -rf $new_tree: $!";
	}
    }
    if ( system( "cp -r $old_tree $new_tree" ) ) {
	die "Failed to: cp -r $old_tree $new_tree: $!";
    }
}

sub do_or_die {
    my $cmd = shift;
    if ( system( $cmd ) ) {
	die "Failed to: $cmd: $!";
    }
}

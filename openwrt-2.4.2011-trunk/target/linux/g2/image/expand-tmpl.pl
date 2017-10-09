# This simple perl script is called to expand
# templates (like uboot-env.tmpl) that contain
# "variables".  
#
# Usage:
#
#  expand-tmpl.pl var1=val1 var2=val2 ... < in.tmpl > out
#
# Within the input template, "$var1" will be replaced with "val1", etc.
#
while( $arg = shift @ARGV ) {
    ($var,$val) = split( /=/, $arg, 2 );
    push( @args, { var => $var, val => $val } );
    eval "\$$var = \"$val\";";
}
while( <> ) {
    $line = $_;
    chomp $line;
    $line =~ s/^\s+//g;
    $line =~ s/\s+$//g;
    next if ( $line =~ /^$/ );
    next if ( $line =~ /^#/ );
    next if ( $line =~ /setenv b bootm/ );  # fix up after
    next if ( $line =~ /setenv rootfs_address/ );  # fix up after
    foreach $arg ( @args ) {
	$line =~ s/\$$arg->{var}/$arg->{val}/g;
    }
    print $line, "\n";
}
if ( !defined( $rootfs_ram_cp ) ||
     $rootfs_ram_cp eq '0x0' ||
     $rootfs_ram_cp eq '0' ||
     $rootfs_ram_cp == 0 ) {
    print "setenv b bootm \${kernel_address}\n";
    print "setenv rootfs_address -\n";
}
else {
    print "setenv b bootm \${kernel_address} \${rootfs_address}\n";
    print "setenv rootfs_address \${rootfs_address}\n";
}

#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
# Create SIS files for Writable SWI Certstore testing.
#

use strict;
my $cert_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates";

# --------------------------------------------------------------------------

sub mtime($) {
    my ($file) = shift;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,
      $size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($file);

    $mtime = 0 if ! defined( $mtime );
    return $mtime;
}

# --------------------------------------------------------------------------

# control whether to replace uids in ini file
#
# Note that this script will update the package uids in
# testwritableswicertstore.ini automiatically if 'uidgen' has been
# specified on the command line.  This need only be done if the uids
# change.
die "Incorrect number of args.\n" if (($#ARGV +1 != 2) and ($#ARGV +1 != 3));
 
my $platform = lc ( $ARGV[0] );
my $variant = lc ( $ARGV[1] );
my $install_drive = lc( $ARGV[2] );

# control whether to replace uids in ini file
my $uidgen = 0;

if (defined($ARGV[2]) and $ARGV[2] eq "uidgen")
{
    $uidgen = 1;
}

print "Build test executables and SIS files to test writable swi certstore\n";

my $arm5path = "\\epoc32\\winscw\\c\\tswi\\tsis\\data\\armv5\\wsc";

if ($platform =~ /armv5/i)
{
    use File::Path;
    mkpath "$arm5path", 1, 0777;
    -d $arm5path or die "Couldn't create $arm5path\n";

    my $testiby = "\\epoc32\\rom\\include\\tswisis.iby";
    print "Creating $testiby\n";
    unlink $testiby;
    open(IBYFILE, ">> $testiby" ) or die "Could not open $testiby\n";
    
    # These exes are used by the testexecute test script
    print IBYFILE "file=ABI_DIR\\BUILD_DIR\\removetest_setup.exe \\sys\\bin\\removetest_setup.exe\n";
    print IBYFILE "file=ABI_DIR\\BUILD_DIR\\clean_certstore.exe	\\sys\\bin\\clean_certstore.exe\n";

    print IBYFILE "data = \\epoc32\\winscw\\c\\tswi\\tuiscriptadaptors\\scripts\\testwritableswicertstore.script \\tswi\\tuiscriptadaptors\\scripts\\testwritableswicertstore.script\n";
    print IBYFILE "data = \\epoc32\\winscw\\c\\tswi\\tuiscriptadaptors\\scripts\\testwritableswicertstore.ini \\tswi\\tuiscriptadaptors\\scripts\\testwritableswicertstore.ini\n";
}

sub process_ini(%)
{
    return if not $uidgen;
    my $args = $_[0];

    my $name = get($args, "name");
    my $pkguid = get($args, "pkguid");

    my $ini = "..\\tuiscriptadaptors\\scripts\\testwritableswicertstore.ini";
    -e $ini or die "$ini does not exist\n";

    open (PKGREAD, "<$ini") or die "Couldn't open $ini for reading\n";
    my @filecontents = <PKGREAD>;
    close PKGREAD;

    die if (chmod(0777, $ini) != 1);

    $pkguid =~ m/0x([0-9a-fA-F]+)/;
    $pkguid = $1;

    open (PKGWRITE, ">$ini") or die "Couldn't open $ini for writing\n";
    foreach my $line (@filecontents)
    {
	$line =~ s/uid=([0-9a-fA-F]+)[ \t]*;$name.sis/uid=$pkguid ;$name.sis/i;
	
	print PKGWRITE $line;
    }

    close PKGWRITE;
}

sub processsis($)
{
    my $sis = $_[0];
    -e $sis or die "'$sis' does not exist\n";

    $sis =~ m|(.*)[\\/]+(.*)$|;
    my $sisdir = $1;
    my $sisfile = $2;

    use File::Copy;
    if ($platform =~/winscw/i)
    {
	my $target = "\\epoc32\\winscw\\c\\tswi\\tsis\\data\\wsc";
	-d $target or mkpath $target or die "Couldn't create $target\n";

	print "Copying $sis to $target\n";
	
	copy($sis, $target) or die "Couldn't copy $sis to $target\n";
    }
    elsif ($platform =~/armv5/i)
    {
	print "Copying $sis to $arm5path\n";

	copy($sis, "$arm5path\\$sisfile") 
	    or die "Couldn't copy $sis to $arm5path\\$sisfile\n";

	print IBYFILE "data = ZDRIVE\\tswi\\tsis\\data\\armv5\\wsc\\$sisfile \\tswi\\tsis\\data\\wsc\\$sisfile\n"; 
    }
    else
    {
	die "Unkown platform '$platform'\n";
    }
}

sub get(%$;$)
{
    my $hash = $_[0];
    my $what = $_[1];

    die "hash key undefined\n" if not defined $what;

    if (not defined($hash->{$what}))
    {
	return $_[2] if defined $_[2];
	die "'$what' is undefined\n";
    }
    
    return $hash->{$what};
}

my $romtcb_certs = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis\\SymbianTestRootTCBCARSA";
my $romtcb = "..\\tsisfile\\data\\signedsis\\SymbianTestRootTCBCARSA";

# Number Allocation: 0x80172f9f To 0x80172fb2 Inclusive.
# All allocated to certstore updaters 

my @targets = (
	       # deliver corrupt certstore and verify that further
	       # updates can be made
	       {
		   "name" => "corruptsc",
		   "uid" => "0x80172f9f",
		   "pkguid" => "0x80172fa0",
		   "certstore" => "data/junk",
	       },
	       # deliver empty certstore and verify that further
	       # updates can be made
	       {
		   "name" => "emptysc", 
		   "uid" => "0x80172fa1",
		   "pkguid" => "0x80172fa2",
		   "certstore" => "certs/empty_swicertstore.dat",
	       },
	       # verify that we can deliver an update following
	       # either the corrupt or empty update
	       {
		   "name" => "updatesc",
		   "uid" => "0x80172fa3",
		   "pkguid" => "0x80172fa4",
		   "certstore" => "certs/capabilities/swicertstore_update.dat",
	       },
	       # Deliver a certstore that contains duplicate
	       # certificates (use the rom certstore)
	       {
		   "name" => "duplicatesc",
		   "uid" => "0x80172fa5",
		   "pkguid" => "0x80172fa6",
		   "certstore" => "$romtcb/../swicertstore.dat",
	       },
	       # Deliver a certstore that contains a mandatory
	       # certificate
	       {
		   "name" => "mandatorysc",
		   "uid" => "0x80172fa7",
		   "pkguid" => "0x80172fa8",
		   "certstore" => "certs/mandatory/swicertstore_update.dat",
	       },
	       # Deliver a certstore that contains an expired version
	       # of a certificate in the rom certstore
	       {
		   "name" => "expiredsc",
		   "uid" => "0x80172fa9",
		   "pkguid" => "0x80172faa",
		   "certstore" => "certs/expired/swicertstore_update.dat",
	       },
	       # Deliver a certstore that contains a renewed version
	       # of a certificate in the rom certstore
	       {
		   "name" => "renewedsc",
		   "uid" => "0x80172fab",
		   "pkguid" => "0x80172fac",
		   "certstore" => "certs/renewed/swicertstore_update.dat",
	       },
	       # Deliver a certstore that contains the root of a
	       # certificate chain.
	       { "name" => "chainsc", 
		 "uid" => "0x80172fad", 
		 "pkguid" => "0x80172fae", 
		 "certstore" => "certs/chain/swicertstore_update.dat"
	       }, 
	       );

# Create empty certstore, and change last modified time to the epoch as
# we don't want the re-build of exe to be dependent on this empty file
open(FILE, ">certs/empty_swicertstore.dat");
close FILE;
utime 0, 0, "certs/empty_swicertstore.dat";

foreach my $target (@targets)
{
    # Generate sis files for writable swi certstore update tests

    my $name = get($target, "name");
    my $uid = get($target, "uid");
    my $pkguid = get($target, "pkguid");
    my $certstore = get($target, "certstore");

    my $cert = get($target, "cert", "$romtcb_certs\\cacert.pem");
    -e $cert or die "$cert does not exist\n";
    
    my $key = get($target, "key", "$romtcb_certs\\cakey.pem");
    -e $key or die "$key does not exist\n";

    my $sisfile = "build-$platform-$variant/$name/$name.sis";
    my $swicertstoretobin = "../../source/certstoretobin/SwiCertStoretobin.pl";
    mkdir( "build-$platform-$variant", 0777 );

    # Only remake executables if they require rebuilding
    if (   mtime($sisfile) < mtime($key)
        || mtime($sisfile) < mtime($cert)
        || mtime($sisfile) < mtime($certstore)
        || mtime($sisfile) < mtime($swicertstoretobin))
    {
        print "Attempting to generate SIS files for $name\n";

        my @command = ("perl $swicertstoretobin",
		   "-v", "-p$platform", "-t$variant", "-cgm", "-u$uid", 
		   "-s$pkguid", "-i$certstore", "-o$name", "-C$cert", "-K$key",
		   "-lavendor", "-navendor", "-r${install_drive}");
		
        my $cmd = join(' ', @command);
		print "$cmd\n";
        open ( RUNSWICSTOBIN, "$cmd |") || die "Failed whilst creating SIS file for '$name' test: $!\n";
        while (<RUNSWICSTOBIN> ) {
           print;
        }
        close RUNSWICSTOBIN;
        ($? >> 8) == 0 or die "Failed whilst creating SIS file for '$name' test\n";

        # Now rename this built directory from "gen" into build-<plat>-<opt>
        # since executables are built into "gen" regardless of platform
        if ( -d "build-$platform-$variant\\$name" ) {
           system("rmdir /s /q build-$platform-$variant\\$name");
        }
        system("move gen\\$name build-$platform-$variant");

    } else {
        print "$sisfile up-to-date\n";
    }

    processsis($sisfile);
    process_ini($target);
}

my $romexpired = "$cert_path\\swi\\test\\tsisfile\\data\\signedsis\\SymbianTestRootExpiredCARSA";
my @packages = (
		# Number Allocation 0x80172A37 To 0x80172A4A Inclusive.
 		{
 		    "name" => "capabilities",
		    "pkguid" => "0x80172A37",
 		    "cert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cacert.der",
 		    "key" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cakey.pem",
 	        },
 		{
 		    "name" => "doublesigned",
		    "pkguid" => "0x80172A38",
 		    "cert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cacert.der",
 		    "key" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\capabilities\\cakey.pem",
 		    "cert2" => "$romtcb_certs\\cacert.pem",
 		    "key2" =>  "$romtcb_certs\\cakey.pem"
 		},
 		{
 		    "name" => "mandatory",
		    "pkguid" => "0x80172A39",
 		    "cert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\mandatory\\cacert.der",
 		    "key" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\mandatory\\cakey.pem"
		},
		{"name" => "overwrite0",
		 "pkguid" => "0x80172A3A"},
		{"name" => "overwrite1",
		 "pkguid" => "0x80172A3B"},
		{"name" => "overwrite2",
		 "pkguid" => "0x80172A3C"},
		{"name" => "remove0",
		 "pkguid" => "0x80172A3D"},
		{"name" => "remove1",
		 "pkguid" => "0x80172A3E"},
		{"name" => "remove2",
		 "pkguid" => "0x80172A3F"},
		{"name" => "remove3",
		 "pkguid" => "0x80172A40"},
		{"name" => "remove4",
		 "pkguid" => "0x80172A41"},
		{"name" => "remove5",
		 "pkguid" => "0x80172A42"},
		{"name" => "remove6",
		 "pkguid" => "0x80172A43"},
 		{
 		    "name" => "renewed",
		    "pkguid" => "0x80172A44",
 		    "cert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\renewed\\cacert.der",
 		    "key" => "$romexpired\\cakey.pem"
		},
		{
 		    "name" => "chain",
		    "pkguid" => "0x80172A45",
 		    "cert" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\chain\\intermediate\\cacert.der",
 		    "key" => "$cert_path\\swi\\test\\writableswicertstore\\certs\\chain\\intermediate\\cakey.pem"
		},
		
	    );

foreach my $target (@packages) 
{
    my $file = get($target, "name");
    my $pkguid = get($target, "pkguid");
    my $sisdir = "data";

    my $pkgdir = get($target, "pkgdir", "packages");
    -d $pkgdir or die "$pkgdir does not exist\n";
    
    my $cert = get($target, "cert", "$romtcb_certs\\cacert.pem");
    -e $cert or die "$cert does not exist\n";

    my $key = get($target, "key", "$romtcb_certs\\cakey.pem");
    -e $key or die "$key does not exist\n";

    my $pkg = "$pkgdir\\${file}.pkg";
    -e $pkg or die "$pkg does not exist\n";
    
    print "Munging platform and variant in package file\n";
    
    die "chmod failed\n" if (chmod(0777, $pkg) != 1);
    
    open (PKGREAD, "<$pkg") or die "Couldn't open $pkg for reading\n";
    my @filecontents = <PKGREAD>;
    close PKGREAD;
    
    open (PKGWRITE, ">$pkg") or die "Couldn't open $pkg for writing\n";
    foreach my $line (@filecontents)
    {
	$platform =~ /armv5/i and $line =~ s/winscw/$platform/ig;	
	$platform =~ /winscw/i and $line =~ s/armv5/$platform/ig;
	$variant =~ /urel/i and $line =~ s/udeb/$variant/ig;
	$variant =~ /udeb/i and $line =~ s/urel/$variant/ig;
	$line =~ s/\(0x[0-9a-f]+\),/\($pkguid\),/i;

	# print $line;
	print PKGWRITE $line;
    }
    print "\n";
    close PKGWRITE;

    my $unsignedsis = "$sisdir\\${file}-unsigned.sis";
    my $sis = "$sisdir\\${file}.sis";

    my @command = ("MakeSis $pkg $unsignedsis");

    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating SIS files for $file test\n";

    print "Signing $unsignedsis with $cert and $key\n";

    @command = ("signsis", "-sv", $unsignedsis, $sis, $cert, $key);
    
    system(@command);
    ($? >> 8) == 0 or die "Failed whilst creating SIS files for $file test\n";

    if (exists($target->{"cert2"}))
    {
	my $cert = get($target, "cert2");
	-e $cert or die "$cert does not exist\n";

        my $key = get($target, "key2");
	-e $key or die "$key does not exist\n";

	print "Signing $sis with $cert and $key\n";
	
	@command = ("signsis", "-s", $sis, 
		    "$sisdir\\${file}-doublesigned.sis", $cert, $key);
    
	system(@command);
	($? >> 8) == 0 or 
	    die "Failed whilst creating SIS files for $file test\n";

	copy("$sisdir\\${file}-doublesigned.sis", $sis);
    }
    
    processsis("$sisdir/${file}.sis");
    process_ini($target)
}

close IBYFILE;


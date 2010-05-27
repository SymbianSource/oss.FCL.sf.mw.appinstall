#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

use Cwd;
use strict;
use File::Basename;
use File::Path;
use File::Copy;     # for future portability
use Getopt::Std;

# Note that although Win32 internally will quite happily accept
# forward slashes as directory separators, both cmd.exe and
# makesis.exe/signsis.exe interpret these as option separators,
# so use backslashes when calling these programs.

die "EPOCROOT not defined" if !defined ($ENV{EPOCROOT});

my $makesis = "$ENV{EPOCROOT}epoc32\\tools\\makesis.exe";
my $signsis = "$ENV{EPOCROOT}epoc32\\tools\\signsis.exe";
my $makekeys = "$ENV{EPOCROOT}epoc32\\tools\\makekeys.exe";
my $createsis = "$ENV{EPOCROOT}epoc32\\tools\\createsis.bat";

my $cert_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates";
if ( ! -x $makesis || ! -x $signsis || ! -x $makekeys ) {
   die "$makesis, $makekeys and/or $signsis are not executable";
}

my %opt;
getopts( "vch:", \%opt ) or usage();

my $verbose = 0;
my $verbose = 1 if $opt{v};

my $platform = lc($ARGV[0]);
my $configuration = lc($ARGV[1]);

usage() if $opt{h};
clean() if $opt{c};
usage() if !defined($ARGV[0]) || !defined($ARGV[1]);

# --------------------------------------------------------------------------

sub usage() {
    print STDERR << "EOF";
    usage: $0 [-hcv] <platform> <udeb|urel>
     -h        : this (help) message
     -v        : verbose output
     -c        : clean up signed .sis files
EOF
   exit;
}

# --------------------------------------------------------------------------

sub clean() {
	my $cleandir1 = "build-signed-$platform-$configuration";
	my $cleandir2 = "build-unsigned-$platform-$configuration";
	my $cleandir3 = "build-selfsigned-$platform-$configuration";
	rmtree($cleandir1, $verbose);
	rmtree($cleandir2, $verbose);
	rmtree($cleandir3, $verbose);
	exit;
}

# --------------------------------------------------------------------------

sub System($) {
   my $cmd = shift;
   print "$cmd\n" if $verbose;
   system($cmd);
}

# --------------------------------------------------------------------------

sub mtime($) {
    my ($file) = shift;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,
      $size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($file);

    $mtime = 0 if ! defined( $mtime );
    return $mtime;
}

# --------------------------------------------------------------------------

sub EditFile ($$)
{
	my ($file, $coderef) = @_;
	local $_ = ReadFile($file);
	&$coderef();
	WriteFile($file, $_);
}

# Read the contents of a file into a string and return it
sub ReadFile ($)
{
	my ($file) = @_;
	open FILE, "<$file" or die "Can't read file: $file";
	local $/ = undef;
	my $data = <FILE>;
	close FILE;
	return $data;
}

# Replace a file with a string
sub WriteFile ($$)
{
	my ($file, $data) = @_;

	# Ensure directory exists
	mkpath(dirname($file), $verbose);

	system("attrib -r $file");
	open FILE, ">$file" or die "Can't write file: $file";
	print FILE $data;
	close FILE;
}

# create unsigned SIS files

sub MakeSisFiles($$$$) {

	my ($platform, $configuration, $builddir, $enddir) = @_;

	my $targetdir = "build-$builddir-$platform-$configuration";
	mkpath($targetdir, $verbose);
	
	
	# Now build each file
	opendir DIR, $builddir;
	my @pkgfiles = grep(/\.pkg/, readdir(DIR));
	closedir DIR;

	foreach my $pkgfile (@pkgfiles) {
		if (mtime("$targetdir/$pkgfile") < mtime("$builddir/$pkgfile")) {
			# Copy the package files to the build dir for processing
			copy("$builddir/$pkgfile", "$targetdir");

			EditFile("$targetdir/$pkgfile",
				  sub { s/<PLATFORM>/$platform/gm;
					s/<CONFIGURATION>/$configuration/gm; });
		}

		$pkgfile =~ s/\.pkg//;
		if (   mtime("$targetdir/$pkgfile.sis") < mtime("$targetdir/$pkgfile.pkg")
		    || mtime("$targetdir/$pkgfile.sis") < mtime($makesis)) {
			System("$makesis $targetdir\\$pkgfile.pkg $targetdir\\$pkgfile.sis");
			copy("$targetdir/$pkgfile.sis", "$enddir/$pkgfile.sis");
		} else {
			print "$enddir/$pkgfile.sis is up-to-date\n";
		}
	}
}

# Create self signed SIS files

sub CreateSisFiles($$$$) {

	my ($platform, $configuration, $builddir, $enddir) = @_;

	my $targetdir = "build-$builddir-$platform-$configuration";
	mkpath($targetdir, $verbose);
	
	
	# Now build each file
	opendir DIR, $builddir;
	my @pkgfiles = grep(/\.pkg/, readdir(DIR));
	closedir DIR;

	foreach my $pkgfile (@pkgfiles) {
		if (mtime("$targetdir/$pkgfile") < mtime("$builddir/$pkgfile")) {
			# Copy the package files to the build dir for processing
			copy("$builddir/$pkgfile", $targetdir);

			EditFile("$targetdir/$pkgfile",
				  sub { s/<PLATFORM>/$platform/gm;
					s/<CONFIGURATION>/$configuration/gm; });
		}

		$pkgfile =~ s/\.pkg//;
		if (   mtime("$enddir/$pkgfile.sis") < mtime("entropy/$pkgfile.entropy")
		    || mtime("$enddir/$pkgfile.sis") < mtime("$targetdir/$pkgfile.pkg")
		    || mtime("$enddir/$pkgfile.sis") < mtime($createsis))
		{
			System("$createsis create -pass foobar $targetdir\\$pkgfile.pkg < entropy\\$pkgfile.entropy");
			copy("$targetdir/$pkgfile.sis","$enddir/$pkgfile.sis");
		} else {
			print "$enddir/$pkgfile.sis is up-to-date\n";
		}
		
		unlink("key-gen.key");
		unlink("cert-gen.cer");
	}
}

sub CreateAndSignSisFiles($$$$) {

	my ($platform, $configuration, $builddir, $enddir) = @_;
	
	# This method is more complex than the others, we
	# need signatures to match test cases.
	
	# First create the base SIS files
	
	MakeSisFiles($platform, $configuration, $builddir, $enddir);
	
	# Now sign them in various configurations
	
	my $testDN = "CN=Symbian Test Certificate OU=Symbian";
	my $targetdir = "build-$builddir-$platform-$configuration";
	mkpath($targetdir, $verbose);
	
	
	# Test case 3.1
	my $final = "$enddir/3-1signedusergrantable.sis";
	if (    mtime($final) < mtime("$targetdir/3-1signedexeusercaps.sis")
	     || mtime($final) < mtime("entropy/testcase3.1.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/Root5RSA.cer")
	     || mtime($final) < mtime("data/Root5RSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.1.entropy");
		System("$signsis -s $targetdir\\3-1signedexeusercaps.sis $targetdir\\tmp1.sis cer.cer key.key foobar");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis $cert_path\\swi\\test\\tautosigning\\data\\Root5RSA.cer data\\Root5RSA.key");
	
		copy("$targetdir/tmp2.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
	
	# Test case 3.2
	my $final = "$enddir/3-2signeddrmfirst.sis";
	if (    mtime($final) < mtime("$targetdir/3-2signedexedrmcap.sis")
	     || mtime($final) < mtime("entropy/testcase3.2.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/Root5RSA.cer")
	     || mtime($final) < mtime("data/Root5RSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.2.entropy");
		System("$signsis -s $targetdir\\3-2signedexedrmcap.sis $targetdir\\tmp1.sis cer.cer key.key foobar");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis $cert_path\\swi\\test\\tautosigning\\data\\Root5RSA.cer data\\Root5RSA.key");
	
		copy("$targetdir/tmp2.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
	
	# Test case 3.3
	my $final = "$enddir/3-3signeddrmlast.sis";
	if (    mtime($final) < mtime("$targetdir/3-2signedexedrmcap.sis")
	     || mtime($final) < mtime("entropy/testcase3.3.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/Root5RSA.cer")
	     || mtime($final) < mtime("data/Root5RSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.3.entropy");
		System("$signsis -s $targetdir\\3-2signedexedrmcap.sis $targetdir\\tmp1.sis $cert_path\\swi\\test\\tautosigning\\data\\Root5RSA.cer data\\Root5RSA.key");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis cer.cer key.key foobar");
	
		copy("$targetdir/tmp2.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
	
	# Test case 3.4
	my $final = "$enddir/3-4signeduserauthorised.sis";
	if (    mtime($final) < mtime("$targetdir/3-1signedexeusercaps.sis")
	     || mtime($final) < mtime("entropy/testcase3.4.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/TestCARSA.cer")
	     || mtime($final) < mtime("data/TestCARSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.4.entropy");
		System("$signsis -s $targetdir\\3-1signedexeusercaps.sis $targetdir\\tmp1.sis $cert_path\\swi\\test\\tautosigning\\data\\TestCARSA.cer data\\TestCARSA.key");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis cer.cer key.key foobar");
	
		copy("$targetdir/tmp2.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
	
	# Test case 3.5
	my $final = "$enddir/3-5signedallauthorised.sis";
	if (    mtime($final) < mtime("$targetdir/3-3signedexeallcaps.sis")
	     || mtime($final) < mtime("entropy/testcase3.5.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/TestCARSA.cer")
	     || mtime($final) < mtime("data/TestCARSA.key")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/Root5RSA.cer")
	     || mtime($final) < mtime("data/Root5RSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.5.entropy");
		System("$signsis -s $targetdir\\3-3signedexeallcaps.sis $targetdir\\tmp1.sis $cert_path\\swi\\test\\tautosigning\\data\\TestCARSA.cer data\\TestCARSA.key");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis $cert_path\\swi\\test\\tautosigning\\data\\Root5RSA.cer data\\Root5RSA.key");
		System("$signsis -s $targetdir\\tmp2.sis $targetdir\\tmp3.sis cer.cer key.key foobar");
	
		copy("$targetdir/tmp3.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("$targetdir/tmp3.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
	
	# Test case 3.6
	my $final = "$enddir/3-6signeduserunauthorised.sis";
	if (    mtime($final) < mtime("$targetdir/3-1signedexeusercaps.sis")
	     || mtime($final) < mtime("entropy/testcase3.6.entropy")
	     || mtime($final) < mtime("$cert_path/swi/test/tautosigning/data/Root5RSA.cer")
	     || mtime($final) < mtime("data/Root5RSA.key")
	     || mtime($final) < mtime($makekeys)
	     || mtime($final) < mtime($signsis))
	{
		System("$makekeys -cert -password foobar -dname \"$testDN\" key.key cer.cer < entropy\\testcase3.6.entropy");
		System("$signsis -s $targetdir\\3-1signedexeusercaps.sis $targetdir\\tmp1.sis $cert_path\\swi\\test\\tautosigning\\data\\Root5RSA.cer data\\Root5RSA.key");
		System("$signsis -s $targetdir\\tmp1.sis $targetdir\\tmp2.sis cer.cer key.key foobar");
	
		copy("$targetdir/tmp2.sis", $final);
		unlink("$targetdir/tmp1.sis");
		unlink("$targetdir/tmp2.sis");
		unlink("cer.cer");
		unlink("key.key");
	} else {
		print "$final up-to-date\n";
	}
}

my $targetdir = "\\epoc32\\winscw\\c\\tswi\\tautosigning";

if ($platform =~ /armv5/i) {
	$targetdir = "$targetdir\\armv5";
} else {
	$targetdir = "$targetdir\\data";
}

mkpath($targetdir, $verbose);

# Unsigned cases

my $builddir = "unsigned";
MakeSisFiles($platform, $configuration, $builddir, $targetdir);

# Self-Signed test cases
$builddir = "selfsigned";
CreateSisFiles($platform, $configuration, $builddir, $targetdir);

# Signed and Self-Signed test cases
$builddir = "signed";
CreateAndSignSisFiles($platform, $configuration, $builddir, $targetdir);


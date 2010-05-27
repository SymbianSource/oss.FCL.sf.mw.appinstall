#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use Getopt::Std;
use File::Basename;
use File::Copy;     # for future portability

# Note that although Win32 internally will quite happily accept
# forward slashes as directory separators, both cmd.exe and
# makesis.exe/signsis.exe interpret these as option separators,
# so use backslashes when calling these programs.

die "EPOCROOT not defined" if !defined ($ENV{EPOCROOT});
my $makesis = "$ENV{EPOCROOT}epoc32\\tools\\makesis.exe";
my $signsis = "$ENV{EPOCROOT}epoc32\\tools\\signsis.exe";
if ( ! -x $makesis || ! -x $signsis ) {
   die "$makesis and $signsis are not executable";
}

my %opt;
getopts( "vch:", \%opt ) or usage();

my $platform = lc($ARGV[0]);
my $configuration = lc($ARGV[1]);
my $packagedir = "$ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\pkg";
my $builddir = "$ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\sis_${platform}_${configuration}";
my $log = "$ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\logfile.txt";

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
   foreach my $file (getFiles($builddir, "\.pkg\$")) { unlink("$builddir/$file"); }
   foreach my $file (getFiles($builddir, "\.sis\$")) { unlink("$builddir/$file"); }
   exit;
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

# Return an array of files matching a regexp in a directory
sub getFiles($$) {
    my $dir = shift;
    my $regfiles = shift;
    my @files;

    if ( opendir DIR, $dir ) {
       @files = grep (/$regfiles/, readdir(DIR));
       closedir DIR;
    }
    return @files;
}

# --------------------------------------------------------------------------

# Edit a file
sub EditFile ($$) {
	my ($file, $coderef) = @_;
	local $_ = ReadFile($file);
	&$coderef();
	WriteFile($file, $_);
}

# --------------------------------------------------------------------------

# Read the contents of a file into a string and return it
sub ReadFile ($) {
	my ($file) = @_;
	open FILE, "<$file" or die "Can't read from $file: $!";
	local $/ = undef;
	my $data = <FILE>;
	close FILE;
	return $data;
}

# --------------------------------------------------------------------------

# Replace a file with a string
sub WriteFile ($$) {
	my ($file, $data) = @_;

	# Ensure directory exists
	if ($file =~ basename($1) && ! -e $1) {
		mkdir($1);
	}

	system("attrib -r $file");
	open FILE, ">$file" or die "Can't write to $file: $!";
	print FILE $data;
	close FILE;
}

# --------------------------------------------------------------------------

print "\nBuilding test executables for $platform $configuration configuration\n\n";

if ($platform =~ /armv5/i)
{
	mkdir("/epoc32/winscw/c/tswi/tinterpretsis/data/armv5/");
}

mkdir($builddir);


print "Processing files in directory: $builddir\n\n";

foreach my $file ( getFiles( $packagedir, "\.pkg\$" )) {
	
	# Copy package files to builddir if not present or package more recent.
	# Some package files are re-written with platform, config and builddir.
	#
	if ( mtime("$packagedir/$file") > mtime("$builddir/$file") )
	{
		print "Copying $packagedir/$file to $builddir/$file and re-writing\n" if $opt{v};
		copy("$packagedir/$file", "$builddir/$file");

		# replace <PLATFORM> with the platform and <CFG> with the configuration
		EditFile("$builddir/$file" , sub { s/<PLATFORM>/$platform/gm;
						   s/<CONFIGURATION>/$configuration/gm;
						   s/<BUILD>/$builddir/gm; });
	}
	# Run makesis/signsis if needed on package files
	#
	$file =~ s/\.pkg//;
	if (    mtime("$builddir/$file.sis") < mtime("$builddir/$file.pkg")
	     || mtime("$builddir/$file.sis") < mtime("$ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.cer")
	     || mtime("$builddir/$file.sis") < mtime("$ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.key")
	     || mtime("$builddir/$file.sis") < mtime($makesis)
	     || mtime("$builddir/$file.sis") < mtime($signsis))
	{
		# build sis file
		system ("$makesis $builddir\\$file.pkg $builddir\\$file-tmp.sis");
		print ("$makesis $builddir\\$file.pkg $builddir\\$file-tmp.sis\n") if $opt{v};
		system("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.cer $ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.key");
		print ("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.cer $ENV{EPOCROOT}epoc32\\winscw\\c\\tswi\\tinterpretsis\\certs\\default.key\n") if $opt{v};
		print "\n" if $opt{v};
	} else {
		print ("$builddir/$file.sis is up-to-date\n");
	}

	# If the package file contains the string "CREATE_UNSIGNED_SIS"
	# copy the unsigned sis file too. Note that these SIS files are
	# not currently added to the ROM, since we can't change the
	# swipolicy file on the fly on the ROM.

	if (grep(/CREATE_UNSIGNED_SIS/, ReadFile("$builddir/$file.pkg")))
	{
		rename("$builddir/$file-tmp.sis", "$builddir/$file-unsigned.sis");
	} else {
		unlink("$builddir/$file-tmp.sis");
	}

	
}


# Keep original method names for ease of merging/compatibility
sub SignFile($$$$)  { SignFileLen(1, @_); }
sub SignFile2($$$$) { SignFileLen(2, @_); }

sub SignFileLen($$$$$)
{
	my ($len, $infile, $outfile, $signwith, $iby) = @_;
	my $certpath="$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis";
	my $keypath = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\swi\\test\\tsisfile\\data\\signedsis";
	my $sispath=$builddir;


	my $type="rsa";
	if ($signwith =~ /DSA/i) {
		$type="dsa";
	}

	my $pemfile = "$certpath\\$signwith\\cert_chain_$type\_len$len\\chain_$type\_len$len.cert.pem ";
	my $keyfile = "$keypath\\$signwith\\cert_chain_$type\_len$len\\" . $type .  "$len.key";

	# If infile, keyfile, pemfile or signsis.exe is more recent than
	# outfile, or outfile is non-existent then sign the sis file.
	#
	if (   mtime("$sispath/$outfile") < mtime("$sispath/$infile")
	    || mtime("$sispath/$outfile") < mtime("$signsis")
	    || mtime("$sispath/$outfile") < mtime("$pemfile")
	    || mtime("$sispath/$outfile") < mtime("$keyfile")) {
		printf("\t$infile with $signwith to $outfile\n");
		my $cmd = "$signsis -s $sispath\\$infile $sispath\\$outfile $pemfile $keyfile";
		system($cmd);
		print "$cmd\n\n" if $opt{v};
	} else {
		print("$sispath/$outfile is up-to-date\n");
	}
}


sub SignFileWithSpecifiedCert($$$$$)
{

	my ($infile, $outfile, $signcert, $signkey, $iby) = @_;
	my $certpath="$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis";
 	my $keypath = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\swi\\test\\tsisfile\\data\\signedsis";

	if ($signkey =~ /\.pem$/)
	{
		$keypath = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis";
	}
	 
 	my $sispath=$builddir;

	if (   mtime("$sispath/$outfile") < mtime("$sispath/$infile")
	    || mtime("$sispath/$outfile") < mtime("$signsis")
	    || mtime("$sispath/$outfile") < mtime("$keypath/$signkey")
	    || mtime("$sispath/$outfile") < mtime("$certpath/$signcert")) {
		printf("\t$infile with $signcert and $signkey to $outfile\n");
		my $cmd = "$signsis -s $sispath\\$infile $sispath\\$outfile $certpath\\$signcert $keypath\\$signkey";
		system($cmd);
		print "$cmd\n\n" if $opt{v};
	} else {
		print("$sispath/$outfile is up-to-date\n");
	}
}


#
# Sign the files as necessary for the following tests
#

printf("Signing files...\n");
SignFile("exe.sis", "exe_s.sis", "Root5CA",\*iby);
SignFile("dll_exe_dup_sid.sis", "dll_exe_dup_sid_s.sis", "Root5CA",\*iby);
SignFile("dll_dll_dup_sid.sis", "dll_dll_dup_sid_s.sis", "Root5CA",\*iby);
SignFile("eclipse.sis", "eclipse_s.sis", "Root5CA",\*iby);
SignFile("partial.sis", "partial_s.sis", "Root5CA",\*iby);
SignFile("patch.sis", "patch_s.sis", "Root5CA",\*iby);
SignFile("overwrite.sis", "overwrite_s.sis", "Root5CA",\*iby);
SignFile("private_fail.sis", "private_fail_s.sis", "Root5CA",\*iby);
SignFile("duplicate_sid.sis", "duplicate_sid_s.sis", "Root5CA",\*iby);
SignFile("depend1.sis", "depend1_s.sis", "Root5CA",\*iby);
SignFile("depend2.sis", "depend2_s.sis", "Root5CA",\*iby);
SignFile("depend3.sis", "depend3_s.sis", "Root5CA",\*iby);
SignFile("depend4.sis", "depend4_s.sis", "Root5CA",\*iby);
SignFile("depend5.sis", "depend5_s.sis", "Root5CA",\*iby);
SignFile("depend6.sis", "depend6_s.sis", "Root5CA",\*iby);
SignFile("functions_base.sis", "functions_base_s.sis", "Root5CA",\*iby);
SignFile("functions.sis", "functions_s.sis", "Root5CA",\*iby);
SignFile("stub_file_depend.sis", "stub_file_depend_s.sis", "Root5CA",\*iby);
SignFile("fn_flag.sis", "fn_flag_s.sis", "Root5CA",\*iby);
SignFile("testconfig.sis", "testconfig_s.sis", "Root5CA",\*iby);

# DEF070572 - SIS files signed by multiple chains to test OCSP result dialog
#SignFileWithSpecifiedCert("tswinocapability_Root5.sis", "testocspresultdialog_a.sis", "SymbianTestRootExpiredCARSA\\cacert.pem", "SymbianTestRootExpiredCARSA\\cakey.pem", \*iby);
#SignFileWithSpecifiedCert("testocspresultdialog_a.sis", "testocspresultdialog_b.sis", "..\\..\\..\\tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.cert.pem", "..\\..\\..\\tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.key.pem", \*iby);

printf("\n");


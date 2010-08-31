#
# Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

my $cert_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates";
my %opt;
getopts( "vch:", \%opt ) or usage();

my $platform = lc($ARGV[0]);
my $configuration = lc($ARGV[1]);
my $packagedir = "packages";
my $non_iby_packagedir = "non_iby_packages";
my $packages_interpretsis_testcase = "packages_interpretsis_testcase";
my $builddir = "buildsis_${platform}_${configuration}";

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
	if ($1 && $file =~ basename($1) && ! -e $1) {
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
	mkdir("/epoc32/winscw/c/tswi/tsis/data/armv5/");
	my $testexeiby = "/epoc32/rom/include/tswiexes.iby";
	unlink $testexeiby;
	open(iby, ">> $testexeiby" ) || die "Could not open $testexeiby: $!";
}

mkdir($builddir);


print "Processing files in directory: $builddir\n\n";
MakeSISFiles($packagedir,"yes");
MakeSISFiles($non_iby_packagedir,"no");
MakeSISFiles($packages_interpretsis_testcase,"no");


# Copy emulator version of preinstalled exes to location script expects.
# They will be moved by the test script to the e: drive.
if ($platform =~/winscw/i)
	{
	mkdir("/epoc32/winscw/c/tswi/tuiscriptadaptors/data");
	copy("/epoc32/release/$platform/$configuration/console_app_longrun_forpackage.exe",
	     "/epoc32/winscw/c/tswi/tuiscriptadaptors/data/preinstalled_console_app.exe");
	copy("/epoc32/release/$platform/$configuration/runtestexe2_forpackage.exe",
	     "/epoc32/winscw/c/tswi/tuiscriptadaptors/data/preinstalled_runtestexe2.exe");
	copy("/epoc32/release/$platform/$configuration/runwait1.exe",
	     "/epoc32/winscw/c/tswi/tuiscriptadaptors/data/runwait1.exe");
	copy("/epoc32/release/$platform/$configuration/runwait2.exe",
	     "/epoc32/winscw/c/tswi/tuiscriptadaptors/data/runwait2.exe");
	copy("/epoc32/release/$platform/$configuration/runwait3.exe",
	     "/epoc32/winscw/c/tswi/tuiscriptadaptors/data/runwait3.exe");
	}

AddCustomSisFiles(\*iby, $platform);


sub MakeSISFiles($$)
  {
    my ($dir, $add_to_iby) = @_;
    foreach my $file ( getFiles( $dir, "\.pkg\$" ))
      {
	# Copy package files to builddir if not present or package more recent.
	# Some package files are re-written with platform, config and builddir.
	#
	if ( mtime("$dir/$file") > mtime("$builddir/$file") )
	{
		print "Copying $dir/$file to $builddir/$file and re-writing\n" if $opt{v};
		copy("$dir/$file", "$builddir/$file");

		# replace <PLATFORM> with the platform and <CFG> with the configuration
		EditFile("$builddir/$file" , sub { s/<PLATFORM>/$platform/gm;
						   s/<CONFIGURATION>/$configuration/gm;
						   s/<BUILD>/$builddir/gm; });
	}

	# Run makesis/signsis if needed on package files
	#
	$file =~ s/\.pkg//;
	if (    mtime("$builddir/$file.sis") < mtime("$builddir/$file.pkg")
	     || mtime("$builddir/$file.sis") < mtime("$cert_path/swi/test/testexes/certs/default.cer")
	     || mtime("$builddir/$file.sis") < mtime("certs/default.key")
	     || mtime("$builddir/$file.sis") < mtime($makesis)
	     || mtime("$builddir/$file.sis") < mtime($signsis))
	{
		# build sis file
		system("$makesis $builddir\\$file.pkg $builddir\\$file-tmp.sis");
		print ("$makesis $builddir\\$file.pkg $builddir\\$file-tmp.sis\n") if $opt{v};
		system("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $cert_path\\swi\\test\\testexes\\certs\\default.cer certs\\default.key");
 		print ("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $cert_path\\swi\\test\\testexes\\certs\\default.cer certs\\default.key\n") if $opt{v};
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
		AddIby(\*NULL, $builddir, $platform, "$file-unsigned");
	} else {
		unlink("$builddir/$file-tmp.sis");
	}

	if ($add_to_iby eq "yes")
	  {
	    AddIby(\*iby, $builddir, $platform, $file);
	  }
	elsif ($platform =~/winscw/i)
	  {
	    copy("$builddir/$file.sis", "/epoc32/winscw/c/tswi/tsis/data/$file.sis");
	  }
      }
  }

# Find all of the custom sis files for each platform and make sure they are exported
sub AddCustomSisFiles($$)
{
	my ($iby, $platform) = @_;
	my @sisFiles;

	opendir DIR, "custom/sis/$platform";
	@sisFiles = readdir DIR;
	closedir DIR;

	foreach my $sisFile (@sisFiles)
	{
		next unless $sisFile =~ /\.sis$/i;
		$sisFile =~ s/\.sis//g;
		AddIby($iby, "custom\\sis\\$platform", $platform, $sisFile);
	}

	# Special case for testsidcheck_ROM.exe
	# must be placed in \system\bin\ with other executables
	# must have a different filename so comparison is done by SID
	if ($platform =~ /armv5/i)
	{
		print $iby "file = ABI_DIR\\BUILD_DIR\\testsidcheck_ROM.exe	sys\\bin\\testsidcheckROM.exe\n";
	}

	if ($platform =~/winscw/i)
	{
		# copy the testsidcheck_ROM.exe into the Z:\\sys\\bin\\ directory
		copy("/epoc32/release/$platform/$configuration/testsidcheck_ROM.exe",
		     "/epoc32/release/$platform/$configuration/Z/sys/bin/testsidcheckROM.exe");
	}

}

sub AddIby($$$$)
{
	my ($iby, $builddir, $platform, $entry) = @_;

	if ($platform =~/winscw/i)
	{
		copy("$builddir/$entry.sis", "/epoc32/winscw/c/tswi/tsis/data/$entry.sis");
	}
	if ($platform =~ /armv5/i)
	{
		copy("$builddir/$entry.sis", "/epoc32/winscw/c/tswi/tsis/data/armv5/$entry.sis");
		print $iby "data = ZDRIVE\\tswi\\tsis\\data\\armv5\\$entry.sis \\tswi\\tsis\\data\\$entry.sis\n";
	}
}

sub AddIby2($$$)
{
	my ($iby, $platform, $entry) = @_;
	if ($platform =~ /armv5/i)
	{
	print $iby "data = ZDRIVE\\tswi\\tsis\\data\\armv5\\$entry \\tswi\\tsis\\data\\$entry\n";
	}
}

sub AddNormalFiles($)
{
	my ($iby) = @_;
	if ($platform =~ /armv5/i)
	{
	print $iby "#ifdef __SECURITY_NORMAL_SWICERTSTORE__\n";
	}
}

sub EndNormalFiles($)
{
	my ($iby) = @_;
	if ($platform =~ /armv5/i)
	{
	print $iby "#endif\n";
	}
}

# Keep original method names for ease of merging/compatibility
sub SignFile($$$$)  { SignFileLen(1, @_); }
sub SignFile_noiby($$$)  { SignFileLen_noiby(1, @_); }
sub SignFile2($$$$) { SignFileLen(2, @_); }
sub SignFile2_noiby($$$) { SignFileLen_noiby(2, @_); }

sub SignFileLen($$$$$)
{
	my ($len, $infile, $outfile, $signwith, $iby) = @_;
	my $certpath="$ENV{'SECURITYSOURCEDIR'}/installationservices/switestfw/testcertificates/swi/test/tsisfile/data/signedsis/";			
 	my $keypath = "../tsisfile/data/signedsis";
	my $sispath="";
	if ($platform =~ /armv5/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data\\armv5";
		}
	if ($platform =~/winscw/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data";
		}

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
		printf("\t$infile with $signwith to $outfile $pemfile $keyfile\n");
		my $cmd = "$signsis -s $sispath\\$infile $builddir\\$outfile $pemfile $keyfile";
		system($cmd);
		print "$cmd\n\n" if $opt{v};
		copy("$builddir\\$outfile", "$sispath\\$outfile");
	} else {
		print("$sispath/$outfile is up-to-date\n");
	}
	AddIby2($iby, $platform, $outfile);
}

sub SignFileLen_noiby($$$$)
{
	my ($len, $infile, $outfile, $signwith) = @_;
	my $certpath="$ENV{'SECURITYSOURCEDIR'}/installationservices/switestfw/testcertificates/swi/test/tsisfile/data/signedsis/";
	my $keypath="../tsisfile/data/signedsis";
	my $sispath="";
	if ($platform =~ /armv5/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data\\armv5";
		}
	if ($platform =~/winscw/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data";
		}

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
		printf("\t$infile with $signwith to $outfile $pemfile $keyfile\n");
		my $cmd = "$signsis -s $sispath\\$infile $builddir\\$outfile $pemfile $keyfile";
		system($cmd);
		print "$cmd\n\n" if $opt{v};
		copy("$builddir\\$outfile", "$sispath\\$outfile");
	} else {
		print("$sispath/$outfile is up-to-date\n");
	}
}


sub SignFileWithSpecifiedCert($$$$$)
{
	my ($infile, $outfile, $signcert, $signkey, $iby) = @_;
	
	my $certpath="$ENV{'SECURITYSOURCEDIR'}/installationservices/switestfw/testcertificates/swi/test/";
 	my $keypath = "";
 	if ($signkey =~ /.pem$/)
 		{
 		$keypath = "$ENV{'SECURITYSOURCEDIR'}/installationservices/switestfw/testcertificates/swi/test/";
 		}
 	else
 	{
 		$keypath = "../tsisfile/data/signedsis";
 	}
 
 
	my $sispath="";

	if ($platform =~ /armv5/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data\\armv5";
		}
	if ($platform =~/winscw/i)
		{
		$sispath="\\epoc32\\winscw\\c\\tswi\\tsis\\data";
		}

	if (   mtime("$sispath/$outfile") < mtime("$sispath/$infile")
	    || mtime("$sispath/$outfile") < mtime("$signsis")
	    || mtime("$sispath/$outfile") < mtime("$keypath/$signkey")
	    || mtime("$sispath/$outfile") < mtime("$certpath/$signcert")) {
		printf("\t$infile with $signcert and $signkey to $outfile\n");
		my $cmd = "$signsis -s $sispath\\$infile $builddir\\$outfile $certpath$signcert $keypath\\$signkey";
		printf($cmd);
		system($cmd);
		print "$cmd\n\n" if $opt{v};
		copy("$builddir\\$outfile", "$sispath\\$outfile");
	} else {
		print("$sispath/$outfile is up-to-date\n");
	}
	AddIby2($iby, $platform, $outfile);
}


# Create SIS files that depend on signed versions of others

sub CreateEmbeddingSisFiles($$$$)
{
	my ($platform, $configuration, $platform, $builddir) = @_;
	my $packagedir = "embed";

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
	     || mtime("$builddir/$file.sis") < mtime("$cert_path/swi/test/testexes/certs/default.cer")
	     || mtime("$builddir/$file.sis") < mtime("certs/default.key")
	     || mtime("$builddir/$file.sis") < mtime($makesis)
	     || mtime("$builddir/$file.sis") < mtime($signsis))
	{
		# build sis file
		# because of the way embedded package work, we'll have to change directory
		chdir($builddir);
		system("$makesis $file.pkg $file-tmp.sis");
		print ("$makesis $file.pkg $file-tmp.sis\n") if $opt{v};
		chdir("..");
		system("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $cert_path\\swi\\test\\testexes\\certs\\default.cer certs\\default.key");
		print ("$signsis -s $builddir\\$file-tmp.sis $builddir\\$file.sis $cert_path\\swi\\test\\testexes\\certs\\default.cer certs\\default.key\n") if $opt{v};
		print "\n" if $opt{v};
	} else {
		print ("$builddir/$file.sis is up-to-date\n");
	}


	AddIby(\*iby, $builddir, $platform, $file);

}
}

#
# Sign the files as necessary for the following tests
#

printf("Signing files...\n");

#Tests:
#
#User:
#
#Do these three with grant user caps dialog response Yes/No

#	tusergrantable unsigned	-	should ask
# N/A

#	tusergrantable signed with SymbianTestDSACA - should ask
SignFile("tswiusergrantable.sis","tswiusergrantable_DSACA.sis","SymbianTestRootCADSA",\*iby);

#	tusergrantable signed with SymbianTestRSACA - should ask
SignFile("tswiusergrantable.sis","tswiusergrantable_RSACA.sis","SymbianTestRootCARSA",\*iby);

#tusergrantable signed with SymbianTestDSACA, SymbianTestRSACA - should pass
SignFile("tswiusergrantable_RSACA.sis","tswiusergrantable_RSACA_DSACA.sis","SymbianTestRootCADSA",\*iby);

#tusergrantable signed with SymbianTestDSACA, SymbianTestRSACA - used for mandatory testing
SignFile("tswiusergrantable_RSACA.sis","tswiusergrantable_RSACA_Root5.sis","Root5CA",\*iby);

#tusergrantable signed with SymbianTestDSACA, SymbianTestRSACA - used for mandatory testing
SignFile("tswiusergrantable_DSACA.sis","tswiusergrantable_DSACA_Root5.sis","Root5CA",\*iby);


#System:

#tswisystemcapability signed with Root5CA	-	Should pass
SignFile("tswisystemcapability.sis","tswisystemcapability_Root5CA.sis","Root5CA",\*iby);

#runwaitmax signed with Root5CA	-	Should pass
SignFile("runwaitmax.sis","runwaitmax_Root5CA.sis","Root5CA",\*iby);

#runwait signed with Root5CA	-	Should pass
SignFile("runwait.sis","runwait_Root5CA.sis","Root5CA",\*iby);

#runwait_pa signed with Root5CA	-	Should pass
SignFile("runwait_pa.sis","runwait_pa_Root5CA.sis","Root5CA",\*iby);

#runwait_ecomfail with Root5CA	-	Should pass
SignFile("runwait_ecomfail.sis","runwait_ecomfail_Root5CA.sis","Root5CA",\*iby);

#runwait _failsigned with Root5CA	-	Should pass
SignFile("runwait_fail.sis","runwait_fail_Root5CA.sis","Root5CA",\*iby);

#runmimehandler signed with Root5CA	-	Should pass
SignFile("runmimehandler.sis","runmimehandler_Root5CA.sis","Root5CA",\*iby);

#tswisystemcapability signed with SymbianTestDSACA	-	should fail, unsigned system capabilities
SignFile("tswisystemcapability.sis","tswisystemcapability_DSACA.sis","SymbianTestRootCADSA",\*iby);

#tswisystemcapability signed with Root5CA	-	Should pass
SignFile("tswisystemcapability_option.sis","tswisystemcapability_option_Root5CA.sis","Root5CA",\*iby);

#tswisystemcapability signed with SymbianTestDSACA	-	should fail, unsigned system capabilities
SignFile("tswisystemcapability_option.sis","tswisystemcapability_option_DSACA.sis","SymbianTestRootCADSA",\*iby);

#tswisystemcapability signed with SymbianTestRootCADiffSerial	-	Should pass
SignFile2("tswisystemcapability.sis","tswisystemcapability_DiffSerial.sis","SymbianTestRootCADiffSerial",\*iby);

#tswiallcapabilities unsigned - should fail
# N/A

#tswiallcapabilities signed with Root5CA - should pass
SignFile("tswiallcapabilities.sis","tswiallcapabilities_Root5.sis","Root5CA",\*iby);

#tswiallcapabilities signed with SymbiantestRSACA - should fail
SignFile("tswiallcapabilities.sis","tswiallcapabilities_RSACA.sis","SymbianTestRootCARSA",\*iby);

#tswiallcapabilities signed with Root5CA, SymbiantestRSACA - should pass
SignFile("tswiallcapabilities_Root5.sis","tswiallcapabilities_Root5_RSACA.sis","SymbianTestRootCARSA",\*iby);

#tswisignedcap_8	signed with Root5CA - should fail
SignFile("tswisignedcap_8.sis","tswisignedcap_8_Root5.sis","Root5CA",\*iby);

#tswisignedcap_8	signed with Root5CA, SymbianTestRSACA - should fail
SignFile("tswisignedcap_8_Root5.sis","tswisignedcap_8_Root5_RSACA.sis","SymbianTestRootCARSA",\*iby);

#tswisignedcap_8	signed with Root5CA, SymbianTestRSACA, SymbianTestDSACA - should pass
SignFile("tswisignedcap_8_Root5_RSACA.sis","tswisignedcap_8_Root5_RSACA_DSACA.sis","SymbianTestRootCADSA",\*iby);


#tswisignedcap_8	signed with Root5CA, SymbianTestDSACA - should fail
SignFile("tswisignedcap_8_Root5.sis","tswisignedcap_8_Root5_DSACA.sis","SymbianTestRootCADSA",\*iby);

#tswisignedcap_8	signed with Root5CA, SymbianTestDSACA, SymbianTestRSACA - should pass
SignFile("tswisignedcap_8_Root5_DSACA.sis","tswisignedcap_8_Root5_DSACA_RSACA.sis","SymbianTestRootCARSA",\*iby);

# eclipsing upgrades, correctly signed
SignFile("eclipsingdllupgradesp.sis","eclipsingdllupgradesp_Root5CA.sis","Root5CA",\*iby);
SignFile("eclipsingdllupgradepu.sis","eclipsingdllupgradepu_Root5CA.sis","Root5CA",\*iby);
SignFile("eclipsingdllupgradewildcardsp.sis","eclipsingdllupgradewildcardsp_Root5CA.sis","Root5CA",\*iby);

SignFile("twoeclipses1.sis","twoeclipses1_Root5CA.sis","Root5CA",\*iby);
SignFile("twoeclipses2.sis","twoeclipses2_Root5CA.sis","Root5CA",\*iby);
SignFile("fileeclipsepu.sis","fileeclipsepu_Root5CA.sis","Root5CA",\*iby);
SignFile("fileeclipsepu2.sis","fileeclipsepu2_Root5CA.sis","Root5CA",\*iby);
SignFile_noiby("fileeclipsepu3.sis","fileeclipsepu3_DSACA.sis","SymbianTestRootCADSA");
SignFile("fileeclipsepu3_DSACA.sis","fileeclipsepu3_Root5CA.sis","Root5CA",\*iby);
SignFile_noiby("fileeclipsesa.sis","fileeclipsesa_DSACA.sis","SymbianTestRootCADSA");
SignFile("fileeclipsesa_DSACA.sis","fileeclipsesa_Root5CA.sis","Root5CA",\*iby);
SignFile("eclipsenotpresent.sis","eclipsenotpresent_Root5CA.sis","Root5CA",\*iby);

SignFile("stubprivatesidsp.sis","stubprivatesidsp_Root5CA.sis","Root5CA",\*iby);
SignFile("stubprivatesidwildcardsp.sis","stubprivatesidwildcardsp_Root5CA.sis","Root5CA",\*iby);

SignFile("stubupgradewildcard.sis","stubupgradewildcard_Root5CA.sis","Root5CA",\*iby);

#tswinocapability	signed with SymbianTestRSACA - should fail because mandatory Root5CA certificate is required
SignFile("tswinocapability.sis","tswinocapability_RSA.sis","\SymbianTestRootCARSA",\*iby);

#tswinocapability	signed with Root5CA - should pass
SignFile2("tswinocapability.sis","tswinocapability_Root5.sis","Root5CA",\*iby);

#INC065244A	signed with Root5CA - should pass
SignFile("inc065244A.sis","inc065244A_Root5.sis","\Root5CA",\*iby);

# DEF072973
SignFile("testrun_exe2.sis","testrun_exe2_Root5.sis","Root5CA",\*iby);
SignFile("testrun_exeRI.sis","testrun_exeRI_Root5.sis","Root5CA",\*iby);
SignFile("testrun_exeRI_pu.sis","testrun_exeRI_pu_Root5.sis","Root5CA",\*iby);
SignFile("testrun_exeRI_sp.sis","testrun_exeRI_sp_Root5.sis","Root5CA",\*iby);
SignFile("testrun_exeRIRW2.sis","testrun_exeRIRW2_Root5.sis","Root5CA",\*iby);

#CR1240
SignFile("datetime_0001-unsigned.sis","datetime_0001_signed.sis","SymbianTestRootCADiffSerial",\*iby);

# DEF086668
SignFile("testrun_exeRI_longrun_pu.sis","testrun_exeRI_longrun_pu_Root5.sis","Root5CA",\*iby);

#INC065244B	signed with Root5CA - should pass
SignFile("inc065244b.sis","inc065244B_Root5.sis","Root5CA",\*iby);

#CR EGUO-6G9J6M - DLL VID correctness enforcement in SWIS
SignFile("tswidllnocapabilityprotectedvid.sis", "tswidllnocapabilityprotectedvid_Root5.sis", "Root5CA",\*iby);
SignFile("tswidllnocapabilityunprotectedvid.sis", "tswidllnocapabilityunprotectedvid_Root5.sis", "Root5CA",\*iby);

SignFile("tswinocapabilityhardcode.sis", "tswinocapabilityhardcode_root5.sis", "Root5CA", \*iby);
SignFile("backuprestore_test_rom_stub_pu_upgrade.sis","backuprestore_test_rom_stub_pu_upgrade_Root5.sis","\Root5CA",\*iby);
SignFile("backuprestore_test_sa_base.sis","backuprestore_test_sa_base_Root5.sis","\Root5CA",\*iby);
SignFile("backuprestore_test_pu_upgrade.sis","backuprestore_test_pu_upgrade_Root5.sis","\Root5CA",\*iby);

# DEF070572 - SIS files signed by multiple chains to test OCSP result dialog
SignFileWithSpecifiedCert("tswinocapability_Root5.sis", "testocspresultdialog_a.sis", "tsisfile\\data\\signedsis\\SymbianTestRootExpiredCARSA\\cacert.pem", "tsisfile\\data\\signedsis\\SymbianTestRootExpiredCARSA\\cakey.pem", \*iby);
SignFileWithSpecifiedCert("testocspresultdialog_a.sis", "testocspresultdialog_b.sis", "tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.cert.pem", "tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.key.pem", \*iby);

# INC110222 - SIS files signed by a revoked cert then an unknown cert to test OCSP result dialog
SignFileWithSpecifiedCert("tswinocapability_Root5.sis", "tswinocapability_revoked.sis", "tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.cert.pem", "tdevcerts\\SymbianTestRootCARSA_OCSP\\SymbianTestRootCARSA\\certs\\revoked_user_caps.key.pem", \*NULL);
SignFileWithSpecifiedCert("tswinocapability_revoked.sis", "tswinocapability_unknown.sis", "tsisfile\\data\\signedsis\\extendedkeyusage\\standardsigningoid_eku.pem", "tsisfile\\data\\signedsis\\extendedkeyusage\\ekuprivatekey.pem", \*iby);

SignFileWithSpecifiedCert("tswinocapability-unsigned.sis", "tswinocapability_ekustdcodesigning.sis", "tsisfile\\data\\signedsis\\extendedkeyusage\\standardsigningoid_eku.pem", "tsisfile\\data\\signedsis\\extendedkeyusage\\ekuprivatekey.pem", \*iby);
SignFileWithSpecifiedCert("tswinocapability-unsigned.sis", "tswinocapability_ekualtcodesigning.sis", "tsisfile\\data\\signedsis\\extendedkeyusage\\alternativesigningoid_eku.pem", "tsisfile\\data\\signedsis\\extendedkeyusage\\ekuprivatekey.pem", \*iby);
SignFileWithSpecifiedCert("tswinocapability-unsigned.sis", "tswinocapability_ekunocodesigning.sis", "tsisfile\\data\\signedsis\\extendedkeyusage\\no_eku.pem", "tsisfile\\data\\signedsis\\extendedkeyusage\\ekuprivatekey.pem", \*iby);
SignFileWithSpecifiedCert("tswinocapability-unsigned.sis", "tswinocapability_ekuserverauth.sis", "tsisfile\\data\\signedsis\\extendedkeyusage\\serverauth_eku.pem", "tsisfile\\data\\signedsis\\extendedkeyusage\\ekuprivatekey.pem", \*iby);

# INC084207 - Upgrades to ROM package requiring trusted signing
SignFile("eclipsingromexesp.sis", "eclipsingromexesp_root5.sis", "root5ca", \*iby);
SignFile("eclipsingromexepu.sis", "eclipsingromexepu_root5.sis", "root5ca", \*iby);
SignFile("eclipsingromsaupgrade.sis", "eclipsingromsaupgrade_root5.sis", "root5ca", \*iby);
SignFile("eclipsingmultifilessp.sis", "eclipsingmultifilessp_root5.sis", "root5ca", \*iby);
SignFile("romstubupgradesa.sis", "romstubupgradesa_root5.sis", "root5ca", \*iby);

# INC080962 - Preinstalled packages used for preinstalled delete testing
# Tests are run on mcs rom so they need signing with root5ca
SignFile("testpreinstalledexes.sis", "testpreinstalledexes_root5.sis", "root5ca", \*iby);
SignFile("testpreinstalledexes_patch.sis", "testpreinstalledexes_patch_root5.sis", "root5ca", \*iby);

# DEF122219 SWI run-on-install flag does not work for pre-installed applications.
SignFile("preinstalledapprun.sis", "preinstalledapprun_root5.sis", "root5ca", \*iby);


# SIS patch to use with trust testing SP upgrades
SignFile("filesp-signed.sis", "filesp-signed_root5.sis", "root5ca", \*iby);

SignFile("priv-orphan3-unsigned.sis","priv-orphan3.sis","Root5CA",\*iby);

# Verify that executables in sub-directories of z:\sys\bin\ are included in the SID cache.
SignFile("tswisidcache1.sis","tswisidcache1_root5.sis","Root5CA",\*iby);
SignFile("tswisidcache2.sis","tswisidcache2_root5.sis","Root5CA",\*iby);

# CR956 Tests are run on mcs rom so they need signing with root5ca

SignFile("cr956_rbs.sis","cr956_rbs_root5.sis","Root5CA",\*iby);
SignFile("cr956_rrrirbs.sis","cr956_rrrirbs_root5.sis","Root5CA",\*iby);
SignFile("cr956_exe_testrun.sis","cr956_exe_testrun_root5.sis","Root5CA",\*iby);
SignFile("testrun_exe_CR956.sis","testrun_exe_CR956_root5.sis","Root5CA",\*iby);

AddNormalFiles(\*iby);

# CR1027 - ROM Upgrade with SA type SIS
SignFile("CR1027ECLIPSEROMSARU.sis", "CR1027ECLIPSEROMSARU_root5.sis", "root5ca", \*iby);
SignFile("CR1027NONROMSARU.sis", "CR1027NONROMSARU_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMSANORU.sis", "CR1027ROMSANORU_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMUpgradeSA.sis", "CR1027ROMUpgradeSA_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMUpgradeSADiffUid.sis", "CR1027ROMUpgradeSADiffUid_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSASameVersion.sis", "CR1027ROMUpgradeSASameVersion_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSADiffName.sis", "CR1027ROMUpgradeSADiffName_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSANR.sis", "CR1027ROMUpgradeSANR_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradePU.sis", "CR1027ROMUpgradePU_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMUpgradePUDiffFileName.sis", "CR1027ROMUpgradePUDiffFileName_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSP.sis", "CR1027ROMUpgradeSP_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMUpgradeSPSameName.sis", "CR1027ROMUpgradeSPSameName_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSP2.sis", "CR1027ROMUpgradeSP2_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSP3.sis", "CR1027ROMUpgradeSP3_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSP2RU.sis", "CR1027ROMUpgradeSP2RU_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMWrongPU.sis", "CR1027ROMWrongPU_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMUpgradePURUNR.sis", "CR1027ROMUpgradePURUNR_root5.sis", "root5ca", \*NULL);
SignFile("CR1027ROMUpgradeSP2NR.sis", "CR1027ROMUpgradeSP2NR_root5.sis", "root5ca", \*NULL);
SignFile("secondromupgradesa.sis", "secondromupgradesa_root5.sis", "root5ca", \*iby);
SignFile("wrongromeclipsingsa.sis", "wrongromeclipsingsa_root5.sis", "root5ca", \*iby);
SignFile("wrongromeclipsingpu.sis", "wrongromeclipsingpu_root5.sis", "root5ca", \*iby);
SignFile("hiddensa.sis", "hiddensa_root5.sis", "root5ca", \*iby);

SignFile("smlsyncagent.sis", "smlsyncagent_root5.sis", "root5ca", \*iby);
SignFile("eclipsingdllupgradepu_on_c_and_emmc.sis", "eclipsingdllupgradepu_on_c_and_emmc_root5.sis", "root5ca", \*iby);
SignFile("eclipsingdllupgradesa_on_c_and_emmc.sis", "eclipsingdllupgradesa_on_c_and_emmc_root5.sis", "root5ca", \*iby);

#Swi Support for RFS
SignFile("swi_rfs_test_SA.sis", "swi_rfs_test_SA_root5.sis", "root5ca", \*iby);
SignFile("swi_rfs_test_PU.sis", "swi_rfs_test_PU_root5.sis", "root5ca", \*iby);
SignFile("swi_rfs_test_SP.sis", "swi_rfs_test_SP_root5.sis", "root5ca", \*iby);
SignFile("swi_rfs_test_SP_2.sis", "swi_rfs_test_SP_2_root5.sis", "root5ca", \*iby);

# CR1122 - Wildcard support for ROM based stub.
SignFile("CR1122WCROMUpgradeSA.sis", "CR1122WCROMUpgradeSA_root5.sis", "root5ca", \*iby);
SignFile("CR1122WCROMUpgradeSA1.sis", "CR1122WCROMUpgradeSA1_root5.sis", "root5ca", \*iby);

# PDEF114642  - Adorned filename support (eclipsing ROM files using adorned filenames)
SignFile("adornedfilenamestesting_31_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_sa.sis", "adornedfilenamestesting_31_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_sa_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_32_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_sa.sis", "adornedfilenamestesting_32_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_sa_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_33_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru.sis", "adornedfilenamestesting_33_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_34_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru.sis", "adornedfilenamestesting_34_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_35_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru.sis", "adornedfilenamestesting_35_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_36_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru.sis", "adornedfilenamestesting_36_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_37_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu.sis", "adornedfilenamestesting_37_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_38_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu.sis", "adornedfilenamestesting_38_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_39_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp.sis", "adornedfilenamestesting_39_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_310_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp.sis", "adornedfilenamestesting_310_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp_root5.sis", "root5ca", \*iby);
SignFile("adornedfilenamestesting_23_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp.sis", "adornedfilenamestesting_23_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp_root5ca.sis", "root5ca", \*NULL);

EndNormalFiles(\*iby);
AddNormalFiles(\*iby);
 
# For rucert changes PREQ1912
SignFileWithSpecifiedCert("CR1027ROMUpgradeSA_root5.sis", "CR1027ROMUpgradeSA_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("CR1027ROMUpgradeSADiffUid_root5.sis", "CR1027ROMUpgradeSADiffUid_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*NULL);
SignFileWithSpecifiedCert("ruromupgraderucertsa.sis", "ruromupgraderucertsa_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertpu.sis", "ruromupgraderucertpu_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsp.sis", "ruromupgraderucertsp_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsaoversa.sis", "ruromupgraderucertsaoversa_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsp_overwrite.sis", "ruromupgraderucertsp_overwrite_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusanrflagromupgarde.sis", "rusanrflagromupgarde_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusaromupgarde.sis", "rusaromupgarde_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rupunrflagromupgarde.sis", "rupunrflagromupgarde_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruspnrflagromupgarde.sis", "ruspnrflagromupgarde_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("nonruromupgraderucertsa.sis", "nonruromupgraderucertsa_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa1.sis", "ruromupgraderucertsa1_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertpu1.sis", "ruromupgraderucertpu1_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsp1.sis", "ruromupgraderucertsp1_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_diffdrive.sis", "ruromupgraderucertsa_diffdrive_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rupunrromupgradeoverrucertsa.sis", "rupunrromupgradeoverrucertsa_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusanrromupgradeovernormalrusa.sis", "rusanrromupgradeovernormalrusa_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruspromupgardeovernr.sis", "ruspromupgardeovernr_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusanrflagromupgarde_stub.sis", "rusanrflagromupgarde_stub_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("romupgrademorecapsthansucert.sis", "romupgrademorecapsthansucert_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_stubuid.sis", "ruromupgraderucertsa_stubuid_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("sucertsaovernonsucert.sis", "sucertsaovernonsucert_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("romupgradeembed.sis", "romupgradeembed_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_stubname.sis", "ruromupgraderucertsa_stubname_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_stubchk.sis", "ruromupgraderucertsa_stubchk_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_sid.sis", "ruromupgraderucertsa_sid_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_backuprestore.sis", "ruromupgraderucertsa_backuprestore_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_diffdrive_h4.sis", "ruromupgraderucertsa_diffdrive_h4_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_private.sis", "ruromupgraderucertsa_private_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_diffdrive1_h4.sis", "ruromupgraderucertsa_diffdrive1_h4_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_diffdrive1.sis", "ruromupgraderucertsa_diffdrive1_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsaoversa_diffuid.sis", "ruromupgraderucertsaoversa_diffuid_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("saupgradenrspsucert.sis", "saupgradenrspsucert_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("nrsaupgradesprunrsu.sis", "nrsaupgradesprunrsu_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusanrromupgradestub_sprunrsu.sis", "rusanrromupgradestub_sprunrsu_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusanrromupgradestub.sis", "rusanrromupgradestub_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsastub_sprunrsu.sis", "ruromupgraderucertsastub_sprunrsu_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusaruupgrade_purunr.sis", "rusaruupgrade_purunr_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("rusastubupgrade_punrru.sis", "rusastubupgrade_punrru_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("ruromupgraderucertsa_protectedsid.sis", "ruromupgraderucertsa_protectedsid_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("emmc_format_base_distributed_NR.sis", "emmc_format_base_distributed_NR_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("emmc_format_base_on_c_and_t_NR.sis", "emmc_format_base_on_c_and_t_NR_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

# new tests for media propagation of packages signed by SU.

SignFileWithSpecifiedCert("tpropagation_sasu_armv5.sis", "tpropagation_sasu_armv5_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("tpropagation_sasu_winscw.sis", "tpropagation_sasu_winscw_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("tpropagation_spsu_armv5.sis", "tpropagation_spsu_armv5_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("tpropagation_spsu_winscw.sis", "tpropagation_spsu_winscw_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

# non-removable test
SignFileWithSpecifiedCert("nonremovablebase.sis", "nonremovablebase_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("int_nr_option_04.sis", "int_nr_option_04_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("int_nr_option_05.sis", "int_nr_option_05_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);
SignFileWithSpecifiedCert("sp_int_nr_option_06.sis", "sp_int_nr_option_06_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

SignFile("romupgrademorecapsthansucert_sucert.sis", "romupgrademorecapsthansucert_sucert_root5.sis", "root5ca", \*iby);

SignFile("romupgradenonrucertsa.sis", "romupgradenonrucertsa_root5.sis", "root5ca", \*iby);
SignFile("romupgradenonrucertsp.sis", "romupgradenonrucertsp_root5.sis", "root5ca", \*iby);
SignFile("ruromupgradenonrucertpu.sis", "ruromupgradenonrucertpu_root5.sis", "root5ca", \*iby);
SignFile("nonrucertpunrflag.sis", "nonrucertpunrflag_root5.sis", "root5ca", \*iby);
SignFile("rusanrflagromupgardenonsucert.sis", "rusanrflagromupgardenonsucert_root5.sis", "root5ca", \*iby);
SignFile("simplesanonsucert.sis", "simplesanonsucert_root5.sis", "root5ca", \*iby);
EndNormalFiles(\*iby);
CreateEmbeddingSisFiles($platform, $configuration, $platform, $builddir);

SignFile("emdedprotectedsid.sis", "emdedprotectedsid_root5.sis", "root5ca", \*iby);

SignFile("cr956_rbs_embed.sis","cr956_rbs_embed_root5.sis","Root5CA",\*iby);
SignFile("cr956_rbs_sh.sis","cr956_rbs_sh_root5.sis","Root5CA",\*iby);

AddNormalFiles(\*iby);

#CR1027 - Signing Embedded packages.
SignFile("CR1027ROMSAEmbeddedNonROMSA.sis", "CR1027ROMSAEmbeddedNonROMSA_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMSAEmbeddedROMSANoRU.sis", "CR1027ROMSAEmbeddedROMSANoRU_root5.sis", "root5ca", \*iby);
SignFile("CR1027ROMSAEmbeddedROMSARU.sis", "CR1027ROMSAEmbeddedROMSARU_root5.sis", "root5ca", \*iby);
SignFile("CR1027SAEmbeddedROMSARU.sis", "CR1027SAEmbeddedROMSARU_root5.sis", "root5ca", \*iby);

SignFile("textfile.sis", "textfile_root5ca.sis", "root5ca", \*iby);

#DEF115255-signing the package
SignFile("samestubpkgnamewithru.sis", "samestubpkgnamewithru_root5.sis", "root5ca", \*iby);


#INC116616
SignFile("saromupgradediffpkgname.sis", "saromupgradediffpkgname_root5.sis", "root5ca", \*iby);
SignFileWithSpecifiedCert("romupgrade_base.sis", "romupgrade_base_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

#PDEF118284-signing the package
SignFile("testappinuse.sis", "testappinuse_root5.sis", "root5ca", \*iby);

#DEF123261
SignFile("largenoofuids.sis", "largenoofuids_root5.sis", "root5ca", \*iby);

# INC125338
SignFileWithSpecifiedCert("sasu_nonru_inc125338.sis", "sasu_nonru_inc125338_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

#DEF125174

SignFileWithSpecifiedCert("rom_su_saru.sis", "rom_su_saru_all.sis", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\symbiantestallcapabilitiesca.pem", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\cakey.pem", \*iby);
SignFileWithSpecifiedCert("rom_su_saru_all.sis", "rom_su_saru_all_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

SignFileWithSpecifiedCert("romeclipse_su_spru.sis", "romeclipse_su_spru_all.sis", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\symbiantestallcapabilitiesca.pem", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\cakey.pem", \*iby);
SignFileWithSpecifiedCert("romeclipse_su_spru_all.sis", "romeclipse_su_spru_all_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

SignFileWithSpecifiedCert("romoverwrite_su_spru.sis", "romoverwrite_su_spru_all.sis", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\symbiantestallcapabilitiesca.pem", "tsisfile\\data\\signedsis\\symbiantestallcapabilitiesca\\cakey.pem", \*iby);
SignFileWithSpecifiedCert("romoverwrite_su_spru_all.sis", "romoverwrite_su_spru_all_sucert.sis", "tsisfile\\data\\signedsis\\sucert\\sucert.der", "tsisfile\\data\\signedsis\\sucert\\sucertkey.pem", \*iby);

# INC126755 - create packages for backup & restore tests
SignFile("br1_base.sis","br1_base_root5.sis","Root5CA",\*iby);
SignFile("br1_bigpu.sis","br1_bigpu_root5.sis","Root5CA",\*iby);
SignFile("br1_bigsp1.sis","br1_bigsp1_root5.sis","Root5CA",\*iby);
SignFile("br1_bigsp1_1.sis","br1_bigsp1_1_root5.sis","Root5CA",\*iby);
SignFile("br1_bigsp2.sis","br1_bigsp2_root5.sis","Root5CA",\*iby);
SignFile("romstubupgradepu.sis","romstubupgradepu_root5.sis","Root5CA",\*iby);
SignFile("romstubupgradesaru.sis","romstubupgradesaru_root5.sis","Root5CA",\*iby);

# Post manufacture installation of a Layered Execution Environment
SignFile("tswtype_valid1.sis", "tswtype_valid1_root5.sis", "root5ca", \*iby);
SignFile("tswtype_valid2.sis", "tswtype_valid2_root5.sis", "root5ca", \*iby);
SignFile("tswtype_valid3.sis", "tswtype_valid3_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid1.sis", "tswtype_invalid1_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid2.sis", "tswtype_invalid2_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid3.sis", "tswtype_invalid3_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid4.sis", "tswtype_invalid4_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid5.sis", "tswtype_invalid5_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid6.sis", "tswtype_invalid6_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid7.sis", "tswtype_invalid7_root5.sis", "root5ca", \*iby);
SignFile("tswtype_invalid8.sis", "tswtype_invalid8_root5.sis", "root5ca", \*iby);
SignFile("tswtype_native_invalid1.sis", "tswtype_native_invalid1_root5.sis", "root5ca", \*iby);
SignFile("tswtype_valid1_upgrade.sis", "tswtype_valid1_upgrade_root5.sis", "root5ca", \*iby);
SignFile("tswtype_valid1_invalidupgrade.sis", "tswtype_valid1_invalidupgrade_root5.sis", "root5ca", \*iby);

# SWI applicaiton registration integreation
SignFile("swiappregintegration_base02_ru.sis", "swiappregintegration_base02_ru_root5.sis", "root5ca", \*iby);
EndNormalFiles(\*iby);

close iby;

printf("\n");

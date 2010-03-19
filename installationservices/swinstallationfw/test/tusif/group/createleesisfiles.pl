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
#


use Cwd;
use File::Copy;
use File::Basename;

# Helpers
sub ReadFile($);
sub WriteFile($$);
sub EditFile($$$);

print "This script creates SIS files containing LEEs for SIF integration tests\n";

my $PLATFORM=@ARGV[0];
my $CONFIGURATION=@ARGV[1];

my $securitySourceDir = $ENV{SECURITYSOURCEDIR};
my $CERTPATH = "$securitySourceDir\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis\\Root5CA\\cert_chain_dsa_len1\\first.dsa.cer";
my $KEYPATH = "$securitySourceDir\\installationservices\\swi\\test\\tsisfile\\data\\signedsis\\Root5CA\\cert_chain_dsa_len1\\dsa1.key";
my $PKGDIR = "$securitySourceDir\\installationservices\\swinstallationfw\\test\\tusif\\scripts\\data";
my $BUILDDIR = "\\epoc32\\release\\$PLATFORM\\$CONFIGURATION";
my $DATADIR = "$BUILDDIR\\z\\tusif\\tswtype\\data";
my $ARMV5DATADIR = "\\epoc32\\data\\z\\tusif\\tswtype\\data";

my $BASE = 'sifswtypebase';
my $UPGRADE = 'sifswtypeupgrade';
my $suffix = '_edited';

# Replace <BUILDMIDPATH> and <ZDRIVEMIDPATH> in the PKG files with the current values
my $ZDRIVEMIDPATH = 'data';
if (lc($PLATFORM) ne "armv5")
	{
	$ZDRIVEMIDPATH = "release\\$PLATFORM\\$CONFIGURATION";
	}
my $BUILDMIDPATH = "release\\$PLATFORM\\$CONFIGURATION";
EditFile("$PKGDIR\\$BASE.pkg", "$PKGDIR\\$BASE$suffix.pkg" , sub { s/<BUILDMIDPATH>/$BUILDMIDPATH/gm; s/<ZDRIVEMIDPATH>/$ZDRIVEMIDPATH/gm; });
EditFile("$PKGDIR\\$UPGRADE.pkg", "$PKGDIR\\$UPGRADE$suffix.pkg" , sub { s/<BUILDMIDPATH>/$BUILDMIDPATH/gm; s/<ZDRIVEMIDPATH>/$ZDRIVEMIDPATH/gm; });

my $dir = getcwd;
chdir $PKGDIR;

# Generate SIS files for SIF integration tests
my $CREATESISCMD = "createsis create -cert $CERTPATH -key $KEYPATH";
`$CREATESISCMD $PKGDIR\\$BASE$suffix.pkg`;
`$CREATESISCMD $PKGDIR\\$UPGRADE$suffix.pkg`;
unlink "*.cer"; #delete all certificates to avoid failure on checklocationofcertificates test

# Copy generated SIS files onto the Z drive
if (lc($PLATFORM) ne "armv5")
	{
	unlink "$DATADIR\\$BASE.sis";
	move("$PKGDIR\\$BASE$suffix.sis", "$DATADIR\\$BASE.sis");
	unlink "$DATADIR\\$UPGRADE.sis";
	move("$PKGDIR\\$UPGRADE$suffix.sis", "$DATADIR\\$UPGRADE.sis");
	}
else
	{
	mkdir "$ARMV5DATADIR" unless -d "$ARMV5DATADIR";
	unlink "$DATADIR\\$BASE.sis";
	move("$PKGDIR\\$BASE$suffix.sis", "$ARMV5DATADIR\\$BASE.sis");
	unlink "$DATADIR\\$UPGRADE.sis";
	move("$PKGDIR\\$UPGRADE$suffix.sis", "$ARMV5DATADIR\\$UPGRADE.sis");
	}

unlink "$PKGDIR\\$BASE$suffix.pkg";
unlink "$PKGDIR\\$UPGRADE$suffix.pkg";

chdir $dir;


# Read the contents of a file into a string and return it
sub ReadFile ($) {
	my ($file) = @_;
	open FILE, "<$file" or die "Can't read from $file: $!";
	local $/ = undef;
	my $data = <FILE>;
	close FILE;
	return $data;
}

# Replace a file with a string
sub WriteFile ($$) {
	my ($file, $data) = @_;

	# Ensure directory exists
	if ($file =~ basename($1) && ! -e $1)
		{
		mkdir($1);
		}
	
	if (-e $file)
		{
		system("attrib -r $file");
		}
	
	open FILE, ">$file" or die "Can't write to $file: $!";
	print FILE $data;
	close FILE;
}

# Edit a file
sub EditFile ($$$) {
	my ($infile, $outfile, $coderef) = @_;
	local $_ = ReadFile($infile);
	&$coderef();
	WriteFile($outfile, $_);
}


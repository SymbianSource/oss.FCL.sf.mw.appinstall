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
# This script creates reference binary packages that may consists of embedded packages
#

use File::Copy;

sub MakeBinPkg ($);

my $PLATFORM=@ARGV[0];
my $CONFIGURATION=@ARGV[1];

my $SecuritySourceDir = $ENV{SECURITYSOURCEDIR};
my $RefSamplePath = "$SecuritySourceDir\\installationservices\\refswinstallationplugin\\sample";
my $RefDataPath = "$SecuritySourceDir\\installationservices\\refswinstallationplugin\\test\\scripts\\data";
my $IntegTestDataPath = "$SecuritySourceDir\\installationservices\\swinstallationfw\\test\\tusif\\scripts\\data";
my $SifTestDataPath = "$SecuritySourceDir\\installationservices\\swinstallationfw\\test\\scripts\\data";
my $BUILDDIR = "\\epoc32\\release\\$PLATFORM\\$CONFIGURATION";

my $DATADIR = "$BUILDDIR\\z\\tusif\\tsifintegration\\data";
my $ARMV5DATADIR = "\\epoc32\\data\\z\\tusif\\tsifintegration\\data";

my $REFDATADIR = "$BUILDDIR\\z\\tusif\\tsifrefinstaller\\data";
my $REFARMV5DATADIR = "\\epoc32\\data\\z\\tusif\\tsifrefinstaller\\data";

my $BASE = 'base.sifrefpkg';
my $BASE3 = 'base3.sifrefpkg';
my $UPGRADE = 'upgrade.sifrefpkg';
my $INVALID = 'invalid.sifrefpkg';
my $INVALID2 = 'invalid2.sifrefpkg';
my $INVALID3 = 'invalid3.sifrefpkg';
my $INVALID4 = 'invalid4.sifrefpkg';
my $INVALID5 = 'invalid5.sifrefpkg';
my $INVALID6 = 'invalid6.sifrefpkg';
my $INVALID7 = 'invalid7.sifrefpkg';
my $INVALID8 = 'invalid8.sifrefpkg';
my $INVALID9 = 'invalid9.sifrefpkg';
my $INVALID10 = 'invalid10.sifrefpkg';
my $INVALID11 = 'invalid11.sifrefpkg';
my $INVALID12 = 'invalid12.sifrefpkg';
my $BASE2 = 'base2.sifrefpkg';
my $SISCHILD = 'sifintegrationtestbase.sis';
my $FILEINUSETEST = 'fileinusetest.sifrefpkg';
my $BINBASE = 'base.sifrefbinpkg';
my $BINBASE3 = 'base3.sifrefbinpkg';
my $BINUPGRADE = 'upgrade.sifrefbinpkg';
my $BININVALID = 'invalid.sifrefbinpkg';
my $BININVALID2 = 'invalid2.sifrefbinpkg';
my $BININVALID3 = 'invalid3.sifrefbinpkg';
my $BININVALID4 = 'invalid4.sifrefbinpkg';
my $BININVALID5 = 'invalid5.sifrefbinpkg';
my $BININVALID6 = 'invalid6.sifrefbinpkg';
my $BININVALID7 = 'invalid7.sifrefbinpkg';
my $BININVALID8 = 'invalid8.sifrefbinpkg';
my $BININVALID9 = 'invalid9.sifrefbinpkg';
my $BININVALID10 = 'invalid10.sifrefbinpkg';
my $BININVALID11 = 'invalid11.sifrefbinpkg';
my $BININVALID12 = 'invalid12.sifrefbinpkg';
my $BINFILEINUSETEST = 'fileinusetest.sifrefbinpkg';
my $BINCHILD = 'child.sifrefbinpkg';
my $GRANDCHILD = 'test.nonnativepackage.txt';
my $BINPARENT = 'compound.sifrefbinpkg';
my $BINHOMOGENEOUSPARENT = 'homogeneouscompound.sifrefbinpkg';

print "Generating simple packages...\n";

# Create a simple base package
MakeBinPkg("--pkg $RefSamplePath\\$BASE --binpkg $RefSamplePath\\$BINBASE");
MakeBinPkg("--pkg $RefSamplePath\\$UPGRADE --binpkg $RefSamplePath\\$BINUPGRADE");
MakeBinPkg("--pkg $RefSamplePath\\$INVALID --binpkg $RefSamplePath\\$BININVALID");
MakeBinPkg("--pkg $RefDataPath\\$BASE3 --binpkg $RefDataPath\\$BINBASE3");
MakeBinPkg("--pkg $RefDataPath\\$INVALID2 --binpkg $RefDataPath\\$BININVALID2");
MakeBinPkg("--pkg $RefDataPath\\$INVALID3 --binpkg $RefDataPath\\$BININVALID3");
MakeBinPkg("--pkg $RefDataPath\\$INVALID4 --binpkg $RefDataPath\\$BININVALID4");
MakeBinPkg("--pkg $RefDataPath\\$INVALID5 --binpkg $RefDataPath\\$BININVALID5");
MakeBinPkg("--pkg $RefDataPath\\$INVALID6 --binpkg $RefDataPath\\$BININVALID6");
MakeBinPkg("--pkg $RefDataPath\\$INVALID7 --binpkg $RefDataPath\\$BININVALID7");
MakeBinPkg("--pkg $RefDataPath\\$INVALID8 --binpkg $RefDataPath\\$BININVALID8");
MakeBinPkg("--pkg $RefDataPath\\$INVALID9 --binpkg $RefDataPath\\$BININVALID9");
MakeBinPkg("--pkg $RefDataPath\\$INVALID10 --binpkg $RefDataPath\\$BININVALID10");
MakeBinPkg("--pkg $RefDataPath\\$INVALID11 --binpkg $RefDataPath\\$BININVALID11");
MakeBinPkg("--pkg $RefDataPath\\$INVALID12 --binpkg $RefDataPath\\$BININVALID12");
MakeBinPkg("--pkg $RefDataPath\\$FILEINUSETEST --binpkg $RefDataPath\\$BINFILEINUSETEST");
print "Generating embedded packages...\n";

# Create a compound package that consists of 3 embedded packages of the same type
MakeBinPkg("perl $RefSamplePath\\makebinpkg.pl --pkg $RefDataPath\\$BASE2 --emb $RefSamplePath\\$BINBASE --emb $RefDataPath\\$BINBASE3 --binpkg $RefDataPath\\$BINHOMOGENEOUSPARENT");
# Create a compound package that consists of 3 embedded packages of mixed types
MakeBinPkg("perl $RefSamplePath\\makebinpkg.pl --pkg $RefSamplePath\\$BASE --emb $SifTestDataPath\\$GRANDCHILD --binpkg $IntegTestDataPath\\$BINCHILD");
MakeBinPkg("perl $RefSamplePath\\makebinpkg.pl --pkg $RefDataPath\\$BASE2 --emb $IntegTestDataPath\\$BINCHILD --emb $BUILDDIR\\$SISCHILD --emb $RefDataPath\\$BINBASE3 --binpkg $IntegTestDataPath\\$BINPARENT");

# Copy the generated package onto the Z drive
# move reference packages
my $CURREFDATADIR = '';
if (lc($PLATFORM) ne "armv5")
	{
	$CURREFDATADIR = $REFDATADIR;
	}
else
	{
	$CURREFDATADIR = $REFARMV5DATADIR;
	}
unlink "$CURREFDATADIR\\$BINBASE";
unlink "$CURREFDATADIR\\$BINUPGRADE";
unlink "$CURREFDATADIR\\$BININVALID";
unlink "$CURREFDATADIR\\$BININVALID2";
mkdir "$CURREFDATADIR" unless -d "$CURREFDATADIR";
move("$RefSamplePath\\$BINBASE", "$CURREFDATADIR\\$BINBASE");
move("$RefSamplePath\\$BINUPGRADE", "$CURREFDATADIR\\$BINUPGRADE");
move("$RefSamplePath\\$BININVALID", "$CURREFDATADIR\\$BININVALID");
move("$RefDataPath\\$BINBASE3", "$CURREFDATADIR\\$BINBASE3");
move("$RefDataPath\\$BININVALID2", "$CURREFDATADIR\\$BININVALID2");
move("$RefDataPath\\$BININVALID3", "$CURREFDATADIR\\$BININVALID3");
move("$RefDataPath\\$BININVALID4", "$CURREFDATADIR\\$BININVALID4");
move("$RefDataPath\\$BININVALID5", "$CURREFDATADIR\\$BININVALID5");
move("$RefDataPath\\$BININVALID6", "$CURREFDATADIR\\$BININVALID6");
move("$RefDataPath\\$BININVALID7", "$CURREFDATADIR\\$BININVALID7");
move("$RefDataPath\\$BININVALID8", "$CURREFDATADIR\\$BININVALID8");
move("$RefDataPath\\$BININVALID9", "$CURREFDATADIR\\$BININVALID9");
move("$RefDataPath\\$BININVALID10", "$CURREFDATADIR\\$BININVALID10");
move("$RefDataPath\\$BININVALID11", "$CURREFDATADIR\\$BININVALID11");
move("$RefDataPath\\$BININVALID12", "$CURREFDATADIR\\$BININVALID12");
move("$RefDataPath\\$BINHOMOGENEOUSPARENT", "$CURREFDATADIR\\$BINHOMOGENEOUSPARENT");
move("$RefDataPath\\$BINFILEINUSETEST", "$CURREFDATADIR\\$BINFILEINUSETEST");

# move integration packages
my $CURDATADIR = '';
if (lc($PLATFORM) ne "armv5")
	{
	$CURDATADIR = $DATADIR;
	}
else
	{
	$CURDATADIR = $ARMV5DATADIR;
	}

unlink "$CURDATADIR\\$BINPARENT";
mkdir "$CURDATADIR" unless -d "$CURDATADIR";
move("$IntegTestDataPath\\$BINPARENT", "$CURDATADIR\\$BINPARENT");

unlink "$IntegTestDataPath\\$BINCHILD";

# Read the contents of a file into a string and return it
sub MakeBinPkg ($) {
	my ($args) = @_;
	my $cmd = "perl $RefSamplePath\\makebinpkg.pl $args";
	`$cmd`;
}
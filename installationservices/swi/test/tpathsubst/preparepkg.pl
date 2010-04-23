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


use strict;
 

sub mod_time($)
# returns a file's last modified time in seconds since the epoch
	{
	return (stat $_[0])[9];
	}

# -----------------------------------------------------------------------


my $platform = $ARGV[0];

my $pkgdir = "testpkg";
my $builddir = "build";
my $sign_cert = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis\\Root5CA\\ca.pem";
my $sign_key = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\\\tsisfile\\data\\signedsis\\Root5CA\\ca.key.pem";

print "Build path subst test packages for $platform.\n";


system("mkdir $builddir\\");
my $target_dir = "\\epoc32\\winscw\\c\\tswi\\tpathsubst\\data";
system("mkdir $target_dir\\");


# if building for hardware, create iby file
my $hardware = $platform !~ /wins/i;
my $iby_name = '\epoc32\rom\include\tpathsubst.iby';
if ($hardware)
	{
	unlink $iby_name;
	open(IBY, ">> $iby_name");	
	}


# get list of package files
opendir DIR, $pkgdir;
my @pkgfiles = grep (/\.pkg/, readdir(DIR));
closedir DIR;


# create and sign each sis file
my $target;	# needs to be seen by continue
foreach my $entry (@pkgfiles)
	{
	$entry =~ s/\.pkg//;	# remove .pkg suffix
	my $pkg_file = "$pkgdir\\$entry.pkg";

	# don't rebuild this SIS file if the package file has not changed
	$target = "$target_dir\\$entry.sis";
#	if (-e $target && mod_time($target) >= mod_time($pkg_file)) {
#		print "$target is up to date.\n";
#		next;
#	}

	my $make_cmd = "makesis $pkg_file $builddir\\$entry-tmp.sis";
	print "$make_cmd\n";
	system($make_cmd);

	my $sign_cmd = "signsis $builddir\\$entry-tmp.sis $builddir\\$entry.sis $sign_cert $sign_key";
	print "$sign_cmd\n";
	system($sign_cmd);

	my $copy_cmd = "copy /y $builddir\\$entry.sis $target";
	print "$copy_cmd\n";
	system($copy_cmd);
	}

continue
	{
	if ($hardware)
		{
		my $rom_target = "\\tswi\\tpathsubst\\data\\$entry.sis";
		my $iby_line = "data=$target\t$rom_target";
		print "appending \"$rom_target\" to $iby_name";
		print IBY "$iby_line\n";
		}
	}

if ($hardware)
	{
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\data\armv5\sp_eclipse.sis "tswi\tpathsubst\data\sp_eclipse.sis"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\data\armv5\eclipse_failure.sis "tswi\tpathsubst\data\eclipse_failure.sis"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\scripts\tpathsubst.script "tswi\tpathsubst\scripts\tpathsubst.script"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\scripts\tpathsubst.ini "tswi\tpathsubst\scripts\tpathsubst.ini"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\scripts\tpathsubst_install.xml "tswi\tpathsubst\scripts\tpathsubst_install.xml"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\scripts\tpathsubst_uninstall.xml "tswi\tpathsubst\scripts\tpathsubst_uninstall.xml"', "\n";
	print IBY 'data=\epoc32\winscw\c\tswi\tpathsubst\scripts\tsubsteddrive_install.xml "tswi\tpathsubst\scripts\tsubsteddrive_install.xml"', "\n";
	print IBY 'file=\epoc32\release\armv5\urel\setsubst.exe "\sys\bin\setsubst.exe"', "\n";
	print IBY 'file=\epoc32\release\armv5\urel\setsubst.exe "\sys\bin\tps_ecl.exe"', "\n";
	}

close IBY;


# -----------------------------------------------------------------------


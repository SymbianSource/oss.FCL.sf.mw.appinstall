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

print "Copy writable SWI certstore test script and ini file\n";

use strict;
use File::Copy;

my @files = ("testwritableswicertstore.script", 
	     "testwritableswicertstore.ini",
	     "simple.xml",
	     "uninstall.xml");

my $tswidir = "tswi\\tuiscriptadaptors\\scripts";
my $targetdir = "\\epoc32\\winscw\\c\\$tswidir";
my $sourcedir = "..\\tuiscriptadaptors\\scripts";

foreach(@files)
{
    my $source = "$sourcedir\\$_";
    my $target = "$targetdir\\$_";

    print "Copying $source to $target\n";

    if (-e $target)
    {
	unlink $target or die "Couldn't unlink $target\n";
    }

    -e $source or die "$source does not exist\n";
    copy($source, $target);
    -e $target or die "$target does not exist\n";
}

my $store = "..\\tsisfile\\data\\signedsis\\swicertstore.dat";
my $udebstore = "\\epoc32\\release\\winscw\\udeb\\z\\resource\\swicertstore.dat";
my $urelstore = "\\epoc32\\release\\winscw\\urel\\z\\resource\\swicertstore.dat";

-e $store or die "$store does not exist";

#$mode = '0777'; 

#chmod $mode, $udebstore; 
#chmod $mode, $udebstore; 

-e $udebstore and unlink $udebstore;
-e $urelstore and unlink $urelstore;


copy ($store, $udebstore) or die "Failed to copy rom certstore";
copy ($store, $urelstore) or die "Failed to copy rom certstore";

if ($ARGV[0] eq "notest")
{
    exit 0;
}


my $testdir = "\\epoc32\\RELEASE\\WINSCW\\UDEB";
chdir "$testdir" or die "chdir $testdir failed";

my @command = ("testexecute", 
	       "c:\\$tswidir\\testwritableswicertstore.script");

system(@command);

($? >> 8) == 0 or die "Error: @command Failed\n";


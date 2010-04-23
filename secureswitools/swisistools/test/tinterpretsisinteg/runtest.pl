#
# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# runtest.pl is used to runtests defined in an xml file.
#

use strict;

use FindBin;
use lib "$FindBin::Bin";
use testcontroller;
use testlistreader;
use Cwd;
use defaulttestroutine;
use File::Copy;

#processing of command line arguments
sub processArgs {
    my ($controller, @args) = @_;
	

    my $usage = <<EOF;
runtests [-v] [-h] [<test list>] [-o <tests>]
     -v          verbose. Displays test title
     -h          this help text
     -e          Use enhanced test routine that checks result of setup and cleanup
     <test list> One or more xml filenames containing test definitions
                 If no test list is defined it will try the default file
                 testlist.xml
     -o <tests>  Only run the tests defined in the specified list. This option
                 allows a subset of tests to be executed. This option must be
                 the last option and the tests referenced in this file must
                 already have been defined in previous files.

EOF

    my $hasTestFile = 0;
    my @normalFiles;
    my $specialFile;
    my $enhanced = 0;
    while (@args)
    {
	my $arg = shift @args;
	if (lc($arg) eq "-v")
	{
	    $controller->setVerbose();
	}
	elsif (lc($arg) eq "-o")
	{
	    $arg = shift @args;
	    die  "File '$arg' does not exist\n" if (! (-e $arg) );
	    $specialFile = $arg;
	    last;
	}
	elsif (lc($arg) eq "-e")
	{
	    $enhanced = 1;
	}
	elsif (lc($arg) eq "-h")
	{
	    die $usage;
	}
	elsif (lc($arg) eq "-d")
	{
	    $controller->{'debug'} = 1;
	}
	else
	{
	    die  "File '$arg' does not exist\n" if (! (-e $arg) );
	    push @normalFiles, $arg;
	    $hasTestFile = 1;
	}
    }
    
    return ($specialFile, $enhanced,$hasTestFile);
}

my %testEntry;
my $controller = 0;
my $parser = 0;

my $special = 0;
my $enhanced = 0; 
my $hasTestFile = 0;
my @file;

sub StartTest
{
use XML::DOM;

$parser->parseFiles($special, @file);
if ($enhanced)
{
    $controller->setTestRoutine(\&enhancedTestRoutine);
}
else
{
    $controller->setTestRoutine(\&defaultTestRoutine);
}

$controller->runTests();
use Cwd;
my $dir = cwd;
my $xml = "xml";
my $domDir = $dir."/".$xml;
chdir $domDir;
my $dom = "dom.pm";
unlink $dom;

}

my $epocRoot = $ENV{'EPOCROOT'};
my $ScrMain = $epocRoot."epoc32\\release\\winscw\\udeb\\z\\sys\\install\\scr\\provisioned\\scr.db";
my $ScrBackup = $epocRoot."epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\backupscr.db";
	
sub backupScr()
	{
	
	copy($ScrMain, $ScrBackup);
	}

sub restoreScr()
	{
	copy($ScrBackup, $ScrMain);
	}

# -------------- Start of script --------------

my $runType = shift;

if($runType eq "native" || $runType eq "usifnative")
{
%testEntry;
my $logIntFile = "\\epoc32\\winscw\\c\\interpretsis_test_harness.txt";
$controller = testcontroller->new($logIntFile, $runType);

$parser = testlistreader->new($controller,$runType);
($special, $enhanced, $hasTestFile) = processArgs($controller, @ARGV);

my $nativeXmlFile="\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\testlist.xml"; # default filename
if ($hasTestFile == 0)
    {
	# use default test list filename
	die  "File '$nativeXmlFile' does not exist\n" if (! (-e $nativeXmlFile) );
    }
push(@file,$nativeXmlFile);



# Delete the log file
unlink("$logIntFile");

StartTest();
@file = ();
}

elsif( $runType eq "usif")
{
	my $regfilepath =0;
	my @nativeregfiles = ("sisregistry_5.2.txt","sisregistry_5.3.txt","sisregistry_5.1.txt");
	my $regfile = 0;
	%testEntry;
	my $logDbFile = "\\epoc32\\winscw\\c\\interpretsis_test_harness_db.txt";

	backupScr();
	$controller = testcontroller->new($logDbFile, $runType);
	

	$regfilepath = "\\epoc32\\release\\winscw\\udeb\\z\\system\\data\\";
	# delete the registry file if present
	
	foreach $regfile (@nativeregfiles) 
		{
		$regfile = $regfilepath.$regfile;
		unlink($regfile);
		}
	$parser = testlistreader->new($controller,$runType);

	($special, $enhanced, $hasTestFile) = processArgs($controller, @ARGV);
	my $dbXmlfile="\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\testlistdb.xml"; # default filename
	if ($hasTestFile == 0)
		{
		# use default test list filename
		die  "File '$dbXmlfile' does not exist\n" if (! (-e $dbXmlfile) );
		}
	push(@file,$dbXmlfile);
	
	# Delete the log file
	unlink("$logDbFile");

	StartTest();

	restoreScr();

}

else
	{
	print "Please specify the type of installation to be performed (native or usif).";
	}
	

#
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Perl script that test Dumpchainvaliditytoool with different ranges
#

use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;


$logFile = "\\epoc32\\winscw\\c\\dumpchainvalidity_test.txt";
use File::Copy;
use Cwd;
use File::Find ;

#
#Function to write log into file
#
sub WriteLog 
	{
	my ($log) = @_;
	unless( open($fh, ">> $logFile")) 
		{
		printf STDERR "Can\'t open $logfile:$!\n";
		return;
		}
	printf $fh $log;
	printf $log;
	close $fh;
	}

sub chainvalidity 
	{
	my ($pkgfile) = @_[0];
	my ($expectedResult) = @_[1];
	my ($outputLog) = @_[2];
	$outputLog =~ s/\.log//;
	$neededfolder="";
	$neededfile="";
	@retval = system("DumpChainValidityTool.exe $pkgfile> $outputLog.log");
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);
	$NumberOfTests++;
	if( $? == $expectedResult) 
	{
	sleep(2);
	my $res=`diff "$outputLog.out" "$outputLog.log"`;
	if(length($res) == 0)
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed: Files $outputLog.out and $outputLog.log are not matched\n\n");
		WriteLog("Failed\n\n");
		}
	}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	unlink("$outputLog.log");

}


unlink($logFile);
WriteLog("DumpChainValidityTool test.\n\n");

#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;

#
# Array of contents of tests and expected results
#
#                file name,   					expected , expected log,            Title
#								result     message
#               ---------------------------------------------------------------------------------------------------- 
@TestItems = (	["data\\onechain.sis data\\swicertstore.dat"		,0,    , "Created",		"Test for chainvalidity with one valid chain", "chainvalidity-output\\onechain.log"],
		["data\\somevalidchain.sis data\\swicertstore.dat"		,0,    , "Created",		"Test for chainvalidity with some valid chains", "chainvalidity-output\\somevalidchain.log"],
		["data\\multichain_allValid.sis data\\swicertstore.dat"	,0,    , "Created",		"Test for chainvalidity with all chains valid", "chainvalidity-output\\allvalidchain.log"],
		["data\\selfsigned.SIS data\\swicertstore.dat"		,0,    , "Created",		"Test for SIS file signed with a selfsigned certificate", "chainvalidity-output\\selfsigned.log"],
		["data\\simple_devsimplesigned.SIS data\\swicertstore.dat"	,0,    , "Created",		"Test for SIS file signed with a developer certificate", "chainvalidity-output\\devcert2.log"],
		["data\\unsigned.sis data\\swicertstore.dat"		,0,    , "Created",		"Test for chainvalidity with sis file having no chain", "chainvalidity-output\\unsigned.log"],
		["data\\multichain_allValid.sis data\\AllMandatory.dat"	,0,    , "Created",	"Test for chainvalidity with sis files not signed by all the mandatory certificates present in certstore", "chainvalidity-output\\missing_mandatory.log"],
		["data\\multichain_allValid.sis data\\Mandatory.dat"	,0,    , "Created",	"Test for chainvalidity with sis files not signed by all the mandatory certificates present in certstore", "chainvalidity-output\\mandatory.log"],
		["data\\multichain_allValid.sis data\\corrupted.dat"	,1792,    , "File format error",	"Test for chainvalidity with corrupted certstore", "chainvalidity-output\\corrupteddat.log"],
		["data\\corrupted.sis data\\swicertstore.dat"		,1536,    , "File format error",	"Test for chainvalidity with corrupted sis file", "chainvalidity-output\\corruptedsis.log"],
	     );




#
# Do test for each elements of TestItems array
#
for my $Test ( @TestItems )
	{
	$testid = sprintf "SEC-SWI-I-CHAINVAL-00%d\n",$NumberOfTests+1;
	WriteLog($testid);
	$logMsg = sprintf "%s\n", $Test->[3];
	WriteLog($logMsg);
	chainvalidity($Test->[0], $Test->[1],$Test->[4]);
	}

opendir(DIR, ".\\Chain");
while (defined($files=readdir(DIR)))
	{
	$dirname="chain\\";
	$files =~ s/\.pem//;
	$dirname.= $files;
	if(-e "$dirname.pem")
		{
		unlink("$dirname.pem");
		}
	close(DIR);
	}

$dirpath = "Chain";
rmdir($dirpath);

#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 


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
# Perl script that test DumpInstallFileStatustool
#

use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;

$logFile = "\\epoc32\\winscw\\c\\dumpinstallstatus_test.txt";
use File::Copy;
use Cwd;

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


sub installstatus 
	{
	my ($pkgfile) = @_[0];
	my ($expectedResult) = @_[1];
	my ($outputLog) = @_[2];
	$outputLog =~ s/\.log//;
	$neededfolder="";
	$neededfile="";
	@retval = system("DumpInstallFileStatusTool.exe $pkgfile> $outputLog.log");
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);
	$NumberOfTests++;
	if( $? == $expectedResult) 
		{
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
			}
		}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	unlink("$outputLog.log");
	sleep(1);
	}


unlink($logFile);
WriteLog("DumpinstallfilestatusTool test.\n\n");
#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;
#
# Array of contents of tests and expected results
#
#                	file name,   						expected , expected log,            Title
#										result     message
#               ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- ---------------------------------------------------------------------------  
@TestItems = (	["data\\testfor2exes_unsigned.SIS data\\swicertstore.dat"			,0, , "Created",	"Unsigned SIS File", "installstatus-output\\unsignedSIS.log"],
		["data\\testfor2exes_invaidsumchain.SIS data\\swicertstore.dat ReadUserData WriteUserData ReadDeviceData WriteDeviceData", 0,    , "Created",	"SIS File having executable whose capabiltites is satisfied by signing certificates in conjuction with User Specified Capabilities\n(ReadUserData WriteUserData ReadDeviceData WriteDeviceData)", "installstatus-output\\multichain_usercaps1.log"],
		["data\\testfor2exes_multichain.SIS data\\swicertstore.dat"			,0,    , "Created",	"SIS File having executable whose capabiltites is satisfied by signing certificates.", "installstatus-output\\multichain_nomissingcaps.log"],
		["data\\testfor2exes_multichain.SIS data\\swicertstore.dat Location"			,0,    , "Created",	"SIS File having executable whose capabiltites are neither satisfied by signing certificates nor by signing certificates in conjuction with User Specified Capabilities", "installstatus-output\\multichain_missingcaps.log"],
		["data\\testfor2exes_devcert.SIS data\\swicertstore.dat",0,    , "Created",	"SIS File signed with developer certificate (granting ReadUserData,WriteUserData and Location) and not validated by certstore.\n Hence SIS File having executable whose capabiltites are not satisfied by signing certificates .", "installstatus-output\\Devcerts.log"],
		["data\\testfor2exes_multichain.SIS data\\corrupted.dat"			,1792,    , "File format error",	"Test for installfilestatus with corrupted certstore", "installstatus-output\\corrupteddat.log"],
		["data\\corrupted.sis data\\swicertstore.dat"				,1536,    , "File format error",	"Test for installfilestatus with corrupted sis file", "installstatus-output\\corruptedsis.log"],
	     );




#
# Do test for each elements of TestItems array
#
for my $Test ( @TestItems )  
	{
	$testid = sprintf "SEC-SWI-I-DUMPINSTALLFILESTATUS -000%d\n",$NumberOfTests+1;
	WriteLog($testid);
	$logMsg = sprintf "%s\n", $Test->[3];
	WriteLog($logMsg);
	installstatus($Test->[0], $Test->[1],$Test->[4]);
	}


opendir(DIR, ".\\Chain");
while (defined($file=readdir(DIR)))
	{
	$dirname="chain\\";
	$file =~ s/\.pem//;
	$dirname.= $file;
	if(-e "$dirname.pem")
		{
		unlink("$dirname.pem");
		}
	close(DIR);
	}
$dirpath = "Chain";
rmdir($dirpath);
unlink("Test.txt");
unlink("installstatus.txt");

#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

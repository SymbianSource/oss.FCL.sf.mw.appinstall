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
# Perl script that test DumpSWIRegistrytool
#

use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;

$logFile = "\\epoc32\\winscw\\c\\dumpregistry_test.txt";
unlink("$logFile");
use File::Copy;

#
#Function to write log into file
#
sub WriteLog {
	my ($log) = @_;
	unless( open($fh, ">> $logFile")) 
		{
		rintf STDERR "Can\'t open $logfile:$!\n";
		return;
		}
	printf $fh $log;
	printf $log;
	close $fh;
}


sub registryTest {
	my ($pkgfile) = @_[0];
	my ($expectedResult) = @_[1];
	my ($pkgname) = @_[2];
	my ($vendorname) = @_[3];
	$registryPath = ".\\data";
	$registryOutputPath = ".\\dumpregistry-output\\";
	$pkgfile =~ s/\.dat//;
	WriteLog( "Package test\n\n");
	@retval = system("DumpSWIRegistryTool.exe $registryPath -p $pkgfile > $pkgfile.log ");
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);
	$NumberOfTests++;
	if( $? == $expectedResult) 
		{
		if(!($pkgfile eq 80000003))
			{
			my $respkg=`diff "$registryOutputPath$pkgfile.out" "$pkgfile.log"`;
			if(length($respkg) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $pkgfile.out and $pkgfile.log are not matched\n\n");
				}
			unlink("$pkgfile.log");
			}
		else
			{
			$NumberOfPassed++;
			WriteLog("Passed\n\n");
			}
		}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	WriteLog( "Registry test\n\n");
	@retval = system("DumpSWIRegistryTool.exe $registryPath -r $pkgfile \"$pkgname\" \"$vendorname\"> $pkgfile.log");
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);
	$NumberOfTests++;
	
	if( $? == $expectedResult) 
		{
		if(!($pkgfile eq 80000003))
			{
			my $resreg=`diff "$registryOutputPath$pkgfile.tmp" "$pkgfile.log"`;
			if(length($resreg) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $pkgfile.tmp and $pkgfile.log are not matched\n\n");
				}

			}
		else
			{
			$NumberOfPassed++;
			WriteLog("Passed\n\n");
			}
		unlink("$pkgfile.log");	
		}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}	
	}

WriteLog("DumpSWIRegistryTool test.\n\n");

#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;

#
# Array of contents of test pkgs and expected results
#
#                file name,   expected , expected log,            Title
#				result     message
#               ---------------------------------------------------------------------------------------------------- 
@TestItems = (	["1000000d", 0,    , "Created",		"Registry Test for SIS files having SIS Properties","Test capabilities2","Vendor"],
		["80000010", 0,    , "Created",		"Registry Test for SIS files having MIME Type","Test Mime-GE","Vendor"],
		["80212345", 0,    , "Created",		"Registry Test for SIS files having Embedded and Dependency","TestSA","Vendor"],
		["8100000b", 0,    , "Created",		"Registry Test for Big file(4MB)","BigNc","Vendor"],
		["811111f8", 0,    , "Created",		"Registry to check operators","testexpressions_operators","Unique Vendor Name"],
		["811111fb", 0,    , "Created",		"Registry to for sis file with else options (ConditionalEvaluation)","testexpressions_else","Unique Vendor Name"],
		["811111fc", 0,    , "Created",		"Registry to check sis file with else if options (ConditionalEvaluation)","testexpressions_elseif","Unique Vendor Name"],
		["81111107", 0,    , "Created",		"Registry Test for MultiLanguage","MultiLanguageOption-FR","Vendor"],
		["81115000", 0,    , "Created",		"Registry Test for executable in embedded","053177c_0","Unique Vendor Name"],
		["81115011", 0,    , "Created",		"Registry Test with no executable","053177c_0","Unique Vendor Name"],
		["81115013", 0,    , "Created",		"Registry Test with 2 Embedded SIS files executable","053177c_3","Unique Vendor Name"],
	#	["81231235", 0,    , "Created",		"Registry Test for Partial Upgrade","TestAppInUse Suite","Unique Vendor Name"],
		["81231235", 0,    , "Created",		"Registry Test for Patch","testappinuse_patch","Unique Vendor Name"],
		["a0000206", 0,    , "Created",		"Registry Test for SIS files having multiple Data files","3 Data Files","Symbian"],
		["81000008", 0,    , "Created",		"Registry Test for File NULL","File Null - Remove a file created in another apps import directory","Symbian"],
	#	["80000003",768,    , "Created",	"Registry Test for old registry format","Depend-EN","Vendor"],
	     );

#
# Do test for each elements of TestItems array
#
for my $Test ( @TestItems )  
	{
	$testid = sprintf "SEC-SWI-I- REGISTRY -000%d\n",$NumberOfTests+1;
	WriteLog($testid);
	$logMsg = sprintf "%s\n", $Test->[3];
	WriteLog($logMsg);
	registryTest($Test->[0], $Test->[1],$Test->[4],$Test->[5]);
	}

#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

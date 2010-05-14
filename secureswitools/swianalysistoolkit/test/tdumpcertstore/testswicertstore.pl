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
# Perl script that test Dumpswicertstore tools
#

use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;

$logFile = "\\epoc32\\winscw\\c\\dumpswicertstore_test.txt";
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

#
# #
sub CertstoreFile 
	{
	my ($datfile) = @_[0];
	my ($expectedResult) = @_[1];
	$datfile =~ s/\.dat//;
	$neededfolder="";
	$neededfile="";
	$outputDir = "dumpcertstore-output\\";
	if($datfile eq "\\epoc32\\winscw\\c\\resource\\swicertstore\\dat")
		{
		$writable = "writable.log";
		@retval = system("dumpswicertstoretool.exe $datfile > $writable");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
	
		if( $? == $expectedResult) 
			{
			$writableoutput = "$outputDir" ."writable.out";
			my $res=`diff "$writableoutput" "writable.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $writableoutput and writable.log are not matched\n\n");
				}
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}
		unlink("writable.log");
		}
	elsif($datfile eq "somecapabilities.dat \\epoc32\\winscw\\c\\resource\\swicertstore\\dat")
		{
		$both1 = "both1.log";
		@retval = system("dumpswicertstoretool.exe $datfile > $both1");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
		if( $? == $expectedResult) 
			{
			$both1output = "$outputDir" ."both1.out";
			my $res=`diff "$both1output" "both1.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $both1output  and both1.log are not matched\n\n");
				}
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}
		 unlink("both1.log");
		}
	elsif($datfile eq "somecapabilities.dat")
		{
		$Yoption = "capdisplay_yoption.log";
		@retval = system("dumpswicertstoretool.exe -y $datfile > $Yoption");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
		if( $? == $expectedResult) 
			{
			$both1output = "$outputDir" ."capdisplay_yoption.out";
			my $res=`diff "$both1output" "capdisplay_yoption.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $both1output  and capdisplay_yoption.log are not matched\n\n");
				}
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}

		@retval = system("dumpswicertstoretool.exe -d $datfile > $Yoption");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
		if( $? == $expectedResult) 
			{
			$both1output = "$outputDir" ."capdisplay_doption.out";
			my $res=`diff "$both1output" "capdisplay_doption.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $both1output  and capdisplay_doption.log are not matched\n\n");
				}
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}

		 unlink("capdisplay_yoption.log");
		}
	
	elsif($datfile eq "NoMandatory.dat \\epoc32\\winscw\\c\\resource\\swicertstore\\dat")
		{
		$both2 = "both2.log";
		@retval = system("dumpswicertstoretool.exe $datfile > $both2");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
		if( $? == $expectedResult) 
			{
			$both2output = "$outputDir" ."both2.out";
			my $res=`diff "$both2output" "both2.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $both2output and both2.log are not matched\n\n");
				}
				
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}
		unlink("both2.log");
	
		}
	else
		{
		@retval = system("dumpswicertstoretool.exe $datfile.dat > $datfile.log");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		$NumberOfTests++;
		if( $? == $expectedResult) 
			{
			$outputDat = "$outputDir" ."$datfile.out";
			my $res=`diff "$outputDat" "$datfile.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Passed\n\n");
				}
			else
				{
				$NumberOfFailed++;
				WriteLog("Failed: Files $outputDat and $datfile.log are not matched\n\n");
				}
			if("$pkgfile.dat" eq "AllCapabilities.dat")
				{
				WriteLog("Test for Extract Certificate\n");
				@retval = system("dumpswicertstoretool.exe -c $datfile.dat > $datfile.log");
				$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
				WriteLog( $logMsg);
				$NumberOfTests++;
				if( $? == $expectedResult) 
					{
					$neededfile = "Root5CA.der";
					if(-e $neededfile && -s $neededfile)
						{
						$NumberOfPassed++;
						WriteLog("Passed\n\n");
						}
					else
						{
						$NumberOfFailed++;
						WriteLog("Failed\n\n");
						}
					}
				else
					{
					$NumberOfFailed++;
					WriteLog("Failed\n\n");
					}
				}
			
				
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			}
	   unlink("$datfile.log");
		}

	}

#
# Main
#
# Create environment and control test flow to testing Dumpswicertstoretool.EXE 
#

unlink($logFile);
WriteLog("DumpSWICertstoreTool test.\n\n");

#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;


#                file name,   			expected , expected log,            Title
#						result     message
#               ---------------------------------------------------------------------------------------------------- 
@TestItems = (	["somecapabilities.dat", 	0,    , "Created",		"Test for certstore with certificates having some capablities"],
		["AllCapabilities.dat", 	0,    , "Created",		"Test for certstore with some certificates granting  all capabilities, some granting no capabilities and some granting few capabilities"],
		["somecapabilities.dat", 	0,    , "Created",		"Test for certstore with some certificate (few mandatory flag set and few not set)"],
		["\\epoc32\\winscw\\c\\resource\\swicertstore\\dat", 0,    , "Created",		"Test for writable certstore with multiple files(having corrupt file/files)"],
		["somecapabilities.dat.dat \\epoc32\\winscw\\c\\resource\\swicertstore\\dat", 0,    , "Created", "Test for certstore with Certificates in C: and not in Z: (Merge)"],
		["NoMandatory.dat.dat \\epoc32\\winscw\\c\\resource\\swicertstore\\dat", 0,    , "Created",	"Test for certstore with Certificates in Z: and not in C: (Merge)"],
		["somecapabilities.dat.dat \\epoc32\\winscw\\c\\resource\\swicertstore\\dat", 0,    , "Created", "Test for Certificates in both certstore with different settings (capability and mandatory)(Merge))"],
		["corrupted.dat", 		1024,    , "Verification failed",		"Corrupted certstore"],
		["unicodecertificatename.dat", 	0,    , "Created",		"Test for unicode certificate names"],
		["somecapabilities.dat.dat", 	0,    , "Created",		"Test of tool with y and d option for capabilities having text information "],
		["suflagsupport.dat", 	0,    , "Created",		"Test for certstore with certificates having su flag set"],
	     );




#
# Do test for each elements of TestItems array
#
for my $Test ( @TestItems )  
	{
	$testid = sprintf "SEC-SWI-I-CERTSTORE-000%d\n",$NumberOfTests+1;
	WriteLog($testid);
	$logMsg = sprintf "%s\n", $Test->[3];
	WriteLog($logMsg);
	CertstoreFile($Test->[0], $Test->[1]);
	}

opendir(DIR, ".");
while (defined($file=readdir(DIR)))
	{
	$datfile = $file;
	$file =~ s/\.der//;
	$datfile= ~ s/\.dat//;
	if(-e "$file.der")
		{
		unlink("$file.der");
		}
	close(DIR);
	}

opendir(DIR, ".");
while (defined($file=readdir(DIR)))
	{
	$file =~ s/\.dat//;
	if(-e "$file.dat")
		{
		unlink("$file.dat");
		}
	close(DIR);
	}
#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

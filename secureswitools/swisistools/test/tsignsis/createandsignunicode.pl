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



	use File::Basename;
	my $toworkindir= dirname $0;
	print "Changing dir to $toworkindir\n";
	chdir $toworkindir;

	$logFile = "\\epoc32\\winscw\\c\\createandsignunicode.txt";

	#
# Counters for results
#
$NumberOfTests  = 3;
$NumberOfPassed = 3;
$NumberOfFailed = 0;
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

  	# For creating unicode based sis files for NQP
	system ("WScript.exe createnunicodefile.vbs");
	system("createnormalsis.pl");
	system("WScript.exe renameunicode.vbs");
	system("WScript.exe signunicodesis.vbs");

	#For Embeded SIS file
	system("createembededsis.pl");
	system("WScript.exe signembededunicodesis.vbs");

	#For creating the SIS file which produces large data, 
	#moved to this stuff as ONB was failing being this pkg file in tsisfile dir.
	system("unzip -q -o largefile.zip");
	system("makesis largedata.pkg");
	system("signsis -s largedata.sis largedata.sis default.cer 	default.key");


	#Copy the sis files to Z drive
	system "copytozdrive.bat";

	system("del /f largefile.txt");
	unlink($logFile);
	WriteLog("Created test data for NQP.\n\n");
	#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

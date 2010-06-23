#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# testdbtool.pl
# This perl file is used to test the dbtool used for generating database.
# @internalComponent
#

use strict;
use warnings;
use File::Path;
use File::Copy;
use Cwd;

my $LOGFD;

#
# Error Messages
#
my $KErrFileOpen 		= "Error opening file";

#
# Error Codes
#
# General errors
#
my %ErrorCode;
$ErrorCode{"EUnintializedCode"} = -1;
$ErrorCode{"ENone"} = 0;
$ErrorCode{"EEnvNotSpecified"} = 1;

# options exception
$ErrorCode{"EInvalidArgument"} = 10;
$ErrorCode{"EMandatoryOption"} = 11;
$ErrorCode{"EMutuallyExclusive"} = 12;
$ErrorCode{"EFileNotPresent"} = 13;
$ErrorCode{"EFileExists"} = 14;

# xml exceptions
$ErrorCode{"EWarning"}=20;
$ErrorCode{"EError"} = 21;
$ErrorCode{"EFatalError"} = 22;
$ErrorCode{"EParseError"} = 23;
$ErrorCode{"EResetError"} = 24;

# database exceptions
$ErrorCode{"ELibraryLoadError"}=30;
$ErrorCode{"ESqlArgumentError"} = 31;
$ErrorCode{"ESqlNotFoundError"} = 32;
$ErrorCode{"ESqlCorrupt"} = 33;


my $KEndl 			= "\n";
my $KSpace			= " ";
my $KHeader 		= "dbtool Test Suite";

my $KOption 		= "option";
my $KResult 		= "result";
my $KExpectedResult	= "Expected result";
my $KDbExists		= "checkdbexists";
my $KOutcome 		= "Outcome";
my $KPass		= "PASS";
my $KFail		= "FAIL";
my $KAppPath		= "$ENV{'EPOCROOT'}epoc32\\tools\\";
my $KAppName 		= $KAppPath."dbtool.exe";

my $KPath		= "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\tswi\\tdbtool\\";
my $KInputPath	= $KPath."data\\";

my $numTotal = 0;
my $numPassed = 0;
my $numFailed = 0; 

my @id;
my @dbpath = 0;
my @option;
my @result;


################## functions ##################


sub main()
	{
	InitLogFile();
	# Display header
	LogString($KHeader);
	LogString($KEndl);
	TestDbTool();

	my $run = "Run: $numTotal\n";
	my $passed = "Passed: $numPassed\n";
	my $summary = "$numFailed tests failed out of $numTotal\n";
	LogString($run);
	LogString($passed);
	LogString($summary);
	}

sub InitLogFile()
	{
	my $KLogPath = "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\";
	if(! -e $KLogPath )
	{
	mkdir($KLogPath);
	}
	my $KLogFileName	= $KLogPath."tdbtool.txt";
	# Open Log file in append mode
	unless(open LOGFD, ">>$KLogFileName")
		{
		print STDOUT $KErrFileOpen.$KSpace.$KLogFileName.$KEndl;
		exit;
		}
	}

sub TestDbTool()
	{
	my $KScriptFile = $KPath."tdbtool.ini"; 
	unless(open FD, "<$KScriptFile" )
		{
		LogString($KErrFileOpen.$KSpace.$KScriptFile);
		LogString($KEndl);
        return;
		}

	GetInput();
	close FD;

	my $i = 0;
	my $id;
	my %Input;

	while($id[$i])
	{
        $Input{$id[$i]}{$KOption} = $option[$i];
        $Input{$id[$i]}{$KResult} = $result[$i];
		$Input{$id[$i]}{$KDbExists} = $dbpath[$i];
		$i = $i + 1;
	}
	
	my $completecommand;
	my $testresult;
	my $expected;

    for $id ( sort keys % Input )
		{
		$numTotal++;
		$completecommand = $KAppName.$Input{$id}{$KOption};
		$testresult = system($completecommand);
 		$testresult = $testresult >> 8;
		$testresult = $testresult;
		LogString($KEndl."Test ID = $id $KEndl");
       	LogString("$KOption = $Input{$id}{$KOption} $KEndl");
		LogString("Complete command: ".$completecommand.$KEndl);
       	
		$expected = $Input{$id}{$KResult};
		my $expectedErrorCode = $ErrorCode{$expected};

		LogString("$KExpectedResult = ".$expectedErrorCode." ($Input{$id}{$KResult})".$KEndl);
                
		LogString("Actual Result:".$testresult.$KEndl);

		LogString("$KOutcome = ");
        
		if($testresult == $expectedErrorCode)
        	{
			if($Input{$id}{$KDbExists} )
				{
				my $db = $Input{$id}{$KDbExists};
				if( -e "$db")
					{
					LogString($KPass);
					$numPassed++;
					}
				else
					{
					LogString($KFail);
					$numFailed++;
					}
				}
			else
				{
				LogString($KPass);
				$numPassed++;
				}
			}
        else
        	{
           LogString($KFail);
		   $numFailed++;
        	}
		LogString($KEndl);
		}


=comment until cut;
    for $id ( keys %Input  )
		{
        	print "---------------------", $KEndl;
		    print "Test ID = $id", $KEndl;
            print "$KOption = $Input{$id}{$KOption}", $KEndl;
            print "$KResult = $Input{$id}{$KResult}", $KEndl;
		}
	print $KEndl, "END OF FILE", $KEndl;
=cut

}

# Takes a string as it's argument and replaces every instance of %EPOCROOT% in the argument with the relevant environment variable's value.
sub ReplaceEpocDir
	{
	my $string_args = $_[0];
	my $path_prefix = $ENV{'EPOCROOT'};
	
	while ()
		{
		last if !($string_args =~ s/%EPOCROOT%/$path_prefix/);
		}
	return $string_args;
	}

sub GetInput()
	{
	# File descriptor passed by the callee
		
	my $i = 0;
	my $startPos;
	my $endPos;

	while(<FD>)
		{
		# Replaces all instances of %EPOCROOT% in $_.
		$_ = ReplaceEpocDir($_);
		if ($_ !~/^[\t ]*#/)
			{
			if($_ =~/^\[.*\]/)
				{
                $startPos = rindex $_, "[";
                $startPos = $startPos + 1;
                $endPos = rindex $_, "]";
                $id[$i] = substr $_, $startPos, $endPos - $startPos;
				}
			
			if($_ =~/^$KDbExists/)
				{
				$startPos = rindex $_, "=";
                $startPos = $startPos + 1;
                $dbpath[$i] = substr $_, $startPos;
				$dbpath[$i]=~s/\s//g;
				chomp($dbpath[$i]);
				}

			if($_ =~/^$KOption/)
				{
				$startPos = rindex $_, "=";
                $startPos = $startPos + 1;
                $option[$i] = substr $_, $startPos;
				chomp($option[$i]);
				}
			
			if($_ =~/^$KResult/)
				{
				$startPos = rindex $_, "=";
                if($startPos eq -1)
                    {
                    die "Invalid result type";
                    }
				$startPos = $startPos + 1 ;
                $result[$i] = substr $_, $startPos;
				chomp($result[$i]);
    			$i++;
				}

			} # end of if
		} # end of while
	} # end of GetInput()


sub CreateFile()
	{
	
	my $filename = $_[0];
	unless(open FILEFD, ">$filename")
		{
		LogString($KErrFileOpen.$KSpace.$filename);
		LogString($KEndl);
		die;
		}
	
	close FILEFD;	
	}




sub LogString()
	{
	my @strBuffer = @_;
	print LOGFD @strBuffer;
	print STDOUT @strBuffer;
	}

main();

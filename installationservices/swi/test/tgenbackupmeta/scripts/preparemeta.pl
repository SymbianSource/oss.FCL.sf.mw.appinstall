#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This perl file is used to test and generate meta-data for sis files.
# @internalComponent
#

use strict;
use warnings;
use File::Path;
use File::Copy;
use Cwd;

my $LOGFD;
my $PLATFORM;
my $BUILD;


#
# Error Messages
#
my $KErrFileOpen 		= "Error opening file";
my $KErrInvalidOption 	= "Invalid options provided";


#
# Error Codes
#
# General errors
#
my %ErrorCode;
$ErrorCode{"ESuccess"}	 				= 0;
$ErrorCode{"ESystemError"} 				= -1;
$ErrorCode{"EParameterRepeat"}			= -2;
$ErrorCode{"EInvalidOrder"} 			= -3;
$ErrorCode{"EMandatoryOptions"} 		= -4;
$ErrorCode{"EInvalidOptions"} 			= -5;
$ErrorCode{"EInvalidFormat"} 			= -6;
$ErrorCode{"EInvalidParameters"} 		= -7;
$ErrorCode{"ENoValidFile"} 				= -8;
$ErrorCode{"EFileCompression"} 			= -9;
$ErrorCode{"EFileErrorWhileWriting"}	= -10;
$ErrorCode{"EFileErrorWhileReading"}	= -11;
$ErrorCode{"EFileFormatError"} 			= -12;
$ErrorCode{"EFileCannotOpenForWrite"}	= -13;
$ErrorCode{"EFileCannotOpenForRead"}	= -14;
$ErrorCode{"EUnexpectedValue"} 	   		= -15;
$ErrorCode{"EDirNotPresent"} 			= -16;
$ErrorCode{"EFileNotPresent"} 			= -17;


my $KEndl 			= "\n";
my $KSpace			= " ";
my $KHeader 		= "Genbackupmeta Test Suite";

my $KSisName		= "sisname";
my $KNumSis			= "numsis";
my $KSisData		= "sisdata";
my $KFileName		= "filename";
my $KNumFile		= "numfile";
my $KFileData		= "filedata";
my $KOption 		= "option";
my $KResult 		= "result";
my $KExpectedResult	= "Expected result";
my $KOutcome 		= "Outcome";
my $KPass			= "PASS";
my $KFail			= "FAIL";
my $KAppPath		= "$ENV{'EPOCROOT'}epoc32\\RELEASE\\winc\\DEB\\";
my $KAppName 		= $KAppPath."genbackupmeta";

my $KPath			= "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\swi\\test\\tgenbackupmeta\\scripts\\";
my $KInputDir		= $KPath."input";
my $KOutputPath		= "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\tswi\\tgenbackupmeta\\scripts\\";
my $KMetaDir		= $KOutputPath."metadata";

my $KDataDir = $KOutputPath."data";
my @id;
my @testcase;
my @result;
my @sisname;
my @numsis;
my @filename;
my @numfile;



################## functions ##################


sub main()
	{
	$PLATFORM = $ARGV[0];
	$BUILD = $ARGV[1];
	if(!defined($BUILD) || !defined($PLATFORM))
	{
	print STDOUT "Parameters required: preparemeta.pl BUILD<winscw> PLATFORM<udeb/urel>".$KEndl;
	exit;
	}

	
	InitEnv();
	# Display header
	LogString($KHeader);
	LogString($KEndl);
	CopyFiles();
	GenBackupMeta();
	CleanEnv();
	}

sub InitEnv()
	{
	my $KInputDirBuild = $KInputDir."\\".$PLATFORM."_".$BUILD;
	my $KMetaDirBuild = $KMetaDir."\\".$PLATFORM."_".$BUILD;
	my $KDataDirBuild = $KDataDir."\\".$PLATFORM."_".$BUILD;
	InitLogFile();
	if(! -e $KDataDir)
	{
	mkdir($KDataDir);
	}
	opendir(DIR,$KDataDirBuild) or mkdir($KDataDirBuild);
	closedir(DIR);
	if(! -e $KInputDir)
	{
	mkdir($KInputDir);
	}
	opendir(DIR,$KInputDirBuild) or mkdir($KInputDirBuild);
	closedir(DIR);
	
	if(! -e $KMetaDir)
	{
	mkdir($KMetaDir);
	}
	opendir(DIR,$KMetaDirBuild) or mkdir($KMetaDirBuild);
	closedir(DIR);
	}

sub InitLogFile()
	{
	my $KLogPath = "$ENV{'EPOCROOT'}epoc32\\WINSCW\\C\\logs\\";
	my $KLogPathMeta = "$ENV{'EPOCROOT'}epoc32\\WINSCW\\C\\logs\\genbackupmeta\\";
	if(! -e $KLogPath )
	{
	mkdir($KLogPath);
	}
	if(! -e $KLogPathMeta)
	{
	mkdir($KLogPathMeta);
	}
	my $KLogFileName	= $KLogPathMeta."genbackupmeta.log";
	# Open Log file in append mode
	unless(open LOGFD, ">>$KLogFileName")
		{
		print STDOUT $KErrFileOpen.$KSpace.$KLogFileName.$KEndl;
		exit;
		}
	}

sub CleanEnv()
	{
	if(-e $KInputDir)
		{
		rmtree($KInputDir);
		}

	close LOGFD;
	}

sub GenBackupMeta()
	{
	my $KScriptFile = $KPath."backupmeta"."_".$BUILD.".ini"; 
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
        $Input{$id[$i]}{$KOption} = $testcase[$i];
        $Input{$id[$i]}{$KResult} = $result[$i];
		$i = $i + 1;
	}
	
	my $completecommand;
	my $testresult;
	my $expected;

    for $id ( sort keys % Input )
		{
		LogString("Test ID = $id $KEndl");
       	LogString("$KOption = $Input{$id}{$KOption} $KEndl");
       	LogString("$KExpectedResult = $Input{$id}{$KResult} $KEndl");
		$completecommand = $KAppName.$Input{$id}{$KOption};
		LogString("Complete command: ".$completecommand);
		$testresult = system("cd $KPath & ".$completecommand);
 		$testresult = system("cd $KPath");
		$testresult = system($completecommand);
		$testresult = $testresult >> 8;
		$testresult = -$testresult;
        
		$expected = $Input{$id}{$KResult};
        
		LogString("$KOutcome = ");
        
		if($testresult == $ErrorCode{$expected})
        	{
            LogString($KPass);
       		}
        else
        	{
           LogString($KFail);
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

# Takes a string as it's argument and replaces every instance of %SECURITYSOURCEDIR% in the argument with the relevant environment variable's value.
sub ReplaceSourceDir
	{
	my $string_args = $_[0];
	my $path_prefix = $ENV{'SECURITYSOURCEDIR'};
	
	# The genbackupmeta tool seems to have difficulties dealing with pathnames that lack the drive letter, so if missing from SECURITYSOURCEDIR add it here.
	unless ($path_prefix =~ /^\D:/)
		{
		my $current_dir = getcwd();
		$current_dir =~ /^(\D:)/;	# $1 = driveletter and semicolon
		$path_prefix = $1 . $ENV{'SECURITYSOURCEDIR'};
		}
	
	# There may be multiple instances of %SECURITYSOURCEDIR% in $string_args, all of them need to be replaced.	
	while ()
		{
		last if !($string_args =~ s/%SECURITYSOURCEDIR%/$path_prefix/);
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
		# Replaces all instances of %SECURITYSOURCEDIR% in $_.
		$_ = ReplaceSourceDir($_);
		if ($_ !~/^[\t ]*#/)
			{
			if($_ =~/^\[.*\]/i)
				{
                    $startPos = rindex $_, "[";
                    $startPos = $startPos + 1;
                    $endPos = rindex $_, "]";
                    $id[$i] = substr $_, $startPos, $endPos - $startPos;
				}
			
			if($_ =~/^$KSisName/i)
				{
				    $startPos = rindex $_, "=";
                    $startPos = $startPos + 1;
                    $sisname[$i] = substr $_, $startPos;
					$sisname[$i]=~s/\s//g;
					chomp($sisname[$i]);
					CreateSis($sisname[$i]);
				}
			
			if($_ =~/^$KNumSis/i)
				{
				    $startPos = rindex $_, "=";
                    $startPos = $startPos + 1;
                    $numsis[$i] = substr $_, $startPos;
					$numsis[$i]=~s/\s//g;
					chomp($numsis[$i]);
					if( $numsis[$i] > 0 )
						{
						AddSisData($sisname[$i],$numsis[$i]);
						}
				}
			
			if($_ =~/^$KFileName/i)
				{
				    $startPos = rindex $_, "=";
                    $startPos = $startPos + 1;
                    $filename[$i] = substr $_, $startPos;
					$filename[$i]=~s/\s//g;
					chomp($filename[$i]);
					CreateFile($filename[$i]);
				}
			
			if($_ =~/^$KNumFile/i)
				{
				    $startPos = rindex $_, "=";
                    $startPos = $startPos + 1;
                    $numfile[$i] = substr $_, $startPos;
					$numfile[$i]=~s/\s//g;
					chomp($numfile[$i]);
					if( $numfile[$i] > 0 )
						{
						AddFileData($filename[$i],$numfile[$i]);
						}
				}

			if($_ =~/^$KOption/i)
				{
				    $startPos = rindex $_, "=";
                    $startPos = $startPos + 1;
                    $testcase[$i] = substr $_, $startPos;
					chomp($testcase[$i]);
				}
			
			if($_ =~/^$KResult/i)
				{
				    $startPos = rindex $_, "E";
                    if($startPos eq -1)
                    	{
                        die "Invalid result type";
                    	}
                    $result[$i] = substr $_, $startPos;
					chomp($result[$i]);
    				$i++;
				}

			} # end of if
		} # end of while
	} # end of GetInput()


sub CreateSis()
	{
	
	my $sisname = $_[0];
	unless(open SISFD, ">$sisname")
		{
		LogString($KErrFileOpen.$KSpace.$sisname);
		LogString($KEndl);
		die;
		}
		
	close SISFD;
	
	}

sub AddSisData()
	{
	
	my $sisname = $_[0];
	my $numsis	= $_[1];

	unless(open SISFD, ">>$sisname")
		{
		LogString($KErrFileOpen.$KSpace.$sisname);
		LogString($KEndl);
		die;
		}
	
	my $temp = 0;
	my $sisdata;
	my $startPos;
	my $endPos;

	while($temp != $numsis )
		{
		if( $_ = ~/^$KSisData/i )
			{
			$_ = <FD>;
			$startPos = rindex $_, "=";
            $startPos++;
            $sisdata = substr $_, $startPos;
			$sisdata=~s/\s//g;
			chomp($sisdata);
			print SISFD $sisdata;
			print SISFD $KEndl;
			}
		$temp++;
		}
		
	close SISFD;
	}

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

sub AddFileData()
	{
	
	my $filename = $_[0];
	my $numfile = $_[1];

	unless(open FILEFD, ">>$filename")
		{
		LogString($KErrFileOpen.$KSpace.$filename);
		LogString($KEndl);
		die;
		}
	
	my $temp = 0;
	my $filedata;
	my $startPos;
	my $endPos;

	while($temp != $numfile )
		{
		if( $_ = ~/^$KFileData/i )
			{
			$_ = <FD>;
			$startPos = rindex $_, "=";
            $startPos = $startPos + 1;
            $filedata = substr $_, $startPos;
			$filedata=~s/\s//g;
			chomp($filedata);
			print FILEFD $filedata;
			print FILEFD $KEndl;
			}
		
		$temp++;
		}
	close FILEFD;	
	}

sub CopyFiles
	{
	exit "EPOCROOT not defined" if !defined ($ENV{EPOCROOT});
	my $EPOC = "$ENV{EPOCROOT}epoc32\\release\\$PLATFORM\\$BUILD\\";
	my $Dest = $KDataDir."\\".$PLATFORM."_".$BUILD;
	my $CopySuccess = "Copy successful for:";
	my $CopyFailed = "Copy failed::";
	my $Src;
	my $FileName;
	my $SrcAutosigning = "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\tswi\\tautosigning\\data\\";
	my $SrcDevcerts = "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\tswi\\tdevcerts\\data\\";
	my $SrcTsis = "$ENV{'EPOCROOT'}epoc32\\winscw\\c\\tswi\\tsis\\data\\";

	
	$FileName = "file1.txt";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswidllnocapabilityprotectedvid.dll";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswinocapability.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswiprotectedsid.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswiprotectedvid.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswisystemcapability_protected.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswiusergrantable_all_protected.exe";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "wsc_exe.exe";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswiallcapabilities.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisystemcapability.exe";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisignedCap_8.EXE";
	$Src = $EPOC.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
# copying sis files
	
	$FileName = "1-2unsignedexenocap.sis";
	$Src = $SrcAutosigning.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_01.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}	
	
	$FileName = "aggregate_05.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "corrupt_01.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_01.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}


	$FileName = "mismatch_05.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_02.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "corrupt_02.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_06.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_03.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_07.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "corrupt_03.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_03.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_07.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "aggregate_04.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "corrupt_04.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "devcertwarn.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_04.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_08.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_02.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_06.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "mismatch_10.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "02_protected_vid_mismatch.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "ocsp_03.sis";
	$Src = $SrcDevcerts.$FileName;
	
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}


	$FileName = "filebase.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "filepu.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "filesp.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswiallcapabilities.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "tswisystemcapability.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisystemcapability_DSACA.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswiallcapabilities_RSACA.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisignedcap_8.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisignedcap_8_Root5.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswisignedcap_8_Root5_RSACA.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "tswinocapability_RSA.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "wsc\\capabilities.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}

	$FileName = "wsc\\doublesigned.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "testuntrustedVid.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "testuntrustedSid.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	$FileName = "testuntrustedPackage.sis";
	$Src = $SrcTsis.$FileName;
	if(copy($Src, $Dest))
		{
		LogString($CopySuccess.$KSpace.$Src.$KEndl);
		}
	else
		{
		LogString($CopyFailed.$KSpace.$Src.$KEndl);
		}
	
	}

sub LogString()
	{
	my @strBuffer = @_;
	print LOGFD @strBuffer;
	print STDOUT @strBuffer;
	}

main();

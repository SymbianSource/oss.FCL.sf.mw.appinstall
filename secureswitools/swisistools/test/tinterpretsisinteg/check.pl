#
# Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# ARGV 1 : Log File Name
# ARGV 2 : Test Id
# ARGV 3 : parameter file name (files to be searched)
# ARGV 4 : Testing (scenarios)
# ARGV 5 : positivetc (positive test case or negative)
# ARGV 6 : removedrives ("both" - remove C & Z drives, "c_only" - remove C drive only, "rom_only" - remove rom drive only, "none" - leave in current state)
#


use strict;
use File::Find;
use File::Compare;
use File::Basename;
use Cwd;

if (@ARGV <6)
	{
	print "Failed: Invalid parameters\n", join(", ",@ARGV);
	exit();
	}
# setup Path
my $CURR_WORK_DIR = getcwd();
my $CDRIVE_PATH =$CURR_WORK_DIR."\\cdrive";
my $ROMDRIVE_PATH =$CURR_WORK_DIR."\\romdrive";
my $ORIGINAL_DATA_PATH = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\data";
my $PARAMS_PATH = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\parameters_file";
my $TEST_DIR = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg";
my $testdata = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\sisfiles";
my $logs = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\logs";
my $iniFile = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\parameters_file\\parameters.ini";
my $logFile = "\\epoc32\\winscw\\c\\interpretsis_test_harness.txt";
# Process Command line arguments
my $logfilename		 =  shift @ARGV;
my $testid			 =  shift @ARGV;
my $param_filename	 =  shift @ARGV;# contains the file to be searched
my $testing			 =  shift @ARGV;# 1 (only check) 2 (check and compare) 3 (check and get text)
my $positivetc		 =  shift @ARGV;# positive/negative Test case
my $removedrives	 =  shift @ARGV;# remove drives (both/c_only/rom_only/none)
my $comparetext      =	shift @ARGV;# Text to be compared from file
my $batchfile		 =  shift @ARGV;
my $LogfileSearch	 =  shift @ARGV;# to search the logs gereated for H002 in logs folder.
my $LogFilePresent	 = 	shift @ARGV;# to search the logs gereated for H002 in logs folder.


# flags
my $searchresult;  # flag for searching file
my $passflag = 0;  # flag for pass or fail
my $debug = 0;     # flag to turn on debug log

# Variables needed by functions
my $outputfileforcompare; 
my $filename;
my @searchedpath;

if($testing eq 1)
{
	&Initialize();
	&check();
}
elsif($testing eq 2)
{
	&Initialize();
	&check();
	&CompareFileWithOriginal($ORIGINAL_DATA_PATH);
}
elsif($testing eq 3)
{
	&Initialize();
	&check();
	CheckFileText();
}

# To check  sis files which are not supported by interpretsis are not processess through makesis "-c" also.
elsif($testing eq 4)
{
	&Initialize();
	&check();
	&RemoveDir($testdata);
	&RunBatch($batchfile);
	&SisCheck();
}
# To check that Makesis will reject pre-installed packages marked as non-removable and of type PA and PP.
elsif($testing eq 5)
{
	&RunBatch($batchfile);
	&SisCheck();
}
elsif($testing eq 6)
{
	&Initialize();
	&check();
	if(!$passflag)
	{
		goto res;
	}
	
	&SearchLog($LogfileSearch,$logs);
	&RemoveDir($logs);					
}

elsif($testing eq 7)
{
	&Initialize();
	&check();
	&UnicodeFileCheck();
}

res:
&Result();

if($removedrives eq "both")
{
	&RemoveDir($CDRIVE_PATH);
	&RemoveDir($ROMDRIVE_PATH);
}
elsif($removedrives eq "c_only")
{
	&RemoveDir($CDRIVE_PATH);
}
elsif($removedrives eq "rom_only")
{
	&RemoveDir($ROMDRIVE_PATH);
}

&RemoveDir($testdata);


# Folder doesn't exist ,create it
sub Initialize()
{
	WriteLog("INFO: Initialize()\n");
	if(!-d $ROMDRIVE_PATH) 
	{
		my $dir = `mkdir $ROMDRIVE_PATH`;
	}
	if(!-d $CDRIVE_PATH) 
	{
		my $dir = `mkdir $CDRIVE_PATH`;
	}
}
# Checks for file/files
sub check()
{
	WriteLog("INFO: check($param_filename)\n");
	my @param_array = &GetParams($param_filename); 
	my $temp;
	my $filenotexist = 0;

	if ($param_filename eq 'PARAM_INI' && $#param_array == -1)
	{
	  $passflag = 1;
	  return; 
	}

	foreach $temp (@param_array)
	{		
		# Rest flags
		$searchresult = 0;
		$filenotexist = 0;
	
		$filename = lc trim($temp);
		if($filename eq "")
		{
			# Blank line found - no file to check
			$passflag = 1;
			return; 
		}

		# if the filename has been prefixed with "*", the file should not be present on the system drive (C:\)
		if(rindex($filename,"*") == 0)
		{
			$filenotexist = 1;
			$filename = substr($filename,1);
		}
		
		find (\&scan, $CDRIVE_PATH);
		if ($positivetc eq "positive") 
		{	
			# If the file is not found and the 'not exists' flag is not set OR the file is found 
			# when the 'not exists' flag is set...Fail the test
			if(($searchresult == 0 && $testing != 2 && $filenotexist == 0) || ($searchresult == 1 && $testing != 2 && $filenotexist == 1))
			{
			$passflag = 0;
			return;
			}
		}

		# The "not exists" flag is not checked within negative tests to prevent ambiguity
		if ($positivetc eq "negative" && $searchresult == 1 && $testing != 2)
		{
			$passflag = 0;
			return;
		}

		$passflag = 1;					
	}
}

sub scan
{	
	WriteLog("INFO: scan()\n");
	my $path = $File::Find::name;
	
	if (-f $path) 
	{	
		chomp($filename);
		if (basename($path) eq $filename)
		{
			$searchresult =  1;
			push @searchedpath,$File::Find::name;
			return;
		}
	}
}

# Searches for text in the file
sub CheckFileText()
{
	my @param_array = &GetParams($param_filename);
	my $file;

	WriteLog("INFO: CheckFileText($param_filename)\n");

	if($passflag eq 1)
	{
	  foreach $file (@param_array)
	    { 
	      my $filename = trim($file);
	      my @filepath  =  grep(/$filename/,@searchedpath);
	      my $output_file  =  @filepath[0];
	      WriteLog("INFO: CheckFileText $filename output ($output_file)\n");
	      open(FILE,$output_file ) || die;	
	      my $param_list  =  <FILE>;
	      close(FILE );
	      if(trim($param_list) eq trim($comparetext))
		{
		  $passflag = 1;
		  if ($positivetc eq "negative") 
		    {
		      goto res;
		    }	
		}
	      else
		{
		  $passflag = 0;
		  if ($positivetc eq "positive") 
		    {
		      goto res;
		    }			
		}
	    }
	}
	else
	{
		goto res;
	}
}



sub trim($)
{
	WriteLog("INFO: trim()\n");
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}
# Compares the original data with the installed data
sub CompareFileWithOriginal($) 
{	
	if($passflag == 1)
	{
		my @outfilespath;
		my $originalfile  =  $_[0]."/".$filename;
		$passflag = 0;
		WriteLog("INFO: CompareFileWithOriginal() originalfile = $originalfile\n");
		my $outputfileforcompare; 
		@outfilespath  =  grep(-f, @searchedpath);
		foreach $outputfileforcompare (@outfilespath)
		{	
			if(compare($outputfileforcompare,$originalfile) ==  0)
			{ 
		  		$passflag = 1;
				if($positivetc eq "negative")
				{
					 $passflag = 0;
				}
			}
			else
			{
				$passflag = 0;
				if($positivetc eq "negative")
				{
					$passflag = 1;
				}
			}
		}
	}
}


# Get the files to be searched from the parameters files.
sub GetParams($)
{
	WriteLog("INFO: GetParams($_[0])\n");
	my $file = $_[0];
	my $param_file = $PARAMS_PATH."/".$file;
	my @param_list = ();
	my $sectionFound = 0;
	if ($file eq 'PARAM_INI')
	{
		# Read the parameter .ini file
		my @iniText = ();

		if (open(INI_FILE, $iniFile))
		{
			@iniText = <INI_FILE>;
			close(INI_FILE);
		}
		
		for (my $line = 0;  $line < @iniText;  $line++)
		{
			if ($iniText[$line] =~ /^\s*\[$testid\]\s*$/i)
			{
				$sectionFound = 1;
				next;
			}
			
			if ($sectionFound && ($iniText[$line] !~ /^\s*$/))
			{
				push (@param_list, trim($iniText[$line]));
			}
			else
			{
				if ($sectionFound == 1)
				{
					return @param_list;
				}
			}
		}
	}
	else
	{
		opendir (FILE, $PARAMS_PATH) || die;
		open(FILE, $param_file) || die "can't open file $param_file $!";	
		@param_list = <FILE>;
		close(FILE);
	}
	
	return @param_list;
}
# Cleans off all the files present in the folder passed as an argument.
sub RemoveDir($)
{
	WriteLog("INFO: RemoveDir($_[0])\n");
	my $dir  =  $_[0];
	my $name;
	opendir(DIRHANDLE, $dir) || die "can't open dir $dir $!";
	foreach $name (sort readdir(DIRHANDLE))
		{
		if ($name ne "." && $name ne ".." && $name ne "indir1" && $name ne "indir2") 
			{
			if (-d "$dir/$name")
				{
					&RemoveDir("$dir/$name");
					rmdir("$dir/$name");
				} 
			else 
				{
					unlink("$dir/$name");
				}
			}
		}
	closedir(DIRHANDLE);
}

# Checks for the presence of the sis files in test data folder
sub SisCheck()
{
	WriteLog("INFO: SisCheck()\n");
opendir FILE, $testdata || die ;
my $sis = grep (/\.sis$/, readdir(FILE));
if($testing eq 5)
	{
	if ($sis == 1) 
		{
			$passflag = 1;
			return;
		}
			$passflag = 0;
			return;
	}
if ($sis)
	{ 
		$passflag = 0;
		goto res;
	}
else
	{
		$passflag = 1;
	}
}

# Executes the batch file passed as argument
sub RunBatch($)
{
my $batchfile = $_[0];
`/epoc32/winscw/c/tswi/tinterpretsisinteg/$batchfile`;
}

sub Result()
{
	if($passflag == 1)
	{
		print "Passed\n";
	}
	else
	{
		print "Failed\n";
	}
}

sub SearchLog($$)# testcase H002
{
	WriteLog("INFO: SearchLog($_[0])\n");
my $searchfile = $_[0];
my $path = $_[1];
if ($searchfile eq "")
	{
	return;
	}
opendir(D,$path) or die "Could not read directory $path!\n";
my @files = readdir(D);
closedir(D);
$passflag = 0;
while (@files) 
	{
	my $files = shift @files;
	if( $files =~ m/$searchfile/i)
		{
		$passflag = 1;
		}
	}

if($LogFilePresent eq "absent" && $passflag == 1)
	{
	$passflag = 0;
	}
if($LogFilePresent eq "absent" && $passflag == 0)
	{
	$passflag = 1;
	}
}

#
#Function to write log into file
#
sub WriteLog {
  if ($debug)
    {
      my ($log) = @_;
      my $fh;

      unless( open($fh, ">> $logFile"))
	{
	  printf STDERR "Can\'t open $logFile:$!\n";
	  return;
	}
      printf $fh $log;
      close $fh;
    }
}


sub UnicodeFileCheck()# INC124436
{
my $testval=system("WScript.exe //B //Nologo $TEST_DIR\\inc124436.vbs");
if($testval == 0)
	{
		$passflag = 0;
	}
	else
	{
		$passflag = 1;
	}
}


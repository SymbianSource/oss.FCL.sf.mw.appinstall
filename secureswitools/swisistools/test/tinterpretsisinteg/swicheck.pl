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
use File::Copy;
use Cwd;

if (@ARGV <6)
	{
	print "Failed: Invalid parameters\n", join(", ",@ARGV);
	exit();
	}
# setup Path
my $CURR_WORK_DIR = getcwd();
my $TEST_EXECUTE = "\\epoc32\\release\\WINSCW\\udeb\\testexecute.exe";
my $CDRIVE_PATH =$CURR_WORK_DIR."\\cdrive";
my $ROMDRIVE_PATH =$CURR_WORK_DIR."\\romdrive";
my $DDRIVE_PATH ="\\ddrive";
my $EDRIVE_PATH =$CURR_WORK_DIR."\\edrive";
my $ORIGINAL_DATA_PATH = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\data";
my $SWICDRIVE_PATH = "\\epoc32\\winscw\\c";
my $PARAMS_PATH = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\parameters_file";
my $testdata = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\sisfiles";
my $TEF_LOGS_PATH= "\\epoc32\\winscw\\c\\logs\\testexecute\\";
my $iniFile = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\parameters_file\\parameters.ini";
my $logFile = "\\epoc32\\winscw\\c\\interpretsis_test_harness.txt";
# Process Command line arguments
my $logfilename		 =  shift @ARGV;
my $testid			 =  shift @ARGV;
my $param_filename	 =  shift @ARGV;# contains the file to be searched
my $testing			 =  shift @ARGV;
my $positivetc		 =  shift @ARGV;# positive/negative Test case
my $removedrives	 =  shift @ARGV;# remove drives (both/c_only/rom_only/none)
my $uid				 =  shift @ARGV;
my $batchfile		 =  shift @ARGV;
my $scriptfile		 =  shift @ARGV;
my $htmlfile		 =  shift @ARGV;
my $NRsearch		 =  shift @ARGV;
my $errorcodesearch	 =  shift @ARGV;
my $comparetext      =	shift @ARGV;# Text to be compared from file
# flags
my $passflag = 0;  # flag for pass or fail
my $count;         # to keep track of results.
my @searchedpath;
my $searchresult;  # flag for searching file
my $filename;
my $debug = 0;     # flag to turn on debug log

if($testing eq 1)
{
	check($CDRIVE_PATH);

	if(!$passflag)
	{
		goto res;
	}
	&TefFunction($scriptfile);
	&CompareIntSwi();
}

elsif($testing eq 2)
{	
	&TefFunction($scriptfile);
	check($SWICDRIVE_PATH);
}
elsif($testing eq 3)
{
	&TefFunction($scriptfile); 
	$passflag = 0;
	&CheckTefLogs($htmlfile,$NRsearch,$errorcodesearch,$TEF_LOGS_PATH);
}
elsif($testing eq 4)
{	
	&TefFunction($scriptfile);
	check($SWICDRIVE_PATH);
	&CheckTefLogs($htmlfile,$NRsearch,$errorcodesearch,$TEF_LOGS_PATH);
	&Cleanup();
}
elsif($testing eq 5)
{	
	&TefFunction($scriptfile);
	check($SWICDRIVE_PATH);
	my @file = &GetParams($param_filename);
	&ReadText(@file);
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
elsif($removedrives eq "all")
{
	&RemoveDir($CDRIVE_PATH);
	&RemoveDir($ROMDRIVE_PATH);
	&RemoveDir($DDRIVE_PATH);
	&RemoveDir($EDRIVE_PATH);
}

&RemoveDir($testdata);


# Checks for file/files
sub check($)
{	
	WriteLog("INFO: check($_[0])\n");

	my $drive_path=$_[0];
	my @param_array = &GetParams($param_filename); 

	if ($param_filename eq 'PARAM_INI' && $#param_array == -1)
	{
	  $passflag = 1;
	  return; 
	}

	my $temp;
	my $filenotexist = 0;

	$searchresult = 0;
	foreach $temp (@param_array)											
	{
		# Rest flags
		$searchresult = 0;
		$filenotexist = 0;		
		
		$filename = trim($temp);
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

		find (\&scan,$drive_path);
		if ($positivetc eq "positive") 
		{	
			# If the file is not found and the 'not exists' flag is not set OR the file is found 
			# when the 'not exists' flag is set...Fail the test
			if(($searchresult == 0 && $filenotexist == 0) || ($searchresult == 1 && $filenotexist == 1))
			{
			$passflag = 0;
			return;
			}
		}
	
		# The "not exists" flag is not checked within negative tests to prevent ambiguity
		if ($positivetc eq "negative" && $searchresult == 1)
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
		
	if (-f $path && !($File::Find::name=~m/data/)) # Exclude the data folder path of harness
	{	
		chomp($filename);
		if (basename($path) eq $filename)
		{
			$searchresult =  1;
			$File::Find::name=~ s/\//\\/g;
			push @searchedpath,$File::Find::name;
			return;
		}
	}
}

# Get the files to be searched from the parameters files.
sub GetParams($)
{
	WriteLog("INFO: GetParams($_[0]\)\n");

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
		open(FILE, $param_file) || die;	
		@param_list = <FILE>;
		close(FILE);
	}
	
	return @param_list;
}



# Executes the batch file passed as argument
sub RunBatch($)
{
  WriteLog("INFO: RunBatch($_[0])\n");
  my $batchfile = $_[0];
  `/epoc32/winscw/c/tswi/tinterpretsisinteg/$batchfile`;
}



# Cleans off all the files present in the folder passed as an argument.
sub RemoveDir($)
{
	WriteLog("INFO: RemoveDir($_[0])\n");
	my $dir  =  $_[0];
	my $name;
	opendir(DIRHANDLE, $dir) || die "can't open file $dir $!";
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

sub trim($)
{
	WriteLog("INFO: trim($_[0])\n");
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}
sub CompareIntSwi()
{	
  	WriteLog("INFO: CompareIntSwi()\n");
	my @searchswi;
	foreach my $interpretsisfilepath (@searchedpath)											
	{
		my $filenameswi = basename(trim($interpretsisfilepath));
		if($filename eq "")
		{
			 return;
		}
		elsif($interpretsisfilepath=~/bin/)
		{	
			my $bin_path=$SWICDRIVE_PATH."\\sys\\bin\\".$filenameswi;	
			$bin_path=~ s/ \/ / \\ /g;
			push @searchswi,$bin_path;
		}
		elsif($interpretsisfilepath=~/hash/)
		{	
			my $hash_path=$SWICDRIVE_PATH."\\sys\\hash\\".$filenameswi;
			$hash_path=~ s/\//\\/g;
			push @searchswi,$hash_path;
		}
		elsif($interpretsisfilepath=~/$uid/)
		{	
			my $registry_path=$SWICDRIVE_PATH."\\sys\\install\\sisregistry\\".$uid."\\".$filenameswi;
			$registry_path=~ s/\//\\/g;
			push @searchswi,$registry_path;
		}
	}
    my $count=@searchswi;
    foreach my $interpretsisfilepath (@searchedpath)
	{
		foreach my $swifiles  (@searchswi)
		{
			if(compare($interpretsisfilepath,$swifiles) ==  0)
			{
				if(-s $interpretsisfilepath == -s $swifiles)
				{	
					$count--;
				}
			}
			else
			{
				$passflag = 0;
			}

		
		}	
	}
	if($count == 0)
	{
		$passflag = 1;
	}
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
sub TefFunction($)
{
	WriteLog("INFO: TefFunction($_[0])\n");
	my $script=$_[0];
	`$TEST_EXECUTE c:\\tswi\\tinterpretsisinteg\\scripts\\$script`;
}


sub CheckTefLogs
{
	WriteLog("INFO: CheckTefLogs($_[3].$_[0])\n");
	my $LOGFILE=$_[3].$_[0];
	opendir (FILE,$TEF_LOGS_PATH) || die;
	open(FILE, $LOGFILE) or die("Could not open log file or directory $!");
	foreach my $line (<FILE>)
	{
	my $NRvalue=$_[1];
	my $Errocodevalue=$_[2];
	if (($line =~/$NRvalue/) || ($line =~/$Errocodevalue/))
	{
	$passflag = 1;
	}
	}
	close(LOGFILE);

}

# Searches for text in the file
sub ReadText(@)
{
	WriteLog("INFO: ReadText($_[0])\n");

	if($passflag eq 1)
	{		
		my $file  =  $_[0];
		my @filepath  =  grep(/$file/,@searchedpath);
		my $output_file  =  @filepath[0];
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
	else
	{
		goto res;
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

# Performs any cleanup which needs to take place after a TEF script has been executed
sub Cleanup()
{
	my $cr1027RomStub = "\\epoc32\\release\\WINSCW\\udeb\\z\\system\\install\\CR1027ROMUpgradeStub.sis";
	my $cr1027RomStub_temp = "\\epoc32\\release\\WINSCW\\udeb\\z\\system\\CR1027ROMUpgradeStub.sis";
	move($cr1027RomStub_temp, $cr1027RomStub);
}


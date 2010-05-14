#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# runtests.pl is used to run Interpretsis unit tests.
# 
#

use integer;
use Cwd;
use autouse 'Win32::Process' => qw(Create INFINITE);
use File::Copy;

$INT_PATH="\\epoc32\\tools\\";
$INT_CMD ="interpretsis.exe";

#$C_DRIVE ="\\epoc32\\winscw\\c";
#$Z_DRIVE ="\\epoc32\\RELEASE\\WINSCW\\UDEB\\Z";
$C_DRIVE ="\\cdrive";
$Z_DRIVE ="\\romdrive";

$SIS_PATH="\\epoc32\\winscw\\c\\tswi\\tinterpretsis\\sis_winscw_udeb\\";
$CONFIG_PATH="\\epoc32\\winscw\\c\\tswi\\tinterpretsis\\";

$PREPARE_SIS="\\epoc32\\winscw\\c\\tswi\\tinterpretsis\\preparesis.pl winscw udeb";
$CMD = "$INT_PATH$INT_CMD";
$PARAMS =" -c $C_DRIVE -z $Z_DRIVE -w INFO -l \\epoc32\\winscw\\c\\interpretsis_test.txt -s "; 

$logFile = "\\epoc32\\winscw\\c\\interpretsis_test.txt";
$numTotal = 0;
$numPassed = 0;
$numFailed = 0; 
my $usif = 0;
#
#Function to write log into file
#
sub WriteLog {
	my ($log) = @_;

	unless( open($fh, ">> $logFile")) {
		printf STDERR "Can\'t open $logFile:$!\n";
		return;
	}
	printf $fh $log;
	printf $log;
	close $fh;
}

sub RunTest($$$) {
	my $sis = $_[0];
	my $expected = $_[1];
	my $config_file = $_[2];
	$numTotal++;
 
	$sis = join(" -s $SIS_PATH", split(',', $sis));
	WriteLog("\nInstalling $sis\n");
	
	if ($^O eq "MSWin32") {
		# do this because system() doesn't handle the return code
		# correctly on windows :-(
		sub error () {
			print Win32::FormatMessage( Win32::GetLastError() );
		}
		
		Create($ProcessObj,
			   $CMD,
			   "$PARAMS  $SIS_PATH$sis -i $CONFIG_PATH$config_file ",
			   0,
			   NORMAL_PRIORITY_CLASS,
			   ".") || die error() ;
		
		$ProcessObj->Wait(INFINITE);
		$ProcessObj->GetExitCode($y);
	} else {
		system("$CMD $PARAMS $SIS_PATH$sis -i $CONFIG_PATH$config_file");
		$y = $? >> 8;
		if ($y & 0x80) {
			$y -= 256;
		}

	}

 
	my $result = $y;
	
	# If a check case
	if ($expected == '99' )
	{
		$exists = $C_DRIVE."\\test_exists_func.txt";
		$approp = $C_DRIVE."\\test_approp_func_true.txt";
		$package = $C_DRIVE."\\test_package_func.txt";
	 	# Check to see if the files are installed or not
	 	if (-e $exists && -e $approp && -e $package)
	 	{
			WriteLog("*** Success\n\n");
			$numPassed++;	 	
	 	} 
	 	else
	 	{
			WriteLog("--- Fail: test result was $result expected 0\n\n");
			$numFailed++;
	 	}
	 	
	}
	else
	{
		if ($result == $expected) {
			WriteLog("*** Success\n\n");
			$numPassed++;
		} else {
			WriteLog("--- Fail: result was $result expected $expected\n\n");
			$numFailed++;
		}
	}
}


sub RemoveDir($)
{
	my $dir = $_[0];
	opendir(DIRHANDLE, $dir) || die "can't open file $dir";
	
	foreach $name (sort readdir(DIRHANDLE)) {
		if ($name ne "." && $name ne "..") {
			if (-d "$dir/$name") {
				&RemoveDir("$dir/$name");
				rmdir("$dir/$name");
			} else {
				unlink("$dir/$name");
			}
		}
	}
	closedir(DIRHANDLE);
	rmdir($dir);
}

sub PrepareSis()
{
	# Run
	system "$PREPARE_SIS";
}

sub CopyScrDb()
{
	if($usif eq "usif")
	{
		$EpocRom = "\\epoc32\\release\\winscw\\udeb\\z";
		$ScrPath = "\\sys\\install\\scr\\provisioned\\scr.db";
		$ScrDbPath = $EpocRom.$ScrPath;
		$NewPath = $Z_DRIVE.$ScrPath;
		
		copy($ScrDbPath, $NewPath);
		WriteLog("Copying DB from $ScrDbPath to $NewPath\n");
	}
}

sub SetUpDbEnv()
{
	if($usif eq "usif")
	{
		my $current_dir = getcwd();
		chdir($Z_DRIVE);
		mkdir("sys");
		chdir("sys");
		mkdir("install");
		chdir("install");
		mkdir("scr");
		chdir("scr");
		mkdir("provisioned");
		chdir $current_dir;
		CopyScrDb();
	}
}
# if these test cases are being run for the new symbian universal software installer
# then copy the scr database to the specified location in the rom drive.
$usif="usif" if ($ARGV[0] =~ /\s*usif\s*/i);

open(G, ">$logFile");
close(G);

PrepareSis();

mkdir($C_DRIVE);
mkdir($Z_DRIVE);
SetUpDbEnv();

open(F, ">$C_DRIVE/i_am_an_orphan.txt");
print F "orphan I am\n";
close(F);

# plain exe & private dir text file should pass
RunTest("exe_s.sis",0, "config_normal.ini");

if ($usif)
{
	# Appropriate error message should be generated if SQLite library is not present
	my $sqlLiteLib = "\\epoc32\\tools\\sqlite3.dll";
	my $sqlLiteLibBackup = "\\epoc32\\tools\\sqlite3.dll.test.backup";

	if (-e $sqlLiteLib) 
	{
		rename($sqlLiteLib,$sqlLiteLibBackup);
	}

	RunTest("exe_s.sis",-43, "config_normal.ini");

	if (-e $sqlLiteLibBackup) 
	{
		rename($sqlLiteLibBackup,$sqlLiteLib);
	}

	# Appropriate error message should be generated if SQLite library is 
	# present but not in one of the paths mentioned in PATH environment variable.
	my $sqlLiteLibMovePath = "\\epoc32\\winscw\\c\\tswi\\sqlite3.dll";

	if (-e $sqlLiteLib) 
	{
		move($sqlLiteLib,$sqlLiteLibMovePath);
	}

	RunTest("exe_s.sis",-43, "config_normal.ini");

	if (-e $sqlLiteLibMovePath) 
	{
		move($sqlLiteLibMovePath,$sqlLiteLib);	
	}

	# In case of multiple occurences of SQLite library, verify that the one
	# present in the location mentioned in the PATH environment variable is taken.
	
	open(FH,">$sqlLiteLibMovePath"); # create an empty file
	close(FH);
	RunTest("exe_s.sis",0, "config_normal.ini");
	unlink $sqlLiteLibMovePath; # remove the empty file


	# Appropriate error message should be generated if "scrtool.exe" library is not present
	my $scrTool = "\\epoc32\\tools\\scrtool.exe";
	my $scrToolBackup = "\\epoc32\\tools\\scrtool.exe.test.backup";

	if (-e $scrTool) 
	{
		rename($scrTool,$scrToolBackup);
	}

	RunTest("exe_s.sis",-42, "config_normal.ini");

	if (-e $scrToolBackup) 
	{
		rename($scrToolBackup,$scrTool);
	}

	# Appropriate error message should be generated if "scrtool.exe" is 
	# present but not in one of the paths mentioned in PATH environment variable.
	my $scrToolMovePath = "\\epoc32\\winscw\\c\\tswi\\scrtool.exe";

	if (-e $scrTool) 
	{
		move($scrTool,$scrToolMovePath);
	}

	RunTest("exe_s.sis",-42, "config_normal.ini");

	if (-e $scrToolMovePath) 
	{
		move($scrToolMovePath,$scrTool);	
	}

	# In case of multiple occurences of "scrtool.exe", verify that the one
	# present in the location mentioned in the PATH environment variable is taken.
	open(FH,">$scrToolMovePath"); # create an empty file
	close(FH);
	RunTest("exe_s.sis",0, "config_normal.ini");
	unlink $sqlLiteLibMovePath; # remove the empty file
}

# reinstall should pass
RunTest("exe_s.sis",0, "config_normal.ini");

# patch should pass
RunTest("patch_s.sis",0, "config_normal.ini");

# partial upgrade should pass - exe uses different SID
RunTest("partial_s.sis",0, "config_normal.ini");

# writing to someone else's private dir 
RunTest("private_fail_s.sis", -4, "config_normal.ini");

# overwrite an orphaned file
RunTest("overwrite_s.sis", 0, "config_normal.ini");

# clean the directory for the next batch of tests
RemoveDir($C_DRIVE);
mkdir($C_DRIVE);
CopyScrDb();

# plain exe & private dir text file should pass
RunTest("exe_s.sis",0, "config_normal.ini");

# attempt to eclipse
RunTest("eclipse_s.sis", -9, "config_normal.ini");

# duplicate sid 
RunTest("duplicate_sid_s.sis", -9, "config_normal.ini");

# dll which has a duplicated sid as the exe in exe_s.sis 
RunTest("dll_exe_dup_sid_s.sis", -9, "config_normal.ini");

# dll which has a duplicated sid as the dll in dll_exe_dup_sid_s.sis 
RunTest("dll_dll_dup_sid_s.sis", -9, "config_normal.ini");

# depend on exe
RunTest("depend1_s.sis", 0, "config_normal.ini");

# depend on depend1
RunTest("depend2_s.sis", 0, "config_normal.ini");

# depend on depend1 and depend2
RunTest("depend3_s.sis", 0, "config_normal.ini");

# depend on depend1 and none existent package
RunTest("depend4_s.sis", -1, "config_normal.ini");

# mutually dependent
RunTest("depend5_s.sis,depend6_s.sis", -1, "config_normal.ini");

# base
RunTest("functions_base_s.sis", 0, "config_normal.ini");

# exists
RunTest("functions_s.sis", 99, "config_normal.ini");

# plain text file, with non-critical error in config file; should pass
RunTest("testconfig_s.sis",0, "config_warn.ini");

# stub sis file dependancy
$ROM_STUB_SYS="\\romdrive\\system";
$ROM_STUB_INS="\\romdrive\\system\\install";
mkdir($ROM_STUB_SYS);
mkdir($ROM_STUB_INS);
copy ("\\epoc32\\winscw\\c\\tswi\\tinterpretsis\\pkg\\stub_file.sis","\\romdrive\\system\\install\\stub_file.sis");
copy ("\\epoc32\\winscw\\c\\tswi\\tinterpretsis\\pkg\\2000stub.sis", "\\romdrive\\system\\install\\2000stub.sis");
RunTest("stub_file_depend_s.sis", 0, "config_normal.ini");

RemoveDir($C_DRIVE);
mkdir($C_DRIVE);

# delete file on uninstall
RunTest("fn_flag_s.sis", 0, "config_normal.ini");

RemoveDir($C_DRIVE);
RemoveDir($Z_DRIVE);

$run = "Run: $numTotal\n";
$passed = "Passed: $numPassed\n";
$summary = "$numFailed tests failed out of $numTotal\n";
WriteLog($run);
WriteLog($passed);
WriteLog($summary);

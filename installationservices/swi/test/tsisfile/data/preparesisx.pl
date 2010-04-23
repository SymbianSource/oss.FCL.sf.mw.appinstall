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

use Cwd;
use strict;
use File::Basename;

# firstly, get the current date
# DATEARRAY[0] will be the day, DATEARRAY[1] will be the date - eg 01/12/2002
#my @DATEARRAY = split(/ /, `date /t`);

# now change the date to something before the certificates expire
#system("date 01/01/2002");


BEGIN { my $key = "EPOCROOT";
	my $epocroot = $ENV{"EPOCROOT"};
	push (@INC, $epocroot."epoc32/tools/"); }

use E32Variant; 

 my @variantMacros = Variant_GetMacroList();
 my $found = "";

 foreach my $item (@variantMacros)
   {
     #print "$item\n";
     if ($item =~ "__SECURITY_PLATSEC_ARCH__")
       {
 	print "FOUND\n";
 	$found = "true";
       }
   }

# if ($found)
#   {
#     print "OK\n";
#   }
#exit;

# add an _epoc_drive_d into epoc.ini
if (open(EPOCINI, ">> \\epoc32\\data\\epoc.ini"))
{
    print EPOCINI "\n_epoc_drive_d \\epoc32\\drive_d\n";
    close(EPOCINI);
}

# Change it back again
#system("date $DATEARRAY[1]");

# Build test scripts

my $realdir = "scripts";
chdir ($realdir);

if( ! (-e $realdir && -d $realdir))
{
    mkdir($realdir,0777);
}

my $testscript = "tsis.script";
my $testini = "tsis.ini";
my $testsisby = "tsistestdata.iby";
my $testsignedsisiby = "tsissignedtestdata.iby";

unlink $testscript;
unlink $testini;
unlink $testsisby;
unlink $testsignedsisiby;

if( not open( iby, ">$testsisby" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testsisby (we are in $dir)\n");
    #die;
  }

if( not open( signediby, ">$testsignedsisiby" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testsignedsisiby (we are in $dir)\n");
    #die;
  }


printf (iby "REM SISX Parsing test data\n\n") ;
printf (iby "define ZDRIVE \\epoc32\\winscw\\c \n \n");
printf (iby "data = ZDRIVE\\tswi\\tsis\\scripts\\$testscript \t\t tswi\\tsis\\scripts\\$testscript\n");
printf (iby "data = ZDRIVE\\tswi\\tsis\\scripts\\$testini \t\t tswi\\tsis\\scripts\\$testini\n\n");


if( not open( script, ">$testscript" ) )
{
    my $dir = cwd();
    print( "\nUnable to open $testscript (we are in $dir)\n");
    #die;
}

printf (script "LOAD_SUITE tsis -SharedData\n") ;

if( not open( ini, ">$testini" ) )
{
    my $dir = cwd();
    print( "\nUnable to open $testini (we are in $dir)\n");
    #die;
}

my $dir = "../data/";

opendir DIR, $dir;

my @pkgfiles = grep (/\.pkg/, readdir(DIR));

closedir DIR;

my $entry;
my $fullini = "z:\\tswi\\tsis\\scripts\\".$testini;


printf (script "\nPRINT");

my $UniqueTestIDPrefix = "API-SEC-SISXParser-"; 
my $UniqueTestID = 1;

foreach $entry (@pkgfiles) 
{
	# These SIS files are signed by trusted certificates so the tsis script should except EValidationSucceeded
    my @signed_prefixes = ("testuninstall_", "testspnr_");
    $entry =~ s/\.pkg//;
	
    printf (script "\n//! \@file");
    printf (script "\n//! \@SYMTestCaseID $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
    printf (script "\n//! \@SYMTestCaseDesc Recognizing, parsing and verifying $entry.sis");
    printf (script "\n//! \@SYMTestPriority Critical");
    printf (script "\n//! \@SYMTestActions Recognized, parsers and verifies $entry.sis");
    printf (script "\n//! \@SYMTestExpectedResults KErrNone, ESignatureNotPresent");
    (my $Second, my $Minute, my $Hour, my $Day, my $Month, my $Year, my $WeekDay, my $DayOfYear, my $IsDST) = localtime(time);
    $Year += 1900;$Month += 1;
    printf (script "\n//! \@SYMCreationDate ".sprintf("%02d",$Day)."/".sprintf("%02d",$Month)."/$Year");

    printf (script "\nSTART_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
    printf (script "\nRUN_TEST_STEP 100 tsis RecognizeStep $fullini $entry");
    printf (script "\nRUN_TEST_STEP 100 tsis ParseStep $fullini $entry");
    printf (script "\nRUN_TEST_STEP 100 tsis VerifyIntegrityStep $fullini $entry");
    printf (script "\nRUN_TEST_STEP 120 tsis VerifyIntegrityStepNewParser $fullini $entry");
    printf (script "\nRUN_TEST_STEP 100 tsis VerifySignatureStep $fullini $entry");
    printf (script "\nEND_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
    printf (script "\nPRINT\n");
    $UniqueTestID++;

    printf (ini "\n[$entry]\n");    
    printf (ini "filename = $entry.sis\n");    
    printf (ini "result1 = 0 // KErrNone \n");
	
	my $validationSucceeded = 0;
	foreach my $exception (@signed_prefixes) {
		if (index($entry, $exception) == 0) {
			$validationSucceeded = 1;
		}
	}
	
    if ( $validationSucceeded )  { 
		printf (ini "validationresult = 0 // EValidationSucceeded\n");	
    } 
	else {
		printf (ini "validationresult = 1 // ESignatureSelfSigned\n");     	
    }
	
    printf (iby "data = ZDRIVE\\tswi\\tsis\\data\\$entry.sis \t\t tswi\\tsis\\data\\$entry.sis\n");
}

printf (iby "data = ZDRIVE\\tswi\\tsis\\data\\expected_license_text.txt \t\t tswi\\tsis\\data\\expected_license_text.txt\n");


close script;
close ini;

############################################################################################
############################################################################################


print( "\nGenerating Signed SISX files...\n");

my $testscript = "tsis_signed.script";
my $testini = "tsis_signed.ini";

unlink $testscript;
unlink $testini;

if( not open( script, ">$testscript" ) )
{
    my $dir = cwd();
    print( "\nUnable to open $testscript (we are in $dir)\n");
    #die;
}

printf (script "LOAD_SUITE tsis -SharedData\n") ;

#
# Make sure the correct certstore is in place!
#

printf (script "\n") ;
printf (script "RUN_UTILS MkDir c:\\tmp\\\n") ;
printf (script "RUN_UTILS MkDir c:\\tmp\\tsis\\\n") ;

if ($found)
  {
    printf ("found");
    printf (script "\n") ;
    printf (script "RUN_UTILS MkDir c:\\private\\101f72a6\\\n") ;
    printf (script "RUN_UTILS CopyFile c:\\private\\101f72a6\\cacerts.dat c:\\tmp\\tsis\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile c:\\private\\101f72a6\\certclients.dat c:\\tmp\\tsis\\certclients.dat\n") ;

    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\cacerts.dat c:\\private\\101f72a6\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\certclients.dat c:\\private\\101f72a6\\certclients.dat\n") ;
  }
else
  {
    printf ("NOT found");
    printf (script "\n") ;
    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\cacerts.dat c:\\tmp\\tsis\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\certclients.dat c:\\tmp\\tsis\\certclients.dat\n") ;

    printf (script "RUN_UTILS MkDir c:\\System\\Data\\\n") ;
    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\cacerts.dat c:\\System\\Data\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile z:\\tswi\\tsis\\data\\certstore\\certclients.dat c:\\System\\Data\\certclients.dat\n") ;
  }

printf (script "PRINT\n") ;

if( not open( ini, ">$testini" ) )
{
    my $dir = cwd();
    print( "\nUnable to open $testini (we are in $dir)\n");
    #die;
}

my $dir = "../data/tobesigned";

opendir DIR, $dir;

my @pkgfiles = grep (/\.pkg/, readdir(DIR));

closedir DIR;

my $entry;
my $fullini = "z:\\tswi\\tsis\\scripts\\".$testini;

my $UniqueTestIDPrefix = "API-SEC-SecurityManager-"; 
my $UniqueTestID = 1;

printf (iby "data = ZDRIVE\\tswi\\tsis\\scripts\\$testscript \t\t tswi\\tsis\\scripts\\$testscript\n");
printf (iby "data = ZDRIVE\\tswi\\tsis\\scripts\\$testini \t\t tswi\\tsis\\scripts\\$testini\n\n");

foreach $entry (@pkgfiles) 
{
    $entry =~ s/\.pkg//;
    my ($j, $directory, @roots, $root_dir);
    @roots = qw(Root5CA CADSA CARSA canotinstore);
    
    ROOTDIR: for $root_dir (@roots)
    {
	for ($j=1; $j <= 3; $j++)
	{
	    # We only need one sis file with the signing root not in the swicertstore.
	    next ROOTDIR if (($root_dir =~ /canotinstore/) && (($j != 1) || ($entry !~ /logo/i)));

	    $directory = "$root_dir\\rsa_len$j";
	    my ($data) = $root_dir . $entry . "_rsa_l$j";
	    my ($dataName) = $entry . "_rsa_l$j";
	    
	    printf (script "\n//! \@file");
	    printf (script "\n//! \@SYMTestCaseID $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	    printf (script "\n//! \@SYMTestCaseDesc Verifying the RSA signed SISX file $entry.sis");
	    printf (script "\n//! \@SYMTestPriority Critical");
	    printf (script "\n//! \@SYMTestActions Verifies the RSA signed SISX file $entry.sis");
	    printf (script "\n//! \@SYMTestExpectedResults EValidationSucceded");
	    (my $Second, my $Minute, my $Hour, my $Day, my $Month, my $Year, my $WeekDay, my $DayOfYear, my $IsDST) = localtime(time);
	    $Year += 1900;$Month += 1;
	    printf (script "\n//! \@SYMCreationDate ".sprintf("%02d",$Day)."/".sprintf("%02d",$Month)."/$Year");
	    
	    printf (script "\nSTART_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	   # printf (script "\nRUN_TEST_STEP 100 tsis ParseStep $fullini $data");
	   # printf (script "\nRUN_TEST_STEP 100 tsis VerifyIntegrityStep $fullini $data");
	    printf (script "\nRUN_TEST_STEP 100 tsis VerifySignatureStep $fullini $data");
	    printf (script "\nEND_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	    printf (script "\nPRINT\n\n");

	    $UniqueTestID++;
	    printf (ini "\n[$data]\n");
	    printf (ini "filename = $directory\\$dataName.sis\n");
	    if ($root_dir =~ /canotinstore/)
		{
		printf (ini "result1 = 0 // KErrNone \n");    
		printf (ini "validationresult = 1 //ESignatureSelfSigned \n");    
		}
	    else
		{
		printf (ini "result1 = 0 // KErrNone \n");    
		printf (ini "validationresult = 0 //EValidationSucceded \n");    
		}

	    if (($dataName =~ /testnonremovable/) or ($dataName =~ /testremovable/))
	    {
	        printf (signediby "#ifdef __SECURITY_NORMAL_SWICERTSTORE__\n");
	    	printf (signediby "data = ZDRIVE\\tswi\\tsis\\data\\$directory\\$dataName.sis \t\t tswi\\tsis\\data\\$directory\\$dataName.sis\n");
		printf (signediby "#endif\n");        	
    	    } 
    	    else
    	    {
	    	printf (signediby "data = ZDRIVE\\tswi\\tsis\\data\\$directory\\$dataName.sis \t\t tswi\\tsis\\data\\$directory\\$dataName.sis\n");
    	    }

	    # Skip DSA for test sis file with signing root not in the swicertstore.
	    next ROOTDIR if ($root_dir =~ /canotinstore/);
	    $directory = "$root_dir\\dsa_len$j";
	    $data =~ s/rsa/dsa/;
	    $dataName =~ s/rsa/dsa/;

	    printf (script "\n//! \@file");
	    printf (script "\n//! \@SYMTestCaseID $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	    printf (script "\n//! \@SYMTestCaseDesc Verifying the DSA signed SISX file $entry.sis");
	    printf (script "\n//! \@SYMTestPriority Critical");
	    printf (script "\n//! \@SYMTestActions Verifies the DSA signed SISX file $entry.sis");
	    printf (script "\n//! \@SYMTestExpectedResults EValidationSucceded");
	    (my $Second, my $Minute, my $Hour, my $Day, my $Month, my $Year, my $WeekDay, my $DayOfYear, my $IsDST) = localtime(time);
	    $Year += 1900;$Month += 1;
	    printf (script "\n//! \@SYMCreationDate ".sprintf("%02d",$Day)."/".sprintf("%02d",$Month)."/$Year");

	    printf (script "\nSTART_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	  #  printf (script "\nRUN_TEST_STEP 100 tsis ParseStep $fullini $data");
	  #  printf (script "\nRUN_TEST_STEP 100 tsis VerifyIntegrityStep $fullini $data");
	    printf (script "\nRUN_TEST_STEP 100 tsis VerifySignatureStep $fullini $data");
	    printf (script "\nEND_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID));
	    printf (script "\nPRINT\n\n");

	    $UniqueTestID++;
	    printf (ini "\n[$data]\n");
	    printf (ini "filename = $directory\\$dataName.sis\n");
	    printf (ini "result1 = 0 // KErrNone \n");    
	    printf (ini "validationresult = 0 //EValidationSucceded \n");  
	    
	    if (($dataName =~ /testnonremovable/) or ($dataName =~ /testremovable/))
	    {
	        printf (signediby "#ifdef __SECURITY_NORMAL_SWICERTSTORE__\n");
	        printf (signediby "data = ZDRIVE\\tswi\\tsis\\data\\$directory\\$dataName.sis \t\t tswi\\tsis\\data\\$directory\\$dataName.sis\n");
	        printf (signediby "#endif\n");        	
    	    }
    	    else
    	    {
	    	printf (signediby "data = ZDRIVE\\tswi\\tsis\\data\\$directory\\$dataName.sis \t\t tswi\\tsis\\data\\$directory\\$dataName.sis\n");
 	    }
 	}
    }
}

if ($found)
  {
    printf (script "\n") ;
    printf (script "RUN_UTILS CopyFile c:\\tmp\\tsis\\cacerts.dat c:\\private\\101f72a6\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile c:\\tmp\\tsis\\certclients.dat c:\\private\\101f72a6\\certclients.dat\n") ;
	printf (script "RUN_UTILS DeleteFile c:\\tmp\\tsis\\cacerts.dat\n") ;
	printf (script "RUN_UTILS DeleteFile c:\\tmp\\tsis\\certclients.dat\n") ;
  }
else
  {
    printf (script "\n") ;
    printf (script "RUN_UTILS CopyFile c:\\tmp\\tsis\\cacerts.dat c:\\tswi\\tsis\\data\\certstore\\cacerts.dat\n") ;
    printf (script "RUN_UTILS CopyFile c:\\tmp\\tsis\\certclients.dat c:\\tswi\\tsis\\data\\certstore\\certclients.dat\n") ;
  }

close script;
close ini;
close signediby;
############################################################################################
############################################################################################

my $scriptdir = cwd();

# execute the makeall batch file - make sure we are in the correct path
chdir("../data");

print( "\nScript generation completed except for tsis_failure scripts. Generating SISX files...\n");

system "makeall.bat";

chdir("$scriptdir");

#my $output = `makeall.bat`;
#print $output;

############################################################################################

print( "\nGenerating Failure Testing SISX scripts...\n");

my %PackageExcludeList = (tabincomment => 'tabincomment', testrun=> 'testrun', 
		     unique=> 'unique', vendor=> 'vendor');


my $testscript = "tsis_failure.script";
my $hwtestscript = "tsis_failure_hardware.script";
my $testini = "tsis_failure.ini";
my $hwtestini = "tsis_failure_hardware.ini";
my $testcorruptsisby = "tsiscorrupttestdata.iby";
my $testLscript = "tsis_large.script";
my $testLini = "tsis_large.ini";
my $fullLini = "c:\\tswi\\tsis\\scripts\\".$testLini;

# Since the test data for this test take up more space than available on the ROMs
# We limit the testing on hardware to SIS files under 1K.
my $maxHwSisSize = 1024;

unlink $testcorruptsisby;
unlink $testscript;
unlink $hwtestscript;
unlink $testini;
unlink $hwtestini;

if( not open( ciby, ">$testcorruptsisby" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testcorruptsisby (we are in $dir)\n");
    #die;
  }

if( not open( script, ">$testscript" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testscript (we are in $dir)\n");
    #die;
  }

if( not open( Lscript, ">$testLscript" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testscript (we are in $dir)\n");
    #die;
  }

if( not open( hwscript, ">$hwtestscript" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $hwtestscript (we are in $dir)\n");
    #die;
  }

printf (ciby "REM SISX Parsing *Failure* test data\n") ;
printf (ciby "REM WARNING This file can greatly increase the rom size\n\n") ;
printf (ciby "define ZDRIVE \\epoc32\\winscw\\c \n \n");

printf (ciby "data = ZDRIVE\\tswi\\tsis\\scripts\\$hwtestscript \t\t tswi\\tsis\\scripts\\$testscript\n");
printf (ciby "data = ZDRIVE\\tswi\\tsis\\scripts\\$hwtestini \t\t tswi\\tsis\\scripts\\$testini\n\n");

printf (script "LOAD_SUITE tsis -SharedData\n") ;
printf (Lscript "LOAD_SUITE tsis -SharedData\n") ;

printf (hwscript "LOAD_SUITE tsis -SharedData\n") ;

if( not open( ini, ">$testini" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $testini (we are in $dir)\n");
    #die;
  }

if( not open( hwini, ">$hwtestini" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $hwtestini (we are in $dir)\n");
    #die;
  }

if( not open( Lini, ">$testLini" ) )
  {
    my $dir = cwd();
    print( "\nUnable to open $hwtestini (we are in $dir)\n");
    #die;
  }

my $file  = "../data/pkg.config";

open(HANDLE, $file) || die("Could not open file!");
my @pkgfiles=<HANDLE>;

close(HANDLE);
my $entry;
my $fullini = "z:\\tswi\\tsis\\scripts\\".$testini;
my $corruptswitch = "bA";
my $corruptdir = "corrupted".$corruptswitch;
my $UniqueTestIDPrefix = "API-SEC-SISXParserFailure-"; 
my $UniqueTestID = 0;

(my $Second, my $Minute, my $Hour, my $Day, my $Month, my $Year, my $WeekDay, my $DayOfYear, my $IsDST) = localtime(time);
$Year += 1900;$Month += 1;
my $DateString = "\n//! \@SYMCreationDate ".sprintf("%02d",$Day)."/".sprintf("%02d",$Month)."/$Year";

my %TestcaseHeaderandResults = ();

push(@{$TestcaseHeaderandResults{bA}}, (
			"Attempt to parse corrupted SISX files with bad array counts.",
			"Parse corrupted SISX files with bad array counts.",
			"EBugArrayCount",
			0,
			"ParseStep",
			(-10141,-10143,-20)));

push(@{$TestcaseHeaderandResults{bB}}, (
			"Attempt to parse corrupted SISX files with insane blobs.",
			"Parse corrupted SISX files with insane blobs.",
			"EBugInsaneBlob",
			1,
			"ParseStep",
			(-10102,-20,-10108)));

push(@{$TestcaseHeaderandResults{bF}}, (
			"Attempt to parse corrupted SISX files with duff field types.",
			"Parse corrupted SISX files with duff field types.",
			"EBugDuffFieldType",
			0,
			"ParseStep",
			(-10139)));

push(@{$TestcaseHeaderandResults{bH}}, (
			"Attempt to verify the integrity of a corrupted SISX files with bad hashes.",
			"Verify corrupted SISX files with bad hashes.",
			"EBugHashError",
			1,
			"VerifyIntegrityStep",
			(0,-10253)));

push(@{$TestcaseHeaderandResults{bK}}, (
			"Attempt to parse a corrupted SISX files with unknown SIS data.",
			"Parse corrupted SISX files with unknown SIS data.",
			"EBugUnknownData",
			0,
			"ParseStep",
			(0,-20,-10139)));

push(@{$TestcaseHeaderandResults{bL}}, (
			"Attempt to parse a corrupted SISX files with bad field lengths.",
			"Parse corrupted SISX files with bad field lengths.",
			"EBugInvalidLength",
			0,
			"ParseStep",
			(-10102,-10139)));

push(@{$TestcaseHeaderandResults{bM}}, (
			"Attempt to parse a corrupted SISX files with missing fields.",
			"Parse corrupted SISX files with missing fields.",
			"EBugMissingField",
			0,
			"ParseStep",
			(-10139)));

push(@{$TestcaseHeaderandResults{bN}}, (
			"Attempt to parse a corrupted SISX files with negative field lengths.",
			"Parse corrupted SISX files with negative field lengths.",
			"EBugNegativeLength",
			0,
			"ParseStep",
			(-10102)));

push(@{$TestcaseHeaderandResults{bS}}, (
			"Attempt to parse a corrupted SISX files with insane strings.",
			"Parse corrupted SISX files with insane strings.",
			"EBugInsaneString",
			0,
			"ParseStep",
			(-10143,-20,-10108,-10139)));

push(@{$TestcaseHeaderandResults{bT}}, (
			"Attempt to parse a SISX file which encodes short lengths in 64 bits instead of 32.",
			"Parse SISX files with length enncoded as 64 bits.",
			"EBug32As64",
			0,
			"ParseStep",
			(0)));

push(@{$TestcaseHeaderandResults{bU}}, (
			"Attempt to parse a corrupted SISX files with unexpected SIS fields.",
			"Parse corrupted SISX files with unexpected SIS fields.",
			"EBugUnexpectedField",
			0,
			"ParseStep",
			(-10139)));

push(@{$TestcaseHeaderandResults{bX}}, (
			"Attempt to parse a corrupted SISX files with unknown SIS fields.",
			"Parse corrupted SISX files with unknown SIS fields.",
			"EBugUnknownField",
			0,
			"ParseStep",
			(-10139)));




my %ResultsHash = (0 => "KErrNone",
		-9 => "KErrOverflow",
		-20 => "KErrCorrupt",
		-10102 => "KErrSISFieldLengthInvalid",
		-10108 => "KErrSISFieldBufferTooShort",
		-10139 => "KErrSISUnexpectedFieldType",
		-10141 => "KErrSISArrayReadError",
		-10142 => "KErrSISArrayTypeMismatch",
		-10143 => "KErrSISInvalidStringLength",
		-10253 => "KErrBadHash");

			

my $corruptswitch;
my @largepkg = ("testbignc.pkg");
push(@pkgfiles, @largepkg);

foreach $corruptswitch (sort(keys %TestcaseHeaderandResults))
	{


	my $corruptdir="corrupted".$corruptswitch;
	(my $testdescription, my $testactions, my $bugtype, my $useexcludelist, my $steptype, my @expectedresults) = @{$TestcaseHeaderandResults{$corruptswitch}};

	$UniqueTestID++;
	my $header = "\n//! \@file".
			"\n//! \@SYMTestCaseID $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID).
			"\n//! \@SYMTestCaseDesc ".$testdescription.
			" The corrupted files have been generated by makesis using the -$corruptswitch command line switch.".
			"\n//! \@SYMTestPriority Critical".
			"\n//! \@SYMTestActions $testactions".
			"\n//! \@SYMTestExpectedResults";

	# DEF069580 : removes preinstalled files test from BH only
	my @newarray;    
	if($corruptswitch =~ m/bH/)
         	{
		@newarray = (@pkgfiles);
		@pkgfiles = grep(!/testpreinstalled_patch/, @pkgfiles);
		@pkgfiles = grep(!/testpreinstalled_patch2/, @pkgfiles);
		@pkgfiles = grep(!/testpreinstalled_install/, @pkgfiles);
		}
	my $iniresults = "";
	my $resultcount = 1;
	for my $resultkey (@expectedresults)
		{
		if ($resultcount > 1)
			{
			$header = $header."\,";
			}
		$header = $header." ".$ResultsHash{$resultkey};
		$iniresults = $iniresults."result".$resultcount++." = ".$resultkey." // ".$ResultsHash{$resultkey}." \n";	
		}

	$header = $header.
		$DateString.
		"\nSTART_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID).
		"\n// ".$bugtype."SISX Parser Failure CSISFieldRoot::".$bugtype." ($corruptswitch)";

	printf(script "$header");
	$header =~ s/$UniqueTestIDPrefix/API-SEC-SISXParserFailure-HW-/g;
	printf(hwscript "$header");

	my $iniheader = "\n// ** CSISFieldRoot::".$bugtype." \n";
	printf(ini "$iniheader");
	printf(hwini "$iniheader");

	foreach $entry (@pkgfiles) 
		{
		$entry=~ s/\n//;
		$entry =~ s/\.pkg//;
		if ($PackageExcludeList{$entry} && $useexcludelist)
			{
			# This file is in the exclude list!
			}
		else
			{
				
				if($entry =~ /testbignc/)
					{
					# Tests in Lscript are to be given a different TestCaseID
					$header =~ s/API-SEC-SISXParserFailure-HW-/API-SEC-SISXParserFailure-Large-/g;
					printf(Lscript "$header");
					$header = ""; 
					my $scriptlines = "\nRUN_TEST_STEP 100 tsis $steptype $fullLini $corruptswitch$entry".
							"\nPRINT";
					printf (Lscript "$scriptlines");
						my $inilines = "\n[$corruptswitch$entry]\n".
							"filename = $corruptdir\\$entry.sis\n".
							$iniresults;
					printf (Lini "$inilines");
					}
				else
					{
					my $scriptlines = "\nRUN_TEST_STEP 100 tsis $steptype $fullini $corruptswitch$entry".
							"\nPRINT";
					printf (script "$scriptlines");
					my $inilines = "\n[$corruptswitch$entry]\n".
							"filename = $corruptdir\\$entry.sis\n".
							$iniresults;
					printf (ini "$inilines");
					my $sisfilename = $dir."/".$corruptdir."/".$entry.".sis";
					my $sisfilesize = -s $sisfilename;
					if ($sisfilesize <= $maxHwSisSize)
						{
						printf (hwscript "$scriptlines");
						printf (hwini "$inilines");
						printf (ciby "data = ZDRIVE\\tswi\\tsis\\data\\$corruptdir\\$entry.sis \t\t tswi\\tsis\\data\\$corruptdir\\$entry.sis\n");
						}	
					}		
				
			}

		}
	# DEF069580 : removes preinstalled files test from BH only
      	if($corruptswitch =~ m/bH/)
	{
	@pkgfiles = (@newarray);
	}

	my $taillines = "\nEND_TESTCASE $UniqueTestIDPrefix".sprintf ("%04d",$UniqueTestID)."\nPRINT\n\n";
	printf(script "$taillines");
	$taillines =~ s/$UniqueTestIDPrefix/API-SEC-SISXParserFailure-HW-/g;
	printf(hwscript "$taillines");
	$taillines =~ s/API-SEC-SISXParserFailure-HW-/API-SEC-SISXParserFailure-Large-/g;
	printf(Lscript "$taillines");
	}

close script;
close hwscript;
close ini;
close hwini;
close ciby;
close Lscript;
close Lini;
print( "\nGenerating Failure Testing SISX scripts: DONE\n");

system("copy $testcorruptsisby ..\\..\\..\\inc\\");
system("copy $testcorruptsisby \\epoc32\\rom\\include");
system("copy $testscript \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");
system("copy $hwtestscript \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");
system("copy $testini \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");
system("copy $hwtestini \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");
system("copy $testLscript \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");
system("copy $testLini \\epoc32\\winscw\\c\\tswi\\tsis\\scripts");


############################################################################################

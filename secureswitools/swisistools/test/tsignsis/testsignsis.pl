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
#

# Perl script that test SignSIS tools
use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;

$logFile = "/epoc32/winscw/c/signsis_test.txt";


#
# Function to create a file (test.txt or any PKG)
#
sub CreateFile {
	my ($filename, $contents) = @_;

	unless (open($fh, "> $filename")) {
		printf STDERR "Can't open $filename: $!\n";
		return;
	}
	print $fh $contents;
	close $fh;


}

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
# Run SIGNSIS with prepared sis file, log and administrate its result
#
sub SignSISFile 
	{
	my ($sisfile) = @_[0];
	my ($expectedResult) = @_[1];
	$sisfile =~ s/\.sis//;
	$neededfolder="";
	$neededfile="";
	$emptylinecertsisfile="./data/emptylinescert.signed.sis";
	if("$sisfile.signed.sis" eq $emptylinecertsisfile)
		{
		 @retval = system("/epoc32/tools/signsis -o -p $sisfile.signed.sis > $sisfile.log");
		 $logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		 WriteLog( $logMsg);
		 if ( $^O =~ /^MSWIN32$/i )
			{
			 $result = `findstr /C:"Issuer" $sisfile.log`;
		 	}
		 else
			{
			 $result = system(`grep "Issuer" $sisfile.log`) ;
			}
		}
	else
		{
		@retval = system("/epoc32/tools/signsis -o $sisfile.sis > $sisfile.log");
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);
		if ( $^O =~ /^MSWIN32$/i )
			{
			 $result = `findstr /C:"Issued by" $sisfile.log `;
		 	}
		 else
			{
			 $result = system(`grep "Issued by" $sisfile.log`);
			}
		@retval = system("/epoc32/tools/signsis -o -p $sisfile.sis > $sisfile.log");
		}
		
	$neededfolder="Chain"; 
	$neededfile .= "$neededfolder"."/". "cert1.pem";
	$NumberOfTests++;
	if( $? == $expectedResult && -e $neededfile && -s $neededfile && length($result)>0 ) 
		{
		if("$sisfile.signed.sis" eq $emptylinecertsisfile)
			{
			my $res=`diff "./data/emptylinescertandkey.out" "$sisfile.log"`;
			if(length($res) == 0)
				{
				$NumberOfPassed++;
				WriteLog("Signing successful\n\n");
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
			$NumberOfPassed++;
			WriteLog("Passed\n\n");
			}
		}
	elsif($? == $expectedResult)
		{
		 if("$sisfile.sis" eq "simple.sis" ||"$sisfile.sis" eq "test04.sis")
			{
			if ( $^O =~ /^MSWIN32$/i )
				{
				 $result = `findstr /C:"No primary signatures" $sisfile.log `;
			 	}
		 	else
				{
				 $result = system(`grep "No primary signatures" $sisfile.log`);
				}
			if(length($result)>0)
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
			$NumberOfPassed++;
			WriteLog("Passed\n\n");		
			}
		}
	else
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink("$sisfile.log");
	unlink("$sisfile.signed.sis");
	opendir(DIR, $neededfolder);
	while (defined($file=readdir(DIR)))
		{
		unlink("$neededfolder"."/"."$file");
		} 
	close(DIR);
	}

#
# New test code for DEF093156 - SIGNSIS does not process correctly non European symbols in cmd line 
# This test checks whether the pkg file with japanese symbols is signed by signsis successfully .
#
sub TestDEF093156 {

	my $path = "\\epoc32\\winscw\\c\\tswi\\tsignsis";
 	$DEF093156Log = "DEF093156.log";
 	$ResFile="ResultSignSIS.txt";

	WriteLog("Test for DEF093156 - Test for successful creation of a signed sis file with japanese symbols created by signsis.\n");
  
 	# sign a pkg with japanese symbols using a certificate and a file with japanese sym,bols
	system("WScript.exe //B //Nologo $path\\testsignsis.vbs > $DEF093156Log");

 	$NumberOfTests++;
	if ( -f $ResFile )
	{
		$NumberOfPassed++;
 		WriteLog("Passed\n\n");
 	}
	else
	{
 		$NumberOfFailed++;
 		WriteLog("Failed\n\n");
	}

 	unlink $DEF093156Log;
	unlink $ResFile;
}


#
# New test code for DEF093156 - SIGNSIS does not process correctly non European symbols in cmd line 
# This test checks whether the pkg file with japanese symbols is signed by signsis successfully .
#
sub TestLDEF093156 {

 	$DEF093156Log = "DEF093156.log";
	$ResFile="ResultSignSIS.txt";
	$SignedSISFile = "pkg_乕乖乗乘_kanji_signed.SIS";

# Data to write in the created file for Linux test 
	$TempData = "Exist";

	system("cp -f pkg_kanji.SIS  pkg_乕乖乗乘_kanji.SIS");	
 	system("cp -f kanji_key.key  丟両丣kanji_key.key");	
	system("cp -f kanji_cer.cer  丁丂七丄kanji_cer.cer");

	system("/epoc32/tools/signsis -v -s pkg_乕乖乗乘_kanji.SIS pkg_乕乖乗乘_kanji_signed.SIS  丁丂七丄kanji_cer.cer 丟両丣kanji_key.key 乕乖乗乘pass > DEF093156Log");

	WriteLog("Test for DEF093156 - Test for successful creation of a signed sis file with japanese symbols created by signsis.\n");
  
	if( -f $SignedSISFile){
		CreateFile($ResFile, $TempData);
	}		
 	
	$NumberOfTests++;
	
	if ( -f $ResFile ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	

 	unlink $DEF093156Log;
	unlink $ResFile;
}

#
# Test code for DEF111563 - SignSIS fails if an embedded package has a dependency.
# This test checks whether an embedding pkg (whose embedded pkg has a dependency ) is signed by signsis successfully.
# 

sub TestDEF111563 {

	$pkgEmbeddedFile = "Emdedded.pkg";
	$sisEmbeddedFile = "Embedded.sis";
	$pkgFile = "Emdedding.pkg";
	$sisFile = "Embedding.sis";
	$DEF111563LogFile = "DEF111563.log";

	WriteLog("DEF111563 - Test for successful creation of signed sis file for package whose embedded pkg has dependency.\n");

	# Generate test PKG file contents for embedded pkg file.
	$PkgFile = sprintf( $EmbeddedpkgFile, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgEmbeddedFile , $PkgFile);	
	
	# Create a sis file
	my $result = system("/epoc32/tools/makesis $pkgEmbeddedFile $sisEmbeddedFile > $DEF111563LogFile");

	# Sign the embedded sis file with dependency using SignSIS.
	my $result1 = system("/epoc32/tools/signsis -s $sisEmbeddedFile $sisEmbeddedFile ./signingdata/good-r5.pem ./signingdata/good-r5.key.pem > $DEF111563LogFile");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $EmbeddingpkgFile, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgFile , $PkgFile);	
	
	# Create a sis file
	my $result2 = system("/epoc32/tools/makesis $pkgFile $sisFile > $DEF111563LogFile");

	# Sign the embedding sis file using SignSIS.
	my $result3 = system("/epoc32/tools/signsis -s $sisFile $sisFile ./signingdata/good-r5.pem ./signingdata/good-r5.key.pem > $DEF111563LogFile");
	

	$NumberOfTests++;
	
	if ($result == 0 && $result1 == 0  && $result2 == 0 && $result3 == 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgEmbeddedFile; 
	unlink $sisEmbeddedFile;
	unlink $pkgFile; 
	unlink $sisFile;
	unlink $DEF111563LogFile;
}

#
# Test code for DEF138538 - Signsis crashes when displaying signing info for SIS files. 
# This test checks tat Singnsis displays the signing information for SIS files correctly 
# and doesn't fail if the subject name of the signing certificate is in the UTF8 format. 
# 

sub TestDEF138538 {

	$sisFileName = "\\epoc32\\winscw\\c\\tswi\\tsignsis\\data\\CertWithUnicodeSubjectLine.SIS";
	$DEF138538LogFile = "DEF138538.log";

	WriteLog("DEF138538 - Test if Singnsis displays the signing information for SIS file correctly.\n");

	# Create a sis file
	my $result1 = system("/epoc32/tools/signsis.exe -p -o $sisFileName > $DEF138538LogFile");

	my $result2 = 0;
	$result2 = `findstr /C:"Issuer: CN=Nokia Online CA, O=Nokia" $DEF138538LogFile`;

	$NumberOfTests++;
	
	if (($result1 == 0) && $result2) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	
	unlink $DEF138538LogFile;
}

#
# Package template string to generate an embedded PKG file having a dependency.
#
$EmbeddedpkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Test Embedded\"}, (0x01001235), 1, 1, 1, TYPE=SA
;
%{\"Symbian Software Ltd\"}
:\"Symbian Software Ltd\"
;
;Installation name and header data
(0x101f74a0),%s,{\"Test Embedded\"}
;
(0x01001225), *, *, *, {\"embedded2\"}
;
";

#
# Package template string to generate an embedding PKG file whose embedded pkg has a dependency.
#
$EmbeddingpkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Test Application SA1\"}, (0x01001234), 1, 0, 1, TYPE=SA
;
%{\"Symbian Software Ltd\"}
:\"Symbian Software Ltd\"
;
;Installation name and header data
(0x01001234),%s,{\"Test Application SA1\"}
;
(0x01001235), *, *, *, {\"embedded\"}
;
\@\"embedded.sis\" , (0x01001235)
;
";

#
# Main
#
# Create environment and control test flow to testing SIGNSIS.EXE 
# 
#

unlink($logFile);
WriteLog("SIGNSIS test.\n\n");

#
# Counters for results
#
$NumberOfTests  = 0;
$NumberOfPassed = 0;
$NumberOfFailed = 0;

#
# Windows "FindStr" only understand "\" for path to read the log file
#
if ( $^O =~ /^MSWIN32$/i )
{

#
# Array of contents of test sis file and expected results
#
#                file name,   expected , expected log,            Title
#				result     message
#               ---------------------------------------------------------------------------------------------------- 
@TestItems = (	["simple.signed.sis", 0,    , "Created",		"Test for normal Signed sis file"],
		["simple.sis", 0,    , "Created",		"Test for Unsigned sis file"],
		["data\\signedctltest.sis", 0,    , "Created",		"Test for Stub sis controller file"],
		["data\\pkgromctltest.sis", 0,    , "Created",		"Test for Stub sis controller for package file in ROM"],
		["data\\nonsisfile.sis", 256,    , "Verification Failed",	"Test for Non sis or non Stub sis controller file"],
		["data\\corrupted.sis", 256,    , "Verification Failed",	"Corrupted sis file"],
		["data\\eshell_hybrid.sis", 256,    , "Verification Failed",	"Stored and computed hash values do not match"],
		["data\\emptylinescert.sis", 0,    , "Created",	"Test for successful signing when certificate(.cer) and key has few empty lines."],
	     );

}

# Linux tests :

else

{

#
# Array of contents of test sis file and expected results
#
#                file name,   expected , expected log,            Title
#				result     message
#               ---------------------------------------------------------------------------------------------------- 
@TestItems = (	["simple.signed.sis", 0,    , "Created",		"Test for normal Signed sis file"],
		["simple.sis", 0,    , "Created",		"Test for Unsigned sis file"],
		["data/signedctltest.sis", 0,    , "Created",		"Test for Stub sis controller file"],
		["data/pkgromctltest.sis", 0,    , "Created",		"Test for Stub sis controller for package file in ROM"],
		["data/nonsisfile.sis", 256,    , "Verification Failed",	"Test for Non sis or non Stub sis controller file"],
		["data/corrupted.sis", 256,    , "Verification Failed",	"Corrupted sis file"],
		["data/eshell_hybrid.sis", 256,    , "Verification Failed",	"Stored and computed hash values do not match"],
		["data/emptylinescert.sis", 0,    , "Created",	"Test for successful signing when certificate(.cer) and key has few empty lines."],
	     );


}

#
# Do test for each elements of TestItems array
#
	$datapath = "data/";
	opendir(DIR,"./data/");
	my @pkgfiles = readdir DIR;
	foreach my $files (@pkgfiles)
		{
		$files =~ s/\.pkg//;

		$pkgfilepath =  "$datapath"."$files";
		$pkgfile = "./data/emptylinescert.sis";
		if(-e "$pkgfilepath.pkg")
			{
			@retval = system("makesis -v $pkgfilepath.pkg $files.sis > $files.log");
		    	if( $? != 0)
				{
		    		WriteLog("\n\nProblem While Creating makesis for $files.pkg\n");
			    	}	
	    		else
				{
				WriteLog("\n\nCreated $files.sis file from $files.pkg\n");
				WriteLog("\n\nSigning $files.sis using certificate\n\n");
				@retval = system("/epoc32/tools/signsis -s $pkgfilepath.sis $files.signed.sis ./signingdata/good-r5.pem 						./signingdata/good-r5.key.pem");
				if( $? != 0)
					{
					WriteLog("\n\nProblem While signing $files.sis\n");
					}
		    		}
			}

		if($pkgfilepath  eq $pkgfile)
			{
			$files =~ s/\.sis//;
			$signedsispath = "data\\" . $files;
			@retval = system("/epoc32/tools/signsis -s $pkgfilepath $signedsispath.signed.sis ./signingdata/emptylinescert.pem  ./signingdata/emptylinescert.key.pem");
			}
		}

for my $Test ( @TestItems )  
	{
	# Do SIGNSIS test
	$testid = sprintf "SEC-SWI-I-SIGNSIS-000%d\n",$NumberOfTests+1;
	WriteLog($testid);
	$logMsg = sprintf "%s", $Test->[3];
	WriteLog($logMsg);
	WriteLog("\t$Test->[0]\n");
	SignSISFile($Test->[0], $Test->[1]);
	}

opendir(DIR, ".");
while (defined($file=readdir(DIR)))
	{
	$file =~ s/\.sis//;
	if(-e "$file.sis")
		{
		unlink("$file.sis");
		unlink("$file.log");
		}
	rmdir("chain");
	}
close(DIR);


#
# Test for DEF093156.
#
if ($^O =~ /^MSWIN32$/i)
	{
	 TestDEF093156();
	}
if ($^O =~ /^LINUX$/i)
	{
	TestLDEF093156();
	}

#
# Test for DEF111563.
#
TestDEF111563();


#
# Test for DEF138538 .
#
TestDEF138538 ();


#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

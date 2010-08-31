#
# Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Perl script that creates PKG files and test DumpSIS tools with different ranges
#

use File::Basename;
my $scriptdir= dirname $0;
print "Changing dir to $scriptdir\n";
chdir $scriptdir;

my $runMode = $ARGV[0];

# Set the log file based on the invocation parameter
if($runMode eq "usif")
{
$logFile = "\\epoc32\\winscw\\c\\dumpsis_test_usif.txt";
}
else
{
$logFile = "\\epoc32\\winscw\\c\\dumpsis_test.txt";
}

#Main
# Create environment and control test flow to testing DumpSIS.EXE 
# 
#

unlink($logFile);
WriteLog("DUMPSIS test.\n\n");

#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;


CreateMakeSIS();

#
# Array of test data used for each individual test iteration which contains details of the test pkgs and their expected results
#
#		<file name>	 	-	Path of the SIS file being used as a parameter to DumpSIS
#		<expected result>	-	Expected error code result returned from the DumpSIS tool
#		<expected log cmdline>	-	Expected log output to the command line as a result of the test
#		<expected log pkg>	-	Expected string to be present within the PKG file dumped by DumpSIS ("-" if not checked)
#		<title>			-	Title used to identify the individual test in the Results Log 	
#
# 		Array Format:  [<file name>,<expected result>,<expected log cmdline>,<expected log pkg>,<title>]
#               ------------------------------------------------------------------------------------------------------------------------------------ 
@TestItems = (	["simple.sis",			0,	"-",	"-",	"Test for normal sis file"],
		["package\\CtlTest.sis",	0,	"Dumping Stub SIS Controller",	"-",	"Test for Stub SIS Controller"],
		["StubTest.sis",		0,	"-",	"-",	"Test for Stub SIS file"],
		["DepTest.sis",			0,	"-",	"-",	"Test for Dependency file"],
		["TestFileNullE.sis",		0,	"-",	"-",	"Test for file null sysbin file"],
		["TestFileNullB.sis",		0,	"-",	"-",	"Test for file null private file"],
		["TestFileNullC.sis",		0,	"-",	"-",	"Test for file null resource file"],
		["TestFileNullD.sis",		0,	"-",	"-",	"Test for file null restore"],
		["TestFileNullA.sis",		0,	"-",	"-",	"Test for file null import"],
		["Embsis0.sis", 		0,	"-",	"-",	"Test for Embedded sis file"],
		["Embsis1.sis",			0,	"-",	"-",	"Test for Embedded sis file"],
		["EmbTest.sis",			0,	"-",	"-",	"Test for Embedded sis file"],
		["MultiLangTest.sis",		0,	"-",	"-",	"Test for multiple language"],
		["CondTest.sis",		0,	"-",	"-",	"Test for If Else condition"],
		["UnicodeTest.sis",		0,	"-",	"-",	"Test for Unicode sis file"],
		["CondTestElseif.sis",		0,	"-",	"-",	"Test for ElseIf condition"],
		["OprTest.sis",			0,	"-",	"-",	"Test for operator condition"],
		["LogoTest.sis",		0,	"-",	"-",	"Test for MIME sis file"],
		["package\\CtlTestROM.sis",	0,	"Dumping Stub SIS Controller",	"-",	"Test for Stub sis controller with package in ROM"],
		["package\\TestCtlEmb.sis",	0,	"Dumping Stub SIS Controller",	"-",	"Test for Embedded Stub sis controller "],
		["package\\ExeTest.sis",	0,	"-",	"-",	"Test for 1 WINSCW and 2 ARM Executable sis file"],
		["test20.sis",			256,	"-",	"-",	"Test for corrupted sis file"],
		["test21.sis",			256,	"-",	"-",	"Test for corrupted stub sis controller file"],
		["test22.sis",			256,	"-",	"-",	"Test for corrupted stub sis file"],
		["package\\appv11.sis",		256,	"-",	"-",	"Test for old sis file format"],
		["simple_RBS.sis",		0,	"-",	"-",	"Test for RBS flag"],
		["simple_rrrbs.sis",		0,	"-",	"-",	"Test for RR,RBS flag"],
        ["simple_frra.sis",		0,	"-",	"\"file0\"-\"!:\\Documents\\InstTest\\file1.txt\", FR, RI, RA",	"Test for FR,RA flag"],
        ["simple_frra_v.sis",		0,	"-",	"\"file0\"-\"!:\\Documents\\InstTest\\file0.txt\", FR, RI, RA",	"Test for FR,RUNAFTERINSTALL flag"],
	     ); 


# CR1125 - Add Package Versions to SIS File Conditionals Test Cases
# Array of test PKG data and expected results for each test case and associated iterations
#
#		<file name>	 	-	Path of the SIS file being used as a parameter to DumpSIS
#		<expected result>	-	Expected error code result returned from the DumpSIS tool
#		<expected log cmdline>	-	Expected log output to the command line as a result of the test
#		<initial condition>	-	The condition used to construct the "IF" statement within the PKG File and perform a specific test 
#						on the Tool 
#		<expected condition>	-	Expected "IF" statement condition string to be present within the PKG file dumped by DumpSIS
#		<title>			-	Title used to identify the individual test in the Results Log 	
#
#
# 		Array Format:  [<file name>,<expected result>,<expected log cmdline>,<initial condition>,<expected condition>,<title>]	
#               ---------------------------------------------------------------------------------------------------------------------------------------- 
@TestItems1 = (	["pkgversion_c001_a.sis",	0,	"-",	"VERSION(0xE1000001,=,1,2,3)",	"VERSION(0xE1000001,=,1,2,3)",	"SEC-SWI-PKGVER-C001 : A"],

		["pkgversion_c001_b.sis", 	0, 	"-",	"VERSION(0xE1000001,<>,1,2,3)",	"VERSION(0xE1000001,<>,1,2,3)",	"SEC-SWI-PKGVER-C001 : B"],

		["pkgversion_c001_c.sis", 	0, 	"-",	"VERSION(0xE1000001,>,1,2,3)",	"VERSION(0xE1000001,>,1,2,3)",	"SEC-SWI-PKGVER-C001 : C"],

		["pkgversion_c001_d.sis", 	0, 	"-",	"VERSION(0xE1000001,>=,1,2,3)", "VERSION(0xE1000001,>=,1,2,3)",	"SEC-SWI-PKGVER-C001 : D"],

		["pkgversion_c001_e.sis", 	0, 	"-",	"VERSION(0xE1000001,<,1,2,3)",	"VERSION(0xE1000001,<,1,2,3)",	"SEC-SWI-PKGVER-C001 : E"],

		["pkgversion_c001_f.sis", 	0, 	"-",	"VERSION(0xE1000001,<=,1,2,3)",	"VERSION(0xE1000001,<=,1,2,3)",	"SEC-SWI-PKGVER-C001 : F"],
		
		["pkgversion_c002_a.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,3\")",	"VERSION(0xE1000001,=,1,2,3)",	"SEC-SWI-PKGVER-C002 : A"],

		["pkgversion_c002_b.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,NE,1,2,3\")",	"VERSION(0xE1000001,<>,1,2,3)",	"SEC-SWI-PKGVER-C002 : B"],

		["pkgversion_c002_c.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,GT,1,2,3\")",	"VERSION(0xE1000001,>,1,2,3)",	"SEC-SWI-PKGVER-C002 : C"],

		["pkgversion_c002_d.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,GE,1,2,3\")",	"VERSION(0xE1000001,>=,1,2,3)",	"SEC-SWI-PKGVER-C002 : D"],

		["pkgversion_c002_e.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,LT,1,2,3\")",	"VERSION(0xE1000001,<,1,2,3)",	"SEC-SWI-PKGVER-C002 : E"],

		["pkgversion_c002_f.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,LE,1,2,3\")",	"VERSION(0xE1000001,<=,1,2,3)",	"SEC-SWI-PKGVER-C002 : F"],

		["pkgversion_c003_a.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?test,ET,1,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?test,ET,1,2,3\")",	"SEC-SWI-PKGVER-C003 : A"],

		["pkgversion_c003_b.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,test,1,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,test,1,2,3\")",	"SEC-SWI-PKGVER-C003 : B"],

		["pkgversion_c003_c.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,test,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,test,2,3\")",	"SEC-SWI-PKGVER-C003 : C"],

		["pkgversion_c003_d.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,test,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,test,3\")",	"SEC-SWI-PKGVER-C003 : D"],

		["pkgversion_c003_e.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,test\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,test\")",	"SEC-SWI-PKGVER-C003 : E"],

#		Invalid test case. Test case should be moved to makesis. Or test should run on generated sis as this sis should not
#		have generated. 	
#		["pkgversion_c004_a.sis",	256,	"-",	"EXISTS(\"\")",	"-",	"SEC-SWI-PKGVER-C004 : A"],

		["pkgversion_c004_b.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?\")",	"SEC-SWI-PKGVER-C004 : B"],

		["pkgversion_c004_c.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,\")",	"SEC-SWI-PKGVER-C004 : C"],

		["pkgversion_c004_d.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,\")",	"SEC-SWI-PKGVER-C004 : D"],

		["pkgversion_c004_e.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,\")",	"SEC-SWI-PKGVER-C004 : E"],

		["pkgversion_c004_f.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,\")",	"SEC-SWI-PKGVER-C004 : F"],

		["pkgversion_c004_g.sis",	0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,3,\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE1000001,ET,1,2,3,\")",	"SEC-SWI-PKGVER-C004 : G"],

		["pkgversion_c005.sis",		0,	"-",	"VERSION(0xE1000001,=,1,2,3)",	"VERSION(0xE1000001,=,1,2,3)",	"SEC-SWI-PKGVER-C005"],

		["pkgversion_cov1.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\?0xE1000001,ET,1,2,3\")",	"EXISTS(\"\\sys\\install\\?0xE1000001,ET,1,2,3\")",	"SEC-SWI-PKGVER Coverage Test 1 - Invalid Version Condition Prefix"],

		["pkgversion_cov2.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE100,ET,1,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE100,ET,1,2,3\")",	"SEC-SWI-PKGVER Coverage Test 2 - Invalid Uid Format Test 1"],

		["pkgversion_cov3.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE01234S6,ET,1,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE01234S6,ET,1,2,3\")",	"SEC-SWI-PKGVER Coverage Test 3 - Invalid Uid Format Test 2"],

		["pkgversion_cov4.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,-2,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,-2,2,3\")",	"SEC-SWI-PKGVER Coverage Test 4 - Negative Major Component"],

		["pkgversion_cov5.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,128,2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,128,2,3\")",	"SEC-SWI-PKGVER Coverage Test 5 - Major Component > 127"],

		["pkgversion_cov6.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,-2,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,-2,3\")",	"SEC-SWI-PKGVER Coverage Test 6 - Negative Minor Component"],

		["pkgversion_cov7.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,100,3\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,100,3\")",	"SEC-SWI-PKGVER Coverage Test 7 - Minor Component > 99"],

		["pkgversion_cov8.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,2,-2\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,2,-2\")",	"SEC-SWI-PKGVER Coverage Test 8 - Negative Build Component"],

		["pkgversion_cov9.sis",		0,	"-",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,2,32768\")",	"EXISTS(\"\\sys\\install\\pkgversion\\?0xE0123456,ET,1,2,32768\")",	"SEC-SWI-PKGVER Coverage Test 9 - Build Component > 32767"],

		["pkgversion_cov10.sis",	0,	"-",	"((EXISTS(\"OddString\"))or(language=1))",	"(exists(\"OddString\"))OR((LANGUAGE)=(1))",	"SEC-SWI-PKGVER Odd String with exists() test"],

		
	     ); 

# PREQ2525 - Added SUPPORTED_LANGUAGE token to .pkg File format for supporting device supported language installation
# Array of test PKG data and expected results for each test case and associated itterations
#
#              	File name,	Supported language, Package header , Vendor name , Default vendor name ,Language block , Supported language block , Option list , Test case ID ,	Expected error code	,	Expected log commandLine , Expected value in dumped PKG file
#               -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
@TestItems2 = (	 ["test01.sis",	"EN , FR "		,	" \"Supported Language EN\" , \"Supported Language FR\" "								,	"\"Symbian Software Ltd. EN \" , \"Symbian Software Ltd. FR\""									,	" "							 ,	" "																																																					,	"IF SUPPORTED_LANGUAGE = 01 \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 02 \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF "																																																											,	" "																																																											,	"API-Inst-PREQ2525-DumpSIS-01",	0	,"-",	"IF Supported_Language = 2"					],
				 ["test02.sis",	"EN , FR , GE "	,	" \"Supported Language EN\" , \"Supported Language FR\" , \"Supported Language GE\" "	,	"\"Symbian Software Ltd. EN \" , \"Symbian Software Ltd. FR\" , \"Symbian Software Ltd. GE\""	,	": \"Symbian Software Ltd.\"",	" "																																																					,	"IF LANGUAGE = 01 AND SUPPORTED_LANGUAGE = 01  \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 02 \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF "																																																											,	" "																																																											,	"API-Inst-PREQ2525-DumpSIS-02",	0	,"-",	"IF ((LANGUAGE)=(1))AND(Supported_Language = 1)"					],
				 ["test03.sis",	"EN , FR , GE "	,	" \"Supported Language EN\" , \"Supported Language FR\" , \"Supported Language GE\" "	,	"\"Symbian Software Ltd. EN \" , \"Symbian Software Ltd. FR\" , \"Symbian Software Ltd. GE\""	,	": \"Symbian Software Ltd.\"",	" "																																																					,	"IF exists(\"\\sys\\install\\supportedlanguage\\?01\") \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 02 \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF \n\nIF SUPPORTED_LANGUAGE = 03 \n\n\"SupportedLanguageGE.txt\"-\"!:\\sys\\bin\\SupportedLanguageGE.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 157 \n\n\"SupportedLanguageYW.txt\"-\"!:\\sys\\bin\\SupportedLanguageYW.txt\" \n\nENDIF"	,	" "																																																											,	"API-Inst-PREQ2525-DumpSIS-03",	0	,"-", 	"IF Supported_Language = 1"					],
				 ["test04.sis",	"EN , FR"		,	" \"Supported Language EN\" , \"Supported Language FR\" "								,	"\"Symbian Software Ltd. EN \" , \"Symbian Software Ltd. FR\""									,	": \"Symbian Software Ltd.\"",	"IF (LANGUAGE=01) \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\"  \n\nELSEIF (LANGUAGE=02) \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF "			,	"IF SUPPORTED_LANGUAGE = 01  AND LANGUAGE = 02 \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 02 \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF"																																																											,	"!({\"Option1 EN\",\"Option2 EN\" },{\"Option1 FR\" , \"Option2 FR\"})  \n\nif option1 = 1 \n\n\"Option1.txt\"-\"!:\\sys\\bin\\Option1.txt\"; \n\nendif \n\nif option2 = 1 \n\n\"Option1.txt\"-\"!:\\sys\\bin\\Option1.txt\"; \n\nendif \n ",	"API-Inst-PREQ2525-DumpSIS-04",	0	,"-",	"IF (Supported_Language = 1)AND((LANGUAGE)=(2))"					],
				 ["test05.sis",	"EN , FR"		,	" \"Supported Language EN\" , \"Supported Language FR\" "								,	"\"Symbian Software Ltd. EN \" , \"Symbian Software Ltd. FR\""									,	": \"Symbian Software Ltd.\"",	"IF (LANGUAGE=01) \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\"  \n\nELSEIF (LANGUAGE=02) \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF "			,	"IF SUPPORTED_LANGUAGE = 01 OR SUPPORTED_LANGUAGE = 02 \n\n\"SupportedLanguageEN.txt\"-\"!:\\sys\\bin\\SupportedLanguageEN.txt\" \n\nENDIF  \n\nIF SUPPORTED_LANGUAGE = 8754 \n\n\"SupportedLanguageFR.txt\"-\"!:\\sys\\bin\\SupportedLanguageFR.txt\" \n\nENDIF"																																																										,	"!({\"Option1 EN\",\"Option2 EN\" },{\"Option1 FR\" , \"Option2 FR\"})  \n\nif option1 = 1 \n\n\"Option1.txt\"-\"!:\\sys\\bin\\Option1.txt\"; \n\nendif \n\nif option2 = 1 \n\n\"Option2.txt\"-\"!:\\sys\\bin\\Option2.txt\"; \n\nendif \n ",	"API-Inst-PREQ2525-DumpSIS-05",	0	,"-",	"IF (Supported_Language = 1)OR(Supported_Language = 2)"					],

);

#
# Template string to generate PKG file
#
$PkgFileTemp = "	
; File to check DEF092185
;
;Languages
&EN
;
;Header
#{\"logo\"}, (0x80000003), 1, 2, 3, TYPE=SA
;
%{\"Vendor\"}
:\"Vendor\"
;
=\"data\\logotest\\symbianos.mbm\", \"image/x-mbm\", \"c:\\target.mbm\"
;
";

#
# Template string to generate PKG file for DEF100220.
#
$PkgFileTemplate = "	
; File to check DEF100220.
;
;Languages
&EN
;
#{\"TestDEF100220\"}, (0x20000129), 3, 2, 49, TYPE=PA
;
%{\"Vendor\"}
:\"Vendor\"
;
[0x101f7961],0, 0, 0 ,{\"Series60ProductID\"}
;
";

#
# Template string to generate PKG file for DEF105590.
#
$PkgFileDEF105590= "&EN
;
#{\"TestDEF105590\"}, (0x20000130), 1, 2, 3, TYPE=SA  
;
%{\"Vendor\"}
:\"Vendor\"
;
\"\\epoc32\\release\\winscw\\udeb\\console_app_forpackage.exe\"-\"!:\\sys\\bin\\console_app.exe\", FR, RR
";

#
# Template string to generate Expected PKG file after dumpsis for DEF105590.
#
$ExpectedPkgFileDEF105590= "&EN

#{\"TestDEF105590\"}, (0x20000130), 1, 2, 3, TYPE=SA
%{\"Vendor\"}
:\"Vendor\"

;!:\\sys\\bin\\console_app.exe
; File length 57961 (180176)

\"file0\"-\"!:\\sys\\bin\\console_app.exe\", FR, RR
";

#
# Template string to generate PKG file for Hidden flag test.
#
$PkgFileHidden= "&EN
;
#{\"Hidden SA\"}, (0x802730D7), 1, 1, 1, TYPE=SA, H
;
%{\"Crypto Services\"}
:\"Crypto Services\"
";

#
# Template string to generate Expected PKG file after dumpsis for Hidden package.
#
$ExpectedPkgFileHidden= "&EN

#{\"Hidden SA\"}, (0x802730D7), 1, 1, 1, TYPE=SA, H

%{\"Crypto Services\"}
:\"Crypto Services\"
";

#
# CR1125 VERSION Condition Package template string to generate PKG files
#
$PkgFileCR1125VersionTemplate = "
;
; Auto Generated Template PKG File
; VERSION Conditional Testing
;
&EN
;
#{\"CR1125 SEC-SWI-PKGVER\"}, (0xEC001000), 1, 0, 2, TYPE=SA
%{\"Symbian Software Ltd.\"}
: \"Symbian Software Ltd.\"
;
; Version Conditional Block
IF %s
    {
    \"data\\CondTest\\file0\"
    }-\"C:\\tswi\\tpkgver\\test_result_etrue.txt\"
ENDIF
;
";


#
# PREQ2525 SUPPORTED_LANGUAGE  Package template string to generate PKG files
#
$PkgFilePREQ2525SupportedLanguageTemplate = "
;
; Auto Generated Template PKG File
; Supported Language token testing
;
;Language

&%s

;
;Package Header

\#{ %s }, (0xEA001000), 1, 0, 2, TYPE=SA

;Vendor name 

\%{ %s}

;Default vendor name 

%s

;
; Language  Conditional Block

%s
;
; Supported Language  Conditional Block

%s
;
; Option List 

%s
;
";

#
# Template string to generate PKG file for TestNRFlag.
#
$PkgFileWithNRflag = "	
; File to verify NR flag is dumped correctly.
;
;Languages
&EN
;
#{\"TestPacakge\"}, (0x80000077), 1, 0, 0, TYPE=SA, RU, NR
;
%{\"Vendor\"}
:\"Vendor\"
";

#
# Do test for each elements of TestItems array
#
sub CreateMakeSIS
	{
	$packageDir = "package";
	opendir(DIR,".\\package");
	my @files = readdir DIR;
	foreach my $pkgfile (@files)
		{
		$pkgfile =~ s/\.pkg//;
		$packagePath = "$packageDir"."\\"."$pkgfile".".pkg";
		if(-e "$packagePath")
			{
			if("$pkgfile.pkg" eq "StubTest.pkg")
				{
				@retval = system("makesis -s $packagePath $pkgfile.sis > $pkgfile.log");
				}		
			else
				{
				@retval = system("makesis -v $packagePath $pkgfile.sis > $pkgfile.log");
				}
			if( $? != 0)
				{
				WriteLog("\n\nProblem While Creating makesis for $pkgfile.pkg\n");
				}
			else
				{
				WriteLog("\n\nCreated $pkgfile.sis file for the package $pkgfile.pkg\n");
				}
			unlink("$pkgfile.log");
			}
		}
	close(DIR);
	}

for my $Test ( @TestItems )  
	{
	$testid = sprintf "SEC-SWI-I-DUMPSIS-000%d\n",$NumberOfTests+1;
	$NumberOfTests++;

	WriteLog($testid);
	$logMsg = sprintf "%s", $Test->[4];
	WriteLog($logMsg );
	WriteLog("\t$Test->[0]\n");
	DumpSISFile($Test->[0], $Test->[1], $Test->[2], $Test->[3]);
	}

#
# Run CR1125 DumpSIS Tests (TestItems1 array)
#
for my $Test ( @TestItems1 )  
	{
	$NumberOfTests++;

	$logMsg = sprintf "%s - %s\n", $Test->[5], $Test->[3];
	WriteLog($logMsg);

	# Generating PKG file contents String
	my($pkgfile_name) = $Test->[0];
	$pkgfile_name =~ s/\.sis//;

	my($pkgfile_contents) = sprintf($PkgFileCR1125VersionTemplate, $Test->[3]); 

	# Create PKG file
	CreateFile("$pkgfile_name.pkg", $pkgfile_contents);

	if(not -e "$pkgfile_name.pkg") 
		{
		WriteLog("** Problem creating PKG file for $Test->[5] **\n");
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		next;
		}

	# Create SIS Install File 
	@retval = system("makesis -v $pkgfile_name.pkg $pkgfile_name.sis > $pkgfile_name.log");
		
	if( $? != 0) 
		{
		WriteLog("** Problem While Creating SIS file for $Test->[5] **\n");
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		next;
		}
	else 
		{
		WriteLog("Created $Test->[0] file for $Test->[5]\n");
		}

	# Run DumpSIS Test 
	DumpSISFile($Test->[0], $Test->[1], $Test->[2], $Test->[4]);

	unlink("$pkgfile_name.pkg");
	}

#
# Generate files used in .pkg  for  PREQ2525 test 
#
$contents = "This is a dummy file for testing.";
CreateFile('SupportedLanguageEN.r01', $contents);
CreateFile('SupportedLanguageEN.txt', $contents);
CreateFile('SupportedLanguageFR.txt', $contents);
CreateFile('SupportedLanguageGE.txt', $contents);
CreateFile('SupportedLanguageYW.txt', $contents);
CreateFile('Option1.txt', $contents);
CreateFile('Option2.txt', $contents);
#
# Run PREQ2525 DumpSIS Tests (TestItems2 array)
#
for my $Test ( @TestItems2 )  
	{
	$NumberOfTests++;

	$logMsg = sprintf "%s - %s\n", $Test->[8], $Test->[10];
	WriteLog($logMsg);


	# Generating PKG file contents String
	my($pkgfile_name) = $Test->[0];
	$pkgfile_name =~ s/\.sis//;

	my($pkgfile_contents) = sprintf( $PkgFilePREQ2525SupportedLanguageTemplate , $Test->[1],$Test->[2],$Test->[3],$Test->[4],$Test->[5],$Test->[6],,$Test->[7]);
 

	# Create PKG file
	CreateFile("$pkgfile_name.pkg", $pkgfile_contents);

	if(not -e "$pkgfile_name.pkg") 
		{
		WriteLog("** Problem creating PKG file for $Test->[8] **\n");
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		next;
		}

	# Create SIS Install File 
	@retval = system("makesis -v $pkgfile_name.pkg $pkgfile_name.sis > $pkgfile_name.log");
	

	if( $? != 0) 
		{
		WriteLog("** Problem While Creating SIS file for $Test->[8] **\n");
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		next;
		}
	else 
		{
		WriteLog("Created $Test->[0] file for $Test->[8]\n");
		}

	# Run DumpSIS Test 
	DumpSISFile($Test->[0], $Test->[9], $Test->[10] , $Test->[11] );

	unlink("$pkgfile_name.pkg");
	}
unlink("SupportedLanguageEN.r01");
unlink("SupportedLanguageEN.txt");
unlink("SupportedLanguageFR.txt");
unlink("SupportedLanguageGE.txt");
unlink("SupportedLanguageYW.txt");
unlink("Option1.txt");
unlink("Option2.txt");

DeleteFiles("pkg");
DeleteFolder();

TestDEF092185();

TestPDEF095820();

TestDEF100220();

TestRecursiveDirCreation();
CheckCertExtraction();

TestUnicode();

TestDEF105590();

# Run this test only for USIF
if($runMode eq "usif")
{
TestHiddenFlag();
}

SatisfyCoverage();

VerifyErroneousCommandLineArg();

TestNRFlag();

#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests );

sub DeleteFiles
	{
	my ($ext) = @_;
    	opendir(DIR3, ".");
	while (defined($file3=readdir(DIR3)))
    		{
	    	$file3 =~ s/\.$ext//;
		if(-e "$file3.sis")
			{
			rmdir $file3;
			unlink ("$file3.sis");
			unlink ("$file3.log");
			}		
   	    	} 
	close(DIR3);
	}

sub DeleteFolder
	{
	opendir(DIR,".\\package");
	my @files = readdir DIR;
	foreach my $f (@files)
		{
		$f =~ s/\.sis//;
		if(-e "package\\$f.sis")
			{
			opendir(DIR1,"$f");
			my @sisfiles = readdir DIR1;
			foreach my $sis (@sisfiles)
				{
				unlink("$f\\$sis");
				}
			close (DIR1);
			}
		rmdir $f;
		}
	close(DIR);
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
# Run DumpSIS with prepared sis file, log and administrate its result
#
sub DumpSISFile 
	{
	my ($pkgfile) = @_[0];
	my ($expectedResult) = @_[1];
	my ($expectedlog_cmdline) = @_[2];
	my ($expectedlog_pkg) = @_[3];
	$pkgfile =~ s/\.sis//;
	my ($dumpfolder) ="";
	my ($dumppkg) ="";

	@retval = system("dumpsis -x $pkgfile.sis > $pkgfile.log");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$pkgname = $pkgfile; 
	$pkgname =~ s/.*\\//;
	$dumpfolder = $pkgname;
	$dumppkg = "$dumpfolder"."/". "$pkgname.pkg";
	
	if( $? == $expectedResult && -e $dumppkg && -s $dumppkg ) 
		{
		if(CheckLogs($pkgfile,$expectedlog_cmdline,$expectedlog_pkg))
			{
			if("$pkgfile.sis" eq "EmbTest.sis")
				{
				$dumpembedded0 = "$dumpfolder"."/sis0/". "sis0.pkg";
				$dumpembedded1 = "$dumpfolder"."/sis1/". "sis1.pkg";
				if(-e $dumpembedded0 && -s $dumpembedded0 && -e $dumpembedded1 && -s $dumpembedded1)
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

			elsif("$pkgfile.sis" eq "Embsis1.sis")
				{
				$dumpembedded0 = "$dumpfolder"."/sis0/". "sis0.pkg";
				if(-e $dumpembedded0 && -s $dumpembedded0)
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
				if("$pkgfile.sis" eq "pkgversion_c005.sis")
					{
					use Cwd;
    					$rootdir = Cwd::abs_path;
					chdir("$rootdir/$dumpfolder");

					# Create SIS Install File from the Dumped PKG File
					@retval = system("makesis $pkgname.pkg $pkgname.sis");
					
					if($? == 0 && -e "$pkgname.sis") 
						{
						WriteLog("Successfully created $pkgname.sis from dumped PKG file\n");
						$NumberOfPassed++;
						WriteLog("Passed\n\n");
						}
					else
						{
						WriteLog("Error: Failed to create $pkgname.sis from dumped PKG file\n");
						$NumberOfFailed++;
						WriteLog("Failed\n\n");
						}

					chdir($rootdir);
					}
				else
					{
					$NumberOfPassed++;
					WriteLog("Passed\n\n");
					}
				}
			}
		else
			{
			$NumberOfFailed++;
			WriteLog("Failed\n\n");
			} 

		opendir(DIR, $dumpfolder);
		while (defined($files=readdir(DIR)))
			{
			unlink("$dumpfolder"."/"."$files");
			} 
		close(DIR);
		}
	else 
		{
		if( $? == $expectedResult)
			{
			if(CheckLogs($pkgfile,$expectedlog_cmdline,$expectedlog_pkg))
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

		opendir(DIR, $dumpfolder);
		while (defined($file=readdir(DIR)))
			{
			unlink("$dumpfolder"."/"."$file");
			} 	
		close(DIR);
		}

	if("$pkgfile" eq "package\\ExeTest")
		{
		$NumberOfTests++;
		$testid = sprintf "SEC-SWI-I-DUMPSIS-000%d",$NumberOfTests;
		WriteLog($testid);
		
		@retval = system("dumpsis -l $pkgfile.sis > $pkgfile.log");
		WriteLog("\nTest for exe -l option for 1 WINSCW and 2 ARM Executable sis file\n");		
		$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
		WriteLog( $logMsg);

		if( $? == $expectedResult)
			{
			if(CheckLogs($pkgfile,"$pkgfile.out",$expectedlog_pkg))
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

	unlink("$pkgfile.log");

	opendir(DIR, $dumpfolder);
	while (defined($files=readdir(DIR)))
		{
		unlink("$dumpfolder"."/"."$files");
		} 
	close(DIR);
	}
	
sub TestDEF092185
	{
	$DEF092185sis = "DEF092185.sis";
	$DEF092185Log = "DEF092185.Log";
	$DEF092185pkg = "DEF092185.pkg";
	$DumpsisGenPkgPath = "\/DEF092185";
	WriteLog("Test for DEF092185 - Test for verifying that makesis creates sis file successfully when pkg file generated from Dumpsis is used.\n");
 
 	CreateFile('DEF092185.pkg', $PkgFileTemp);
 	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $DEF092185pkg $DEF092185sis > $DEF092185Log ");
	
	# Execute DumpSIS on the created DEF091285.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS $DEF092185sis > $DEF092185Log");
	
	use Cwd;
    $dir = cwd;
	chdir $dir.$DumpsisGenPkgPath;
	
	#Regenerate DEF091860.sis from the dumpsis generated pkg file.
	use File::Copy;
	copy("/epoc32/tools/makesis.exe" , "$dir$DumpsisGenPkgPath");
	$result2  = system("makesis.exe -v $DEF092185pkg $DEF092185sis > $DEF092185Log");
	chdir $dir;
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0 && $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
		
	unlink $DEF092185pkg;
	unlink $DEF092185sis;
	unlink $DEF092185Log;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}

sub CheckFiles($$)
	{
	my ($path, $Files)=@_;
	my $matches = 0;
	
	foreach (@{$Files})
		{
		my $File = $_;
		if ( -e "$path\\$File" && -s "$path\\$File" )
			{
			return 0;
			}
		$matches++;
		}
	if ( $matches > 0 )
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

#
# Function to check the Command Line log and PKG files generated by DumpSIS
#
# Parameters:
# ARG 0 - Name of the SIS file without an Extension
# ARG 1 - Expected log string output by DumpSIS to the Command Line
# ARG 2 - Expected string to appear in the contents of the PKG file generated by DumpSIS
#
# Command Line Log Checking:
# The function firstly examines $expectedLog_cmdline (ARG 1) to determine what type of command line log check
# is required by the test. 
# 
# * If the argument contains "-", the .log file is checked to make sure it is empty and no log information
#   has been output by DumpSIS
# * Else if the argument contains the name of the SIS file (ARG 0) suffixed by ".out", the function compares the
#   contents of the command line log file to that of the .out file to see whether the output from DumpSIS
#   matches the expected file
# * Otherwise, the command line output for the particular SIS is examined to check whether the 
#   string value within $expectedLog_cmdline appears within the log file.
#
# If either check is successful, the $cmdlineResult flag is set to 1
#
#
# PKG File Content Checking:
# The function also examines $expectedLog_pkg (ARG 2) to determine whether a PKG fike content check
# is required by the test.
#
# * If the argument contains "-", the PKG file content check is skipped and deemed successful
# * Otherwise, the PKG file content generated by the DumpSIS tool is examined to check whether the 
#   $expectedLog_pkg string value appears within the PKG file.
#
# If either check is successful, the $pkglineResult flag is set to 1
#
#
# If both flags ($cmdlineResult & $pkglineResult) have been successfully set to 1, the function returns a
# 1 to signal the checks have been successful, otherwise 0 is returned to indicate test failure.
#
sub CheckLogs 
	{
	my ($filename) = @_[0];
	my ($expectedLog_cmdline) = @_[1];
	my ($expectedLog_pkg) = @_[2];
	
	my ($cmdlineResult) = 0;
	my ($pkglineResult) = 0;	

	# -------------------------
	# Command Line log Checking
	# -------------------------
	if($expectedLog_cmdline eq "-") 
		{
		$logMsg = "Expected CmdLine Log: LOG EMPTY\n";
		}
	elsif($expectedLog_cmdline eq "$filename.out")
		{
		$logMsg = sprintf "Expected CmdLine Log: Compare Log against .OUT file - %s\n", $expectedLog_cmdline;
		}
	else 
		{
		$logMsg = sprintf "Expected CmdLine Log: %s\n", $expectedLog_cmdline;
		}
	WriteLog($logMsg);

	
	# Open the contents of the Log File
	unless (open($logcontent, "$filename.log")) 
		{
		printf STDERR "Can't open $filename.log: $!\n";
		return 0;
		}

	if($expectedLog_cmdline eq "-")
		{
		# File has zero bytes in size (is empty)
		if (-z $logcontent) 
			{
			$cmdlineResult = 1;
			}
		else
			{
			$logMsg = "Error: Log File Not Empty\n";
			WriteLog($logMsg);
			}
		}
	elsif($expectedLog_cmdline eq "$filename.out")
		{
		# Compare the contents of the Log against a multiline output file
		if(compare("$filename.out","$filename.log") == 0)
			{
			$cmdlineResult = 1;
			}
		else
			{
			$logMsg = sprintf "Error: Log file does not match %s\n", $expectedLog_cmdline;
			WriteLog($logMsg);
			}
		}
	else
		{
		foreach $logline (<$logcontent>) 
			{
			if ($logline =~ m/\Q$expectedLog_cmdline\E/i) 
				{
				$cmdlineResult = 1;
				}
			}
		
		if($cmdlineResult == 0)
			{
			$logMsg = "Error: Expected Logging Not Found\n";
			WriteLog($logMsg);
			}
		}		
	
	close($logcontent);


	# -------------------------
	# PKG File Content Checking
	# -------------------------

	# Define the dumped PKG path
	$filename =~ s/.*\\//;
	$dumppkg = $filename."\\$filename.pkg";

	if($expectedLog_pkg eq "-") 
		{
		$logMsg = sprintf "Expected PKG Content: NO CHECK\n";
		WriteLog($logMsg);
		$pkglineResult = 1;
		}
	else 
		{
		$logMsg = sprintf "Expected PKG Content: %s\n", $expectedLog_pkg;
		WriteLog($logMsg);

		# Open the contents of the PKG File
		unless (open($pkgcontent,"$dumppkg")) 
			{
			printf STDERR "Can't open $filename.pkg: $!\n";
			return 0;
			}

		foreach $pkgline (<$pkgcontent>) 
			{
			$pkgline =~ tr/\000//d;
			if ($pkgline =~ m/\Q$expectedLog_pkg\E/i) 
				{
				$pkglineResult = 1;
				}
			}
		
		if($pkglineResult == 0)
			{
			$logMsg = "Error: Expected PKG Content Not Found\n";
			WriteLog($logMsg);
			}

		close($pkgcontent);
		}
	
	if($cmdlineResult == 1 && $pkglineResult == 1)
		{
		return 1;
		}

	return 0;
	}

sub TestPDEF095820
	{
	WriteLog("Test for PDEF095820 - Dumpsis doesn't work correctly with embedded sis files.\n");

	$path = "\\epoc32\\winscw\\c\\tswi\\tsis\\data";
	$embed2lvl = "testembed2lvl";
	$PDEF095820Log = "PDEF095820.Log";
	$DumpsisGenPkgPath = "\/$embed2lvl";
 
	# Execute DumpSIS
	my $sisfile = "$path\\$embed2lvl.sis";
	if (! -e $sisfile )
	{
		$NumberOfTests++;
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		exit;
	}
	my $result = system("/epoc32/tools/DUMPSIS -x $path\\$embed2lvl.sis > $PDEF095820Log");
	if ( $result != 0 )
		{
		$NumberOfTests++;
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		exit;
		}

	use Cwd;
    $dir = cwd;
	#chdir $dir.$DumpsisGenPkgPath;
	
	my $filesLvl = 0;

	# Check that all required file are generated and have non-zero sizes
	my @main = ("$embed2lvl.pkg", "sis0.sis", "sis1.sis", "sis2.sis", "file0", "certChain0.der");
	my @sis0 = ("sis0.pkg", "file0", "certChain1.der");
	my @sis1 = ("sis1.pkg", "file0", "certChain2.der");
	my @sis2 = ("sis2.pkg", "file0", "file1", "file2", "file3", "certChain3.der");
	$filesLvl += CheckFiles("$dir$DumpsisGenPkgPath\n",\@main);
	$filesLvl += CheckFiles("$dir$DumpsisGenPkgPath/sis0\n",\@sis0);
	$filesLvl += CheckFiles("$dir$DumpsisGenPkgPath/sis1\n",\@sis1);
	$filesLvl += CheckFiles("$dir$DumpsisGenPkgPath/sis2\n",\@sis2);
	 
	$NumberOfTests++;
	if ( $filesLvl == 4 )   
	    {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else
	    {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	    }
	
	#chdir $dir;
	unlink $PDEF095820Log;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}

sub TestDEF100220
	{
	$DEF100220sis = "DEF100220.sis";
	$DEF100220Log = "DEF100220.Log";
	$DEF100220pkg = "DEF100220.pkg";
	$DumpsisGenPkgPath = "\/DEF100220";
	WriteLog("Test for DEF100220 - Test for verifying that makesis creates sis file successfully when pkg file generated from Dumpsis is used.\n");
 
 	CreateFile('DEF100220.pkg', $PkgFileTemplate);
 	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $DEF100220pkg $DEF100220sis > $DEF100220Log ");
	
	# Execute DumpSIS on the created DEF100220.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS $DEF100220sis > $DEF100220Log");
	
	use Cwd;
    $dir = cwd;
	chdir $dir.$DumpsisGenPkgPath;
	
	#Regenerate DEF100220.sis from the dumpsis generated pkg file.
	use File::Copy;
	copy("/epoc32/tools/makesis.exe" , "$dir$DumpsisGenPkgPath");
	$result2  = system("makesis.exe -v $DEF100220pkg $DEF100220sis > $DEF100220Log");
	chdir $dir;
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0 && $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
		
	unlink $DEF100220pkg;
	unlink $DEF100220sis;
	unlink $DEF100220Log;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}


sub TestRecursiveDirCreation
	{
	$TestSisFile = "DumpsisTest.sis";
	$TestLogFile = "DumpsisTest.Log";
	$TestPkgFile = "DumpsisTest.pkg";
	$DumpsisExtractPath = "\/TestDumpsis\/dir\/creation";
	WriteLog("Test for verifying the ability of dumpsis to create recursive directories if the extraction path is not created.\n");
 
 	CreateFile($TestPkgFile, $PkgFileTemplate);
 	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $TestPkgFile $TestSisFile > $TestLogFile ");
	
	# Execute DumpSIS on the created DumpsisTest.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS -d $DumpsisExtractPath $TestSisFile > $TestLogFile");
	my $result2 = 0;

	if(-e "$DumpsisExtractPath\\$TestPkgFile")
		{
		$result2 = 1;
		}
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0 && $result2 == 1) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
		
	unlink $TestPkgFile;
	unlink $TestLogFile;
	unlink $TestSisFile;
	use File::Path;
	rmtree "\/TestDumpsis";
	}

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

sub TestUnicode {

	my $scriptpath = "\\epoc32\\winscw\\c\\tswi\\tdumpsis\\data\\unicodetest";
	
	WriteLog("Test for DEF099325 - Unicode named sis files\n");
	
	$NumberOfTests++;
	# Generate Japanese pkg 
	system("WScript.exe //B //Nologo $scriptpath\\jpn_filename.vbs");
	
	if ( -f "passed.txt") {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	} else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}
	system("WScript.exe //B //Nologo $scriptpath\\jpn_filename_cleanup.vbs");

	WriteLog("Test for DEF099325 - UCS2 encoding format.\n");

	$NumberOfTests++;
	# Generate Japanese pkg 
	system("WScript.exe //B //Nologo $scriptpath\\ucs2jpn.vbs");
	
	if ( -f "passed.txt") {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	} else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}
	system("WScript.exe //B //Nologo $scriptpath\\ucs2jpn_cleanup.vbs");

	
	WriteLog("Test for DEF099325 - Test for Katakana characters and embedded sis file.\n");

	$NumberOfTests++;
	# Generate Japanese pkg 
	system("WScript.exe //B //Nologo $scriptpath\\embed.vbs");
	
	if ( -f "passed.txt") {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	} else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}
	system("WScript.exe //B //Nologo $scriptpath\\embed_cleanup.vbs");

}

sub TestDEF105590 
	{
	$DEF105590sis = "DEF105590.sis";
	$DEF105590Log = "DEF105590.Log";
	$DEF105590pkg = "DEF105590.pkg";
	$DEF105590Expectedpkg = "DEF105590Expected.pkg";
	$DumpsisGenPkgPath = "\/DEF105590";
	WriteLog("Test for DEF105590 - Test for verifying that RW is not appended to the generated pkg file from Dumpsis from sis file having FR,RR option.\n");
 
 	CreateFile('DEF105590.pkg', $PkgFileDEF105590);
	CreateFile('DEF105590Expected.pkg', $ExpectedPkgFileDEF105590);
	
 	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $DEF105590pkg $DEF105590sis > $DEF105590Log ");
	
	# Execute DumpSIS on the created DEF105590.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS $DEF105590sis > $DEF105590Log");
	use Cwd;
    $dir = cwd;
	chdir $dir;
	use File::Compare;
	my $result3;
	my $file1 = $dir."/".$DEF105590Expectedpkg;
	my $file2 = $dir.$DumpsisGenPkgPath."/".$DEF105590pkg;
	if(compare($file1,$file2))
		{ 
		$result3 = 0;			
		}
	else
		{
		$result3 = 1;
		}
		$NumberOfTests++;
	
	if ($result == 0 && $result1 == 0 && $result3 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	chdir $dir;	
	unlink $DEF105590Expectedpkg;
	unlink $DEF105590pkg;
	unlink $DEF105590sis;
	unlink $DEF105590Log;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}
#
# SEC-DumpSIS-Hide-001 
# Tests the MakeSIS and DumpSIS for the support of a new installtion flag HIDE / H (to represent Hidden component) in the package file.
#
sub TestHiddenFlag 
	{
	$Hiddensis = "Hidden.sis";
	$HiddenLog = "Hidden.Log";
	$Hiddenpkg = "Hidden.pkg";
	$ExpectedHiddenpkg = "ExpectedHidden.pkg";
	$DumpsisGenPkgPath = "\/Hidden";
	WriteLog("Test for Hidden package - Test for verifying that the installation flag HIDE is properly processed by DumpSis.\n");
 
 	CreateFile('Hidden.pkg', $PkgFileHidden);
	CreateFile('ExpectedHidden.pkg', $ExpectedPkgFileHidden);
	
 	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $Hiddenpkg $Hiddensis > $HiddenLog ");
	
	# Execute DumpSIS on the created Hidden.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS $Hiddensis > $HiddenLog");
	use Cwd;
	$dir = cwd;
	chdir $dir;
	use File::Compare;
	my $result3;
	my $file1 = $dir."/".$ExpectedHiddenpkg;
	my $file2 = $dir.$DumpsisGenPkgPath."/".$Hiddenpkg;
	if(compare($file1,$file2))
		{
		$result3 = 0;			
		}
	else
		{
		$result3 = 1;
		}
		$NumberOfTests++;
	
	if ($result == 0 && $result1 == 0 && $result3 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	chdir $dir;	
	unlink $ExpectedHiddenpkg;
	unlink $Hiddenpkg;
	unlink $Hiddensis;
	unlink $HiddenLog;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}

sub CheckCertExtraction
	{
	my $TestLogFile = "DumpsisTest.Log";
	my $TestPkgFile = "DumpsisTest.pkg";
	my $TestSisFile = "DumpsisTest.sis";
	my $TestSignedSisFile = "DumpsisSigned.sis";
	my $outputDir = "ExtractedFiles";
	my $certFileName = "$outputDir\\certChain01.der";

	WriteLog("Testing certificate extraction\n");

 	CreateFile($TestPkgFile, $PkgFileTemplate);

	my $result = system("/epoc32/tools/makeSIS $TestPkgFile $TestSisFile > $TestLogFile");
	
	if(0 == $result)
		{
		$result = system("/epoc32/tools/signsis -s $TestSisFile $TestSignedSisFile \\epoc32\\winscw\\c\\tswi\\tsignsis\\default.cer \\epoc32\\winscw\\c\\tswi\\tsignsis\\default.key > $TestLogFile");
		}

	if(0 == $result)
		{
		$result = system("/epoc32/tools/DUMPSIS -x -i -d $outputDir $TestSignedSisFile > $TestLogFile");
		}

	$NumberOfTests++;

	if ($result == 0 && -e $certFileName) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	$NumberOfTests++;

	WriteLog("Testing version message display\n");
	$result = system("/epoc32/tools/DUMPSIS -v > $TestLogFile");
	if ($result == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $certFileName;
	unlink $TestSignedSisFile;
	unlink $TestSisFile;
	unlink $TestPkgFile;
	unlink $TestLogFile;
	use File::Path;
	rmtree "$outputDir";
	}

sub SatisfyCoverage
	{
	my $TestLogFile = "DumpsisTest.Log";		

	WriteLog("Testing help message display\n");
	my $result = system("/epoc32/tools/DUMPSIS -h > $TestLogFile");

	$NumberOfTests++;

	if ($result == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	$NumberOfTests++;

	WriteLog("Testing version message display\n");
	$result = system("/epoc32/tools/DUMPSIS -v > $TestLogFile");
	if ($result == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $TestLogFile;
	}

sub VerifyErroneousCommandLineArg
	{
	my $TestLogFile = "DumpsisTest.Log";
	my $ErrOutputFile = "DumpsisErr.out"; 
	my $ErrFile = "DumpsisTest.err";
	my $errMsg1 = "DumpSIS: Error in command: no SIS file specified\n";
	my $errMsg2 = "DumpSIS: Error in command: too many SIS files specified\n";
	my $errMsg3 = "DumpSIS: Error in command: No argument supplied with the -d flag\n";

	WriteLog("Testing invalid command line arguments\n");
	CreateFile($ErrFile, $errMsg1);
	my $result1 = system("/epoc32/tools/DUMPSIS 2> $ErrOutputFile > $TestLogFile");
	my $result = 1;

	if( !((compare($ErrFile,$ErrOutputFile) == 0) && $result1 == 256))
		{
		$result = 0;
		}

	CreateFile($ErrFile, $errMsg2);
	$result1 = system("/epoc32/tools/DUMPSIS sis1.sis sis2.sis sis3.sis 2> $ErrOutputFile > $TestLogFile");

	if( !((compare($ErrFile,$ErrOutputFile) == 0) && $result1 == 256))
		{
		$result = 0;
		}

	CreateFile($ErrFile, $errMsg3);
	$result1 = system("/epoc32/tools/DUMPSIS -d 2> $ErrOutputFile > $TestLogFile");

	if( !((compare($ErrFile,$ErrOutputFile) == 0) && $result1 == 256))
		{
		$result = 0;
		}

	$NumberOfTests++;
	if ($result == 1) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $ErrFile;
	unlink $ErrOutputFile;
	unlink $TestLogFile;
	}

sub TestNRFlag
	{
	$TestNRflagsis = "test_nr_flag.sis";
	$TestNRflagLog = "test_nr_flag.Log";
	$TestNRflagpkg = "test_nr_flag.pkg";
	$DumpsisGenPkgPath = "\/test_nr_flag";
	$ExpectedStringInDumpedPackage = "NR";
	WriteLog("Test if the NR flag, in the original package file, is present in the dumped package file \n");

	CreateFile('test_nr_flag.pkg', $PkgFileWithNRflag);
	# Create a sis file
	my $result = system("/epoc32/tools/MAKESIS -v $TestNRflagpkg $TestNRflagsis > $TestNRflagLog ");
	
	# Execute DumpSIS on the created sis file.
	my $result1 = system("/epoc32/tools/DUMPSIS $TestNRflagsis > $TestNRflagLog");
	
	use Cwd;
	$dir = cwd;
	chdir $dir.$DumpsisGenPkgPath;
	open($pkgcontent,$TestNRflagpkg);
	foreach (<$pkgcontent>) 
	{
		$_ =~ tr/\000//d;
		if ($_ =~ m/$ExpectedStringInDumpedPackage/) 
		{
			$result2 = 1;
		}
	}
	close($pkgcontent);

	chdir $dir;
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0 && $result2 == 1) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
		
	unlink $TestNRflagpkg;
	unlink $TestNRflagsis;
	unlink $TestNRflagLog;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	}
@rem
@rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

:: This acts like an .ini file.
:: It will run the portion of the code indicated by the first argument (test ID) of the command line.
:: FORMAT:
:: :<test ID>
:: ECHO PERFORM THE TESTS
:: GOTO END


@ECHO OFF
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\system\install\ mkdir .\romdrive\system\install\ > NUL

GOTO %1

:INT001-001
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-002
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSADiffUid_sucert.sis +sucert -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-003a
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-003b
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSADiffName.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-003c
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSASameVersion.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-003d
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSANR.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-004
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
if not exist .\romdrive\private\10273582 mkdir .\romdrive\private\10273582
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU_root5.sis,\epoc32\winscw\c\tswi\tsis\data\preq2114romupgradepu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0002.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-006
call interpretsis -z .\romdrive -c .\cdrive -n 2 -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_sa.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_pu.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\81111106\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\filePU.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\fileSP.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
type .\cdrive\private\81111106\import\file1.txt | find "French" > NUL
if not %errorlevel%==0 GOTO LAST
GOTO PASS_CLEAN


:INT001-007
call interpretsis -z .\romdrive -c .\cdrive -n 5 -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_sa.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_pu.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114language_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\81111106\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\filePU.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\fileSP.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\81111106\import\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
type .\cdrive\private\81111106\import\file1.txt | find "English" > NUL
if not %errorlevel%==0 GOTO LAST
GOTO PASS_CLEAN


:INT001-008a
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-008b
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2RU.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-009
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStubDiffPkg.sis .\romdrive\system\install\CR1027ROMUpgradeStubDiffPkg.sis > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPRUNR.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-010
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_partial.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-011
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-012
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-013
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-014
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\81000018\ mkdir .\romdrive\private\81000018\
if not exist .\romdrive\rucerttest\ mkdir .\romdrive\rucerttest\
if not exist .\romdrive\eclipsetest\ mkdir .\romdrive\eclipsetest\
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\81000018\rucert_config5.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\rucerttest\rucert_file3.txt > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\eclipsetest.dll > NUL
call copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe .\romdrive\sys\bin\tswisidupgradeexe.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\eclipsetest\file1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt .\romdrive\eclipsetest\file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114romupgradepurunr.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730AD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\rucerttest\rucert_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\eclipsetest\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-015
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_sanr.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_purunr.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-016
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPNR.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT001-017
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\filenull.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\10286380\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\10286380\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:SWI001-001
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_saru.sis -k 5.3 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI001-002
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_sarunr.sis -k 5.3 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI001-003
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI001-004
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\81000018\ mkdir .\romdrive\private\81000018\
if not exist .\romdrive\rucerttest\ mkdir .\romdrive\rucerttest\
if not exist .\romdrive\eclipsetest\ mkdir .\romdrive\eclipsetest\
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\81000018\rucert_config5.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\rucerttest\rucert_file3.txt > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\eclipsetest.dll > NUL
call copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe .\romdrive\sys\bin\tswisidupgradeexe.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\eclipsetest\file1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt .\romdrive\eclipsetest\file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\ruromupgraderucertpu1_sucert.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730AD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\romdrive \epoc32\release\winscw\udeb\z /S /Y > NUL
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END


:SWI001-005
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_partial.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\romdrive \epoc32\release\winscw\udeb\z /S /Y > NUL
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END


:SWI001-006
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -k 5.3 -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\romdrive \epoc32\release\winscw\udeb\z /S /Y > NUL
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END


:SWI001-007
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_43_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\hash\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END


:INT002-001a
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSADiffUid_sucert.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-001b
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMSANORU.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-3 GOTO LAST
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-003
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMSANORU.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-3 GOTO LAST
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-004
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
if not exist .\romdrive\private\10273582 mkdir .\romdrive\private\10273582
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\ruromupgradenonrucertpu_root5.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-9 GOTO LAST
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114romupgradepu2.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-10 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT002-005
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\81000018\ mkdir .\romdrive\private\81000018\
if not exist .\romdrive\rucerttest\ mkdir .\romdrive\rucerttest\
if not exist .\romdrive\eclipsetest\ mkdir .\romdrive\eclipsetest\
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\81000018\rucert_config5.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\rucerttest\rucert_file3.txt > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\eclipsetest.dll > NUL
call copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe .\romdrive\sys\bin\tswisidupgradeexe.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\eclipsetest\file1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt .\romdrive\eclipsetest\file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\ruromupgraderucertpu1_sucert.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730AD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\rucerttest\rucert_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\eclipsetest\file1.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-006
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\81000018\ mkdir .\romdrive\private\81000018\
if not exist .\romdrive\rucerttest\ mkdir .\romdrive\rucerttest\
if not exist .\romdrive\eclipsetest\ mkdir .\romdrive\eclipsetest\
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\81000018\rucert_config5.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\rucerttest\rucert_file3.txt > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\eclipsetest.dll > NUL
call copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe .\romdrive\sys\bin\tswisidupgradeexe.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\eclipsetest\file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\ruromupgraderucertpu1_sucert.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730AD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\rucerttest\rucert_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\eclipsetest\file1.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-007
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPSameName.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-21 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-008
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2RU.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-009
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-010
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-21 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-011
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_2.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spru.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-012
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp_2.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-013
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_purunr.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-18 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-014
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_sanr.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-18 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-015
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spnr.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-18 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-016
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp_2_nr.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-18 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-017
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade3.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade4.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +e +sucert -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT002-018
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\filenullinvalid.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-4 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\10286380\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\10286380\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:SWI002-001
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\10273582\ mkdir .\romdrive\private\10273582\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -k 5.3 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\romdrive \epoc32\release\winscw\udeb\z /S /Y > NUL
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI002-002
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\10273582\ mkdir .\romdrive\private\10273582\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -k 5.3 -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call xcopy .\romdrive \epoc32\release\winscw\udeb\z /S /Y > NUL
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI002-003
call interpretsis -z .\romdrive -c .\cdrive -k 5.3 -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
if not exist \epoc32\winscw\c\Documents\InstTest mkdir \epoc32\winscw\c\Documents\InstTest
call xcopy .\cdrive\sys\install\sisregistry \epoc32\winscw\c\sys\install\sisregistry /S /Y > NUL
call xcopy .\cdrive\Documents\InstTest \epoc32\winscw\c\Documents\InstTest /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:SWI002-004
call interpretsis -z .\romdrive -c .\cdrive -k 5.3 -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spnr_ru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
if not exist \epoc32\winscw\c\Documents\InstTest mkdir \epoc32\winscw\c\Documents\InstTest
call xcopy .\cdrive\sys\install\sisregistry \epoc32\winscw\c\sys\install\sisregistry /S /Y > NUL
call xcopy .\cdrive\Documents\InstTest \epoc32\winscw\c\Documents\InstTest /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO END


:INT003-001
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_11_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis  -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_11_upgrade_0xabaecad5_symbiantest1_dummydllpkg_220_sa.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0002}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-002
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_11_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis  -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-003
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_21_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_21_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp.sis  -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-004
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_21_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_41_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0002}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-005
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_43_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_23_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-006
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_43_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_48_upgrade_0xabaecad5_symbiantest1_dummydllpkg_220_pu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0002}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-007
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_43_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_46_upgrade_0xabaecad5_symbiantest1_dummydllpkg_220_pu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0002}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-008
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\release\winscw\udeb\adornedfilenametestingdll_10_1.dll .\romdrive\sys\bin\adornedfilenametestingdll4.dll > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis .\romdrive\system\install\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_36_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-009
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\release\winscw\udeb\adornedfilenametestingdll_10_1.dll .\romdrive\sys\bin\adornedfilenametestingdll4.dll > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis .\romdrive\system\install\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_310_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-010
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\release\winscw\udeb\adornedfilenametestingdll_10_1.dll .\romdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_stub_0x2baecad8_symbiantest1_dummydll4pkg.sis .\romdrive\system\install\adornedfilenamestesting_stub_0x2baecad8_symbiantest1_dummydll4pkg.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_38_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-011
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\sys\bin mkdir .\romdrive\sys\bin > NUL
if not exist .\romdrive\private\90008ACE mkdir .\romdrive\private\90008ACE > NUL
if not exist .\romdrive\private\89334567 mkdir .\romdrive\private\89334567 > NUL
if not exist .\romdrive\cr1122test mkdir .\romdrive\cr1122test > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityunprotectedvid.dll .\romdrive\sys\bin\wildcard_rom_stub_lib1.dll > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\wildcard_rom_stub_lib2.dll > NUL
call copy \epoc32\release\winscw\udeb\helloworld_forpackage.exe .\romdrive\sys\bin\wildcard_testrun1.exe > NUL
call copy \epoc32\release\winscw\udeb\console_app_forpackage.exe .\romdrive\sys\bin\wildcard_testrun2.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1122test\data1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1122test\fact2.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1122WildCardROMStub.sis .\romdrive\system\install\CR1122WildCardROMStub.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1122WCROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730B1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730B1\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_rom_stub_lib1.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_rom_stub_lib2.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_testrun1.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_testrun2.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_testrun1.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\90008ACE\wcrom1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\89334567\wcrom2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1122test\data1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1122test\fact2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1122test\wildcard_config1.cnf
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1122test\wildcard_config2.cnf
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-012
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config2.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-013
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\81000018\ mkdir .\romdrive\private\81000018\
if not exist .\romdrive\rucerttest\ mkdir .\romdrive\rucerttest\
if not exist .\romdrive\eclipsetest\ mkdir .\romdrive\eclipsetest\
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\81000018\rucert_config5.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\rucerttest\rucert_file3.txt > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\sucertromstub.sis .\romdrive\system\install\ > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\tswidllnocapabilityprotectedvid.dll .\romdrive\sys\bin\eclipsetest.dll > NUL
call copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe .\romdrive\sys\bin\tswisidupgradeexe.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\eclipsetest\file1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt .\romdrive\eclipsetest\file2.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\ruromupgraderucertpu1.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730AD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\rucerttest\rucert_file1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\eclipsetest\file1.txt
if not exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT003-014
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-015
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade3.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade4.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT003-017
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT003-018
mkdir .\romdrive\sys\bin > NUL
mkdir .\romdrive\system\install > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis .\romdrive\system\install\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_36_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_saru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-019
mkdir .\romdrive\sys\bin > NUL
mkdir .\romdrive\system\install > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis .\romdrive\system\install\adornedfilenamestesting_35_stub_0x2baecad8_symbiantest1_dummydll4pkg_110.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_310_upgrade_0x2baecad8_symbiantest1_dummydll4pkg2_220_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-020
mkdir .\romdrive\sys\bin > NUL
mkdir .\romdrive\system\install > NUL
call copy \epoc32\release\winscw\udeb\adornedfilenametestingdll_10_1.dll .\romdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\adornedfilenamestesting_stub_0x2baecad8_symbiantest1_dummydll4pkg.sis .\romdrive\system\install\adornedfilenamestesting_stub_0x2baecad8_symbiantest1_dummydll4pkg.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam002\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_38_upgrade_0x2baecad8_symbiantest1_dummydll4pkg_220_pu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad8\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll4{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-021
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-022
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU_root5.sis -s \epoc32\winscw\c\tswi\tsis\data\preq2114romupgradepu.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0002.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-023
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-024
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam002\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-025
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam002\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT003-026
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam002\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -e -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT003-027
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam001\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:SWI003-001
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -k 5.3 -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spnr_ru.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
call xcopy .\cdrive \epoc32\winscw\c /S /Y > NUL
call xcopy .\edrive \epoc32\drive_e /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END


:INT004-001
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_11_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_65_upgrade_0xabaecad6_symbiantest1_dummydllpkg_220_sa.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad6\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad6\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0002}.dll
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-002
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_43_base_0xabaecad5_symbiantest1_dummydllpkg_210.sis -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_21_upgrade_0xabaecad5_symbiantest1_dummydllpkg2_220_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\abaecad5\00000001_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll{000A0001}.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT004-003
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\release\winscw\udeb\adornedfilenametestingdll_10_1.dll .\romdrive\sys\bin\adornedfilenametestingdll3.dll
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_34_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad7\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad7\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll3{000A0001}.dll
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-004
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1122WildCardROMStub.sis .\romdrive\system\install\CR1122WildCardROMStub.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1122WCROMUpgradeSA1.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-4 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730B1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730B1\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_rom_stub_lib1.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_rom_stub_lib2.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\wildcard_testrun1.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273581\wcrom3.txt
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\90008ACE\wcrom1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\90008ACE\wcrom2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-005
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
if not exist .\romdrive\private\10273582 mkdir .\romdrive\private\10273582
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU_root5.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-3 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0002.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT004-006
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPRUNR.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT004-007
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2.sis +c +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-3 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-008
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +e +sucert -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-009
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade4.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +c -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_puru.sis +c -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-010
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade3.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade4.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis +c -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-011
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis +c -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-012
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
if not exist .\romdrive\Documents\InstTest\ mkdir .\romdrive\Documents\InstTest\
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\Documents\InstTest\upgrade1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +c +sucert -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis +e -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-013
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\cr1027romupgradepudifffilename.sis +e -w info -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
if not %errorlevel%==-9 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_execute2.exe
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-014
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU.sis +c -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU.sis +e -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\edrive\sys\install\sisregistry\802730A2\00000000_0002.ctl
if exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT004-015
mkdir .\romdrive\sys\bin > NUL
mkdir .\romdrive\system\install > NUL
call interpretsis -r \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam003\rombld.rofs.log -t .\romdrive\system\install -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\adornedfilenamestesting_34_upgrade_0x2baecad7_symbiantest1_dummydll3pkg_220_saru.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\2baecad7\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\2baecad7\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\adornedfilenametestingdll3{000A0001}.dll
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT004-016
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute_same_sid.exe > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-9 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT005-001
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT005-004
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\10273582\ mkdir .\romdrive\private\10273582\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis .\romdrive\system\install\ > NUL
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA_sucert.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A1\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT006-006
if not exist .\romdrive\sys\bin\ mkdir .\romdrive\sys\bin\
if not exist .\romdrive\private\10273582\ mkdir .\romdrive\private\10273582\
if not exist .\romdrive\cr1027test\ mkdir .\romdrive\cr1027test\
call xcopy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\ > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\system\install\invalid_stub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-10 GOTO LAST
GOTO PASS_CLEAN


:INT008-001
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_5_3.txt .\romdrive\system\data\sisregistry_5.3.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_53\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-002
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
if exist .\romdrive\system\data rmdir .\romdrive\system\data /S /Q > NUL
mkdir .\romdrive\system\data
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_5_1.txt .\romdrive\system\data\sisregistry_5.1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_51\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-003
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
if exist .\romdrive\system\data\ del .\romdrive\system\data\ /F /Q> NUL
mkdir .\romdrive\system\data\ > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_4_1.txt .\romdrive\system\data\sisregistry_4.0.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_40\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-004
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -k 5.3 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_53\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-005
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_4_1.txt .\romdrive\system\data\sisregistry_4.0.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -k 5.3 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_53\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-006
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_5_3.txt .\romdrive\system\data\sisregistry_5.3.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_53\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-007
if exist .\romdrive\system\data rmdir .\romdrive\system\data /S /Q > NUL
mkdir .\romdrive\system\data
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_5_1.txt .\romdrive\system\data\sisregistry_5.1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_51\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT008-001
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_5_3.txt .\romdrive\system\data\sisregistry_5.3.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
call fc /c  .\cdrive\sys\install\sisregistry\80000001\00000000.reg \epoc32\winscw\c\tswi\tinterpretsisinteg\data\80000001_53\00000000.reg | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT009-001
if not exist .\romdrive\system\data mkdir .\romdrive\system\data > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\version_x_y.txt .\romdrive\system\data\sisregistry_x.y.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT009-002
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\simple.sis -k 38 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-7 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-001
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-002
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-003
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-004
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-005
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spnr.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-006
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2RU_root5.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-007
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2NR.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-008
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\edrive\sys\bin > NUL
mkdir .\edrive\private\89334569 > NUL
call copy \epoc32\release\winscw\udeb\console_app_longrun_forpackage.exe .\edrive\sys\bin\preinstalled_console_app.exe > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\french.txt .\edrive\private\89334569\privatefile.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testpreinstalledexes.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\private\10202dce\81231236_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\81231236\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-010
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-011
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-012
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_spnr_ru.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade5.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-013
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP2NR.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-015
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\preq2114upgrade_saru.sis +e +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_1.sis
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-016
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU.sis +e -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0001.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-017
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\edrive\sys\bin > NUL
mkdir .\edrive\private\89334569 > NUL
call copy \epoc32\release\winscw\udeb\console_app_longrun_forpackage.exe .\edrive\sys\bin\preinstalled_console_app.exe > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\french.txt .\edrive\private\89334569\privatefile.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testpreinstalledexes.sis +e -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\private\10202dce\81231236_0.sis
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\81231236\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\81231236\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\hash\preinstalled_console_app.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-018
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\edrive\sys\bin > NUL
mkdir .\edrive\private\89334569 > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +e +mcardalonenr -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
call del .\edrive\private\10202dce\811111FD_0.sis > NUL
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-019
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\edrive\sys\bin > NUL
mkdir .\edrive\private\89334569 > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +e +mcardnr -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call del .\edrive\private\10202dce\811111FD_0.sis > NUL
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-020
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +e +mcardalone +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-021
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +e +mcard +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-022
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStubDiffPkg.sis .\romdrive\system\install\CR1027ROMUpgradeStubDiffPkg.sis > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPRUNR.sis +e +mcardnr +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT010-023
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\data\ruromupgraderucertsa_diffdrive_h4_sucert.sis +e +mcard +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\private\10202dce\802730A9_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
call fc /c  .\edrive\private\10202dce\802730A9_0.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\data\802730A9_0.sis | find "FC: no differences encountered" > NUL
if not %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-001
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_ru.sis +d +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-7 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-003
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis +e +mcard -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-2 GOTO LAST
set exist_file=.\edrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\private\10202dce\811111FD_0.sis
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-004
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis +e +mcard +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10273582\cr1027_config2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-005
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePURUNR.sis +e +mcardnr +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-2 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_0.sis
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-006
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPNR.sis +d +mcardalonenr +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-7 GOTO LAST
set exist_file=\ddrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=\ddrive\private\10202dce\802730A2_1.sis
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-007
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStubDiffPkg.sis .\romdrive\system\install\CR1027ROMUpgradeStubDiffPkg.sis > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\CR1027ROMUpgradeStub2.sis .\romdrive\system\install\CR1027ROMUpgradeStub2.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSPRUNR.sis +e +mcardnr +sucert -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\edrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\edrive\private\10202dce\802730A2_1.sis
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-008
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testpreinstalledexes.sis +e +mcardalone -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-7 GOTO LAST
set exist_file=.\edrive\private\10202DCE\81231236_0.sis
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\81231236\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\81231236\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-009
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial2.sis +d -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-5 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-010
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +d -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives-invalid.ini > NUL
if not %errorlevel%==-13 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-011
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +d -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives-invalid2.ini > NUL
if not %errorlevel%==-13 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-012
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis +x -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives.ini > NUL
if not %errorlevel%==-7 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-013
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives-invalid3.ini > NUL
if not %errorlevel%==-13 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-014
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives2.ini > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT011-015
if not exist \ddrive mkdir \ddrive > NUL
if not exist .\edrive mkdir .\edrive > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt -i \epoc32\winscw\c\tswi\tinterpretsisinteg\data\config-drives-invalid4.ini > NUL
if not %errorlevel%==-13 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\ddrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT012-001
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -x 0x811111FD -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT012-002
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_partial.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -x 0x811111FD -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0001.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT012-003
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_sp.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -x 0x811111FD -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade4.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade5.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000001_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT012-004
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSA.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -x 0x802730A2 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\bin\saromupgrade_library.dll
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\private\10273582\cr1027_config2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\romdrive\sys\bin\saromupgrade_execute.exe
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\romdrive\sys\bin\saromupgrade_library.dll
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\romdrive\cr1027test\cr1027_file1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\romdrive\private\10273582\cr1027_config1.txt
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT012-005
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradeSP.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -x 0x802730A2 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000001_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\cr1027test\cr1027_file2.txt
if exist %exist_file% GOTO FAIL_FOUND
GOTO PASS_CLEAN


:INT012-006
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\CR1027ROMUpgradePU.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -x 0x802730A2 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\sys\bin\saromupgrade_execute.exe
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000.reg
if not exist %exist_file% GOTO FAIL_NOT_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\802730A2\00000000_0000.ctl
if not exist %exist_file% GOTO FAIL_NOT_FOUND
GOTO PASS_CLEAN


:INT012-007
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -x 2165379581 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
set exist_file=.\cdrive\Documents\InstTest\upgrade1.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade2.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\Documents\InstTest\upgrade3.txt
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000.reg
if exist %exist_file% GOTO FAIL_FOUND
set exist_file=.\cdrive\sys\install\sisregistry\811111FD\00000000_0000.ctl
if exist %exist_file% GOTO FAIL_FOUND
ECHO ***ERRORCODE*** %errorlevel% PASS>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO PASS_CLEAN


:INT013-001
call interpretsis -z .\romdrive -c .\cdrive -x 0x8000001 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-22 GOTO LAST
GOTO PASS_CLEAN


:INT013-002
mkdir .\romdrive\cr1027test > NUL
mkdir .\romdrive\private\10273582 > NUL
mkdir .\romdrive\sys\bin > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\CR1027ROMUpgradeStub.sis .\romdrive\system\install\CR1027ROMUpgradeStub.sis > NUL
call copy \epoc32\release\winscw\udeb\eclipsetest1.dll .\romdrive\sys\bin\saromupgrade_library.dll > NUL
call copy \epoc32\release\winscw\udeb\tsaromupgradeexe.exe .\romdrive\sys\bin\saromupgrade_execute.exe > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt .\romdrive\private\10273582\cr1027_config1.txt > NUL
call copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt .\romdrive\cr1027test\cr1027_file1.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive -x 0x802730A2 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-17 GOTO LAST
GOTO PASS_CLEAN


:INT013-003
call interpretsis -z .\romdrive -c .\cdrive -x 0x8000001 -s \epoc32\winscw\c\tswi\tsis\data\testupgrade_base_nr.sis -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==-7 GOTO LAST
GOTO PASS_CLEAN


:INT014-001
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatainc124436\inc124436.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\inc124436.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tinterpretsisinteg/sisfiles/inc124436.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -x 0x81111105 -w info -l \epoc32\winscw\c\interpretsis_test_harness.txt > NUL
if not %errorlevel%==0 GOTO LAST
GOTO PASS_CLEAN


:PASS_CLEAN
rmdir /S /Q .\cdrive > NUL
rmdir /S /Q .\romdrive > NUL
if exist \ddrive rmdir /S /Q \ddrive > NUL
if exist .\edrive rmdir /S /Q .\edrive > NUL
GOTO PASS


:PASS
set exist_file=
ECHO Passed
GOTO END


:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO FAIL

:FAIL_NOT_FOUND
ECHO File %exist_file% SHOULD exist but NOT FOUND >>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO FAIL


:FAIL_FOUND
ECHO File %exist_file% should NOT exist but FOUND >>\epoc32\winscw\c\interpretsis_test_harness.txt
GOTO FAIL


:FAIL
rmdir /S /Q .\cdrive > NUL
rmdir /S /Q .\romdrive > NUL
if exist \ddrive rmdir /S /Q \ddrive > NUL
if exist .\edrive rmdir /S /Q .\edrive > NUL
set exist_file=
ECHO FAILED
GOTO END


:END


@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem For preq1912- ROM Upgrade without stubs 
rem For UDEB
md \epoc32\release\winscw\udeb\z\rucerttest
md \epoc32\release\winscw\udeb\z\private\81000018
md \epoc32\winscw\c\private\81000018
copy /y \epoc32\release\winscw\udeb\sucertdll.dll 		\epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_library.dll
copy /y \epoc32\release\winscw\udeb\sucerttestexe.exe	\epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_execute.exe
copy /y \epoc32\release\winscw\udeb\sucertcapcheck.exe		\epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_dllmorecaps.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UDEB\Z\rucerttest\rucert_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UDEB\Z\rucerttest\rucert_file2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UDEB\Z\rucerttest\rucert_file3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\81000018\rucert_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\81000018\rucert_config2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\81000018\rucert_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\81000018\rucert_config4.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\81000018\rucert_config5.txt
md \epoc32\release\winscw\udeb\z\cr1027test
md \epoc32\winscw\c\private\10273582
md \epoc32\release\winscw\udeb\z\private\10273582
copy /y \epoc32\release\winscw\udeb\eclipsetest1.dll 		\epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_library.dll
copy /y \epoc32\release\winscw\udeb\tsaromupgradeexe.exe 	\epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_execute.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UDEB\Z\cr1027test\cr1027_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config4.txt


rem For UREL
md \epoc32\release\winscw\urel\z\rucerttest
md \epoc32\release\winscw\urel\z\private\81000018
md \epoc32\winscw\c\private\81000018
copy /y \epoc32\release\winscw\urel\sucertdll.dll		\epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_library.dll
copy /y \epoc32\release\winscw\urel\sucerttestexe.exe 	\epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_execute.exe
copy /y \epoc32\release\winscw\urel\sucertcapcheck.exe		\epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_dllmorecaps.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UREL\Z\rucerttest\rucert_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UREL\Z\rucerttest\rucert_file2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UREL\Z\rucerttest\rucert_file3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UREL\Z\private\81000018\rucert_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UREL\Z\private\81000018\rucert_config2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UREL\Z\private\81000018\rucert_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UREL\Z\private\81000018\rucert_config4.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UREL\Z\private\81000018\rucert_config5.txt
md \epoc32\release\winscw\urel\z\cr1027test
md \epoc32\winscw\c\private\10273582
md \epoc32\release\winscw\urel\z\private\10273582
copy /y \epoc32\release\winscw\urel\eclipsetest1.dll 		\epoc32\release\winscw\urel\z\sys\bin\saromupgrade_library.dll
copy /y \epoc32\release\winscw\urel\tsaromupgradeexe.exe 	\epoc32\release\winscw\urel\z\sys\bin\saromupgrade_execute.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\urel\Z\cr1027test\cr1027_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config4.txt


call %SECURITYSOURCEDIR%\installationservices\switestfw\test\autotesting\setupSwiCertstore.bat swicertstore.dat
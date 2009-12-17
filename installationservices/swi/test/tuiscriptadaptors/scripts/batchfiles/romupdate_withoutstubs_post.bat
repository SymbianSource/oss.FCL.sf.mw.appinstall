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

del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_execute.exe
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\sucertromupgrade_dllmorecaps.exe
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\rucerttest\rucert_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\rucerttest\rucert_file2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\rucerttest\rucert_file3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\81000018\rucert_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\81000018\rucert_config2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\81000018\rucert_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\81000018\rucert_config4.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\81000018\rucert_config5.txt
del /s /f /q \epoc32\WINSCW\c\private\81000018\rucert_config3.txt
rd  /q \epoc32\release\winscw\udeb\z\rucerttest

del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_execute.exe
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\cr1027test\cr1027_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config4.txt
del /s /f /q \epoc32\WINSCW\c\private\10273582\cr1027_config3.txt
rd  /q \epoc32\release\winscw\udeb\z\cr1027test

del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_execute.exe
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\sucertromupgrade_dllmorecaps.exe
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\rucerttest\rucert_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\rucerttest\rucert_file2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\rucerttest\rucert_file3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\81000018\rucert_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\81000018\rucert_config2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\81000018\rucert_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\81000018\rucert_config4.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\81000018\rucert_config5.txt
del /s /f /q \epoc32\WINSCW\c\private\81000018\rucert_config3.txt
rd  /q \epoc32\release\winscw\urel\z\rucerttest

del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\saromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\saromupgrade_execute.exe
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\cr1027test\cr1027_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config4.txt
rd  /q \epoc32\release\winscw\udeb\z\cr1027test

call %SECURITYSOURCEDIR%\installationservices\switestfw\test\autotesting\setupSwiCertstore.bat swicertstore.dat
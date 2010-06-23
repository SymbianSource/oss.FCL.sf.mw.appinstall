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

del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\test\testsidcache1.exe
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\test1\test2\testsidcache2.exe
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\eclipsetest.dll
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\sntpclient.exe
del /s /f /q \epoc32\release\winscw\udeb\z\system\install\eclispingteststub.sis
del /s /f /q \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file3.txt
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_execute.exe
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\cr1027test\cr1027_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\udeb\Z\private\10273582\cr1027_config4.txt
del /s /f /q \epoc32\WINSCW\c\private\10273582\cr1027_config3.txt
rd  /q \epoc32\release\winscw\udeb\z\cr1027test

del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\test\testsidcache1.exe
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\test1\test2\testsidcache2.exe
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\eclipsetest.dll
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\sntpclient.exe
del /s /f /q \epoc32\release\winscw\urel\z\system\install\eclispingteststub.sis
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file2.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file3.txt
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe

del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\saromupgrade_library.dll
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\saromupgrade_execute.exe
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\cr1027test\cr1027_file1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config1.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config3.txt
del /s /f /q \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config4.txt
rd  /q \epoc32\release\winscw\udeb\z\cr1027test

rem cleanup for last 3 eclipse tests
del /s /Q \epoc32\release\winscw\udeb\z\sys\bin\testeclipse{00010001}.dll 
del /s /Q \epoc32\release\winscw\urel\z\sys\bin\testeclipse{00010001}.dll

rem For DEF137195 - Broken stub sis file
del /s /f /q \epoc32\release\winscw\udeb\z\system\install\a_normal_sis_as_stub.sis
del /s /f /q \epoc32\release\winscw\urel\z\system\install\a_normal_sis_as_stub.sis

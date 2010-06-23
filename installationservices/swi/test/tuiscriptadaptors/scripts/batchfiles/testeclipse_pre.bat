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

md \epoc32\release\winscw\udeb\z\sys\bin\test\
md \epoc32\release\winscw\udeb\z\sys\bin\test1\test2\
copy /y \epoc32\release\winscw\udeb\tswisidcache1.exe 		\epoc32\release\winscw\udeb\z\sys\bin\test\testsidcache1.exe
copy /y \epoc32\release\winscw\udeb\tswisidcache2.exe 		\epoc32\release\winscw\udeb\z\sys\bin\test1\test2\testsidcache2.exe
copy /y \epoc32\release\winscw\udeb\eclipsetest1.dll 		\epoc32\release\winscw\udeb\z\sys\bin\eclipsetest.dll
copy /y \epoc32\release\winscw\udeb\sntpclient.exe 		\epoc32\release\winscw\udeb\z\sys\bin\sntpclient.exe

md \epoc32\release\winscw\udeb\z\eclipsetest
copy /y \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis \epoc32\release\winscw\udeb\z\system\install\eclispingteststub.sis
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\eclipsetest\file3.txt
copy /y \epoc32\release\winscw\udeb\tswisidupgradeexe.exe 	\epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

md \epoc32\release\winscw\urel\z\sys\bin\test\
md \epoc32\release\winscw\urel\z\sys\bin\test1\test2\
copy /y \epoc32\release\winscw\urel\tswisidcache1.exe 		\epoc32\release\winscw\urel\z\sys\bin\test\testsidcache1.exe
copy /y \epoc32\release\winscw\urel\tswisidcache2.exe 		\epoc32\release\winscw\urel\z\sys\bin\test1\test2\testsidcache2.exe
copy /y \epoc32\release\winscw\urel\eclipsetest1.dll 		\epoc32\release\winscw\urel\z\sys\bin\eclipsetest.dll
copy /y \epoc32\release\winscw\urel\sntpclient.exe 		\epoc32\release\winscw\urel\z\sys\bin\sntpclient.exe

md \epoc32\release\winscw\urel\z\eclipsetest
copy /y \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis \epoc32\release\winscw\urel\z\system\install\eclispingteststub.sis
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\urel\Z\eclipsetest\file3.txt
copy /y \epoc32\release\winscw\urel\tswisidupgradeexe.exe 	\epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe

rem setup for last 3 eclipse tests
rem These need the swidaemon.exe and testeclipse{00010001}.dll in z:\sys\bin before the test.
copy \epoc32\release\winscw\udeb\swidaemon.exe \epoc32\release\winscw\udeb\z\sys\bin\swidaemon.exe 
copy \epoc32\release\winscw\urel\swidaemon.exe \epoc32\release\winscw\urel\z\sys\bin\swidaemon.exe 

rem create some files to try to eclipse, any contents ok as its the name that matters
copy \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\scripts\testeclipse.script \epoc32\release\winscw\udeb\z\sys\bin\testeclipse{00010001}.dll 
copy \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\scripts\testeclipse.script  \epoc32\release\winscw\urel\z\sys\bin\testeclipse{00010001}.dll

rem For CR1027 - ROM Upgrade with SA SIS
rem For UDEB
md \epoc32\release\winscw\udeb\z\cr1027test
md \epoc32\release\winscw\udeb\z\private\10273582
md \epoc32\winscw\c\private\10273582
copy /y \epoc32\release\winscw\udeb\eclipsetest1.dll 		\epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_library.dll
copy /y \epoc32\release\winscw\udeb\tsaromupgradeexe.exe 	\epoc32\release\winscw\udeb\z\sys\bin\saromupgrade_execute.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UDEB\Z\cr1027test\cr1027_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UDEB\Z\private\10273582\cr1027_config4.txt

rem For UREL
md \epoc32\release\winscw\urel\z\cr1027test
md \epoc32\release\winscw\urel\z\private\10273582
copy /y \epoc32\release\winscw\urel\eclipsetest1.dll 		\epoc32\release\winscw\urel\z\sys\bin\saromupgrade_library.dll
copy /y \epoc32\release\winscw\urel\tsaromupgradeexe.exe 	\epoc32\release\winscw\urel\z\sys\bin\saromupgrade_execute.exe
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\urel\Z\cr1027test\cr1027_file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config3.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\urel\Z\private\10273582\cr1027_config4.txt

rem For DEF137195 - Broken stub sis file
copy /y \epoc32\winscw\c\tswi\tsis\data\a_normal_sis_as_stub.sis \epoc32\release\winscw\udeb\z\system\install\a_normal_sis_as_stub.sis
copy /y \epoc32\winscw\c\tswi\tsis\data\a_normal_sis_as_stub.sis \epoc32\release\winscw\urel\z\system\install\a_normal_sis_as_stub.sis

del /s /Q \epoc32\winscw\c\sys\install\sisregistry
del /s /f /q \epoc32\winscw\c\sys\install\scr\scr.db

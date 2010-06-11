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
@echo off

set PLATFORM=%1%
set CONFIG=%2%

REM Build main tests
for %%i in (tests\*.txt) do perl -w buildsis.pl --platform %PLATFORM% --configuration %CONFIG% --sis-dir sis "%%i"

REM OCSP tests
perl -w buildsis.pl --platform %PLATFORM% --configuration %CONFIG% -sis-dir sis ocsp\ocsp_01.txt 
perl -w buildsis.pl --platform %PLATFORM% --configuration %CONFIG% -sis-dir sis ocsp\ocsp_02.txt 

REM Sign with revoked certificate
signsis sis\%PLATFORM%\ocsp_01.sis sis\%PLATFORM%\ocsp_01.tmp %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tdevcerts\ocsp\revoked_user_caps.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tdevcerts\ocsp\revoked_user_caps.key.pem
signsis sis\%PLATFORM%\ocsp_02.sis sis\%PLATFORM%\ocsp_02.tmp %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tdevcerts\ocsp\revoked_user_caps.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tdevcerts\ocsp\revoked_user_caps.key.pem
cd sis\%PLATFORM%
move ocsp_01.tmp ocsp_01.sis
move ocsp_02.tmp ocsp_02.sis
cd ..\..

REM Create an expired certificate
perl -w buildsis.pl --platform %PLATFORM% --configuration %CONFIG% -sis-dir sis -days 30 ocsp\ocsp_03.txt

REM Additional tests for defect fixes etc. These are generated separately to the main
REM tests to avoid re-generating existing sis files. If the SIS files were generated
REM at build time then these tests should be combined with the main tests.
REM Moved to regular TESTS folder
rem for %%i in (additional_tests\*.txt) do perl -w buildsis.pl --platform %PLATFORM% --configuration %CONFIG% --sis-dir sis "%%i"

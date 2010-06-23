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

for %%i in (tests\*.txt) do perl -w buildsis.pl --platform %PLATFORM% --configuration udeb --sis-dir sis "%%i"

REM OCSP tests
REM perl -w buildsis.pl --platform %PLATFORM% --configuration udeb -sis-dir sis ocsp\ocsp_01.txt 
REM perl -w buildsis.pl --platform %PLATFORM% --configuration udeb -sis-dir sis ocsp\ocsp_02.txt 

REM Sign with revoked certificate
REM signsis sis\%PLATFORM%\ocsp_01.sis sis\%PLATFORM%\ocsp_01.tmp %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\revoked_user_caps.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\revoked_user_caps.key.pem
REM signsis sis\%PLATFORM%\ocsp_02.sis sis\%PLATFORM%\ocsp_02.tmp %SECURITYSOURCEDIR%\\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\revoked_user_caps.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\revoked_user_caps.key.pem
REM cd sis\%PLATFORM%
REM move ocsp_01.tmp ocsp_01.sis
REM move ocsp_02.tmp ocsp_02.sis
REM cd ..\..

REM Create an expired certificate
REM perl -w buildsis.pl --platform %PLATFORM% --configuration udeb -sis-dir sis -days 30 ocsp\ocsp_03.txt 


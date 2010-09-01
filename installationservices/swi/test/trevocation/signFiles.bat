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
@echo on

set PLATFORM=%1%
set CERT_PATH="%SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation"

REM Sign with certificate A
signsis sisInteg\%PLATFORM%\ocsp_01.sis sisInteg\%PLATFORM%\ocsp_01.tmp %CERT_PATH%\ocsp\responder1\certTrev1.cert.pem %CERT_PATH%\ocsp\responder1\certTrev1.key.pem
signsis sisInteg\%PLATFORM%\ocsp_02.sis sisInteg\%PLATFORM%\ocsp_02.tmp %CERT_PATH%\ocsp\responder2\certTrev1.cert.pem %CERT_PATH%\ocsp\responder2\certTrev1.key.pem

REM Sign with certificate B 
signsis sisInteg\%PLATFORM%\ocsp_03.sis sisInteg\%PLATFORM%\ocsp_03.tmp %CERT_PATH%\ocsp\responder1\certTrev2.cert.pem %CERT_PATH%\ocsp\responder1\certTrev2.key.pem
signsis sisInteg\%PLATFORM%\ocsp_04.sis sisInteg\%PLATFORM%\ocsp_04.tmp %CERT_PATH%\ocsp\responder2\certTrev2.cert.pem %CERT_PATH%\ocsp\responder2\certTrev2.key.pem

REM Sign with certificate A 
signsis sisInteg\%PLATFORM%\ocsprevocation.sis      sisInteg\%PLATFORM%\ocsprevocation.tmp %CERT_PATH%\ocsp\responder1\certTrev2.cert.pem %CERT_PATH%\ocsp\responder1\certTrev2.key.pem


REM overwrite the original sis files
cd sisInteg\%PLATFORM%
move ocsp_01.tmp ocsp_01.sis
move ocsp_02.tmp ocsp_02.sis
move ocsp_03.tmp ocsp_03.sis
move ocsp_04.tmp ocsp_04.sis
move ocsprevocation.tmp		 ocsprevocation.sis

REM Return to current directory
cd ..\..

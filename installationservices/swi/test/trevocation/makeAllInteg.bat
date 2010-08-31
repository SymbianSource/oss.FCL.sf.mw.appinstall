@rem
@rem Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

@echo ON
REM cls
echo DO NOT RUN THIS PROGRAM - PRESS CTRL C to Cancel
echo THIS PROGRAM REGENERATES ALL OF THE SIS FILES USED TO TEST DEVELOPER CERTIFICATE EXTENSIONS
pause
rmdir /S /Q sisInteg 

REM The pkg files are hardcoded, but can be updated if needed
mkdir sisInteg\winscw
mkdir sisInteg\armv5 

REM create the package files for WINSCW
cd pkgInteg\winscw
makesis ocsp_01.pkg
makesis ocsp_02.pkg
makesis ocsp_03.pkg
makesis ocsp_04.pkg
makesis ocsp_05.pkg
makesis signingtest.pkg  certifiedsignedval.sis
makesis signingtest.pkg  certifiedsignedVeriSignval.sis
makesis signingtest.pkg  expresssignedval.sis
makesis signingtest.pkg  allsignedval.sis
makesis signingtest.pkg  selfsignedval.sis
makesis signingtest.pkg  certifiedsignedrev.sis
makesis signingtest.pkg  certifiedsignedVeriSignrev.sis
makesis signingtest.pkg  expresssignedrev.sis
makesis signingtest.pkg  allsignedrev.sis
makesis signingtest.pkg  selfsignedrev.sis


REM Embedded Package file
makesis ocsprevocation-sub1.pkg
cd ..
cd ..
signsis pkgInteg\winscw\ocsprevocation-sub1.sis    pkgInteg\winscw\ocsprevocation-sub1.tmp %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\responder1\certTrev2.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\responder1\certTrev2.key.pem
move	pkgInteg\winscw\ocsprevocation-sub1.tmp	   pkgInteg\winscw\ocsprevocation-sub1.sis
cd      pkgInteg\winscw
makesis ocsprevocation.pkg
cd ..
cd ..

copy pkgInteg\winscw\*.sis sisInteg\winscw\*.sis



REM create the package files for ARMV5
cd pkgInteg\armv5
makesis ocsp_01.pkg
makesis ocsp_02.pkg
makesis ocsp_03.pkg
makesis ocsp_04.pkg
makesis signingtest.pkg  certifiedsignedval.sis
makesis signingtest.pkg  certifiedsignedVeriSignval.sis
makesis signingtest.pkg  expresssignedval.sis
makesis signingtest.pkg  allsignedval.sis
makesis signingtest.pkg  selfsignedval.sis
makesis signingtest.pkg  certifiedsignedrev.sis
makesis signingtest.pkg  certifiedsignedVeriSignrev.sis
makesis signingtest.pkg  expresssignedrev.sis
makesis signingtest.pkg  allsignedrev.sis
makesis signingtest.pkg  selfsignedrev.sis

REM Embedded Package file
makesis ocsprevocation-sub1.pkg
cd ..
cd ..
signsis pkgInteg\armv5\ocsprevocation-sub1.sis    pkgInteg\armv5\ocsprevocation-sub1.tmp %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\responder1\certTrev2.cert.pem %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation\ocsp\responder1\certTrev2.key.pem
move    pkgInteg\armv5\ocsprevocation-sub1.tmp	  pkgInteg\armv5\ocsprevocation-sub1.sis
cd      pkgInteg\armv5
makesis ocsprevocation.pkg
cd ..
cd ..

copy pkgInteg\armv5\*.sis sisInteg\armv5\*.sis


call signFiles.bat winscw

call signFiles.bat armv5

:exit


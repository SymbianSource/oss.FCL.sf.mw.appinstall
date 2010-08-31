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

REM Sign with Express / Certified and VeriSign certs  
signsis sisInteg\%PLATFORM%\certifiedsignedval.sis			sisInteg\%PLATFORM%\certifiedsignedval.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsCertifiedSignValidCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsCertifiedSignValidPvtKey.key
signsis sisInteg\%PLATFORM%\certifiedsignedVeriSignval.sis 		sisInteg\%PLATFORM%\certifiedsignedVeriSignval.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsCertifiedVeriSignValidCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsCertifiedVeriSignValidPvtKey.key
signsis sisInteg\%PLATFORM%\expresssignedval.sis			sisInteg\%PLATFORM%\expresssignedval.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsExpressValidCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsExpressValidPvtKey.key
signsis sisInteg\%PLATFORM%\allsignedval.sis				sisInteg\%PLATFORM%\allsignedval.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsAllSignValidCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsAllSignValidPvtKey.key
signsis sisInteg\%PLATFORM%\selfsignedval.sis				sisInteg\%PLATFORM%\selfsignedval.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsSelfSignValidCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\AppInsSelfSignValidPvtKey.key
signsis sisInteg\%PLATFORM%\certifiedsignedrev.sis			sisInteg\%PLATFORM%\certifiedsignedrev.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsCertifiedRevokedCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsCertifiedRevokedPvtKey.key
signsis sisInteg\%PLATFORM%\certifiedsignedVeriSignrev.sis 		sisInteg\%PLATFORM%\certifiedsignedVeriSignrev.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsCertifiedVeriSignRevokedCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsCertifiedVeriSignRevokedPvtKey.key
signsis sisInteg\%PLATFORM%\expresssignedrev.sis			sisInteg\%PLATFORM%\expresssignedrev.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsExpressRevokedCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsExpressRevokedPvtKey.key
signsis sisInteg\%PLATFORM%\allsignedrev.sis				sisInteg\%PLATFORM%\allsignedrev.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsAllSignRevokedCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsAllSignRevokedPvtKey.key
signsis sisInteg\%PLATFORM%\selfsignedrev.sis				sisInteg\%PLATFORM%\selfsignedrev.tmp %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsSelfSignRevokedCert.pem %CERT_PATH%\ocsp\expressandcertifiedsignedCert\Revoked\AppInsSelfSignRevokedPvtKey.key

REM overwrite the original sis files
cd sisInteg\%PLATFORM%
move ocsp_01.tmp ocsp_01.sis
move ocsp_02.tmp ocsp_02.sis
move ocsp_03.tmp ocsp_03.sis
move ocsp_04.tmp ocsp_04.sis
move ocsprevocation.tmp		 ocsprevocation.sis
move certifiedsignedval.tmp	 certifiedsignedval.sis
move certifiedsignedVeriSignval.tmp certifiedsignedVeriSignval.sis
move expresssignedval.tmp		 expresssignedval.sis
move allsignedval.tmp		 allsignedval.sis
move selfsignedval.tmp		 selfsignedval.sis
move certifiedsignedrev.tmp	 certifiedsignedrev.sis
move certifiedsignedVeriSignrev.tmp certifiedsignedVeriSignrev.sis
move expresssignedrev.tmp		 expresssignedrev.sis
move allsignedrev.tmp		 allsignedrev.sis
move selfsignedrev.tmp		 selfsignedrev.sis

REM Return to current directory
cd ..\..

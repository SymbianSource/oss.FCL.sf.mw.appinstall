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
REM This creates the certstore used by software install

SET CERT_PATH="%SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis"
mkdir \epoc32\winscw\c\tswi\certstore

REM  Copy the certs and change from PEM to DER
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\Root5CA\ca.pem -out \epoc32\winscw\c\tswi\certstore\Root5CA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootCADSA\ca.pem -out \epoc32\winscw\c\tswi\certstore\Symbian-Test-DSA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootCARSA\ca.pem -out \epoc32\winscw\c\tswi\certstore\Symbian-Test-RSA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootTCBCARSA\cacert.pem -out \epoc32\winscw\c\tswi\certstore\Symbian-Test-TCB-RSA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootExpiredCARSA\cacert.pem -out \epoc32\winscw\c\tswi\certstore\Symbian-Test-Expired-RSA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\sucert\sucert.pem -out \epoc32\winscw\c\tswi\certstore\sucert.der
xcopy /r /y %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\writableswicertstore\certs\chain\intermediate\cacert.der \epoc32\winscw\c\tswi\certstore\Symbian-Test-Chain-RSA.der
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootCADiffSerial\cacert.pem -out \epoc32\winscw\c\tswi\certstore\SymbianTestRootCADiffSerial.der

REM New all capabilities CA certificate
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\symbiantestallcapabilitiesca\symbiantestallcapabilitiesca.pem -out \epoc32\winscw\c\tswi\certstore\symbiantestallcapabilitiesca.der

openssl x509 -req -in %CERT_PATH%\SymbianTestRootCADiffSerial\cert_chain_rsa_len1\first.rsa.req -out \epoc32\winscw\c\tswi\certstore\second.der -outform DER -CA SymbianTestRootCADiffSerial\cacert.pem -CAkey SymbianTestRootCADiffSerial\cakey.pem -CAserial SymbianTestRootCADiffSerial\cert_chain_rsa_len1\first.rsa.srl -CAcreateserial -days 3650 -extfile SymbianTestRootCADiffSerial\cert_chain_rsa_len1\first.rsa.config -extensions v3_ca
openssl x509 -inform PEM -outform DER -in %CERT_PATH%\SymbianTestRootCADiffSerial\cert_chain_rsa_len1\chain_rsa_len1.cert.pem -out \epoc32\winscw\c\tswi\certstore\first.der


REM  copy the spec
xcopy /r /y certstorespec*.txt \epoc32\winscw\c\tswi\certstore\

REM  build the stores
\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\certstorespec.txt c:\tswi\certstore\swicertstore.dat c:\tswi\certstore\buildlog.txt
\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\certstorespec_mandatory.txt c:\tswi\certstore\swicertstore_mandatory.dat c:\tswi\certstore\buildlog_mandatory.txt

copy \epoc32\winscw\c\tswi\certstore\swicertstore.dat swicertstore.dat

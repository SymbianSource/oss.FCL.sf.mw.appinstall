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

set CA_DIR=%1%
set CERT_DIR=%2%
set NAME=%3%
set SUBJECT=%4%

rem create a certificate request
openssl req -newkey rsa:512 -nodes -out %CERT_DIR%\%NAME%.req.pem -keyout %CERT_DIR%\%NAME%.key.pem -subj %SUBJECT% -config openssl.config -days 3650

rem sign the request
openssl ca -config openssl.config -cert %CA_DIR%\private\ca.pem -keyfile %CA_DIR%\private\ca.key.pem -name SymbianTestRootCARSA -in %CERT_DIR%\%NAME%.req.pem -out %CERT_DIR%\%NAME%.cert.pem -batch -days 3650 -extfile ext\%NAME%.cfg

rem convert to der
openssl x509 -in %CERT_DIR%\%NAME%.cert.pem -outform DER -out %CERT_DIR%\%NAME%.cert.der

rem revoke the certificate
openssl ca -config openssl.config -revoke %CERT_DIR%\%NAME%.cert.pem -name %CA_DIR% -cert %CA_DIR%\private\ca.pem -keyfile %CA_DIR%\private\ca.key.pem -crl_reason keyCompromise

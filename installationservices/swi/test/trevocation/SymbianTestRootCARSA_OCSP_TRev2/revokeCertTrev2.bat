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

set CA_DIR=SymbianTestRootCARSATRev
set CERT_DIR=%CA_DIR%\certs
set NAME=certTrev2
set SUBJECT="/C=UK/O=Symbian/CN=certTrev2"


rem revoke the certificate
openssl ca -config openssl.config -revoke %CERT_DIR%\%NAME%.cert.pem -name %CA_DIR% -cert %CA_DIR%\private\ca.pem -keyfile %CA_DIR%\private\ca.key.pem -crl_reason keyCompromise

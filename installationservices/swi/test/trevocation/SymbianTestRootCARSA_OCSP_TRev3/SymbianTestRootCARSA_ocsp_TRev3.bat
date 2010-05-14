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

set CERT_PATH="%SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\trevocation"

openssl ocsp -index SymbianTestRootCARSATRev\index.txt -CA %CERT_PATH%\SymbianTestRootCARSA_OCSP_TRev3\SymbianTestRootCARSATRev\private\ca.pem 
-rsigner %CERT_PATH%\SymbianTestRootCARSATRev\private\ca.pem -rkey %CERT_PATH%\SymbianTestRootCARSA_OCSP_TRev3\SymbianTestRootCARSATRev\private\ca.key.pem -port 19023 -resp_key_id -ignore_err -req_text -resp_text

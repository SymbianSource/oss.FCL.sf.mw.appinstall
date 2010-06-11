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
cls
echo WARNING
echo After running this script you must update the certificates used in Perforce (swi\tdevcerts)
echo press Ctrl-C to abort
pause

set CA_DIR=SymbianTestRootCARSA
set CERT_DIR=%CA_DIR%\certs

del %CERT_DIR%\* /S /Q
del %CA_DIR%\index.* /S /Q

mkdir %CERT_DIR%
echo 01 > %CA_DIR%\serial
type nul > %CA_DIR%\index.txt

set NAME=revoked_user_caps
set SUBJECT="/C=UK/O=Symbian/CN=revoked_user_caps"

call make_and_revoke.bat %CA_DIR% %CERT_DIR% %NAME% %SUBJECT%
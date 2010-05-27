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
cls
echo WARNING
echo After running this script you must update the certificates used in Perforce (swi\tdevcerts)
echo press Ctrl-C to abort
pause

set CA_DIR=SymbianTestRootCARSATRev
set CERT_DIR=%CA_DIR%\certs

del %CERT_DIR%\* /S /Q
del %CA_DIR%\index.* /S /Q

mkdir %CERT_DIR%
echo 01 > %CA_DIR%\serial
type nul > %CA_DIR%\index.txt

set NAME=certTrev1
set SUBJECT="/C=UK/O=Symbian/OU=Security/CN=certTrev1"

call make_and_DoNotRevoke2.bat %CA_DIR% %CERT_DIR% %NAME% %SUBJECT%

REM create 2nd certificate

rem echo 02 > %CA_DIR%\serial
rem type 01 > %CA_DIR%\index.txt
set CA_DIR2=SymbianTestRootCARSATRev
set CERT_DIR2=%CA_DIR%\certs

set NAME2=certTrev2
set SUBJECT2="/C=UK/O=Symbian/CN=certTrev2"

call make_and_DoNotRevoke2.bat %CA_DIR2% %CERT_DIR2% %NAME2% %SUBJECT2%

PAUSE
REM Do the copy before revoking any certs
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
@echo !!! Do not run this program. !!!
@echo This script regenerates custom generated SIS files and may require tools 
@echo that are not built as standard.
@echo e.g The debug version of makesis, openssl etc 
@echo Press Ctrl-C to exit.
pause

rem DEF053285
%SECURITYSOURCEDIR%\swi\source\makesis\debug\makesis -bP -q1 -r1 def053285_winscw.pkg sis\winscw\def053285-tmp.sis
signsis -s sis\winscw\def053285-tmp.sis sis\winscw\def053285.sis %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\testexes\certs\default.cer ..\certs\default.key
del sis\winscw\def053285-tmp.sis /S /Q

%SECURITYSOURCEDIR%\swi\source\makesis\debug\makesis -bP -q1 -r1 def053285_armv5.pkg sis\armv5\def053285-tmp.sis
signsis -s sis\armv5\def053285-tmp.sis sis\armv5\def053285.sis %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\testexes\certs\default.cer ..\certs\default.key
del sis\armv5\def053285-tmp.sis /S /Q
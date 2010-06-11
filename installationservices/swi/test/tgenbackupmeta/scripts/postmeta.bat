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
rem remove the directories created

rmdir /S /Q %EPOCROOT%epoc32\winscw\c\tswi\test\tgenbackupmeta\scripts\data
rmdir /S /Q %EPOCROOT%epoc32\winscw\c\tswi\test\tgenbackupmeta\scripts\metadata
call %SECURITYSOURCEDIR%\installationservices\switestfw\test\autotesting\useautocfg.bat
call %SECURITYSOURCEDIR%\installationservices\switestfw\test\autotesting\setupSwiCertstore.bat swicertstore.dat
@echo on


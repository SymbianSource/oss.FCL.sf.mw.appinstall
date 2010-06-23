@rem
@rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

set PLATFORM=%1
set CFG=%2

del /f /q %EPOCROOT%epoc32\release\%PLATFORM%\%CFG%\siflauncher.exe

del /f /q %EPOCROOT%epoc32\release\%PLATFORM%\%CFG%\z\resource\apps\siflauncher.mbm
del /f /q %EPOCROOT%epoc32\release\%PLATFORM%\%CFG%\z\resource\apps\siflauncher_loc.rsc
del /f /q %EPOCROOT%epoc32\release\%PLATFORM%\%CFG%\z\private\10003a3f\apps\siflauncher_reg.rsc

del /f /q %EPOCROOT%epoc32\data\z\resource\apps\siflauncher.mbm
del /f /q %EPOCROOT%epoc32\data\z\resource\apps\siflauncher_loc.rsc
del /f /q %EPOCROOT%epoc32\data\z\private\10003a3f\apps\siflauncher_reg.rsc

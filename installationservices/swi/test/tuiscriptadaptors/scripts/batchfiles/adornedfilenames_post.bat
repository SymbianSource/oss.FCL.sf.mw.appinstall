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
REM Remove installed files during tescases 3.7 and 3.8 /exp: PU ROM stub upgrades can't be removed via SWI/

del /s /f /q  \epoc32\winscw\c\sys\bin\adornedfilenametestingdll4.dll
del /s /f /q  \epoc32\winscw\c\sys\bin\adornedfilenametestingdll4{000A0001}.dll
del /s /f /q  \epoc32\winscw\c\sys\hash\adornedfilenametestingdll4.dll
del /s /f /q  \epoc32\winscw\c\sys\hash\adornedfilenametestingdll4{000A0001}.dll

REM Restore the original sisregistry record for ROM stub 0x2baecad8 after testcases 3.7 and 3.8 /exp: PU ROM stub upgrades can't be removed via SWI/

del /s /f /q  \epoc32\winscw\c\sys\install\sisregistry\2baecad8\*.*
copy \epoc32\winscw\c\adornedfilenamesbackup_sisregistry_2baecad8\00000000.reg  \epoc32\winscw\c\sys\install\sisregistry\2baecad8\00000000.reg
copy \epoc32\winscw\c\adornedfilenamesbackup_sisregistry_2baecad8\00000000_0000.ctl  \epoc32\winscw\c\sys\install\sisregistry\2baecad8\00000000_0000.ctl
rd /s /q  \epoc32\winscw\c\adornedfilenamesbackup_sisregistry_2baecad8


@rem
@rem Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
REM Copy the files required to run the script

xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fota_stub_v2.sis \epoc32\release\winscw\udeb\z\system\install\
xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fota_stub_v2.sis \epoc32\release\winscw\urel\z\system\install\

xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateapp2_reg.rsc \epoc32\release\winscw\udeb\z\private\10003a3f\apps\
xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateapp2_reg.rsc \epoc32\release\winscw\urel\z\private\10003a3f\apps\

xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdatestubapp2_reg.rsc \epoc32\release\winscw\udeb\z\private\10003a3f\apps\
xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdatestubapp2_reg.rsc  \epoc32\release\winscw\urel\z\private\10003a3f\apps\

xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fotaupdatestubapp2.sis \epoc32\release\winscw\udeb\z\system\install\
xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fotaupdatestubapp2.sis \epoc32\release\winscw\urel\z\system\install\

xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\sw.txt 		\epoc32\release\winscw\urel\z\resource\versions\
xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\sw.txt 		\epoc32\release\winscw\udeb\z\resource\versions\

del /s /f /q \epoc32\winscw\c\sys\install\scr\
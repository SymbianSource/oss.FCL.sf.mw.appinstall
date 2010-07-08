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

xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fota_stub_v1.sis \epoc32\release\winscw\udeb\z\system\install\
xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fota_stub_v1.sis \epoc32\release\winscw\urel\z\system\install\

xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateapp1_reg.rsc \epoc32\release\winscw\udeb\z\private\10003a3f\apps\
xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateapp1_reg.rsc \epoc32\release\winscw\urel\z\private\10003a3f\apps\

xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdatestubapp1_reg.rsc \epoc32\release\winscw\udeb\z\private\10003a3f\apps\
xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdatestubapp1_reg.rsc \epoc32\release\winscw\urel\z\private\10003a3f\apps\

xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fotaupdatestubapp1.sis \epoc32\release\winscw\udeb\z\system\install\
xcopy /Q /Y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\fotaupdatestubapp1.sis \epoc32\release\winscw\urel\z\system\install\

xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateUDA1_reg.rsc \epoc32\winscw\c\private\10003a3f\apps\

md \epoc32\winscw\c\private\10003a3f\import\apps
xcopy /Q /Y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\fotaupdateUDA2_reg.rsc \epoc32\winscw\c\private\10003a3f\import\apps

rem cleanup anything from previous tests.
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\

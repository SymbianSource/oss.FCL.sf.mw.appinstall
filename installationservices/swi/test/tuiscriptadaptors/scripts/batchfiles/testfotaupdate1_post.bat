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
REM Remove the files added by this script

del /s /f /q \epoc32\release\winscw\udeb\z\system\install\fota_stub_v1.sis
del /s /f /q \epoc32\release\winscw\urel\z\system\install\fota_stub_v1.sis

del /s /f /q \epoc32\release\winscw\udeb\z\system\install\fotaupdatestubapp1.sis
del /s /f /q \epoc32\release\winscw\urel\z\system\install\fotaupdatestubapp1.sis

del /s /f /q \epoc32\release\winscw\udeb\z\private\10003a3f\apps\fotaupdatestubapp1_reg.rsc
del /s /f /q \epoc32\release\winscw\urel\z\private\10003a3f\apps\fotaupdatestubapp1_reg.rsc

del /s /f /q \epoc32\release\winscw\udeb\z\private\10003a3f\apps\fotaupdateapp1_reg.rsc
del /s /f /q \epoc32\release\winscw\urel\z\private\10003a3f\apps\fotaupdateapp1_reg.rsc

del /s /f /q \epoc32\winscw\c\private\10003a3f\apps\fotaupdateUDA1_reg.rsc

del /s /f /q \epoc32\winscw\c\private\10003a3f\import\apps\fotaupdateUDA2_reg.rsc

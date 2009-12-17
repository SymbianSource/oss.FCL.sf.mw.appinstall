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

REM Copy the files required to run the script
copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe \epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

copy \epoc32\release\winscw\urel\tswisidupgradeexe.exe \epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe

xcopy /Q /Y \epoc32\winscw\c\tswi\tbackuprestore\data\a_stub.sis \epoc32\release\winscw\udeb\z\system\install\
xcopy /Q /Y \epoc32\winscw\c\tswi\tbackuprestore\data\a_stub.sis \epoc32\release\winscw\urel\z\system\install\

rem cleanup anything from previous tests.
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\

copy /Y \epoc32\winscw\c\tswi\policies\swipolicy_disallowoverwriteonrestore.ini \epoc32\release\winscw\udeb\z\system\data\swipolicy.ini
copy /Y \epoc32\winscw\c\tswi\policies\swipolicy_disallowoverwriteonrestore.ini \epoc32\release\winscw\urel\z\system\data\swipolicy.ini
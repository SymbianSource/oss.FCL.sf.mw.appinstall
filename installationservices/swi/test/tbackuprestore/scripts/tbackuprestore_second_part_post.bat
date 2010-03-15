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

rd /S /Q \epoc32\winscw\c\sys
move /Y \epoc32\winscw\c\sys_save \epoc32\winscw\c\sys 

del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\backup*.*
del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\metadata*
del /s /f /q \epoc32\winscw\c\sys\bin\backuprestore_test_*
del /s /f /q \epoc32\winscw\c\sys\hash\backuprestore_test_*
del /s /f /q \epoc32\winscw\c\sys\install\scr\scr.*
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\2baecde2\*.*
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\2baecde2
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
rem Set up tests using the automatic connection commdb configuration

call \epoc32\release\wins\udeb\ced -dtextshell -- -i c:\auto.cfg
call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg

del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\backup*.1
del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\metadata*

rem Change the config file of SIM.TSY back
call rename \epoc32\winscw\c\config.txt config2.txt
call rename \epoc32\winscw\c\configsave.txt config.txt

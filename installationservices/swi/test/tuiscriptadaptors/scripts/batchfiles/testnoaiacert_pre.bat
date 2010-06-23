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
REM This batch file will place the new ocspppolicy.ini file into 
REM z:\system\data\ before running the test for installation of testnoaiacert.script
REM Backup the existing ocsppolicy before bringing in a new copy

md \epoc32\release\winscw\udeb\z\testnoaiacert

copy /y \epoc32\release\winscw\udeb\z\system\data\ocsppolicy.ini \epoc32\release\winscw\udeb\z\testnoaiacert\ocsppolicy.ini


copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\ocsppolicy_noaiacert.ini \epoc32\release\winscw\udeb\z\system\data\ocsppolicy.ini


copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\ocsppolicy_noaiacert.ini \epoc32\release\winscw\urel\z\system\data\ocsppolicy.ini




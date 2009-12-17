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

@ECHO Off

REM Setup required before running testexecute overwriteprivate_positive.script
REM Copies the swi policy file specified in %1 to the correct place
REM Called with the the policy file as the only argument

set FILE=\epoc32\winscw\c\tswi\policies\%1

echo Installing SWI cert store data file from %FILE%

mkdir \epoc32\release\winscw\udeb\z\System\Data\
mkdir \epoc32\release\winscw\urel\z\System\Data\
copy %FILE% \epoc32\release\winscw\udeb\z\System\Data\swipolicy.ini
copy %FILE% \epoc32\release\winscw\urel\z\System\Data\swipolicy.ini

echo Copy tswiusergrantable_none.exe into ROM

rem Copy tswiusergrantable_none.exe into ROM
rem
rem This is required for the second test in overwriteprivate.bat
rem testing DEF094558: Overwriting orphaned executables blows up install process
rem 

copy \epoc32\release\winscw\udeb\tswiusergrantable_none.exe \epoc32\release\winscw\udeb\Z\tswi\testexes\tswiusergrantable_none.exe
copy \epoc32\release\winscw\urel\tswiusergrantable_none.exe \epoc32\release\winscw\urel\Z\tswi\testexes\tswiusergrantable_none.exe


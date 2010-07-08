@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

set FILE=\epoc32\winscw\c\tswi\certstore\swicertstore_sucert.dat

echo Installing SWI cert store data file from %FILE%

mkdir \epoc32\release\winscw\udeb\z\resource\
mkdir \epoc32\release\winscw\urel\z\resource\
copy /y %FILE% \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy /y %FILE% \epoc32\release\winscw\urel\z\resource\swicertstore.dat

copy /y \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis \epoc32\release\winscw\urel\z\system\install\eclispingteststub.sis
copy /y \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis \epoc32\release\winscw\udeb\z\system\install\eclispingteststub.sis
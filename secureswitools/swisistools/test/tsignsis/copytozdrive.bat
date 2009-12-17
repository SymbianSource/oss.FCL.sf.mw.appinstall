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
 REM This script will copy the generated sis file to z drive and delete from the current folder.

md \epoc32\release\winscw\udeb\z\tswi\tsis\data
md \epoc32\release\winscw\urel\z\tswi\tsis\data
copy /y \epoc32\winscw\c\tswi\tsignsis\*.sis \epoc32\release\winscw\udeb\z\tswi\tsis\data
copy /y \epoc32\winscw\c\tswi\tsignsis\*.sis \epoc32\release\winscw\urel\z\tswi\tsis\data
del *.sis


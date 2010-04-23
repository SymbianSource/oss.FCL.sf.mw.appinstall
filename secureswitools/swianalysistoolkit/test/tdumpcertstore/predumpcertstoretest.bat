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

xcopy /r /y /e \epoc32\winscw\c\tswi\tdumpcertstore\data\AllCapabilities.dat  \epoc32\winscw\c\tswi\tdumpcertstore\data\00000000
xcopy /r /y /e \epoc32\winscw\c\tswi\tdumpcertstore\data\00000001 \epoc32\winscw\c\resource\swicertstore\dat\
xcopy /r /y /e \epoc32\winscw\c\tswi\tdumpcertstore\data\00000000 \epoc32\winscw\c\resource\swicertstore\dat\
copy \epoc32\winscw\c\tswi\tdumpcertstore\data\00000001 \epoc32\winscw\c\tswi\tdumpcertstore\corrupted.dat
xcopy /r /y /e \epoc32\winscw\c\tswi\tdumpcertstore\data\*.dat \epoc32\winscw\c\tswi\tdumpcertstore






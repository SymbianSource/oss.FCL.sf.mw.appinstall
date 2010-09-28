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

rem Place the languages.txt file to read the device supported languages
md \epoc32\release\winscw\udeb\z\resource\bootdata
md \epoc32\release\winscw\urel\z\resource\bootdata
copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\languages.txt			\epoc32\release\winscw\udeb\z\resource\bootdata\languages.txt
copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\languages.txt			\epoc32\release\winscw\urel\z\resource\bootdata\languages.txt
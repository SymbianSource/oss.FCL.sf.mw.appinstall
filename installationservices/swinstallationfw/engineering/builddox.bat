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

rmdir /S /Q output
mkdir output
del /F /Q UsifApi.chm
del /F /Q UsifApi.pdf
doxygen Doxyfile.cfg > doxygen.log 2>&1
UsifApi.chm
doxygen.log
echo Building PDF...
cd output\latex
make pdf
cd ..\..
ren output\latex\refman.pdf .\UsifApi.pdf
echo Done.

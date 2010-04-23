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
REM Backup the original swicertstore.dat
copy \epoc32\release\winscw\udeb\z\resource\swicertstore_org.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy \epoc32\release\winscw\urel\z\resource\swicertstore_org.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat
del  \epoc32\release\winscw\udeb\z\resource\swicertstore_org.dat
del  \epoc32\release\winscw\urel\z\resource\swicertstore_org.dat

REM Copy the files required to run the script
del /s /f /q \epoc32\release\winscw\udeb\z\eclipsetest\file1.txt
del /s /f /q \epoc32\release\winscw\udeb\z\eclipsetest\file2.txt
del /s /f /q \epoc32\release\winscw\udeb\z\eclipsetest\file3.txt
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

del /s /f /q \epoc32\release\winscw\urel\z\eclipsetest\file1.txt
del /s /f /q \epoc32\release\winscw\urel\z\eclipsetest\file2.txt
del /s /f /q \epoc32\release\winscw\urel\z\eclipsetest\file3.txt
del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe
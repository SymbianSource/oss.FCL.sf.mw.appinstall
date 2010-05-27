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
copy \epoc32\release\winscw\udeb\z\resource\swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore_org.dat
copy \epoc32\release\winscw\urel\z\resource\swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore_org.dat

rem copy appropriate swicertstore
rem currently use the tsis certstore but need to change this when tests improve
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat

REM Copy the files required to run the script
md \epoc32\release\winscw\udeb\z\eclipsetest
copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\release\winscw\udeb\z\eclipsetest\file1.txt
copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\release\winscw\udeb\z\eclipsetest\file2.txt
copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\release\winscw\udeb\z\eclipsetest\file3.txt
copy \epoc32\release\winscw\udeb\tswisidupgradeexe.exe \epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

md \epoc32\release\winscw\urel\z\eclipsetest
copy \epoc32\winscw\c\tswi\teclipsing\data\file1.txt   \epoc32\release\winscw\urel\z\eclipsetest\file1.txt
copy \epoc32\winscw\c\tswi\teclipsing\data\file2.txt   \epoc32\release\winscw\urel\z\eclipsetest\file2.txt
copy \epoc32\winscw\c\tswi\teclipsing\data\file3.txt   \epoc32\release\winscw\urel\z\eclipsetest\file3.txt
copy \epoc32\release\winscw\urel\tswisidupgradeexe.exe \epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe
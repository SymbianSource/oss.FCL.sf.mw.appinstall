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
REM This creates the certstore used by software install


mkdir \epoc32\winscw\c\tswi\certstore
REM xcopy /r /y ..\..\..\..\..\swi\test\tsisfile\data\signedsis\Root5CA\ca.pem\ca.pem .

REM  Copy the certs and change from PEM to DER
xcopy /r /y certificates\*.der \epoc32\winscw\c\tswi\certstore\
REM  copy the spec
xcopy /r /y data\*.str \epoc32\winscw\c\tswi\certstore\

REM  build the stores
\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\AllCapabilities.str c:\tswi\certstore\AllCapabilities.dat c:\tswi\certstore\AllCapabilities.log

\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\AllMandatory.str c:\tswi\certstore\AllMandatory.dat c:\tswi\certstore\AllMandatory.log

\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\suflagsupport.str c:\tswi\certstore\suflagsupport.dat c:\tswi\certstore\suflagsupport.log

\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\NoMandatory.str c:\tswi\certstore\NoMandatory.dat c:\tswi\certstore\NoMandatory.log

\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\somecapabilities.str c:\tswi\certstore\somecapabilities.dat c:\tswi\certstore\somecapabilities.log

\epoc32\release\winscw\udeb\swicertstoretool c:\tswi\certstore\unicodecertificatename.str  c:\tswi\certstore\unicodecertificatename.dat c:\tswi\certstore\unicodecertificatename.log


copy \epoc32\winscw\c\tswi\certstore\AllCapabilities.dat  data\AllCapabilities.dat 
copy \epoc32\winscw\c\tswi\certstore\AllMandatory.dat  data\AllMandatory.dat 
copy \epoc32\winscw\c\tswi\certstore\NoMandatory.dat  data\NoMandatory.dat 
copy \epoc32\winscw\c\tswi\certstore\somecapabilities.dat data\somecapabilities.dat 
copy \epoc32\winscw\c\tswi\certstore\unicodecertificatename.dat  data\unicodecertificatename.dat

mkdir \epoc32\winscw\c\resource\swicertstore\dat

copy data\AllCapabilities.dat  data\00000000
copy data\00000001 corrupted.dat
copy data\00000001 \epoc32\winscw\c\resource\swicertstore\dat
copy data\00000000 \epoc32\winscw\c\resource\swicertstore\dat



REM del /Q \epoc32\winscw\c\resource\swicertstore\dat\*.*
 



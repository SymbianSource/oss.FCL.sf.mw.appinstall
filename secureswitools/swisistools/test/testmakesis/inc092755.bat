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
@echo off

SET CERTS=%SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis\Root5CA
SET BASE=INC092755
SET PLATFORM=%1
SET CFG=%2

echo %BASE% TEST > %BASE%.txt
/epoc32/tools/makesis %BASE%.pkg %BASE%-tmp.sis
/epoc32/tools/signsis %BASE%-tmp.sis %BASE%.sis %CERTS%\ca.pem %CERTS%\ca.key.pem
/epoc32/tools/makesis %BASE%-2.pkg %BASE%-2-tmp.sis
/epoc32/tools/signsis %BASE%-2-tmp.sis %BASE%-2.sis %CERTS%\ca.pem %CERTS%\ca.key.pem

copy %BASE%.sis   \epoc32\release\%PLATFORM%\%CFG%\z\tswi
copy %BASE%-2.sis \epoc32\release\%PLATFORM%\%CFG%\z\tswi

rem We only copy SIS files to the emulator winscw C drive, because this is where we read files
rem from when building ROM images. The \epoc32\armv5\c etc areas are NOT used.
rem It does not matter which platform SIS file we use because it only contains txt files...
copy %BASE%.sis   \epoc32\winscw\c\tswi
copy %BASE%-2.sis \epoc32\winscw\c\tswi

del %BASE%.sis
del %BASE%-tmp.sis
del %BASE%.txt
del %BASE%-2.sis
del %BASE%-2-tmp.sis


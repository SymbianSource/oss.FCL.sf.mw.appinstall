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

@ECHO OFF
if not exist \epoc32\winscw\c\sys\install\integrityservices md \epoc32\winscw\c\sys\install\integrityservices
if not exist \epoc32\drive_e\sys\install\integrityservices md \epoc32\drive_e\sys\install\integrityservices
if not exist \epoc32\winscw\c\sys\install\integrityservices\3 md \epoc32\winscw\c\sys\install\integrityservices\3
if not exist \epoc32\winscw\c\sys\install\sisregistry\80000001 md \epoc32\winscw\c\sys\install\sisregistry\80000001
if not exist \epoc32\winscw\c\sys\install\sisregistry\80000002 md \epoc32\winscw\c\sys\install\sisregistry\80000002
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\66606660666.drv \epoc32\winscw\c\sys\install\integrityservices\66606660666.drv
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\0.drv \epoc32\winscw\c\sys\install\integrityservices\0.drv
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\0.log \epoc32\winscw\c\sys\install\integrityservices\0.log
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\1.drv \epoc32\winscw\c\sys\install\integrityservices\1.drv
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\1.log \epoc32\winscw\c\sys\install\integrityservices\1.log
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\2.drv \epoc32\winscw\c\sys\install\integrityservices\2.drv
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\2.log \epoc32\winscw\c\sys\install\integrityservices\2.log
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\3.drv \epoc32\winscw\c\sys\install\integrityservices\3.drv
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\3.log \epoc32\winscw\c\sys\install\integrityservices\3.log
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\removable_media\2.log \epoc32\drive_e\sys\install\integrityservices\2.log
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\80000001\00000000.reg \epoc32\winscw\c\sys\install\sisregistry\80000001\00000000.reg
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\80000001\00000000_0000.ctl \epoc32\winscw\c\sys\install\sisregistry\80000001\00000000_0000.ctl
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\80000002\00000000.reg \epoc32\winscw\c\sys\install\sisregistry\80000002\00000000.reg
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\80000002\00000000_0000.ctl \epoc32\winscw\c\sys\install\sisregistry\80000002\00000000_0000.ctl
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\file1.txt \epoc32\winscw\c\file1.txt
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\file1.txt \epoc32\drive_e\file2.txt
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\3\0.bak \epoc32\winscw\c\sys\install\integrityservices\3\0.bak
copy /y \epoc32\winscw\c\tswi\tintegrityservices\data\startup\3\1.bak \epoc32\winscw\c\sys\install\integrityservices\3\1.bak

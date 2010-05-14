@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:   Utility script to create test sisx files
@rem


call bldmake bldfiles
call abld build armv5 urel
call abld build winscw udeb
makesis sistest.pkg
makesis sistest_winscw.pkg
signsis sistest.sis sistest.sisx selfsign.cer selfsign.key
signsis sistest_winscw.sis sistest_winscw.sisx selfsign.cer selfsign.key
del /F/Q ..\..\data\mmc\sistest.sisx
move sistest.sisx ..\..\data\mmc\.
del /F/Q ..\..\data\mmc\sistest_winscw.sisx
move sistest_winscw.sisx ..\..\data\mmc\.
del sistest.sis
del sistest_winscw.sis
call abld reallyclean
call bldmake clean

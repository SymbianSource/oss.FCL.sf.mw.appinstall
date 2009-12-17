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
@rem Description:   Utility script to create test widget file
@rem


cd ..
zip widgettest.zip widgettest\Info.plist widgettest\widgettest.html widgettest\widgettest.css widgettest\widgettest.js
ren widgettest.zip widgettest.wgz
del /F/Q ..\data\mmc\widgettest.wgz
move widgettest.wgz ..\data\mmc\.
cd widgettest

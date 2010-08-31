#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Test installer that uses Usif::RSoftwareInstall API.
#
#


TEMPLATE = app
TARGET = testinstaller
DEPENDPATH += .
INCLUDEPATH += .

HEADERS +=  testinstaller.h \
		activerunner.h
SOURCES +=  testinstaller.cpp \
		activerunner.cpp \
        main.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID2 = 0x100039ce
    TARGET.UID3 = 0x0fe91d97      // random UID
    TARGET.VID = VID_DEFAULT

    INCLUDEPATH += /epoc32/include/mw \ 
        /epoc32/include/platform/mw \
        /epoc32/include/mw/hb/hbcore \
        /epoc32/include/mw/hb/hbwidgets
    LIBS += -lHbCore -lHbWidgets -lxqservice -lsif -lscrclient -lswinstcli -lefsrv 

	HEADERS += activerunner_symbian.h
	SOURCES += activerunner_symbian.cpp
	
    rssrules = \
        "newfile = KAppDoesNotSupportNewFile;"
    RSS_RULES += rssrules
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "testinstaller.iby CORE_APP_LAYER_IBY_EXPORT_PATH(testinstaller.iby)"


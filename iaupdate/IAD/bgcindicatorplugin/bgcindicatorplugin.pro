#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:
#

TEMPLATE = lib
TARGET = bgcindicatorplugin
CONFIG += plugin
CONFIG += hb

INCLUDEPATH += .
DEPENDPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../../inc


HEADERS += inc/bgcindicatorplugin.h \
					 inc/bgcindicator.h
SOURCES += src/bgcindicatorplugin.cpp \
		   		 src/bgcindicator.cpp
		   

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002E696

    pluginstub.sources = bgcindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/bgcindicatorplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(bgcindicatorplugin.iby)" 
  
LIBS += -lxqservice
LIBS += -lws32
LIBS += -lapparc
LIBS += -lapgrfx


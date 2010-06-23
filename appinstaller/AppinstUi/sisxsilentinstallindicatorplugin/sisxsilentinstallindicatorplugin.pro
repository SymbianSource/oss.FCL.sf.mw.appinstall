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
# Description:
#

TEMPLATE = lib
TARGET = sisxsilentinstallindicatorplugin
CONFIG += plugin
CONFIG += hb

INCLUDEPATH += .
DEPENDPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../../inc


HEADERS += inc/sisxsilentinstallindicatorplugin.h \ 
				inc/sisxsilentinstallindicator.h

SOURCES += src/sisxsilentinstallindicatorplugin.cpp \ 
				src/sisxsilentinstallindicator.cpp
		   

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002FF6A 

    pluginstub.sources = sisxsilentinstallindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/sisxsilentinstallindicatorplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(sisxsilentinstallindicatorplugin.iby)" 
  
LIBS += -lxqservice
LIBS += -lws32
LIBS += -lapparc  
LIBS += -lapgrfx

# TODO: onko tarpeen apparc lippi ? lapparc 


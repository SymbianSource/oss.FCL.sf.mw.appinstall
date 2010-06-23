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
# Description:  Software install progress indicator
#

TEMPLATE = lib
TARGET = sifuiinstallindicatorplugin
CONFIG += plugin
CONFIG += hb

INCLUDEPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
DEPENDPATH += .

LIBS += -lQtPublishSubscribe

HEADERS += inc/sifuiinstallindicatorplugin.h \
    inc/sifuiinstallindicator.h \
    inc/sifuiinstallindicatorparams.h

SOURCES += src/sifuiinstallindicatorplugin.cpp \
    src/sifuiinstallindicator.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002E690

    pluginstub.sources = sifuiinstallindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub

	crmlFiles.sources = sifuiinstallindicatorplugin.qcrml
	crmlFiles.path = /resource/qt/crml
	DEPLOYMENT += crmlFiles
}

BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/sifuiinstallindicatorplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(sifuiinstallindicatorplugin.iby)"


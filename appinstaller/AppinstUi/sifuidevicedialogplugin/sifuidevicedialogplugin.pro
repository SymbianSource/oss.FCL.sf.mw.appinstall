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
# Description: Software Install Framework (SIF) device dialogs plugin.
#

TEMPLATE = lib
TARGET = sifuidevicedialogplugin
CONFIG += plugin
CONFIG += hb
#TRANSLATIONS = sifuidevicedialogplugin.ts

INCLUDEPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../sifuiinstallindicatorplugin/inc
DEPENDPATH += .

LIBS += -lQtPublishSubscribe

HEADERS += inc/sifuidevicedialogplugin.h \
    inc/sifuidialogdefinitions.h \
    inc/sifuidialog.h \
    inc/sifuidialogtitlewidget.h \
    inc/sifuidialogcontentwidget.h

SOURCES += src/sifuidevicedialogplugin.cpp \
    src/sifuidialog.cpp \
    src/sifuidialogtitlewidget.cpp \
    src/sifuidialogcontentwidget.cpp

RESOURCES += sifuidevicedialogplugin.qrc

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002C3AC

    pluginstub.sources = sifuidevicedialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub

	LIBS += -lfbscli
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "qmakepluginstubs/sifuidevicedialogplugin.qtplugin /epoc32/data/z/pluginstub/sifuidevicedialogplugin.qtplugin" \
  "inc/sifuidialogdefinitions.h MW_LAYER_PLATFORM_EXPORT_PATH(sifuidialogdefinitions.h)" \
  "rom/sifuidevicedialogplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(sifuidevicedialogplugin.iby)" \
  "rom/sifuidevicedialogplugin_resources.iby LANGUAGE_MW_LAYER_IBY_EXPORT_PATH(sifuidevicedialogplugin_resources.iby)"

LIBS += -lxqservice

tests {
    test.depends = sub-src
    test.commands += cd tsrc && $(MAKE) test
    autotest.depends = sub-src
    autotest.commands += cd tsrc && $(MAKE) autotest
    QMAKE_EXTRA_TARGETS += test autotest
}

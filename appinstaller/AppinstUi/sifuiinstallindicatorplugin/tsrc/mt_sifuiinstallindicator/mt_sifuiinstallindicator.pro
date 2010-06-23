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
# Description:  Module tests for SW install progress indicator.
#

TEMPLATE = app
TARGET =
CONFIG += qtestlib hb
DEPENDPATH += .
INCLUDEPATH += . ../../inc
LIBS += -lQtPublishSubscribe

HEADERS += mt_sifuiinstallindicator.h
SOURCES += mt_sifuiinstallindicator.cpp

symbian {
    TARGET.CAPABILITY = CAP_APPLICATION
}


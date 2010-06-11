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
# Description: Unit test project for SIF UI device dialog.
#

CONFIG += qtestlib hb
TEMPLATE = app
TARGET = ut_sifuidevicedialog
DEPENDPATH += .
INCLUDEPATH += . ../../inc

HEADERS += ut_sifuidevicedialog.h
SOURCES += ut_sifuidevicedialog.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_APPLICATION
    TARGET.UID3 = 0xE7DB7B81
}


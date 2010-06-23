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
# Description:  Test startup list update launcher application
#
#


TEMPLATE = app
TARGET = testslulauncher
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += Hb

HEADERS +=  testslulauncher.h
SOURCES +=  testslulauncher.cpp main.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID2 = 0x100039ce
    TARGET.UID3 = 0xed835a87      // random UID
    TARGET.VID = VID_DEFAULT
}



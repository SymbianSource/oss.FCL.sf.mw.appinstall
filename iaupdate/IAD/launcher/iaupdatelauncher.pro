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


TEMPLATE = app
TARGET = iaupdatelauncher 	

symbian {
    TARGET.UID3 = 0x2001FE2F
    TARGET.CAPABILITY = CAP_APPLICATION
    SKINICON = qtg_large_swupdate 
    TRANSLATIONS = swupdate.ts
}    
DEPENDPATH += .
DEPENDPATH += ./src
DEPENDPATH += ./inc

INCLUDEPATH += .
INCLUDEPATH += ../engine/inc

CONFIG += hb

# Input
SOURCES += iaupdatelaunchermain.cpp \
           iaupdatelauncherclient.cpp 

HEADERS = iaupdatelauncherclient.h 

LIBS += -lxqservice
LIBS += -lxqserviceutil
LIBS += -lflogger
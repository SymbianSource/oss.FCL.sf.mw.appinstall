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
TARGET = iaupdateapi 	

symbian {
    TARGET.UID3 = 0x2000F85C
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA=1
    MMP_RULES += "OPTION ARMCC --export_all_vtbl"

    defBlock = \      
    "$${LITERAL_HASH}if defined(EABI)" \
    "DEFFILE  ../eabi/iaupdateapi.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE  ../bwins/iaupdateapi.def" \
    "$${LITERAL_HASH}endif"
     MMP_RULES += defBlock
} 

DEPENDPATH += .
DEPENDPATH += ./src
DEPENDPATH += ./client/src
DEPENDPATH += ./inc
DEPENDPATH += ./client/inc
DEPENDPATH += ../ui/inc
DEPENDPATH += ../engine/inc

INCLUDEPATH += .
INCLUDEPATH += ../ui/inc
INCLUDEPATH += ../engine/inc


CONFIG += hb

# Input
SOURCES += iaupdate.cpp \
           iaupdateparameters.cpp \
           iaupdateresult.cpp \
           iaupdateclient.cpp \
           iaupdatemanager.cpp

HEADERS = iaupdateclient.h \
          iaupdatemanager.h 

LIBS += -lcone
LIBS += -lws32
LIBS += -lxqservice
LIBS += -lxqserviceutil
LIBS += -lflogger

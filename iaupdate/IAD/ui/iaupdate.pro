#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
TARGET = iaupdate


symbian {
    TARGET.UID3 = 0x2000F85A
    TARGET.CAPABILITY = CAP_APPLICATION TrustedUI PowerMgmt
    rssrules = "hidden = KAppIsHidden;"
    RSS_RULES += rssrules
    TRANSLATIONS = swupdate.ts
}

DEPENDPATH += .
DEPENDPATH += ./src
DEPENDPATH += ./inc

INCLUDEPATH += .
INCLUDEPATH += ../engine/inc
INCLUDEPATH += ../api/client/inc
INCLUDEPATH += ../updater/inc
INCLUDEPATH += ../updater/updaterfiles/inc
INCLUDEPATH += ../firmwareupdate/inc
INCLUDEPATH += ../launcher/inc

CONFIG += hb

# Input
SOURCES += iaupdatemain.cpp \
    iaupdateapplication.cpp \
    iaupdatemainwindow.cpp \
    iaupdatemainview.cpp \
    iaupdateengine.cpp \
    iaupdateagreement.cpp \
    iaupdateautomaticcheck.cpp \
    iaupdatedeputils.cpp \ 
    iaupdatefirsttimeinfo.cpp \
    iaupdategloballockhandler.cpp \
    iaupdateinstallationlistener.cpp \
    iaupdatenodefilter.cpp \
    iaupdateparametersfilemanager.cpp \
    iaupdatequeryhistory.cpp \
    iaupdaterefreshhandler.cpp \
    iaupdateroaminghandler.cpp \ 
    iaupdatestarter.cpp \
    iaupdateuiconfigdata.cpp \
    iaupdateuicontroller.cpp \
    iaupdateuitimer.cpp \
    iaupdateserviceprovider.cpp \
    iaupdatewaitdialog.cpp \
    iaupdatenodeid.cpp \
    iaupdatetools.cpp \
    iaupdatesettingdialog.cpp \
    iaupdateresultsdialog.cpp \
    iaupdatedialogutil.cpp

    

HEADERS = iaupdateapplication.h \
    iaupdatemainwindow.h \
    iaupdatemainview.h \
    iaupdateengine.h \
    iaupdatewaitdialog.h \
    iaupdatewaitdialogobserver.h \
    iaupdateagreement.h \
    iaupdateautomaticcheck.h \
    iaupdatedeputils.h \
    iaupdatefileconsts.h \
    iaupdatefirsttimeinfo.h \
    iaupdategloballockhandler.h \
    iaupdateinstallationlistener.h \
    iaupdatenodefilter.h \
    iaupdateparametersfilemanager.h \
    iaupdateprivatecrkeys.h \
    iaupdatequeryhistory.h \
    iaupdaterefreshhandler.h \
    iaupdaterefreshobserver.h \
    iaupdaterequestobserver.h \
    iaupdateresultsinfo.h \
    iaupdateroaminghandler.h \
    iaupdateroaminghandlerobserver.h \
    iaupdatestarter.h \
    iaupdatestarterobserver.h \
    iaupdateuiconfigconsts.h \
    iaupdateuiconfigdata.h \
    iaupdateuicontroller.h \
    iaupdateuicontrollerobserver.h \
    iaupdateuidefines.h \
    iaupdateuids.h \
    iaupdateuitimer.h \
    iaupdateserviceprovider.h \
    iaupdatenodeid.h \
    iaupdatetools.h \
    iaupdatesettingdialog.h \
    iaupdateresultsdialog.h \ 
    iaupdatedialogutil.h


LIBS += -lbafl
LIBS += -liaupdateengine
LIBS += -liaupdateapi
LIBS += -liaupdatefwupdate
LIBS += -liaupdaterfiles
LIBS += -lflogger
LIBS += -lxqservice
LIBS += -lxqserviceutil
LIBS += -lxqutils
LIBS += -lcentralrepository
LIBS += -lcmmanager
LIBS += -lconnmon
LIBS += -lefsrv
LIBS += -lestor
LIBS += -lsysutil
LIBS += -lfeatmgr
LIBS += -lcone
LIBS += -lws32
LIBS += -lapparc
LIBS += -lapgrfx
LIBS += -lstarterclient
LIBS += -lcmapplsettingsui



CONFIG += service

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable


RESOURCES = iaupdate.qrc

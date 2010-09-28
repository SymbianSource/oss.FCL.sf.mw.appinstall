
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

include ( engine.pri )

IncludeBlock = \
	"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
	"$${LITERAL_HASH}include <platform_paths.hrh>"  \
	"$${LITERAL_HASH}include <../inc/catalogsuids.h>" \
	"$${LITERAL_HASH}include <./inc/build_config.mmpi>" 


TEMPLATE = app
TARGET = ncdserver_20019119
CONFIG += qt
 
symbian: { 
	TARGET.CAPABILITY = CAP_APPLICATION 
	TARGET.VID = VID_DEFAULT
	TARGET.UID3 = 0x20019119
	TARGET.EPOCALLOWDLLDATA = 1
	rssrules = "hidden = KAppIsHidden;"
        RSS_RULES += rssrules
	
	INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

	INCLUDEPATH +=   ../provider/inc 
	INCLUDEPATH +=   ../provider/server/inc 
	INCLUDEPATH +=   ../provider/client/inc
	INCLUDEPATH +=   ../provider/storage/inc
	INCLUDEPATH +=   ../provider/protocol/inc
	INCLUDEPATH +=   ../provider/deviceinteraction/inc
	INCLUDEPATH +=   ../provider/purchasehistory/inc
	
	INCLUDEPATH +=   ../inc
	INCLUDEPATH +=   ../engine/inc
	INCLUDEPATH +=   ../engine/transport/inc
	INCLUDEPATH +=   ../engine/accesspointmanager/inc
	INCLUDEPATH += /epoc32/include/platform/mw/cwrt
	INCLUDEPATH += /epoc32/include/ecom

	LIBS += -lWrtDownloadMgr -lapmime -lcmmanager -lecom -lmsgs -lsmcm -lgsmu -linetprotutil 
	LIBS += -lhttpfiltercommon -lecom -lhttp -lesock -lcommdb -lsysutil -lhash -lestlib -lestor -lbafl -lplatformenv
	LIBS += -lXmlEngineUtils  -lXmlEngineDOM -lXmlEngine -lXmlInterface -lxmlframework  -lcharconv -ledbms -lefsrv
  LIBS += -lncdutils_20019119 -lncddevicemgmt_20019119
  LIBS += -lxqutils
}

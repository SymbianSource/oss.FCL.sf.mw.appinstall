/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This module contains the implementation of IAUpdateLauncherClient
*                class member functions.
*
*/



//INCLUDES
#include <qapplication.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>
#include <xqrequestinfo.h>

#include "iaupdatelauncherclient.h"
#include "iaupdatedebug.h"

 

IAUpdateLauncherClient::IAUpdateLauncherClient()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::IAUpdateLauncherClient()");
    mServiceRequest = NULL;
}

IAUpdateLauncherClient::~IAUpdateLauncherClient()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::~IAUpdateLauncherClient() begin");
    if ( mServiceRequest)
    {
       delete mServiceRequest;
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::~IAUpdateLauncherClient() end");
}

void IAUpdateLauncherClient::launch()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::launch() begin");
    mServiceRequest = new XQServiceRequest("com.nokia.services.swupdate.swupdate_interface","startedByLauncher(QString)",false);
    
    XQRequestInfo requestInfo;
    requestInfo.setEmbedded(true);
    mServiceRequest->setInfo(requestInfo);
    QString stringRefreshFromNetworkDenied("0");
    *mServiceRequest << stringRefreshFromNetworkDenied;   
    bool ret = mServiceRequest->send();   
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateLauncherClient::launch() ret %d", ret );
    if (!ret)
    {
        IAUPDATE_TRACE("[IAUPDATE] send failed");
        qApp->quit();
    }
    else
    {
        connect(mServiceRequest, SIGNAL(requestCompleted(QVariant)), this, SLOT(requestCompleted(QVariant)));
        connect(mServiceRequest, SIGNAL(requestError(int)), this, SLOT(requestError(int)));
    }    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::launch() end");
}

void IAUpdateLauncherClient::requestCompleted(const QVariant& /*value*/)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateLauncherClient::requestCompleted()");
    qApp->quit();
}

void IAUpdateLauncherClient::requestError(int err)
{
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateLauncherClient::requestError() %d", err );
    qApp->quit();
}


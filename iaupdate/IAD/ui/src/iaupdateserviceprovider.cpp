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
* Description:   This module contains the implementation of IAUpdateServiceProvider class 
*                member functions.
*
*/

#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include "iaupdateserviceprovider.h"
#include "iaupdateengine.h"
#include "iaupdatedebug.h"

const TSecureId KSIDBackgroundChecker = 0x200211f4;
const TSecureId KSIDCwrtWidget = 0x200267C0;


IAUpdateServiceProvider::IAUpdateServiceProvider(IAUpdateEngine& engine)
: XQServiceProvider( QString("com.nokia.services.swupdate.swupdate_interface")),
  mEngine(&engine),
  mCurrentRequest(NoOperation)
    
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::IAUpdateServiceProvider() begin");
    publishAll();
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::IAUpdateServiceProvider() end");
}

IAUpdateServiceProvider::~IAUpdateServiceProvider()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::~IAUpdateServiceProvider()");
}

void IAUpdateServiceProvider::startedByLauncher(QString refreshFromNetworkDenied)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::startedByLauncher() begin");
    mAsyncReqId = setCurrentRequestAsync();
    mEngine->StartedByLauncherL(refreshFromNetworkDenied.toInt());
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::startedByLauncher() end");
}

void IAUpdateServiceProvider::checkUpdates(QString stringWgId, 
                                           QString stringUid,  
                                           QString searchCriteria, 
                                           QString commandLineExecutable,
                                           QString commandLineArguments,
                                           QString stringShowProgress,
                                           QString stringImportance,
                                           QString stringType,
                                           QString stringRefresh)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::checkUpdates() begin");
    mAsyncReqId = setCurrentRequestAsync();
    mCurrentRequest = CheckUpdates;
    CIAUpdateParameters* params = NULL;
    TRAP_IGNORE(params = CIAUpdateParameters::NewL());
    if (params)
    {
        SetParams(*params, 
                  stringUid,
                  searchCriteria,
                  commandLineExecutable,
                  commandLineArguments,
                  stringShowProgress,
                  stringImportance,
                  stringType,
                  stringRefresh);
  
        if ((requestInfo().clientSecureId() != KSIDBackgroundChecker) && (requestInfo().clientSecureId() != KSIDCwrtWidget))      
        {
            // other processes than backroundchecker are not allowed to cause refresh from network 
            params->SetRefresh( EFalse );
        }
    mEngine->CheckUpdatesRequestL(stringWgId.toInt(),params, params->Refresh() && requestInfo().clientSecureId() == KSIDCwrtWidget);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::checkUpdates() end");
    }
    
}

void IAUpdateServiceProvider::showUpdates(QString stringWgId, 
                                          QString stringUid,  
                                          QString searchCriteria, 
                                          QString commandLineExecutable,
                                          QString commandLineArguments,
                                          QString stringShowProgress,
                                          QString stringImportance,
                                          QString stringType,
                                          QString stringRefresh)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::showUpdates() begin");
    mAsyncReqId = setCurrentRequestAsync();
    mCurrentRequest = ShowUpdates;
    CIAUpdateParameters* params = NULL;
    TRAP_IGNORE(params = CIAUpdateParameters::NewL());
    if (params)
    {
        SetParams(*params, 
                  stringUid,
                  searchCriteria,
                  commandLineExecutable,
                  commandLineArguments,
                  stringShowProgress,
                  stringImportance,
                  stringType,
                  stringRefresh);
    } 
    mEngine->ShowUpdatesRequestL(stringWgId.toInt(),params);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::showUpdates() end");
}



void IAUpdateServiceProvider::updateQuery(QString stringWgId)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::updateQuery() begin");
    mAsyncReqId = setCurrentRequestAsync();
    mCurrentRequest = UpdateQuery;
    mEngine->ShowUpdateQueryRequestL( stringWgId.toInt(), requestInfo().clientSecureId() );        
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::updateQuery() end");
}



void IAUpdateServiceProvider::completeLauncherLaunch(int error)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeLauncherLaunch() begin");
    completeRequest(mAsyncReqId, QVariant(error));    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeLauncherLaunch() end");
}

void IAUpdateServiceProvider::completeCheckUpdates(int countOfAvailableUpdates, int error)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeCheckUpdates() begin");
    QList<QVariant> resultList;
    resultList.append(QVariant(error));
    resultList.append(QVariant(countOfAvailableUpdates));
    completeRequest(mAsyncReqId, QVariant(resultList));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeCheckUpdates() end");
}

void IAUpdateServiceProvider::completeShowUpdates(const CIAUpdateResult* updateResult, int error)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeShowUpdates() begin");
    QList<QVariant> resultList;
    resultList.append(QVariant(error));
    if (updateResult)
    {    
        resultList.append(QVariant(updateResult->SuccessCount()));
        resultList.append(QVariant(updateResult->FailCount()));
        resultList.append(QVariant(updateResult->CancelCount()));
        delete updateResult;
    }    
    else
    {
        resultList.append(QVariant(0));
        resultList.append(QVariant(0));
        resultList.append(QVariant(0));
    }
    completeRequest(mAsyncReqId, QVariant(resultList));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeShowUpdates() end");
}

void IAUpdateServiceProvider::completeUpdateQuery(bool updateNow, int error)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeUpdateQuery() begin");
    QList<QVariant> resultList;
    resultList.append(QVariant(error));
    resultList.append(QVariant(updateNow));
    completeRequest(mAsyncReqId, QVariant(resultList));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::completeUpdateQuery() end");
}


void IAUpdateServiceProvider::SetParams(CIAUpdateParameters& params,
                                        QString& stringUid,  
                                        QString& searchCriteria, 
                                        QString& commandLineExecutable,
                                        QString& commandLineArguments,
                                        QString& stringShowProgress,
                                        QString& stringImportance,
                                        QString& stringType,
                                        QString& stringRefresh) const
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::SetParams() begin");
    params.SetUid(TUid::Uid(stringUid.toInt()));
    TPtrC ptr(reinterpret_cast<const TText*>(searchCriteria.constData()));
    params.SetSearchCriteriaL(ptr);
    ptr.Set(reinterpret_cast<const TText*>(commandLineExecutable.constData()));
    params.SetCommandLineExecutableL(ptr);
    ptr.Set(reinterpret_cast<const TText*>(commandLineArguments.constData()));
    HBufC8* arguments8 = HBufC8::NewL(ptr.Length());
    TPtr8 ptr8(arguments8->Des());
    ptr8.Copy(ptr);
    params.SetCommandLineArgumentsL(ptr8);
    params.SetShowProgress(stringShowProgress.toInt());
    params.SetImportance(stringImportance.toUInt());
    params.SetType(stringType.toUInt());
    params.SetRefresh(stringRefresh.toInt());
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateServiceProvider::SetParams() end");
}



/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of IAUpdateClient
*                class member functions.
*
*/



//INCLUDES
#include <eikenv.h>
#include <apgcli.h>
#include <e32math.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>
#include <xqrequestinfo.h>

#include "iaupdateclient.h"
#include "iaupdateclientdefines.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// IAUpdateClient::IAUpdateClient
// 
// -----------------------------------------------------------------------------
// 
IAUpdateClient::IAUpdateClient(MIAUpdateObserver& observer):
    mObserver(observer)
    {
    mServiceRequest = NULL;
    mCurrentRequestType = NoOperation;
    }

// -----------------------------------------------------------------------------
// IAUpdateClient::~IAUpdateClient
// 
// -----------------------------------------------------------------------------
// 
IAUpdateClient::~IAUpdateClient() 
    {
    if ( mServiceRequest)
        {
        delete mServiceRequest;
        }
    }


// -----------------------------------------------------------------------------
// IAUpdateClient::initRequest
// 
// -----------------------------------------------------------------------------
// 
int IAUpdateClient::initRequest(const CIAUpdateParameters* updateParameters, const QString& message, bool toBackground)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::initRequest() begin");
    int error(KErrNone);
    if (mCurrentRequestType != NoOperation) 
        {
        error = KErrServerBusy;
        }
    else if (!mServiceRequest)
    {
        mServiceRequest = new XQServiceRequest("com.nokia.services.swupdate.swupdate_interface", message, false);
        if (mServiceRequest)
        {    
            connect(mServiceRequest, SIGNAL(requestCompleted(QVariant)), this, SLOT(requestCompleted(QVariant)));
            connect(mServiceRequest, SIGNAL(requestError(int)), this, SLOT(requestError(int)));
        }
        else
        {
            error = KErrNoMemory;
        }
    }
    else
    {
        mServiceRequest->setMessage(message);
    }
    
    
    if (error == KErrNone)
    {    
        XQRequestInfo requestInfo;
        requestInfo.setBackground(toBackground);
        mServiceRequest->setInfo(requestInfo); 
        int wgId = 0;
        CEikonEnv* eikEnv = CEikonEnv::Static();
        if ( eikEnv )
        {
            RWindowGroup owngroup;
            wgId = eikEnv->RootWin().Identifier();
        }
        IAUPDATE_TRACE_1("IAUpdateClient::initRequest() wgId: %d", wgId);
        QString stringWgid;
        stringWgid.setNum(wgId);
        *mServiceRequest << stringWgid;  
        if (updateParameters)
        {    
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() UID: %d", updateParameters->Uid().iUid);
            QString stringUid; 
            stringUid.setNum(updateParameters->Uid().iUid);
            *mServiceRequest << stringUid;
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() searchcriteria: %S", &updateParameters->SearchCriteria());
            *mServiceRequest << qStringFromTDesC(updateParameters->SearchCriteria());
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() executable: %S", &updateParameters->CommandLineExecutable());
            *mServiceRequest << qStringFromTDesC(updateParameters->CommandLineExecutable());
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() arguments: %S8", &updateParameters->CommandLineArguments());
            *mServiceRequest << qStringFromTDesC8(updateParameters->CommandLineArguments());
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() show progress: %d", updateParameters->ShowProgress());
            QString stringShowProgress;
            stringShowProgress.setNum(updateParameters->ShowProgress());
            *mServiceRequest << stringShowProgress;
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() importance: %d", updateParameters->Importance());
            QString stringImportance;
            stringImportance.setNum(updateParameters->Importance());
            *mServiceRequest << stringImportance;
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() type: %d", updateParameters->Type());
            QString stringType;
            stringType.setNum(updateParameters->Type());
            *mServiceRequest << stringType;
                
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() refresh: %d", updateParameters->Refresh());
            QString stringRefresh;
            stringRefresh.setNum(updateParameters->Refresh());
            *mServiceRequest << stringRefresh;
        }
    }                 
     
    
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::initRequest() error code: %d", error );
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::initRequest() end");
    return error;
}

// -----------------------------------------------------------------------------
// IAUpdateClient::checkUpdates
// 
// -----------------------------------------------------------------------------
//
void IAUpdateClient::checkUpdates(const CIAUpdateParameters& updateParameters)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::checkUpdates()");
    QString message("checkUpdates(QString,QString,QString,QString,QString,QString,QString,QString,QString)");
    //QString message("checkUpdates(int,int)");
    int ret = initRequest(&updateParameters,message,!updateParameters.ShowProgress());
    if (ret == KErrNone)
        {
        if (mServiceRequest->send()) 
            {
            mCurrentRequestType = CheckUpdates;
            }
        else
            {
            mObserver.CheckUpdatesComplete(ret,0);        
            }
        }
    }

// -----------------------------------------------------------------------------
// IAUpdateClient::showUpdates
// 
// -----------------------------------------------------------------------------
//
void IAUpdateClient::showUpdates(const CIAUpdateParameters& updateParameters)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::showUpdates()");
    QString message("showUpdates(QString,QString,QString,QString,QString,QString,QString,QString,QString)");
    int ret = initRequest(&updateParameters, message, false);
    if (ret == KErrNone)
        {
        if (mServiceRequest->send())
            {
            mCurrentRequestType = ShowUpdates;
            }
        else
            {
            mObserver.UpdateComplete(ret,NULL);
            }
        }
    }
    

// -----------------------------------------------------------------------------
// IAUpdateClient::updateQuery
// 
// -----------------------------------------------------------------------------
//    
void IAUpdateClient::updateQuery()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::updateQuery() begin");
    QString message("updateQuery(QString)");
    CIAUpdateParameters* nullParameters = NULL;
    int ret = initRequest(nullParameters, message, false);
    if (ret == KErrNone)
    {
        if (mServiceRequest->send()) 
        {
            mCurrentRequestType = UpdateQuery;
        }
        else
        {
            mObserver.UpdateQueryComplete(ret,false);
        }
    }
	IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::updateQuery() end");    
}

// -----------------------------------------------------------------------------
// IAUpdateClient::update
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateClient::update()
{
    mObserver.UpdateComplete(KErrNotSupported,NULL);
}

// -----------------------------------------------------------------------------
// IAUpdateClient::broughtToForeground
// 
// -----------------------------------------------------------------------------
//
void IAUpdateClient::broughtToForeground()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::broughtToForeground() begin");
    if (mServiceRequest)
    {
        QString message("broughtToForeground(int)");
        CIAUpdateParameters* nullParameters = NULL;
        int ret = initRequest(nullParameters, message, false);
        if (ret == KErrNone)
        {
            if (mServiceRequest->send())
            {
                mCurrentRequestType = BroughtToForeground;
            }
        }
    } 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::broughtToForeground() end");
}

// -----------------------------------------------------------------------------
// IAUpdateClient::requestCompleted
// 
// -----------------------------------------------------------------------------
//
void IAUpdateClient::requestCompleted(const QVariant& value)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::requestCompleted() begin");
    RequestType requestType = mCurrentRequestType;
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::requestCompleted()request type: %d", requestType );
    QList<QVariant> resultlist = value.toList();
    int errorCode = resultlist.at(0).toInt();
    CIAUpdateResult* updateResult(NULL);
            
    if ( requestType == ShowUpdates )
    {
        // Update result object is required.
        // Notice that the ownership is transferred later.
        // So, this function does not need to delete updateResult object.
        TRAPD( trapError, updateResult = CIAUpdateResult::NewL() );
        if ( updateResult )
        {
            updateResult->SetSuccessCount(resultlist.at(1).toInt());
            updateResult->SetFailCount(resultlist.at(2).toInt());
            updateResult->SetCancelCount(resultlist.at(3).toInt());
        }
        else
        {
            // Something went wrong when creating update result object.
            // Update the error code accordingly.
            errorCode = trapError;
        }
            // Let's assume that connection is not needed anymore
        if (mServiceRequest)
        {
            delete mServiceRequest;
            mServiceRequest= NULL;
        }
    }
        
    // Inform that no operation is going on anymore.
    // This is required for busy check.
    mCurrentRequestType = NoOperation;
        
    // Use the request type of the ongoing operation to check what callback
    // function to call.
    int countOfUpdates = 0;
    bool updateNow = false;
    switch (requestType)
    {
        case CheckUpdates:
        countOfUpdates = resultlist.at(1).toInt();    
        if (countOfUpdates == 0)
        {
            // Let's assume that connection is not needed anymore
            delete mServiceRequest;  
            mServiceRequest= NULL;
        }
        IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::requestCompleted() count of updates: %d", countOfUpdates );
        mObserver.CheckUpdatesComplete(errorCode, countOfUpdates);
        break;

        case ShowUpdates:
        // Notice that ownership of result object is transferred here.
        IAUPDATE_TRACE_3("[IAUPDATE] IAUpdateClient::requestCompleted() success count: %d failed count: %d cancelled count: %d", updateResult->SuccessCount(), updateResult->FailCount(), updateResult->CancelCount() );
        mObserver.UpdateComplete(errorCode, updateResult);
        break;
                
        case UpdateQuery:
        updateNow = resultlist.at(1).toBool();      
        if ( !updateNow )
        {
            // Let's assume that connection is not needed anymore
            delete mServiceRequest;
            mServiceRequest= NULL;
        }    
        IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateClient::requestCompleted() update now: %d", updateNow );
        mObserver.UpdateQueryComplete(errorCode, updateNow);
        break;
             
        default:
        // Should not ever come here.
        break;
    }
            
    // Do not anything else than return after callback function is called because 
    // this instance can be deleted by a client in a callback function
    // 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::requestCompleted() end");
}

// -----------------------------------------------------------------------------
// IAUpdateClient::requestError
// 
// -----------------------------------------------------------------------------
//
void IAUpdateClient::requestError(int /*err*/)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::requestError() begin");
    if ( mServiceRequest)
        {
            delete mServiceRequest;
            mServiceRequest= NULL;
        }
    RequestType requestType = mCurrentRequestType;
    mCurrentRequestType = NoOperation;
    // because this method is called also when iaupdate is closed normally, error code is not passed to a client   
    CIAUpdateResult* updateResult(NULL);
    switch (requestType)
    {
        case CheckUpdates:
        mObserver.CheckUpdatesComplete(0, 0);
        break;

        case ShowUpdates:
        // Notice that ownership of result object is transferred here.
        TRAP_IGNORE( updateResult = CIAUpdateResult::NewL() );    
        mObserver.UpdateComplete(0, updateResult);
        break;
                    
        case UpdateQuery:
        mObserver.UpdateQueryComplete(0, false);
        break;
                 
        default:
        // Should not ever come here.
        break;
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateClient::requestError() end");
}     

// -----------------------------------------------------------------------------
// IAUpdateClient::qStringFromTDesC16
// 
// -----------------------------------------------------------------------------
//
QString IAUpdateClient::qStringFromTDesC16( const TDesC16& aDes16 )             
{
    return QString::fromUtf16( aDes16.Ptr(), aDes16.Length() );
}

// -----------------------------------------------------------------------------
// IAUpdateClient::qStringFromTDesC8
// 
// -----------------------------------------------------------------------------
//
QString IAUpdateClient::qStringFromTDesC8( const TDesC8& aDes8 )                
{
    return QString::fromUtf8( reinterpret_cast<const char*>( aDes8.Ptr() ), aDes8.Length() );
}

// -----------------------------------------------------------------------------
// IAUpdateClient::qStringFromTDesC
// 
// -----------------------------------------------------------------------------
//
QString IAUpdateClient::qStringFromTDesC( const TDesC& aDes )                    
{
#if defined(_UNICODE)
    return qStringFromTDesC16( aDes );
#else
    return qStringFromTDesC8( aDes );
#endif
}




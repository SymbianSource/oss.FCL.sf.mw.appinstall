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
* Description:   This file contains the header file of the IAUpdateServiceProvider class 
*
*/



#ifndef IAUPDATESERVICEPROVIDER_H
#define IAUPDATESERVICEPROVIDER_H

#include <QtCore/qmetaobject.h>
#include <xqserviceprovider.h>

class CIAUpdateParameters;
class CIAUpdateResult;
class IAUpdateEngine;

class IAUpdateServiceProvider : public XQServiceProvider
{
    Q_OBJECT
    
    public:
    
    enum RequestType
        {
        /**
        * No operation is going on.
        */ 
        NoOperation,       
                
        /**
        * Update check has been requested.
        */
        CheckUpdates,

        /**
        * Show Update operation has been requested.
        */
        ShowUpdates,     
                
        /**
        * Update query has been requested.
        */
        UpdateQuery,
        /**
        * Bring to foreground has been requested.
        */
        BroughtToForeground
        };

    
    IAUpdateServiceProvider(IAUpdateEngine& engine);
    ~IAUpdateServiceProvider();
    
    public slots:
    void startedByLauncher(QString refreshFromNetworkDenied);
    void checkUpdates(QString stringWgId, 
                      QString stringUid,  
                      QString searchCriteria, 
                      QString commandLineExecutable,
                      QString commandLineArguments,
                      QString stringShowProgress,
                      QString stringImportance,
                      QString stringType,
                      QString stringRefresh);
    void showUpdates(QString stringWgId, 
                     QString stringUid,  
                     QString searchCriteria, 
                     QString commandLineExecutable,
                     QString commandLineArguments,
                     QString stringShowProgress,
                     QString stringImportance,
                     QString stringType,
                     QString stringRefresh);

    void updateQuery(QString stringWgId);
    
    private:
    
    void SetParams(CIAUpdateParameters& params,
                   QString& stringUid,  
                   QString& searchCriteria, 
                   QString& commandLineExecutable,
                   QString& commandLineArguments,
                   QString& stringShowProgress,
                   QString& stringImportance,
                   QString& stringType,
                   QString& stringRefresh) const; 
    
    public:  //temp
    
    void completeLauncherLaunch(int error);
    void completeCheckUpdates(int countOfAvailableUpdates, int error);
    void completeShowUpdates(const CIAUpdateResult* updateResult, int error);
    void completeUpdateQuery(bool updateNow, int error);
    
    private:
    
    IAUpdateEngine* mEngine; //not owned
    int mAsyncReqId;
    RequestType mCurrentRequest;
};

#endif      // IAUPDATESERVICEPROVIDER_H
            
// End of File

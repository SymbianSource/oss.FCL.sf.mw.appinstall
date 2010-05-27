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
* Description:   This file contains the header file of the RIAUpdateClient class 
*
*/



#ifndef IA_UPDATE_CLIENT_H
#define IA_UPDATE_CLIENT_H


#include <e32std.h>
#include <e32cmn.h>
#include <iaupdateobserver.h>
#include <QObject>

class CIAUpdateParameters;
class MIAUpdateObserver;
class XQServiceRequest;


class IAUpdateClient : public QObject
//NONSHARABLE_CLASS( IAUpdateClient ) : public QObject
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

    IAUpdateClient(MIAUpdateObserver& aObserver);
     
     
    ~IAUpdateClient();

    int initRequest(const CIAUpdateParameters* updateParameters, const QString& message, bool toBackground);

    void checkUpdates(const CIAUpdateParameters& updateParameters);

    void showUpdates(const CIAUpdateParameters& updateParameters);

    void updateQuery();
    
    void update();
        
    void broughtToForeground();

protected slots:

    void requestCompleted(const QVariant& value);
    
    void requestError(int err);     


private:
     
    QString qStringFromTDesC16( const TDesC16& aDes16 ); 
     
    QString qStringFromTDesC8( const TDesC8& aDes8 );
     
    QString qStringFromTDesC( const TDesC& aDes );

                               
private: //data
    
    XQServiceRequest* mServiceRequest;
    
    // Informs what kind of operation is going on.
    RequestType mCurrentRequestType;
    
    // Observer who will be informed about the completion of the operations. 
    MIAUpdateObserver& mObserver; 
         
    };

#endif // IA_UPDATE_CLIENT_H

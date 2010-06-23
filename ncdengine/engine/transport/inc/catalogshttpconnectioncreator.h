/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_CATALOGSHTTPCONNECTIONCREATOR_H
#define C_CATALOGSHTTPCONNECTIONCREATOR_H

#include <e32base.h>
#include <commdbconnpref.h> // TCommDbConnPref
#include <connpref.h>       // TConnSnapPref

#include "catalogsconnectionmethod.h"

class RConnection;
class CCatalogsHttpConnectionManager;


class CCatalogsHttpConnectionCreator : public CActive
    {
public:

    static CCatalogsHttpConnectionCreator* NewL();
    
    virtual ~CCatalogsHttpConnectionCreator();
    
    /**
     * @param aMethod Connection method. APN id will be updated to this
     * after the connection has been created successfully
     * @param aConnection Connection used for connecting
     * @param aStatus Request status
     * @param aConnectionManager Connection manager is notified of
     * opened accesspoint. Can be NULL
     */
    void ConnectL( 
        TCatalogsConnectionMethod& aMethod, 
        RConnection& aConnection,
        TRequestStatus& aStatus,
        CCatalogsHttpConnectionManager* aConnectionManager );

    
protected:

    CCatalogsHttpConnectionCreator();
    void ConstructL();

protected: // From CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );
    
    void DoConnectL();

    
    
protected: // Protected data members

    class TConnectionItem
        {
    public:        
        TConnectionItem( 
            TCatalogsConnectionMethod& aMethod, 
            RConnection& aConnection, 
            TRequestStatus& aStatus,
            CCatalogsHttpConnectionManager* aConnectionManager ) :
            iConnectionMethod( aMethod ), 
            iConnection( &aConnection ), 
            iStatus( &aStatus ),
            iConnectionManager( aConnectionManager )
            {
            }

    public:        
        TCatalogsConnectionMethod& iConnectionMethod; 
            
        RConnection* iConnection; // Not owned
        TRequestStatus* iStatus;  // Not owned
        CCatalogsHttpConnectionManager* iConnectionManager; // Not owned
        };


protected:

    void StartConnection( 
        const TConnectionItem& aItem, TBool aHandleAsAlwaysAsk );
    
private: // Private data members

    RArray<TConnectionItem> iConnectionQueue;   
    TCommDbConnPref         iIapConnectionPrefs;    
    TConnSnapPref           iSnapConnectionPrefs;
    TBool                   iConnected;
    };

#endif // C_CATALOGSHTTPCONNECTIONCREATOR_H

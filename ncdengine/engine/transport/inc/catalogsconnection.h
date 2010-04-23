/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef R_CATALOGSCONNECTION_H
#define R_CATALOGSCONNECTION_H

#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>

#include "catalogsreferencebase.h"
#include "catalogshttpincludes.h"

class CCatalogsHttpConnectionCreator;


class MCatalogsConnectionStateObserver 
    {
public:
    virtual void ConnectionStateChangedL( TInt aStage ) = 0;
    virtual void ConnectionStateError( TInt aError ) = 0;

protected:

    virtual ~MCatalogsConnectionStateObserver() 
        {
        }
    };


/**
 * Monitors the state of a connection
 */
class CCatalogsConnectionMonitor : public CActive
    {
public:

    static CCatalogsConnectionMonitor* NewL( 
        MCatalogsConnectionStateObserver& aObserver );
        
    virtual ~CCatalogsConnectionMonitor();
    
    void StartL( const TConnectionInfoBuf& aInfo );

protected: // From CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );
    
private:

    CCatalogsConnectionMonitor( MCatalogsConnectionStateObserver& aObserver );
    void ConstructL();
    
private:

    MCatalogsConnectionStateObserver& iObserver;
    RSocketServ iSs;
    RConnection iConnection;    
    TNifProgressBuf iProgressBuf;     
    };
    


/**
 * Connection observer
 */
class MCatalogsConnectionObserver 
    {
public:

    virtual void ConnectionCreatedL( 
        const TCatalogsConnectionMethod& aMethod ) = 0;
    
    virtual void ConnectionError( TInt aError ) = 0;    

protected:

    virtual ~MCatalogsConnectionObserver()
        {
        }
    };
    

/**
 * Network connection
 */
class CCatalogsConnection : public CCatalogsReferenceBase<CActive>
    {
public:

    static CCatalogsConnection* NewL(         
        CCatalogsHttpConnectionCreator& aConnectionCreator,
        CCatalogsHttpConnectionManager& aConnectionManager,
        MCatalogsConnectionObserver& aObserver );
            

    void ConnectL( 
        const TCatalogsConnectionMethod& aMethod );        
    
    RConnection& Connection();   
    RSocketServ& SocketServer();
    
    const TCatalogsConnectionMethod& ConnectionMethod() const; 
    
    TBool IsConnectedL();

protected: // CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

protected:

    CCatalogsConnection( 
        CCatalogsHttpConnectionCreator& aConnectionCreator,
        CCatalogsHttpConnectionManager& aConnectionManager,
        MCatalogsConnectionObserver& aObserver );
    
    void ConstructL();

    virtual ~CCatalogsConnection();
    
private:
    
    MCatalogsConnectionObserver& iObserver;
    CCatalogsHttpConnectionCreator& iConnCreator;
    CCatalogsHttpConnectionManager& iConnManager;
    RSocketServ iSs;
    RConnection iConnection;
    TCatalogsConnectionMethod iConnectionMethod;        
    };
    
#endif // R_CATALOGSCONNECTION_H

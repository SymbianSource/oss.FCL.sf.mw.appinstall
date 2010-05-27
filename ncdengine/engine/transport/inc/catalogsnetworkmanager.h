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
* Description:   Class CCatalogsNetworkManager declation
*
*/


#ifndef C_CATALOGSNETWORKMANAGER_H
#define C_CATALOGSNETWORKMANAGER_H

#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>

#include "catalogshttptypes.h"
#include "catalogsaccesspointobserver.h"


/**
*/
class CCatalogsNetworkManager : public CBase
    {
public:

    static CCatalogsNetworkManager* NewL();

    virtual ~CCatalogsNetworkManager();
    
public:

    void UpdateConnectionsL();
    TBool IsAccessPointOpen( const TCatalogsConnectionMethod& aAp );
       
    void OpenAccessPointL( const TCatalogsConnectionMethod& aAp );
    void CloseAccessPointL( const TCatalogsConnectionMethod& aAp );
    
    void NotifyAccessPointReallyClosedL( const TCatalogsConnectionMethod& aAp );
    
    void AddObserverL( MCatalogsAccessPointObserver& aObserver );
    void RemoveObserver( MCatalogsAccessPointObserver& aObserver );

private:

    void NotifyAccessPointObserversL( 
        const TCatalogsConnectionMethod& aAp,
        const TCatalogsAccessPointEvent& aEvent );

    void ConstructL();    
private:

    RSocketServ iSs;
    RConnection iConnection;
    TUint iConnectionCount;
    
    // Observers are not owned
    RPointerArray<MCatalogsAccessPointObserver> iObservers;  
            
    };

#endif // C_CATALOGSNETWORKMANAGER_H

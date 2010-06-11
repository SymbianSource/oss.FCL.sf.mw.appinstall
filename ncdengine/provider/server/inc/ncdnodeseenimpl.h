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


#ifndef C_NCDNODESEEN_H
#define C_NCDNODESEEN_H

#include "catalogscommunicable.h"
#include "ncdnodeseeninfo.h"

class CNcdNode;

class CNcdNodeSeen : public CCatalogsCommunicable
    {
public:
    static CNcdNodeSeen* NewL( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode );
    static CNcdNodeSeen* NewLC( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode );

    ~CNcdNodeSeen();
    
    TBool IsSeenL();
    void SetSeenL();
    
public: // from CCatalogsCommunicable
    
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
        
    virtual void CounterPartLost(
        const MCatalogsSession& aSession );
    

protected:
    CNcdNodeSeen( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode );
    void ConstructL();
    
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
    void SetSeenRequestL( MCatalogsBaseMessage& aMessage );
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;
    

private:
    CNcdNodeSeenInfo& iSeenInfo;
    CNcdNode& iOwnerNode;    
    };

#endif // C_NCDNODESEEN_H


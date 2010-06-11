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
* Description:   ?Description
*
*/


#ifndef C_NCDNODESEENFOLDER_H
#define C_NCDNODESEENFOLDER_H

#include "catalogscommunicable.h"

class CNcdNodeFolder;
class CNcdNodeIdentifier;

class CNcdNodeSeenFolder : public CCatalogsCommunicable
    {
public:
    static CNcdNodeSeenFolder* NewL( CNcdNodeFolder& aParent );
    static CNcdNodeSeenFolder* NewLC( CNcdNodeFolder& aParent );

    ~CNcdNodeSeenFolder();
        
public: // from CCatalogsCommunicable
    
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
        
    virtual void CounterPartLost(
        const MCatalogsSession& aSession );
    

protected:
    CNcdNodeSeenFolder( CNcdNodeFolder& aParent );
    void ConstructL();
    
    void SetContentsSeenRequestL( MCatalogsBaseMessage& aMessage ) const;
    void NewCountRequestL( MCatalogsBaseMessage& aMessage ) const;
    void NewNodesRequestL( MCatalogsBaseMessage& aMessage ) const;
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;
    
    TInt NewChildCountL( TInt aRecursionLevel ) const;
    void NewChildrenL(
        TInt aRecurssionLevel,
        RPointerArray<CNcdNodeIdentifier>& aNodes ) const;
    void SetContentsSeenL() const;
    

private:
    CNcdNodeFolder& iOwnerNode;
    };

#endif // C_NCDNODESEENFOLDER_H


/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains MStorable interface
*
*/


#ifndef C_NCD_CHILD_ENTITY_H
#define C_NCD_CHILD_ENTITY_H

#include <e32cmn.h>
// RWriteStream, RReadStream
#include <s32strm.h> 

// CNcdNodeFactory::TNcdNodeType
#include "ncdnodefactory.h"

class CNcdNodeIdentifier;


class CNcdChildEntity : public CBase
    {
public:
    static CNcdChildEntity* NewL( 
        TInt aIndex,
        const CNcdNodeIdentifier& aIdentifier,
        TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
        
    static CNcdChildEntity* NewLC( 
        TInt aIndex,
        const CNcdNodeIdentifier& aIdentifier,
        TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
        
    static CNcdChildEntity* NewL( RReadStream& aStream );
    
    static CNcdChildEntity* NewLC( RReadStream& aStream );
    
    static CNcdChildEntity* NewL( const CNcdChildEntity& aChildEntity );
    
    static CNcdChildEntity* NewLC( const CNcdChildEntity& aChildEntity );
    
    ~CNcdChildEntity();
    
    const CNcdNodeIdentifier& Identifier() const;
    
    void SetIdentifierL( const CNcdNodeIdentifier& aIdentifier );
    
    TInt Index() const;
    
    void ExternalizeL( RWriteStream& aStream ) const;
    
    void InternalizeL( RReadStream& aStream );
    
    TBool Equals( const CNcdChildEntity& aObject ) const;
    
    TBool IsTransparent() const;
    
    void SetTransparent( TBool aTransparent );
    
    CNcdNodeFactory::TNcdNodeType NodeType() const;
    
    void SetNodeType( CNcdNodeFactory::TNcdNodeType aNodeType );
    
private:
    CNcdChildEntity( TInt aIndex, TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
    
    CNcdChildEntity();
    
    void ConstructL( const CNcdNodeIdentifier& aIdentifier );
    
private:
    TInt iIndex;
    CNcdNodeIdentifier* iIdentifier;
    TBool iTransparent;
    CNcdNodeFactory::TNcdNodeType iNodeType;
    };

#endif // C_NCD_CHILD_ENTITY_H

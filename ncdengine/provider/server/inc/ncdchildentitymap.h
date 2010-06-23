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


#ifndef C_NCD_CHILD_ENTITY_MAP_H
#define C_NCD_CHILD_ENTITY_MAP_H

#include <e32cmn.h>

class CNcdNodeIdentifier;
class CNcdChildEntity;

class CNcdChildEntityMap : public CBase
    {
public:
    static CNcdChildEntityMap* NewL(
        const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdChildEntity>& aChildArray,
        TInt aChildCount );
        
    static CNcdChildEntityMap* NewLC(
        const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdChildEntity>& aChildArray,
        TInt aChildCount );
        
    ~CNcdChildEntityMap();
    
    const CNcdNodeIdentifier& ParentIdentifier() const;
    
    const RPointerArray<CNcdChildEntity>& ChildArray() const;
    
    TInt ChildCount() const;
    
private:
    CNcdChildEntityMap( TInt aChildCount );
    
    void ConstructL( const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdChildEntity>& aChildArray );
    
private:
    CNcdNodeIdentifier* iParentIdentifier;
    RPointerArray<CNcdChildEntity> iChildArray;
    TInt iChildCount;
    };

#endif // C_NCD_CHILD_ENTITY_MAP_H

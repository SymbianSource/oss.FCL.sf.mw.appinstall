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
* Description:   Interface definition for numeric value query item.
*
*/
	

#ifndef C_NCD_QUERY_NUMERIC_ITEM_H
#define C_NCD_QUERY_NUMERIC_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitemimpl.h"
#include "ncdquerynumericitem.h"

/**
 *  Describes a numeric query item.
 */
class CNcdQueryNumericItem : public CNcdQueryItem,
                             public MNcdQueryNumericItem
    {
    
public:

    static CNcdQueryNumericItem* NewL(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryNumericItem* NewLC(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryNumericItem* NewL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
    
    static CNcdQueryNumericItem* NewLC(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
      
    void InternalizeL( RReadStream& aReadStream );
    
    void InternalizeL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement );
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;
    
public: // From MNcdQueryNumericItem
    
    /**
     * @see MNcdQueryNumericItem
     */
    virtual void SetValueL( const TDesC& aValue );

public: // From MNcdQueryItem

    /**
     * @see MNcdQueryItem
     */
    virtual TNcdInterfaceId Type() const;
    
public: // From CNcdQueryItem
    
    /**
     * @see CNcdQueryItem
     */
    const TDesC& ValueL();
    
protected:

    virtual ~CNcdQueryNumericItem();

private:

    CNcdQueryNumericItem( CNcdQuery& aParent );
    
    virtual void ConstructL();
    };
	
	
#endif //  M_NCD_QUERY_NUMERIC_ITEM_H

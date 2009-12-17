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
* Description:  
*
*/
	

#ifndef C_NCD_QUERY_ITEM_H
#define C_NCD_QUERY_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitem.h"
#include "catalogsbaseimpl.h"
#include "catalogsinterfacebase.h"


class MNcdConfigurationProtocolQueryElement;
class CNcdString;
class CNcdQuery;
class RWriteStream;
class RReadStream;

const TInt KMaxTIntLengthInChars( 11 );
    
/**
 *  Implementation for query item.
 *
 *  Base class for query items. This class is abstract.
 */
class CNcdQueryItem : public CCatalogsInterfaceBase,
                      public virtual MNcdQueryItem
    {
public:
    
    virtual void InternalizeL( RReadStream& aReadStream );
    
    virtual void InternalizeL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement );
    
    virtual void ExternalizeL( RWriteStream& aWriteStream ) const;
        
    const TDesC& Id() const;
    
    TBool IsSet() const;
    
    virtual const TDesC& ValueL() = 0;
    
    /**
     * If ETrue, this item will be visible only inside the engine.
     */
    TBool IsInvisible() const;
    
    /**
     * Internalizes just the id's and response value.
     *      
     */
    //void InternalizeResponseL( RWriteStream& aWriteStream ) = 0;
    
    /**
     * Externalizes just the id's and response value.
     *
     * @note Use a dynamic buffer for the stream,
     * amout of data written can vary.
     */
    //void ExternalizeResponseL( RWriteStream& aWriteStream ) const = 0;

public: // From MNcdQueryItem

    /**
     * @see MNcdQueryItem
     */
    virtual TNcdInterfaceId Type() const;
    
    /**
     * @see MNcdQueryItem
     */
    virtual MNcdQueryItem::TSemantics Semantics() const;

    /**
     * @see MNcdQueryItem
     */
    virtual const TDesC& Label() const;
    
    /**
     * @see MNcdQueryItem
     */
    virtual const TDesC& Message() const;
    
    /**
     * @see MNcdQueryItem
     */
    virtual const TDesC& Description() const;
    
    /**
     * @see MNcdQueryItem
     */
    virtual TBool IsOptional() const;
    
protected:

    CNcdQueryItem( CNcdQuery& aParent );
    
    virtual ~CNcdQueryItem();
    
    virtual void ConstructL();   
    

protected:
    
    CNcdQuery& iParentQuery;
    HBufC* iId;
    MNcdQueryItem::TSemantics iSemantics;
    
    CNcdString* iLabel;
    CNcdString* iMessage;
    CNcdString* iDescription;
    
    mutable HBufC* iLocalizedLabel;
    mutable HBufC* iLocalizedMessage;
    mutable HBufC* iLocalizedDescription;
    
    TBool iIsOptional;
    TNcdInterfaceId iType;
    HBufC* iValue;
    TBool iIsSet;    
    TBool iIsInvisible;

    };
	
#endif //  C_NCD_QUERY_ITEM_H

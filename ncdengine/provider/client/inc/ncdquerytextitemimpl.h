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
* Description:   Interface definition for text query item.
*
*/
	

#ifndef C_NCD_QUERY_TEXT_ITEM_H
#define C_NCD_QUERY_TEXT_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitemimpl.h"
#include "ncdquerytextitem.h"
#include "ncdinterfaceids.h"

class MNcdConfigurationProtocolQueryElement;

/**
 *  Implementation for text query item.
 *
 * @since S60 v3.2
 */
class CNcdQueryTextItem : public CNcdQueryItem,
                          public MNcdQueryTextItem
    {
    
public:

    static CNcdQueryTextItem* NewL(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryTextItem* NewLC(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryTextItem* NewL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
    
    static CNcdQueryTextItem* NewLC(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
      
    void InternalizeL( RReadStream& aReadStream );
    
    void InternalizeL( const MNcdConfigurationProtocolQueryElement& aQueryElement );
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;
    
    const TDesC& Text();    
    
    
public: // From MNcdQueryTextItem

    /**
     * @see MNcdQueryTextItem
     */ 
    virtual void SetTextL( const TDesC& aText );
    
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

    virtual ~CNcdQueryTextItem();
    
private:

    CNcdQueryTextItem( CNcdQuery& aParent );
    
    virtual void ConstructL();

private:
    
    HBufC* iText;

    };
	
	
#endif //  C_NCD_QUERY_TEXT_ITEM_H

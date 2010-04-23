/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface definition for PIN code query item.
*
*/
	

#ifndef C_NCD_QUERY_PIN_CODE_ITEM_H
#define C_NCD_QUERY_PIN_CODE_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitemimpl.h"
#include "ncdquerypincodeitem.h"

/**
 *  Implementation for pin code query item.
 */
class CNcdQueryPinCodeItem : public CNcdQueryItem,
                             public MNcdQueryPinCodeItem
    {

public:

    static CNcdQueryPinCodeItem* NewL(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryPinCodeItem* NewLC(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQueryPinCodeItem* NewL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
    
    static CNcdQueryPinCodeItem* NewLC(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
      
    void InternalizeL( RReadStream& aReadStream );
    
    void InternalizeL( const MNcdConfigurationProtocolQueryElement& aQuery );
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;
    
    TInt PinCode();
    
public: // From MNcdQueryPinCodeItem

    /**
     * @see MNcdQueryPinCodeItem
     */ 
    virtual void SetPinCodeL( const TDesC& aPinCode );
    
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

    virtual ~CNcdQueryPinCodeItem();
    
private:

    CNcdQueryPinCodeItem( CNcdQuery& aParent );
    
    virtual void ConstructL();
    
private:

    HBufC* iPinCode;

    };
	
	
#endif //  C_NCD_QUERY_PIN_CODE_ITEM_H

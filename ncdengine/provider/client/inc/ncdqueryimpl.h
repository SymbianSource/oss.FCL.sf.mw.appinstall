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
	

#ifndef C_NCD_QUERY_H
#define C_NCD_QUERY_H

#include <e32cmn.h>

#include "ncdquery.h"
#include "ncdqueryitemimpl.h"
#include "catalogsbaseimpl.h"
#include "catalogsinterfacebase.h"
#include "ncdsendable.h"

class MNcdQueryItem;
class MNcdConfigurationProtocolQuery;
class RReadStream;
class RWriteStream;
class MDesCArray;
class MDesC8Array;
class MNcdConfigurationProtocolQueryResponse;
class CNcdString;
class MNcdClientLocalizer;
    
/**
 *  Implementation class for query.
 *
 *  This class is used on both sides of the client/server border.
 */
class CNcdQuery : public CCatalogsInterfaceBase,
                  public MNcdQuery,
                  public MNcdSendable
    {    
    
public:

    // all constructors call InternalAddRef
    static CNcdQuery* NewL( RReadStream& aReadStream );
    
    static CNcdQuery* NewLC( RReadStream& aReadStream );
    
    static CNcdQuery* NewL( const MNcdConfigurationProtocolQuery& aQuery,
        TBool aIsSecureConnection = EFalse );
    
    static CNcdQuery* NewLC( const MNcdConfigurationProtocolQuery& aQuery,
        TBool aIsSecureConnection = EFalse );

    // For payment method query
    static CNcdQuery* NewL( const RPointerArray<CNcdString>& aPaymentMethodNames, 
                            const MDesC8Array& aPaymentMethodTypes );

    static CNcdQuery* NewLC( const RPointerArray<CNcdString>& aPaymentMethodNames,
                             const MDesC8Array& aPaymentMethodTypes );
                             
    void SetClientLocalizer( MNcdClientLocalizer* aLocalizer );
    MNcdClientLocalizer* ClientLocalizer() const;

      
    void InternalizeL( RReadStream& aReadStream );
    
    void InternalizeL( const MNcdConfigurationProtocolQuery& aQuery );
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;  
    

    /**
     * Returns the number of query items in this query.
     *
     * @return Query ítem count.
     */
    TInt ItemCount() const;
    
    /**
     * Returns query item by index.
     *
     * @return Query item.
     */
    CNcdQueryItem& QueryItemL( TInt aIndex );
    
    const TDesC& Id() const;
    
    /**
     * Checks whether all query items have been set.
     *
     * @return ETrue if all items are set.
     */
    TBool AllItemsSet() const;
    
public: // From MNcdQuery
    
    /**
     * @see MNcdQuery
     */
    virtual TBool IsOptional() const;
    
    /**
     * @see MNcdQuery
     */
    virtual MNcdQuery::TSemantics Semantics() const;
    
    /**
     * @see MNcdQuery
     */
    virtual const TDesC& MessageTitle() const;
    
    /**
     * @see MNcdQuery
     */
    virtual const TDesC& MessageBody() const;
    
    /**
     * @see MNcdQuery
     */
    virtual RCatalogsArray< MNcdQueryItem > QueryItemsL();

    /**
     * @see MNcdQuery
     */
    virtual void SetResponseL( TResponse aResponse );

    /**
     * @see MNcdQuery
     */
    virtual TResponse Response();
    
    /**
     * @see MNcdQuery
     */
    TBool IsSecureConnection() const;

protected:

    virtual ~CNcdQuery();

private:
    
    CNcdQuery( TBool aIsSecureConnection );
    
    void ConstructL();
    void ConstructL( const RPointerArray<CNcdString>& aPaymentMethodNames,
                     const MDesC8Array& aPaymentMethodTypes );

private:

    HBufC* iId;
    TBool iIsOptional;
    MNcdQuery::TSemantics iSemantics;
    
    CNcdString* iTitle;
    CNcdString* iBody;
    
    mutable HBufC* iLocalizedTitle;
    mutable HBufC* iLocalizedBody;
    
    TResponse iResponse;
    TBool iIsSecureConnection;
    RCatalogsArray< CNcdQueryItem > iItems;
    
    /**
     * Client localizer, not own.
     */
    MNcdClientLocalizer* iClientLocalizer;
    

    };
	
	
#endif //  C_NCD_QUERY_H

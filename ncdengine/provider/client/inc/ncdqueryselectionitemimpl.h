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
	

#ifndef C_NCD_QUERY_SELECTION_ITEM_H
#define C_NCD_QUERY_SELECTION_ITEM_H

#include <e32cmn.h>
#include <badesca.h>

#include "ncdqueryselectionitem.h"
#include "ncdqueryitemimpl.h"
#include "ncdinterfaceids.h"
#include "ncd_cp_queryelement.h"

class CDesC16ArrayFlat;
class CNcdQueryOption;

/**
 *  Implementation for selection query item.
 */
class CNcdQuerySelectionItem : public CNcdQueryItem,
                               public MNcdQuerySelectionItem
                               
    {
    
public:

    static CNcdQuerySelectionItem* NewL(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQuerySelectionItem* NewLC(
        RReadStream& aReadStream, CNcdQuery& aParent );
    
    static CNcdQuerySelectionItem* NewL(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );
    
    static CNcdQuerySelectionItem* NewLC(
        const MNcdConfigurationProtocolQueryElement& aQueryElement,
        CNcdQuery& aParent );

    // For payment method query
    static CNcdQuerySelectionItem* NewL( const RPointerArray<CNcdString>& aPaymentMethodNames,
										 const MDesC8Array& aPaymentMethodTypes,
										 CNcdQuery& aParent );
    static CNcdQuerySelectionItem* NewLC( const RPointerArray<CNcdString>& aPaymentMethodNames,
										  const MDesC8Array& aPaymentMethodTypes,
										  CNcdQuery& aParent );

      
    void InternalizeL( RReadStream& aReadStream );
    
    void InternalizeL( const MNcdConfigurationProtocolQueryElement& aQuery );
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;


public: // From MNcdQuerySelectionItem    
               

    /**
     * @see MNcdQuerySelectionItem
     */
    virtual const MDesCArray& SelectionTexts() const;
    
    /**
     * @see MNcdQuerySelectionItem
     */
    virtual const MDesC8Array& SelectionData() const;
    
    /**
     * @see MNcdQuerySelectionItem
     */
    virtual void SetSelectionL( TInt aIndex );
        
    /**
     * @see MNcdQuerySelectionItem::Selection()
     */
    virtual TInt Selection() const;
    
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

    virtual ~CNcdQuerySelectionItem();
    
private:

    CNcdQuerySelectionItem( CNcdQuery& aParent );
    
    virtual void ConstructL();

    virtual void ConstructL( const RPointerArray<CNcdString>& aPaymentMethodNames,
                             const MDesC8Array& aPaymentMethodTypes );
                             
    void CreateSelectionTextsL();
    
private:

    CDesC16ArrayFlat* iSelectionTexts;
    CDesC8ArrayFlat* iSelectionData;
    RPointerArray< CNcdQueryOption > iOptions;
    TInt iSelection;

    };
	
	
#endif //  M_NCDQUERYSELECTIONITEM_H

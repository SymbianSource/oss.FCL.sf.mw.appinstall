/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_NCD_QUERY_SELECTION_ITEM_H
#define M_NCD_QUERY_SELECTION_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitem.h"
#include "ncdinterfaceids.h"

class MDesCArray;
class MDesC8Array;

/**
 *  Describes a selection query item.
 */
class MNcdQuerySelectionItem : public virtual MNcdQueryItem
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdQuerySelectionItemUid };

    /**
     * Getter for selection texts.
     *
     * 
     * @return Array of selection item names, one for each selection.
     */
    virtual const MDesCArray& SelectionTexts() const = 0;

    /**
     * Getter for additional selection data.
     * 
     * @note Actual contents depend on context. Query semantics can be used to determine
     *  the content type (if any).
     *
     * 
     * @return Array of descriptors containing the additional data for each selection.
     */
    virtual const MDesC8Array& SelectionData() const = 0;

    /**
     * Selects one of the choices.
     *
     * 
     * @param aIndex Index of the selection (index of the selection text in the array
     *  returned by SelectionTexts()
     * @exception KErrArgument Specified index is out of bounds.
     */
    virtual void SetSelectionL( TInt aIndex ) = 0;
    
    
    /**
     * Selection getter
     *
     * @return Index that was set with SetSelectionL
     */
    virtual TInt Selection() const = 0;
    

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdQuerySelectionItem() {}

    };
	
	
#endif //  M_NCDQUERYSELECTIONITEM_H

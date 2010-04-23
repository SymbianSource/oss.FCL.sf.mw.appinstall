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
* Description:   MIAUpdateContainerObserver interface
*
*/



#ifndef IAUPDATECONTAINEROBSERVER_H
#define IAUPDATECONTAINEROBSERVER_H

#include <e32base.h>

class MIAUpdateContainerObserver
{
public: // Observer API
    /**
    * Mark list item
    * @param aMark True ig marking, False if unmarking
    * @param aIndex  Index of the item
    * @return False value if marking/unmarking cancelled when dependencies/dependants not found
    */ 
    virtual TBool MarkListItemL( TBool aMark, TInt aIndex ) = 0;

    /**
    * Set text label to middle soft key
    * @param aVisible   True value when MSK text is visible
    * @param aSelected  True value when list item is selected( marked)
    */
    virtual void SetMiddleSKTextL( TBool aVisible, TBool aSelected ) = 0;
};
	
#endif // IAUPDATECONTAINEROBSERVER_H

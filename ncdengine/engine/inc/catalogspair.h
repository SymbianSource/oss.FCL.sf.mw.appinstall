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
* Description:   Contains CCatalogsPair class
*
*/


#ifndef CCATALOGSPAIR_H
#define CCATALOGSPAIR_H

#include <e32base.h>

/**
 * Simple pair-type
 *
 */
template<typename KeyType, typename ValueType>
class CCatalogsPair : public CBase
    {
public:

    /**
     * @param aKey Key. Ownership is transferred
     * @param aValue Value. Value is copied
     */
    CCatalogsPair( KeyType* aKey, const ValueType& aValue );
    
    virtual ~CCatalogsPair();
    
    const KeyType& Key() const;
    const ValueType& Value() const;
    
    void SetValue( const ValueType& aValue );
        
private:

    KeyType* iKey;
    ValueType iValue;
          
    };
    
#include "catalogspair.inl"    

#endif // CCATALOGSPAIR_H
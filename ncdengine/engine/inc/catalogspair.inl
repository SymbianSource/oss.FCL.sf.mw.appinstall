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


#include "catalogsdebug.h"

template<typename KeyType, typename ValueType>
CCatalogsPair<KeyType, ValueType>::CCatalogsPair( KeyType* aKey, const ValueType& aValue ) :
    iKey( aKey ), iValue( aValue )
    {
    DASSERT( iKey )
    }

template<typename KeyType, typename ValueType>   
CCatalogsPair<KeyType, ValueType>::~CCatalogsPair()
    {
    DLTRACEIN((""));
    delete iKey;
    }    
    
    
template<typename KeyType, typename ValueType>   
const KeyType& CCatalogsPair<KeyType, ValueType>::Key() const
    {
    return *iKey;
    }


template<typename KeyType, typename ValueType>   
const ValueType& CCatalogsPair<KeyType, ValueType>::Value() const
    {
    return iValue;
    }


template<typename KeyType, typename ValueType> 
void CCatalogsPair<KeyType, ValueType>::SetValue( const ValueType& aValue )
    {
    iValue = aValue;
    }
    
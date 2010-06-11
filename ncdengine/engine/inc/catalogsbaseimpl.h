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
* Description:   Contains MCatalogsBase implementation base class definition
*
*/


#ifndef C_CATALOGS_BASE_IMPL_H
#define C_CATALOGS_BASE_IMPL_H

#include <e32cmn.h>
#include <e32base.h>
#include "catalogsbase.h"
#include "catalogsdebug.h"

template< class CBaseClass >
class CCatalogsMetaBase : public CBaseClass
    {
    };

template<>
class CCatalogsMetaBase< CActive > : public CActive
    {
public:

    CCatalogsMetaBase() :
      CActive( EPriorityStandard )
        {}
    };

template< class CBaseClass >
class CCatalogsBase : public CCatalogsMetaBase< CBaseClass >, public virtual MCatalogsBase
    {
public:
    CCatalogsBase()
        : iRefCount( 0 )
        {
        }

    TInt AddRef() const
        { 
        DLTRACEIN(("this: %X, base-ptr: %X, refcount: %d", this, 
            static_cast<const MCatalogsBase*>( this ), iRefCount+1));
        return ++iRefCount;
        }

    TInt Release() const
        { 
        DLTRACEIN(("this: %X, base-ptr: %X", this, 
            static_cast<const MCatalogsBase*>( this )));

        TInt result = --iRefCount; 
        DLTRACE(("refCount = %d", result));
        if( result == 0 ) 
            {
            DLTRACE(("deleting this: %X, base-ptr: %X", this, 
                static_cast<const MCatalogsBase*>( this )));
            delete this; 
            }
        return result;
        }

protected:
    
    mutable TInt iRefCount;
    
    };

#endif // C_CATALOGS_BASE_IMPL_H

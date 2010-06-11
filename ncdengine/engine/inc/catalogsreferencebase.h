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
* Description:   Contains CCatalogsRefString
*
*/


#ifndef C_CATALOGSREFERENCEBASE_H
#define C_CATALOGSREFERENCEBASE_H

#include <e32base.h>

template<class CBaseClass>
class CCatalogsReferenceBase : public CBaseClass
    {
public:

    TInt AddRef() const
        { 
        return ++iRefCount;
        }

    TInt Release() const
        { 
        TInt result = --iRefCount;         
        if( result == 0 ) 
            {
            delete this; 
            }
        return result;
        }
        
        
    TInt RefCount() const 
        {
        return iRefCount;
        }

protected:

    CCatalogsReferenceBase ()
        : iRefCount( 1 )
        {
        }
        
    virtual ~CCatalogsReferenceBase()
        {
        }

protected:
    
    mutable TInt iRefCount;
    
    };


template<>
class CCatalogsReferenceBase<CActive> : public CActive
    {
    
public:

    TInt AddRef() const
        { 
        return ++iRefCount;
        }

    TInt Release() const
        { 
        TInt result = --iRefCount;         
        if( result == 0 ) 
            {
            delete this; 
            }
        return result;
        }

    TInt RefCount() const 
        {
        return iRefCount;
        }

protected:

    CCatalogsReferenceBase () : 
        CActive( EPriorityStandard ), 
        iRefCount( 1 )
        {
        }

    virtual ~CCatalogsReferenceBase()
        {
        }

protected:
    
    mutable TInt iRefCount;
            
    };



#endif // C_CATALOGSREFERENCEBASE_H

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


#ifndef C_CATALOGSREFSTRING_H
#define C_CATALOGSREFSTRING_H

#include <e32base.h>
#include "catalogsreferencebase.h"

class CCatalogsRefString : public CCatalogsReferenceBase<CBase>
    {
public:

    static TInt Order( 
        const CCatalogsRefString& aFirst,
        const CCatalogsRefString& aSecond ) 
        {
        return aFirst.String().Compare( aSecond.String() );        
        }

public:
    
    /**
     * Creates a new CCatalogsRefString
     *
     * @param aString Ownership is transferred. If a leave occurs, aString is
     * delete automatically so it MUST NOT be in cleanupstack
     */
    static CCatalogsRefString* NewLC( HBufC* aString )
        {
        // This ensures that aString is deleted if new( ELeave ) leaves
        CleanupStack::PushL( aString );
        CCatalogsRefString* self = new( ELeave ) CCatalogsRefString( aString );
        CleanupStack::Pop( aString );
        CleanupReleasePushL( *self );    
        return self;
        }
        
    virtual const TDesC& String() const 
        {
        return *iString;
        }
        
    operator const TDesC&() const 
        {
        return *iString;
        }
        
protected:

    CCatalogsRefString( HBufC* aString ) : iString( aString )
        {
        }
    
    virtual ~CCatalogsRefString() 
        {
        delete iString;
        }

private:

    CCatalogsRefString( const CCatalogsRefString& );
    CCatalogsRefString& operator=( const CCatalogsRefString& );
    
private:

    HBufC* iString;        
    };


/**
 * One ugly trick
 *
 * Used for searching in arrays without need to create a copy of the
 * search string
 */
class CCatalogsRefSearchString : public CCatalogsRefString
    {
public:

    explicit CCatalogsRefSearchString( const TDesC& aString ) :
        CCatalogsRefString( NULL ),
        iSearchString( &aString )
        {
        }

    const TDesC& String() const 
        {
        return *iSearchString;
        }

    void SetString( const TDesC& aString ) 
        {
        iSearchString = &aString;
        }
        
    ~CCatalogsRefSearchString() 
        {
        }
        
private:
        
    const TDesC* iSearchString;
    };

    
#endif // C_CATALOGSREFSTRING_H


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
* Description:   Contains CCatalogsStringManager
*
*/


#ifndef C_CCATALOGSSTRINGMANAGER_H
#define C_CCATALOGSSTRINGMANAGER_H

#include <e32base.h>
#include "catalogsarray.h"

class CCatalogsRefString;
class CCatalogsRefSearchString;

class CCatalogsStringManager : public CBase
    {        
public:

    static CCatalogsStringManager* NewL();
    
    static void Delete();
    /**
     * Sets the string manager
     * 
     * @param aManager New string manager. Ownership IS transferred
     */
    static void SetStringManager( CCatalogsStringManager* aManager );
    static CCatalogsStringManager& StringManager() 
        {
        return *iStringManager;
        }

    virtual ~CCatalogsStringManager();
    
    /**
     * 
     */
    const CCatalogsRefString& StringL( 
        TInt aType, 
        const TDesC& aString );

    /**
     * @param aString Ownership is transferred
     */
    const CCatalogsRefString& StringL( 
        TInt aType, 
        HBufC* aString );

    
    void ReleaseString( 
        TInt aType,
        const CCatalogsRefString& aString );

    /**
     * Releases a string from a pointer if the pointer != NULL
     * String pointer is set to NULL afterwards
     */
    void ReleaseString( 
        TInt aType,
        const CCatalogsRefString*& aString );
    
private:
    typedef RCatalogsArray<CCatalogsRefString> RStringArray;
    
private:

    CCatalogsStringManager();
    void ConstructL();

    inline RStringArray& ArrayL( TInt aIndex );
    
    inline TInt FindString( 
        const RStringArray& aArray,
        const TDesC& aString ) const;
    
    
    inline const CCatalogsRefString& AddStringL(    
        RStringArray& aArray,
        HBufC* aString );
        
private:

    static CCatalogsStringManager* iStringManager;
    
    RArray<RStringArray> iStrings;
    
    TLinearOrder<CCatalogsRefString> iOrder;
    CCatalogsRefSearchString* iSearchString;
    };

#endif // C_CCATALOGSSTRINGMANAGER_H

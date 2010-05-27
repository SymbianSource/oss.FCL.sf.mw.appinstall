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
* Description:   MNcdProtocolElementEntity declaration
*
*/


#ifndef NCDPROTOCOLELEMENTQUERYIMPL_H
#define NCDPROTOCOLELEMENTQUERYIMPL_H

#include <e32base.h>

#include "ncd_cp_query.h"

class CNcdString;
class MNcdConfigurationProtocolQueryElement;
class CNcdConfigurationProtocolQueryElementImpl;

class CNcdConfigurationProtocolQueryImpl :  public CBase,
                                            public MNcdConfigurationProtocolQuery
    {
public:
    
    static CNcdConfigurationProtocolQueryImpl* NewL();
    static CNcdConfigurationProtocolQueryImpl* NewLC();
    
    CNcdConfigurationProtocolQueryImpl();

    void ConstructL();
    
    virtual ~CNcdConfigurationProtocolQueryImpl();

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const TDesC& Id() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual MNcdQuery::TSemantics Semantics() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TNcdProtocolQueryTrigger Trigger() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TBool Force() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const TDesC& ResponseUri() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TBool Optional() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const CNcdString& Title() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const CNcdString& BodyText() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TInt QueryElementCount() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const MNcdConfigurationProtocolQueryElement& QueryElementL( 
        TInt aIndex ) const;
        
public:
    
    HBufC* iId;
    MNcdQuery::TSemantics iSemantics;
    TNcdProtocolQueryTrigger iTrigger;
    TBool iForce;
    HBufC* iResponseUri;
    TBool iOptional;

//     HBufC* iTitle;
//     HBufC* iBodyText;
    CNcdString* iTitle;
    CNcdString* iBodyText;


    RPointerArray<CNcdConfigurationProtocolQueryElementImpl> iQueryElements;
    

    };


#endif // NCDPROTOCOLELEMENTQUERYIMPL_H

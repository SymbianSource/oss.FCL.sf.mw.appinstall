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
* Description:   Contains CCatalogsInterfaceIdentifier class
*
*/


#ifndef CATALOGS_INTERFACE_IDENTIFIER_H
#define CATALOGS_INTERFACE_IDENTIFIER_H


#include <e32base.h>


class MCatalogsBase;
class CCatalogsInterfaceBase;


class CCatalogsInterfaceIdentifier: public CBase
    {

public:

    /**
     * @param aInterfaceObject Ownership is not transferred.
     */
    static CCatalogsInterfaceIdentifier* NewL( TAny* aInterfaceObject,
                                               CCatalogsInterfaceBase* aInterfaceBaseObject,
                                               TInt aInterfaceId );
    /**
     * @param aInterfaceObject Ownership is not transferred.
     */
    static CCatalogsInterfaceIdentifier* NewLC( TAny* aInterfaceObject,
                                                CCatalogsInterfaceBase* aInterfaceBaseObject,
                                                TInt aInterfaceId );

    ~CCatalogsInterfaceIdentifier();

    
    /**
     * This object does not own the returned object. So,
     * ownership is not transferred directly. 
     * But, Release may be called
     * to decrease the ref counter of the object.
     */
    const TAny* InterfaceObject() const;
    
    /**
     * This object does not own the returned object. So,
     * ownership is not transferred directly. 
     * But, Release may be called
     * to decrease the ref counter of the object.
     */
    const CCatalogsInterfaceBase* InterfaceBaseObject() const;   
    
    TInt InterfaceId() const;


protected:

    CCatalogsInterfaceIdentifier( TAny* aInterfaceObject,
                                  CCatalogsInterfaceBase* aInterfaceBaseObject,  
                                  TInt aInterfaceId );
    
    void ConstructL();


private:
    // Prevent
    CCatalogsInterfaceIdentifier( const CCatalogsInterfaceIdentifier& aObject );
    CCatalogsInterfaceIdentifier& operator =( const CCatalogsInterfaceIdentifier& aObject );


private: // data

    // Does not own the object
    TAny* iInterfaceObject;
    CCatalogsInterfaceBase* iInterfaceBaseObject;
    TInt iInterfaceId;
    
    };

#endif // CATALOGS_INTERFACE_BASE_H

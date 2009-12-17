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
* Description:   Contains CNcdBaseProxy class
*
*/


#ifndef NCD_BASE_PROXY_H
#define NCD_BASE_PROXY_H


#include <e32base.h>


class MCatalogsClientServer;


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdBaseProxy: public CBase                     
    {

protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     */
    CNcdBaseProxy( MCatalogsClientServer& aSession, 
                   TInt aHandle );

    /**
     * Destructor
     */
    virtual ~CNcdBaseProxy();


public:

    /**
     * ClientServerSession
     *
     * @return MCatalogsClientServer& an object that contains
     * the client server session information
     */
    MCatalogsClientServer& ClientServerSession() const;


    /**
     * Handle
     *
     * @return TInt a handle that identifies the server side object
     * that this proxy uses.
     */
    TInt Handle() const;
        
    
private:
    // Prevent if not implemented
    CNcdBaseProxy( const CNcdBaseProxy& aObject );
    CNcdBaseProxy& operator =( const CNcdBaseProxy& aObject );

    
private: // data

    /**
     * Interface for communicating to the server-side.
     */    
    MCatalogsClientServer& iSession;

    /**
     * Handle to identify the server-side counterpart for object of this
     * class.
     */
    TInt iHandle;
    
    };


#endif // NCD_BASE_PROXY_H

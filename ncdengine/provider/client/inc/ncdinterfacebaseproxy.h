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
* Description:   Contains CNcdInterfaceBaseProxy class
*
*/


#ifndef NCD_INTERFACE_BASE_PROXY_H
#define NCD_INTERFACE_BASE_PROXY_H


#include "catalogsinterfacebase.h"


class MCatalogsClientServer;


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdInterfaceBaseProxy: public CCatalogsInterfaceBase                     
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
    CNcdInterfaceBaseProxy( MCatalogsClientServer& aSession, 
                            TInt aHandle,
                            CCatalogsInterfaceBase* aParent );

    /**
     * Destructor
     */
    virtual ~CNcdInterfaceBaseProxy();


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
        
    
protected:

    /** 
     * Sets the id used for the release message
     */
    void SetReleaseId( TInt aId );
            
private:
    // Prevent if not implemented
    CNcdInterfaceBaseProxy( const CNcdInterfaceBaseProxy& aObject );
    CNcdInterfaceBaseProxy& operator =( const CNcdInterfaceBaseProxy& aObject );

    
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
    
    TInt iReleaseId;
    };


#endif // NCD_INTERFACE_BASE_PROXY_H

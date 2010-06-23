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
* Description:   Header of actual implementation of context-class
*
*/


#ifndef C_CATALOGS_CONTEXT_H
#define C_CATALOGS_CONTEXT_H

#include <e32base.h>

#include "catalogscontext.h"


/**
 *  Server-side class to contain needed info of context
 *
 *  Server-side class to contain needed info of context related
 *  to client-session and the process where the session resides.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsContextImpl : public CBase, public MCatalogsContext
    {

public:


    static CCatalogsContextImpl* NewL( TUid aFamilyId, TSecureId aSecureId,
        TInt aInstanceId );

    static CCatalogsContextImpl* NewLC( TUid aFamilyId, TSecureId aSecureId,
        TInt aInstanceId );

    virtual ~CCatalogsContextImpl();


    /**
     * Function to store a familyid of the client-session
     *
     * @since S60 ?S60_version
     * @param aNewId Id that is stored as a family-id of
     *               the client-session
     */
     void SetFamilyId( TUid aNewId );

    /**
     * Function to store secure id of the client-session's
     * process.
     *
     * @since S60 ?S60_version
     * @param aNewId Value that should be stored as the
     *               secure-id of the client-session's process
     */
     void SetSecureId( TSecureId aNewId );

// from base class MCatalogsContext

    /**
     * From MCatalogsContext.
     * @see MCatalogsContext::FamilyId
     *
     * @since S60 ?S60_version
     * @return TUid
     */
    TUid FamilyId() const;

    /**
     * From MCatalogsContext.
     * @see MCatalogsContext::SecureId
     *
     * @since S60 ?S60_version
     * @return TSecureId
     */    
    TSecureId SecureId() const;

    /**
     * From MCatalogsContext.
     * @see MCatalogsContext::ProviderData
     */
    RBuf8& ProviderDataL( TInt aProviderIndex );
    
    /**
     * From MCatalogsContext.
     * @see MCatalogsContext::InstanceId
     *
     * @since S60 ?S60_version
     * @return TInt
     */
    TInt InstanceId() const;
    

private:

    CCatalogsContextImpl( TUid aFamilyId, TSecureId aSecureId,
        TInt aInstanceId );

    void ConstructL();


private: // data

    /**
     * Uid that represents the family id of the client-session
     */
    TUid iFamilyId;

    /**
     * Secure id that of the client-session's process
     */
    TSecureId iSecureId;

    /**
     * Provider-specific data pointer array.
     */
    RArray< RBuf8 > iProviderData;
    
    /**
     * Instance id that differentiates clients with same family id and sid.
     */
    TInt iInstanceId;

    };


#endif // C_CATALOGS_CONTEXT_H

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
* Description:   Mixinclass for session's context-related tasks and info
*
*/


#ifndef M_CATALOGS_CONTEXT_H
#define M_CATALOGS_CONTEXT_H

#include <e32cmn.h>

/**
 *  Class for session's context-related tasks and info
 *
 *  Server-side class for context-related tasks and info
 *
 *  @since S60 3.2
 */
class MCatalogsContext
    {

public:

    /**
     * Function that returns uid which identifies the family id
     * of the client-session
     *
     * @since S60 3.2
     * @return TUid that identifies the family id
     */
    virtual TUid FamilyId() const = 0;

    /**
     * Function that returns sid of the client-process
     *
     * @since S60 3.2
     * @return TUid that identifies the family id
     */
    virtual TSecureId SecureId() const = 0;

    /**
     * Method for getting a provider-specific data for the
     * context for reading and/or modification. By default the data
     * is empty.
     *
     * @since S60 3.2
     * @param aProviderIndex Local index number of the provider.
     * @return Context data for the specified provider.
     */
    virtual RBuf8& ProviderDataL( TInt aProviderIndex ) = 0;
    
    /**
     * Function that returns the instance id of the client-session.
     * There may be multiple clients with the same family id and SID
     * (i.e. multiple instances of the same client) and in some cases
     * it's necessary to differentiate them. This id serves that purpose.
     * 
     * @return Instance id that differentiates clients using the same family uid
     */
    virtual TInt InstanceId() const = 0;
    
    };


#endif // M_CATALOGS_CONTEXT_H

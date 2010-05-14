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
* Description:   ?description
*
*/


#ifndef C_CATALOGS_SERVER_ENGINE_H
#define C_CATALOGS_SERVER_ENGINE_H

#include <e32base.h>

class MCatalogsSession;
class CNcdProviderManager;

/**
 *  Server-side engine implementation
 *
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsServerEngine : public CBase
    {


public:


    static CCatalogsServerEngine* NewL();

    static CCatalogsServerEngine* NewLC();

    virtual ~CCatalogsServerEngine();

    /**
     * Function to create provider.
     *
     * @since S60 ?S60_version
     * @param aSession Interface that is used to register objects etc.
     * @param aProvider Int (uid) to identify provider that should
     *                  be created
     * @param aHandle Integer used to pass the handle to created
     *                provider back
     */
    void CreateProviderL( MCatalogsSession& aSession,
                          TInt aProvider,
                          TInt& aHandle,
                          TUint32 aOptions );
                         
    /**
     * Informs objects of session removal. They should not use
     * given session in anyway anymore
     *
     * @since S60 ?S60_version
     * @param aSession Session that is not in use anymore.
     */   
    void HandleSessionRemoval( MCatalogsSession& aSession );


protected:


private:

    CCatalogsServerEngine();
    void ConstructL();

private: // data

    CNcdProviderManager* iProviderManager;
    };


#endif // C_CATALOGS_SERVER_ENGINE_H

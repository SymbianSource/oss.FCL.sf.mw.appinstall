/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class CNcdProviderManager declaration
*
*/


#ifndef C_NCD_PROVIDERMANAGER_H
#define C_NCD_PROVIDERMANAGER_H

#include <e32base.h>

#include "ncdprovidercloseobserver.h"
#include "catalogscontext.h"

class CNcdProviderUtils;
class CCatalogsTransport;
class CNcdProvider;
class CNcdStorageManager;
class RFs;

/**
 * This class manages the general managers of all connected
 * families
 */
class CNcdProviderManager : 
    public CBase,
    public MNcdProviderCloseObserver
    {
public:
    
    static CNcdProviderManager* NewL();
    
    virtual ~CNcdProviderManager();

public:
    
    /**
     * Gets or creates a provider for the client
     */
    CNcdProvider& ProviderL( 
        const MCatalogsContext& aContext,
        TBool& aCreated );

    /**
     * Gets a provider
     * 
     * @return Client's provider or NULL if it hasn't been created
     */
    CNcdProvider* Provider( 
        const MCatalogsContext& aContext ) const;
    
    
protected: // MNcdProviderCloseObserver
    
    void ProviderClosed( CNcdProvider& aProvider );

private:    
    
    CNcdProviderManager();
    void ConstructL();
    
    TInt FindFamily( const MCatalogsContext& aContext ) const;

    HBufC* WritableEnginePathL( RFs& aFs );
    
private:
        
    RPointerArray<CNcdProvider> iProviders; // owns one reference
    CNcdProviderUtils* iProviderUtils;      // owned
    CCatalogsTransport* iTransport;         // owned
    CNcdStorageManager* iStorageManager;    // owned
    HBufC* iEngineRootPath;                 // owned
    };

#endif /* C_NCD_PROVIDERMANAGER_H */

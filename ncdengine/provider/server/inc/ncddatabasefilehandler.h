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
* Description:  
*
*/


#ifndef C_NCDDATABASEFILEHANDLER_H
#define C_NCDDATABASEFILEHANDLER_H

#include <e32base.h>
#include "ncdfilehandler.h"
//#include "ncdproviderdefines.h"
#include "ncdnodeclassids.h"

class MNcdStorageClient;
class MNcdStorage;

/**
 * File handler that uses File storage
 */
class CNcdDatabaseFileHandler : CBase, public MNcdFileHandler
    {
public:
 
    /**
     * Creates a new file handler
     */
    static CNcdDatabaseFileHandler* NewL( 
        MNcdStorageClient& aStorageClient,
        NcdNodeClassIds::TNcdNodeClassType aDataType );
        
    ~CNcdDatabaseFileHandler();

public: // MNcdFileHandler    

    /**
     * Parameter aIdOrName is not used
     *
     * @see MNcdFileHandler::MoveFileL()
     */
    void MoveFileL( const TDesC& aSourceFile,
        const TDesC& aNamespaceOrDir, const TDesC& aIdOrName,
        TBool aOverwrite );
        
private:

    CNcdDatabaseFileHandler( MNcdStorageClient& aStorageClient,
        NcdNodeClassIds::TNcdNodeClassType aDataType );
    void ConstructL();
    
private:

    // Gets the storage. Creates it if necessary
    MNcdStorage& StorageL( const TDesC& aNamespace );
        
private:
    
    MNcdStorageClient& iStorageClient;
    NcdNodeClassIds::TNcdNodeClassType iDataType;
    
    };

#endif // C_NCDDATABASEFILEHANDLER_H
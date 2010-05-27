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


#ifndef C_NCDFILESTORAGEFILEHANDLER_H
#define C_NCDFILESTORAGEFILEHANDLER_H

#include <e32base.h>
#include "ncdfilehandler.h"

class MNcdStorageClient;
class MNcdStorage;

/**
 * File handler that uses File storage
 */
class CNcdFileStorageFileHandler : CBase, public MNcdFileHandler
    {
public:
 
    /**
     * Creates a new file handler
     */
    static CNcdFileStorageFileHandler* NewL( 
        MNcdStorageClient& aStorageClient );
        
    ~CNcdFileStorageFileHandler();

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

    CNcdFileStorageFileHandler( MNcdStorageClient& aStorageClient );
    void ConstructL();
    
private:

    // Gets the storage. Creates it if necessary
    MNcdStorage& StorageL( const TDesC& aNamespace );
        
private:
    
    MNcdStorageClient& iStorageClient;
    
    };

#endif // C_NCDFILESTORAGEFILEHANDLER_H
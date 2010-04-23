/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdatePendingNodesFile
*
*/




#ifndef IA_UPDATE_PENDING_NODES_FILE_H
#define IA_UPDATE_PENDING_NODES_FILE_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>

class CIAUpdaterIdentifier;


/**
 *
 *
 * @since S60 v3.2
 */
class CIAUpdatePendingNodesFile : public CBase
    {

public:

    /**
     * @note ReadDataL should be called to get the values from the file.
     * Otherwise, default values are used.
     *
     * @note The default path to the file where ReadDataL gets its data
     * is set to the private directory file of the UI and the default
     * name of the file is used. If the file path should be changed, 
     * then use SetFilePathL for this.
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdatePendingNodesFile* NewL();

    IMPORT_C static CIAUpdatePendingNodesFile* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdatePendingNodesFile();


    IMPORT_C void Reset();


    IMPORT_C TInt Index() const;
    
    IMPORT_C void SetIndex( TInt aIndex );    
    

    IMPORT_C RPointerArray< CIAUpdaterIdentifier >& PendingNodes();
    

    /**
     * @return const TDesC& File path that is used for ReadDataL, WriteDataL
     * and RemoveFile.
     *
     * @since S60 v3.2
     */
    IMPORT_C const TDesC& FilePath() const;

    /**
     * @param aPath The full path of the data file that is used when
     * ReadDataL, WriteDataL and RemoveFile are used.
     *
     * @since S60 v3.2
     */
    IMPORT_C void SetFilePathL( const TDesC& aPath );


    /**
     * @exception Leaves with KErrNotFound if the file was not found.
     * Otherwise system wide error code.
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool ReadDataL();

    IMPORT_C void WriteDataL();


    IMPORT_C TInt RemoveFile();    


private:

    // Prevent these if not implemented
    CIAUpdatePendingNodesFile( const CIAUpdatePendingNodesFile& aObject );
    CIAUpdatePendingNodesFile& operator =( const CIAUpdatePendingNodesFile& aObject );


    CIAUpdatePendingNodesFile();
    
    void ConstructL();


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private: // data

	// file server session
	RFs iFsSession;
	
	// controller data file path
	TFileName iPath;


    // Array of node identifiers that inform what nodes have been set as pending
    // nodes.
    RPointerArray< CIAUpdaterIdentifier > iPendingNodes;
    
    // This index can be used to divide the pending nodes array
    // into two parts. Its up to the user to decide the purpose.
    TInt iIndex;

    };
		
#endif // IA_UPDATE_PENDING_NODES_FILE_H


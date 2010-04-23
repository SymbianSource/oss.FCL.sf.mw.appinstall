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
* Description:   CIAUpdaterFileList
*
*/




#ifndef IA_UPDATER_FILE_LIST_H
#define IA_UPDATER_FILE_LIST_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>

class CIAUpdaterFileInfo;
class CIAUpdaterIdentifier;


/**
 *
 */
class CIAUpdaterFileList : public CBase
    {

public:

    IMPORT_C static CIAUpdaterFileList* NewL();

    IMPORT_C static CIAUpdaterFileList* NewLC();
    
    
    /**
     *
     */    
    IMPORT_C virtual ~CIAUpdaterFileList();


    IMPORT_C void Reset();
        

    IMPORT_C CIAUpdaterIdentifier& Identifier();


    IMPORT_C const TDesC& BundleName() const;
    IMPORT_C void SetBundleNameL( const TDesC& aName );

    /**
     * Checks if the bundle is set as hidden.
     *
     * @return ETrue if file list describes hidden files.
     * Else EFalse.
     */
    IMPORT_C TBool Hidden() const;
    IMPORT_C void SetHidden( TBool aHidden );


    /**
     *
     */
    IMPORT_C const RPointerArray< CIAUpdaterFileInfo >& FileInfos() const;

    /**
     * @param aInfo File info. Ownership is transferred.
     */
    IMPORT_C void AddFileInfoL( CIAUpdaterFileInfo* aInfo );

    /**
     * @param aIndex. Index of the file info that will be removed from the
     * list and deleted.
     */
    IMPORT_C void RemoveFileInfo( TInt aIndex );


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private:

    // Prevent these if not implemented
    CIAUpdaterFileList( const CIAUpdaterFileList& aObject );
    CIAUpdaterFileList& operator =( const CIAUpdaterFileList& aObject );


    CIAUpdaterFileList();
    
    void ConstructL();


private: // data
    
    // Identifier that identifies the node and the content that
    // this file list is for.
    CIAUpdaterIdentifier* iIdentifier;
    
    // The name that can be used to describe the bundel
    HBufC* iBundleName;

    // The file should be hidden
    TBool iHidden;
    
    // Contains file paths.
    RPointerArray< CIAUpdaterFileInfo > iFileInfos;
     
    };
		
#endif // IA_UPDATER_FILE_LIST_H


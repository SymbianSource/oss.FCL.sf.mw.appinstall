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
* Description:   Contains CNcdPreviewManager class
*
*/


#ifndef C_NCDPREVIEWMANAGER_H
#define C_NCDPREVIEWMANAGER_H


#include <e32base.h>
#include <f32file.h>
#include "ncdstoragedataitem.h"

class CNcdNodeIdentifier;
class MNcdStorageManager;
class MNcdFileStorage;
class CNcdGeneralManager;

/**
 * Manages previews
 */
class CNcdPreviewManager : public CBase, public MNcdStorageDataItem
    {
public: // Constructor & destructor

        
    static CNcdPreviewManager* NewL( 
        CNcdGeneralManager& aGeneralManager, TInt aMaxCount );    
        
    virtual ~CNcdPreviewManager();
    
public:    

    /**
     * Adds a new preview file.
     *
     * @param aId Id used to identify the owner of the preview
     * @param aUri Download uri
     * @param aPreviewFile Downloaded preview file
     * @param aMimeType MIME type
     */
    void AddPreviewL( 
        const CNcdNodeIdentifier& aId, 
        const TDesC& aUri, 
        const TDesC& aPreviewFile,
        const TDesC& aMimeType );


    /**
     * Removes a preview file
     *
     * @param aId Id used to identify the owner of the preview
     * @param aUri URI where the preview was downloaded from
     * @return KErrNone or a Symbian error code
     */
    TInt RemovePreviewL( 
        const CNcdNodeIdentifier& aId, 
        const TDesC& aUri );
       
    
    /**
     * Opens a preview file if it exists
     *
     */   
    RFile PreviewL( 
        RFs& aFs,
        const CNcdNodeIdentifier& aId,
        const TDesC& aUri );
    
    
    /**
     * Checks that a preview file exists
     */
    TBool PreviewExists( 
        const CNcdNodeIdentifier& aId,
        const TDesC& aUri ) const;
       
    
    const TDesC& PreviewMimeType(
        const CNcdNodeIdentifier& aId,
        const TDesC& aUri ) const;
           
    /**
     * Removes all previews
     */
    void RemoveAllPreviewsL();     

    void SaveDataL();
    void LoadDataL();
    
public: // MNcdStorageDataItem


    /**
     * @see MNcdStorageDataItem::ExternalizeL()
     */
    void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL()
     */
    void InternalizeL( RReadStream& aStream );

     
private:

    CNcdPreviewManager( CNcdGeneralManager& aGeneralManager, TInt aMaxCount );
    void ConstructL();
    
    CNcdPreviewManager( const CNcdPreviewManager& );
    CNcdPreviewManager& operator=( const CNcdPreviewManager& );
    
    
    // Searches for the preview
    TInt FindPreview( const CNcdNodeIdentifier& aId, const TDesC& aUri ) const;
    
    // Returns the file storage for the client
    MNcdFileStorage& FileStorageL( const CNcdNodeIdentifier& aId );
    
protected:

    /** 
     * Utility class for holding preview related data
     */
    class  CNcdPreviewData : public CBase
        {      
    public:
    
        CNcdPreviewData();
        static CNcdPreviewData* NewLC( 
            const CNcdNodeIdentifier& aId,
            const TDesC& aUri, 
            const TDesC& aPreviewFile,
            const TDesC& aMimeType );
            
        virtual ~CNcdPreviewData();
        
        TBool Equals( const CNcdNodeIdentifier& aId, const TDesC& aUri ) const;
        
        const CNcdNodeIdentifier& Id() const;
        const TDesC& Uri() const;
        const TDesC& Filename() const;
        const TDesC& MimeType() const;

        void ExternalizeL( RWriteStream& aStream );
        void InternalizeL( RReadStream& aStream );

    
    protected:
    
        void ConstructL( 
            const CNcdNodeIdentifier& aId,
            const TDesC& aUri, 
            const TDesC& aPreviewFile,
            const TDesC& aMimeType );
        
        CNcdPreviewData( const CNcdPreviewData& );
        CNcdPreviewData& operator=( const CNcdPreviewData& );
    
    private:
        CNcdNodeIdentifier* iId;
        HBufC* iUri;
        HBufC* iFilename;
        HBufC* iMimeType;
        };

    typedef RPointerArray<CNcdPreviewData> RNcdPreviewArray;

        
private:

    CNcdGeneralManager& iGeneralManager;
    MNcdStorageManager& iStorageManager;

    // Maximum previews per family
    TInt iMaxCount;
    
    // PreviewManager is family specific so we need only one dimensional
    // array for all the previews of a family
    RNcdPreviewArray iPreviews;
        
    };

#endif // C_NCDPREVIEWMANAGER_H

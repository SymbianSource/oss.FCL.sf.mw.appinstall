/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdPreviewManager class implementation
*
*/


#include "ncdpreviewmanager.h"

#include "ncdnodeidentifier.h"
#include "ncdstoragemanager.h"
#include "ncdstorage.h"
#include "ncdfilestorage.h"
#include "ncdproviderdefines.h"
#include "ncdpanics.h"
#include "catalogsutils.h"
#include "ncdstorage.h"
#include "ncdstorageitem.h"
#include "ncddatabasestorage.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== CNcdPreviewData ========


// ---------------------------------------------------------------------------
// Creators
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager::CNcdPreviewData* 
    CNcdPreviewManager::CNcdPreviewData::NewLC( 
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri, 
    const TDesC& aPreviewFile,
    const TDesC& aMimeType )
    {
    CNcdPreviewData* self = new( ELeave ) CNcdPreviewData;
    CleanupStack::PushL( self );
    self->ConstructL( aId, aUri, aPreviewFile, aMimeType );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager::CNcdPreviewData::~CNcdPreviewData()
    {
    DLTRACEIN((""));
    delete iId;
    delete iUri;
    delete iFilename;
    delete iMimeType;
    }


// ---------------------------------------------------------------------------
// Equals
// ---------------------------------------------------------------------------
//        
TBool CNcdPreviewManager::CNcdPreviewData::Equals( 
    const CNcdNodeIdentifier& aId, 
    const TDesC& aUri ) const
    {
    return aId.Equals( *iId ) && aUri.Compare( *iUri ) == 0;
    }
    

// ---------------------------------------------------------------------------
// Id getter
// ---------------------------------------------------------------------------
//        
const CNcdNodeIdentifier& CNcdPreviewManager::CNcdPreviewData::Id() const
    {
    DASSERT( iId );
    return *iId;
    }
    
    
// ---------------------------------------------------------------------------
// URI getter
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdPreviewManager::CNcdPreviewData::Uri() const
    {
    DASSERT( iUri );
    return *iUri;
    }
    
    
// ---------------------------------------------------------------------------
// Filename getter
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdPreviewManager::CNcdPreviewData::Filename() const
    {
    DASSERT( iFilename );
    return *iFilename;
    }


// ---------------------------------------------------------------------------
// Mime type getter
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdPreviewManager::CNcdPreviewData::MimeType() const
    {
    DASSERT( iMimeType );
    return *iMimeType;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager::CNcdPreviewData::CNcdPreviewData()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::CNcdPreviewData::ConstructL( 
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri, 
    const TDesC& aPreviewFile,
    const TDesC& aMimeType )
    {
    DLTRACEIN((""));
    iId = CNcdNodeIdentifier::NewL( aId );
    iUri = aUri.AllocL();
    iFilename = aPreviewFile.AllocL();
    iMimeType = aMimeType.AllocL();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::CNcdPreviewData::ExternalizeL( RWriteStream& aStream )
    {
    iId->ExternalizeL( aStream );
    ExternalizeDesL( *iUri, aStream );
    ExternalizeDesL( *iFilename, aStream );
    ExternalizeDesL( *iMimeType, aStream );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::CNcdPreviewData::InternalizeL( RReadStream& aStream )
    {
    iId = CNcdNodeIdentifier::NewL();
    iId->InternalizeL( aStream );
    InternalizeDesL( iUri, aStream );
    InternalizeDesL( iFilename, aStream );
    InternalizeDesL( iMimeType, aStream );
    }


// ======== PUBLIC MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager* CNcdPreviewManager::NewL( 
    CNcdGeneralManager& aGeneralManager, TInt aMaxCount )
    {
    CNcdPreviewManager* self = new( ELeave ) CNcdPreviewManager(
        aGeneralManager, aMaxCount );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager::~CNcdPreviewManager()
    {
    DLTRACEIN((""));
    // delete previews
    iPreviews.ResetAndDestroy();
    }
    

// ---------------------------------------------------------------------------
// Add preview
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::AddPreviewL( 
    const CNcdNodeIdentifier& aId, 
    const TDesC& aUri, 
    const TDesC& aPreviewFile,
    const TDesC& aMimeType )
    {
    DLTRACEIN((""));

    // Check if the client already has a matching preview
    TInt previewIndex = FindPreview( aId, aUri );
    if ( previewIndex != KErrNotFound ) 
        {   
        // Remove old preview         
        RemovePreviewL( aId, aUri );
        }
    
    DLTRACE(("Create preview data"));
    
    // If we already have maximum number of previews, remove the oldest
    while ( iPreviews.Count() >= iMaxCount ) 
        {
        DLTRACE(("Removing old preview"));
        CNcdPreviewData& data( *iPreviews[0] );
        RemovePreviewL( data.Id(), data.Uri() );
        }

    MNcdFileStorage& fileStorage = FileStorageL( aId );
    TParsePtrC target( aPreviewFile );
    
    // Create new preview data
    CNcdPreviewData* data = CNcdPreviewData::NewLC( aId, aUri, 
        target.NameAndExt(), aMimeType );
    
    DLTRACE(("Moving the file"));
    fileStorage.AddFileL( aPreviewFile, ETrue );
        
    DLTRACE(("Add preview data"));
    iPreviews.AppendL( data );
    CleanupStack::Pop( data );
    
    SaveDataL();    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
TInt CNcdPreviewManager::RemovePreviewL( 
    const CNcdNodeIdentifier& aId, 
    const TDesC& aUri )
    {
    DLTRACEIN((""));
    
    TInt previewIndex = FindPreview( aId, aUri );
    if ( previewIndex != KErrNotFound ) 
        {
        MNcdFileStorage& fileStorage = FileStorageL( aId );        
        TRAPD( err, fileStorage.RemoveFileL( 
            iPreviews[previewIndex]->Filename() ) );
        
        DLINFO(("File removal error code: %i", err ));
        
        // We don't want to delete previewdata if we couldn't
        // delete the file
        if ( err == KErrNone || err == KErrNotFound ) 
            {           
            delete iPreviews[ previewIndex ];
            iPreviews.Remove( previewIndex );
            SaveDataL();
            }
        return err;
        }
        
    return KErrNotFound;
    }
       
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
RFile CNcdPreviewManager::PreviewL( 
    RFs& aFs,
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri )
    {
    DLTRACEIN((""));
        
    TInt previewIndex = FindPreview( aId, aUri );
    if ( previewIndex == KErrNotFound ) 
        {
        User::Leave( KErrNotFound );
        }
    
    MNcdFileStorage& fileStorage = FileStorageL( aId );
        
    RFile file;

    DLTRACE(("Opening preview file"));
    TRAPD( trapError, file = 
          fileStorage.OpenFileL( aFs,
            iPreviews[ previewIndex ]->Filename(), 
            NcdProviderDefines::KNcdSharableFileOpenFlags ) );
            
    if( trapError != KErrNone )
        {
        TRAP_IGNORE( RemovePreviewL( aId, aUri ) );
        User::Leave( trapError );
        }
    
    return file;                                    
    }   
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
TBool CNcdPreviewManager::PreviewExists( 
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri ) const
    {
    DLTRACEIN((""));    
    return FindPreview( aId, aUri ) != KErrNotFound;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdPreviewManager::PreviewMimeType(
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri ) const
    {
    DLTRACEIN((""));
    TInt previewIndex = FindPreview( aId, aUri );
    if ( previewIndex != KErrNotFound ) 
        {            
        return iPreviews[ previewIndex ]->MimeType();
        }
    
    return KNullDesC();
    }
       
               
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::RemoveAllPreviewsL()
    {
    DLTRACEIN((""));

    iStorageManager.RemoveStorageL( 
        iGeneralManager.FamilyName(), 
        NcdProviderDefines::KPreviewStorageNamespace );
    
    iPreviews.ResetAndDestroy();

    SaveDataL();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    TInt previewCount = iPreviews.Count();
    aStream.WriteInt32L( previewCount );

    for( TInt i = 0; i < previewCount; i++ )
        {
        iPreviews[ i ]->ExternalizeL( aStream );
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iPreviews.ResetAndDestroy();
    
    TInt previewCount = aStream.ReadInt32L();
    iPreviews.ReserveL( previewCount );
    
    for( TInt i = 0; i < previewCount; i++ )
        {
        CNcdPreviewData* previewData = new( ELeave ) CNcdPreviewData;
        CleanupStack::PushL( previewData );
        previewData->InternalizeL( aStream );
        iPreviews.Append( previewData );
        CleanupStack::Pop( previewData );
        previewData = NULL;
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
CNcdPreviewManager::CNcdPreviewManager( 
    CNcdGeneralManager& aGeneralManager, TInt aMaxCount ) :
    iGeneralManager( aGeneralManager ),
    iStorageManager( aGeneralManager.StorageManager() ), 
    iMaxCount( aMaxCount )
    {
    NCD_ASSERT_ALWAYS( iMaxCount > 0, ENcdPanicInvalidArgument );
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Find preview
// ---------------------------------------------------------------------------
//        
TInt CNcdPreviewManager::FindPreview( 
    const CNcdNodeIdentifier& aId,
    const TDesC& aUri ) const
    {        
    TInt count = iPreviews.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iPreviews[ i ]->Equals( aId, aUri ) ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// FileStorageL
// ---------------------------------------------------------------------------
//        
MNcdFileStorage& CNcdPreviewManager::FileStorageL(
    const CNcdNodeIdentifier& aId )
    {
    DLTRACEIN(( _L("Namespace: %S"), &aId.NodeNameSpace() ));        
    
    MNcdStorage& storage = iStorageManager.CreateOrGetStorageL( 
        iGeneralManager.FamilyName(), 
        NcdProviderDefines::KPreviewStorageNamespace ); 
        
    // Get rid of invalid characters in the namespace since it'll be
    // used as a directory name
    HBufC* id = EncodeFilenameLC( 
        aId.NodeNameSpace(), 
        iStorageManager.FileSession() );
    
    MNcdFileStorage& fileStorage( storage.FileStorageL( *id ) );
    
    CleanupStack::PopAndDestroy( id );
    return fileStorage;        
    }

// ---------------------------------------------------------------------------
// SaveDataL
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::SaveDataL()
    {
    DLTRACEIN((""));
    
    MNcdDatabaseStorage& database =
        iStorageManager.ProviderStorageL( 
            iGeneralManager.FamilyName() ).DatabaseStorageL( 
                NcdProviderDefines::KDefaultDatabaseUid );
            
    MNcdStorageItem* item = 
        database.StorageItemL( KNullDesC(), 
            NcdProviderDefines::ENcdPreviewManager );        
    
    item->SetDataItem( this );
    item->OpenL();
    
    item->WriteDataL();
    
    item->SaveL();      
    DLTRACEOUT((""));      
    }


// ---------------------------------------------------------------------------
// Loads data
// ---------------------------------------------------------------------------
//        
void CNcdPreviewManager::LoadDataL()
    {
    DLTRACEIN((""));

    MNcdDatabaseStorage& database =
        iStorageManager.ProviderStorageL( 
            iGeneralManager.FamilyName() ).DatabaseStorageL( 
                NcdProviderDefines::KDefaultDatabaseUid );
            
    if( !database.ItemExistsInStorageL( KNullDesC(),
                                        NcdProviderDefines::ENcdPreviewManager ) )
        {
        return;
        }
    
    MNcdStorageItem* item = 
        database.StorageItemL( KNullDesC(), 
            NcdProviderDefines::ENcdPreviewManager );        
               
    item->SetDataItem( this );
    item->ReadDataL();    
    
    DLTRACEOUT((""));      
    }


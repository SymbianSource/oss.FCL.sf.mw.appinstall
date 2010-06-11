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
* Description:   Contains CNcdNodeActivate class implementation
*
*/


#include <bamdesca.h>

#include "ncdnodeactivateimpl.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodeinstallproxy.h"
#include "ncdnodeidentifier.h"
#include "catalogsinterfaceidentifier.h"
#include "ncddeviceinteractionfactory.h"
#include "ncddeviceservice.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdinstalledtheme.h"
#include "ncdinstalledfile.h"
#include "ncdinstalledcontent.h"


CNcdNodeActivate::CNcdNodeActivate( CNcdNodeMetadataProxy& aMetadata )
: CCatalogsInterfaceBase( &aMetadata ),
  iMetadata( aMetadata ) 
    {
    }


void CNcdNodeActivate::ConstructL()
    {
    // Register the interface
    MNcdNodeActivate* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeActivate::KInterfaceUid ) );
    }


CNcdNodeActivate* CNcdNodeActivate::NewL( CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeActivate* self = 
        CNcdNodeActivate::NewLC( aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeActivate* CNcdNodeActivate::NewLC( CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeActivate* self = 
        new( ELeave ) CNcdNodeActivate( aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeActivate::~CNcdNodeActivate()
    {
    DLTRACEIN(("this-ptr: %x", this));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeActivate::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    DLTRACEOUT(("this-ptr: %x", this));
    }


CNcdNodeMetadataProxy& CNcdNodeActivate::Metadata() const
    {
    return iMetadata;
    }


// MNcdNodeActivate functions

void CNcdNodeActivate::ActivateL()
    {
    DLTRACEIN((""));

    // Note that ownership is not transferred here.
    // So, do not add to the cleanup stack.
    // When the items have been installed, they may be activated.
    // Install proxy contains theme name and file information that
    // can be used to activate items.
    CNcdNodeInstallProxy* install( Metadata().Install() );

    if ( install != NULL && install->IsInstalledL() )
        {
        // Create the service that can handle activations
        MNcdDeviceService* service( 
            NcdDeviceInteractionFactory::CreateDeviceServiceLC() );
            
        if ( service == NULL )
            {
            DLERROR(("Could not create service for active interface"));
            
            // For debugging purposes
            DASSERT( EFalse );
            
            User::Leave( KErrNotFound );
            }
    
        // Now we have the install interface to use and to check if some
        // of its material could be activated. Activation may be done
        // after the item has been installed.


        // NOTICE: If you add or remove functionality here,
        // then remember to update the CNcdNodeProxy::InternalizeActivateL()
        // function accordingly. So, the MNcdNodeActivate interface can be
        // provided from the API in correct situations.

        RCatalogsArray<MNcdInstalledContent> content( install->InstalledContentL() );
        CleanupResetAndDestroyPushL( content );

        TBool setSomething = EFalse;
        for ( TInt i = 0; i < content.Count(); ++i )
            {
            MNcdInstalledTheme* theme = content[i]->QueryInterfaceLC<MNcdInstalledTheme>();
            if ( theme ) 
                {
                SetThemeL( *theme, *service );
                setSomething = ETrue;
                CleanupStack::PopAndDestroy( theme );
                break;
                }
                        
            MNcdInstalledFile* file = content[i]->QueryInterfaceLC<MNcdInstalledFile>();
            if ( file )
                {
                SetContentFileL( *file, *install, *service );
                setSomething = ETrue;
                CleanupStack::PopAndDestroy( file );
                break;
                }                       
            }

        CleanupStack::PopAndDestroy( &content );
        
        if ( !setSomething )
            {
            User::Leave( KErrNotFound );
            }
            

        CleanupStack::PopAndDestroy( service );
        }

    DLTRACEOUT((""));
    }


// Other functions

void CNcdNodeActivate::SetThemeL( MNcdInstalledTheme& aTheme, 
    MNcdDeviceService& aService )
    {
    DLTRACEIN((""));
    aService.SetAsThemeL( aTheme.Theme() );
    }
    
    
void CNcdNodeActivate::SetContentFileL( MNcdInstalledFile& aFile, 
    CNcdNodeInstallProxy& aInstall,
    MNcdDeviceService& aService )
    {
    DLTRACEIN((""));
    RBuf fileName;
    fileName.CreateL( KMaxFileName );
    CleanupClosePushL( fileName );

    // Get the file handle to the file that will be activated.    
    RFile file = aFile.OpenFileL();
    CleanupClosePushL( file );

    // Get the filename from the handle. 
    // The name will be used for the activation
    file.FullName( fileName );
    CleanupStack::PopAndDestroy( &file );                

    if ( aInstall.IsPurpose( ENcdItemPurposeRingtone ) )
        {                
        aService.SetAsRingingToneL( fileName );
        }
    else if( aInstall.IsPurpose( ENcdItemPurposeWallpaper ) )
        {                   
        aService.SetAsWallpaperL( fileName );        
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    CleanupStack::PopAndDestroy ( &fileName );        
    DLTRACEOUT(("All is well"));
    }
    


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
* Description:   MNcdProtocolDefaultObserverImpl implementation
*
*/


#include <s32mem.h>
#include <e32property.h>

#include "catalogsdebug.h"
#include "catalogsconstants.h"
#include "catalogscontext.h"
#include "ncd_cp_actionrequest.h"
#include "ncd_cp_clientconfiguration.h"
#include "ncd_cp_detail.h"
#include "ncd_cp_query.h"
#include "ncd_cp_serverdetails.h"
#include "ncd_pp_error.h"
#include "ncd_pp_datablock.h"
#include "ncd_pp_information.h"
#include "ncd_pp_subscription.h"
#include "ncdprotocoldefaultobserverimpl.h"
#include "ncdconfigurationmanager.h"
#include "ncd_cp_cookie.h"
#include "ncdserverdetails.h"
#include "ncdproviderdefines.h"
#include "ncdsubscriptionmanagerimpl.h"

// Action targets
_LIT( KNcdActionTargetMasterServerUri, "masterServerUri" );
_LIT( KNcdActionTargetSkin, "skin" );
_LIT( KNcdActionTargetClient, "client" );



CNcdProtocolDefaultObserver* CNcdProtocolDefaultObserver::NewL(
    MCatalogsContext& aContext,
    MNcdConfigurationManager& aConfigurationManager,
    CNcdSubscriptionManager& aSubscriptionManager,
    const TDesC& aServerUri )
    {
    CNcdProtocolDefaultObserver* self = new(ELeave) 
        CNcdProtocolDefaultObserver( aContext, 
            aConfigurationManager, aSubscriptionManager );
    CleanupStack::PushL( self );
    self->ConstructL( aServerUri );
    CleanupStack::Pop( self );
    return self;
    }

CNcdProtocolDefaultObserver::CNcdProtocolDefaultObserver( 
    MCatalogsContext& aContext,
    MNcdConfigurationManager& aConfigurationManager,
    CNcdSubscriptionManager& aSubscriptionManager )
    : iContext( aContext ),
      iConfigurationManager( aConfigurationManager ),
      iSubscriptionManager( aSubscriptionManager )
    {
    }

CNcdProtocolDefaultObserver::~CNcdProtocolDefaultObserver()
    {
    delete iServerUri;
    }


void CNcdProtocolDefaultObserver::ConstructL( const TDesC& aServerUri )
    {
    iServerUri = aServerUri.AllocL();
    }

//*****************************************************************************
// MNcdParserConfigurationProtocolObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::ConfigurationBeginL( const TDesC& /*aVersion*/, 
                                                       TInt /*aExpirationDelta*/ ) 
    { 
    DLTRACEIN((""));
    }
    
void CNcdProtocolDefaultObserver::ConfigurationQueryL( 
    MNcdConfigurationProtocolQuery* aQuery )
    {
    DLTRACEIN((""));
    // The queries by now should be handled by other interested parties.
    // Do nothing.
    delete aQuery;
    }
    
void CNcdProtocolDefaultObserver::ClientConfigurationL( 
    MNcdConfigurationProtocolClientConfiguration* aConfiguration ) 
    {
    DLTRACEIN((""));
    CleanupDeletePushL( aConfiguration );

    // Should interpret details to grab master server validity information, 
    // for example

    // pass cookies to cookie handler
    TInt cookieCount = aConfiguration->CookieCount();
    DLINFO(("cookie count=%d",cookieCount));
    
    // Get SSID
    const TDesC8& ssid( iConfigurationManager.SsidL( iContext ) );
    
    MNcdServerDetails& details( 
        iConfigurationManager.ServerDetailsL( iContext, 
        *iServerUri,
        NcdProviderDefines::KConfigNamespace ));

    
    for ( TInt i = 0; i < cookieCount; ++i ) 
        {
        const MNcdConfigurationProtocolCookie& cookie( 
            aConfiguration->CookieL( i ) );
        details.AddCookieL( cookie, ssid );
        }

    CleanupStack::PopAndDestroy( aConfiguration );
    DLTRACEOUT((""));
    }

void CNcdProtocolDefaultObserver::ConfigurationDetailsL( 
    CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails ) 
    {
    DLTRACEIN((""));    
    aDetails->ResetAndDestroy();
    delete aDetails;
    }

void CNcdProtocolDefaultObserver::ConfigurationActionRequestL( 
    MNcdConfigurationProtocolActionRequest* aActionRequest ) 
    {
    DLTRACEIN((""));

    CleanupDeletePushL( aActionRequest );
    
    if( aActionRequest->Type() == MNcdConfigurationProtocolActionRequest::ETypeUpdate &&
        aActionRequest->UpdateDetails() )
    	{
    	DLTRACE(( "Update" ));

    	DLINFO(( _L("Target: %S"), &aActionRequest->Target() ));
       	DLINFO(( _L("Id: %S"), &aActionRequest->UpdateDetails()->Id() ));
    	DLINFO(( _L("Version: %S"), &aActionRequest->UpdateDetails()->Version() ));
    	DLINFO(( _L("Uri: %S"), &aActionRequest->UpdateDetails()->Uri() ));
    	DLINFO(( "Forced: %d", aActionRequest->Force() ));

    	DLINFO(( "Client SID %08x", iContext.SecureId().iId ));    	
    	
    	if ( aActionRequest->Target() == KNcdActionTargetMasterServerUri && 
    	     aActionRequest->UpdateDetails()->Uri().Length() ) 
    	    {
    	    const TDesC& currentMaster( iConfigurationManager.MasterServerAddressL( 
    	        iContext ) );
    	        
    	    // Ensure that request was received from the master server
    	    if ( *iServerUri == currentMaster )
    	        {    	        
    	        /**
    	         * @ Improve this check to notice missing '/' at the end of the URIs
    	         */
    	        if ( aActionRequest->UpdateDetails()->Uri() != currentMaster )
    	            {    	            
            	    DLINFO(("Updating MasterServerUri")); 
            	       
            	    iConfigurationManager.SetMasterServerAddressL( iContext,
            	        aActionRequest->UpdateDetails()->Uri(), 0 );    	    
    	            }
    	        else
    	            {
    	            DLINFO(("MasterServerUri didn't change"));
    	            }
    	        }
    	    else
    	        {
    	        DLERROR(( _L("Invalid update request source! Request source: %S"),
    	            iServerUri ));
    	        }
    	    }
    	else if( aActionRequest->Target() == KNcdActionTargetSkin || 
    	         aActionRequest->Target() == KNcdActionTargetClient ) 
    	    {
        	HBufC* msgQueueName = HBufC::NewLC( KCatalogsUpdateQueueNameFormat().Length() + 8 );
        	msgQueueName->Des().Format( KCatalogsUpdateQueueNameFormat, iContext.SecureId().iId );
        	
        	DLINFO(( _L("Opening client's update message queue: %S"), msgQueueName ));
        	RMsgQueueBase queue;
        	
        	TInt err = queue.OpenGlobal( *msgQueueName );
        	if( err == KErrNone )
        	    {        	    
        	    CleanupClosePushL( queue );
                TInt msgSize = queue.MessageSize();

                // Prepare a buffer to write to update information P&S variable.
                CBufFlat* buffer = CBufFlat::NewL( KCatalogsUpdateInformationMaxSize + msgSize );
                CleanupStack::PushL( buffer );
                
                // Externalize the info to the buffer.
                RBufWriteStream stream( *buffer );
                CleanupClosePushL( stream );
                stream << (TInt32)(aActionRequest->Target().Size() + 
                                   aActionRequest->UpdateDetails()->Id().Size() + 
                                   aActionRequest->UpdateDetails()->Version().Size() +
                                   aActionRequest->UpdateDetails()->Uri().Size() +
                                   sizeof( TInt32 ));
                stream << aActionRequest->Target();
                stream << aActionRequest->UpdateDetails()->Id();
            	stream << aActionRequest->UpdateDetails()->Version();
                stream << aActionRequest->UpdateDetails()->Uri();
                stream << (TInt32)aActionRequest->Force();
                
                // Write the update information to the client's update message queue.
                TInt bytesToSend = buffer->Size();
                for( TInt sendPos = 0; sendPos < bytesToSend; sendPos += msgSize )
                    {
                    const TUint8* sendPtr = buffer->Ptr( sendPos ).Ptr();
                    queue.SendBlocking( sendPtr, msgSize );
                    }
                
                DLINFO(( "Update information sent to message queue" ));
                
                CleanupStack::PopAndDestroy( 3, &queue ); // stream-close, buffer, queue
        	    }
            
            // Clean up the local info buffer.
            CleanupStack::PopAndDestroy( msgQueueName ); 
    	    }
    	else
    	    {
    	    DLINFO(("Unknown Action target"));
    	    }
    	}
    // Action request no longer needed, delete it.
    CleanupStack::PopAndDestroy( aActionRequest );
    }


void CNcdProtocolDefaultObserver::ConfigurationServerDetailsL( 
    MNcdConfigurationProtocolServerDetails* aServerDetails )
    {
    DLTRACEIN((""));
    DASSERT( aServerDetails );
    DASSERT( iServerUri );
    DLINFO(("Server supports %d capabilities", 
        aServerDetails->CapabilityCount() ));
    
    CleanupDeletePushL( aServerDetails );
    
    if ( aServerDetails->CapabilityCount() )
        {
        DLTRACE(("Adding capabilities to server details"));
        MNcdServerDetails& details( 
            iConfigurationManager.ServerDetailsL( iContext, 
                *iServerUri,
                NcdProviderDefines::KConfigNamespace ));
        
        // Clear old caps so that we don't just keep on adding them.
        // AddCapabilityL does NOT check for duplicates
        details.ClearCapabilitiesL();

        for ( TInt i = 0; i < aServerDetails->CapabilityCount(); ++i )
            {
            details.AddCapabilityL( aServerDetails->CapabilityL( i ) );
            }
        iConfigurationManager.SaveConfigurationToDbL( iContext );
        }
    CleanupStack::PopAndDestroy( aServerDetails );
    }
    
void CNcdProtocolDefaultObserver::ConfigurationErrorL( 
    MNcdConfigurationProtocolError* aError ) 
    {
    DLTRACEIN((""));
    delete aError;
    }
    
void CNcdProtocolDefaultObserver::ConfigurationEndL() 
    {
    DLTRACEIN((""));
    }


//*****************************************************************************
// MNcdParserInformationObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::InformationL( 
    MNcdPreminetProtocolInformation* aData )
    {
    DLTRACEIN(( _L("Namespace: %S"), &aData->Namespace() ));
    CleanupDeletePushL( aData );
    

    TInt cookieCount = aData->CookieCount();
    DLINFO(("cookie count=%d",cookieCount));

    MNcdServerDetails& details( 
        iConfigurationManager.ServerDetailsL( iContext, *iServerUri,
            aData->Namespace() ));


    // Get SSID
    const TDesC8& ssid( iConfigurationManager.SsidL( iContext ) );
    
    for ( TInt i = 0; i < cookieCount; ++i ) 
        {
        const MNcdConfigurationProtocolCookie& cookie( aData->CookieL( i ) );
        details.AddCookieL( cookie, ssid );
        }
    
    
    // Handle server capabilities
    DLTRACE(("Handle server capabilities"));
    const MNcdConfigurationProtocolServerDetails* serverDetails = 
        aData->ServerDetails();
    
    DLTRACE(("ServerDetails-ptr: %x", serverDetails ));
        
    if ( serverDetails && serverDetails->CapabilityCount() )
        {
        // Clear old caps so that we don't just keep on adding them.
        // AddCapabilityL does NOT check for duplicates
        details.ClearCapabilitiesL();

        DLINFO(("Capabilities: %d", serverDetails->CapabilityCount() ));
        DLTRACE(("Adding capabilities to server details"));
        
        for ( TInt i = 0; i < serverDetails->CapabilityCount(); ++i )
            {
            details.AddCapabilityL( serverDetails->CapabilityL( i ) );
            }
        
        }
    iConfigurationManager.SaveConfigurationToDbL( iContext );
    CleanupStack::PopAndDestroy( aData );
    DLTRACEOUT(("Protocol information handled"));
    }

//*****************************************************************************
// MNcdParserDataBlocksObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::DataBlocksL( 
    CArrayPtr<MNcdPreminetProtocolDataBlock>* aData ) 
    {
    DLTRACEIN((""));
    aData->ResetAndDestroy();
    delete aData;
    }

//*****************************************************************************
// MNcdParserErrorObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN((""));
    delete aData;
    }


//*****************************************************************************
// MNcdParserQueryObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::QueryL( MNcdConfigurationProtocolQuery* aData )
    {
    DLTRACEIN((""));
    delete aData;
    }

//*****************************************************************************
// MNcdParserSubscriptionObserver
//*****************************************************************************

void CNcdProtocolDefaultObserver::ValidSubscriptionL(
    MNcdPreminetProtocolSubscription* aData ) 
    {
    DLTRACEIN((""));
    iSubscriptionManager.InternalizeSubscriptionL(
        iContext.FamilyId(), *iServerUri, *aData, &iContext, NULL );
    delete aData;
    }

void CNcdProtocolDefaultObserver::OldSubscriptionL(
    MNcdPreminetProtocolSubscription* aData ) 
    {
    DLTRACEIN((""));
    iSubscriptionManager.InternalizeSubscriptionL(
        iContext.FamilyId(), *iServerUri, *aData, &iContext, NULL );
    delete aData;
    }

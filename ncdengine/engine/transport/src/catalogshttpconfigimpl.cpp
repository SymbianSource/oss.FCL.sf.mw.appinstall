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


#include <e32cmn.h>
#include <s32strm.h>

#include "catalogshttpconfigimpl.h"
#include "catalogskeyvaluepair.h"
#include "catalogshttpobserver.h"
#include "catalogshttpheadersimpl.h"
#include "catalogshttpconfigobserver.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig* CCatalogsHttpConfig::NewL( 
    MCatalogsHttpConfigObserver* aObserver)
    {
    CCatalogsHttpConfig* self = new ( ELeave ) CCatalogsHttpConfig( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig* CCatalogsHttpConfig::NewL( 
    RReadStream& aStream,
    MCatalogsHttpConfigObserver* aObserver)
    {
    CCatalogsHttpConfig* self = new ( ELeave ) CCatalogsHttpConfig( 
        aObserver );
    CleanupStack::PushL( self );    
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig::~CCatalogsHttpConfig()
    {    
    delete iFilename;    
    delete iDirectory;
   
    // Delete headers
    delete iHeaders;   
    }
        
    
// ---------------------------------------------------------------------------
// Priority setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetPriority( TCatalogsTransportPriority aPriority )
    {
    if ( aPriority != iPriority ) 
        {        
        iPriority = aPriority;
        
        // Notify the operation of a priority change
        if ( iOwnObserver ) 
            {            
            iOwnObserver->HandleHttpConfigEvent( this, 
                ECatalogsHttpCfgPriorityChanged );
            }
        }
    }
    
        
// ---------------------------------------------------------------------------
// Priority getter
// ---------------------------------------------------------------------------
//	
TCatalogsTransportPriority CCatalogsHttpConfig::Priority() const
    {
    return iPriority;
    }
        
        
// ---------------------------------------------------------------------------
// Directory path setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetDirectoryL( const TDesC& aDirectory )
    {
    if( aDirectory.Length() )
        {
        HBufC* temp = aDirectory.AllocL();
        delete iDirectory;
        iDirectory = NULL;
        iDirectory = temp;
        }
    else 
        {
        delete iDirectory;
        iDirectory = NULL;
        }        
    }


// ---------------------------------------------------------------------------
// Directory path getter
// ---------------------------------------------------------------------------
//	
const TDesC& CCatalogsHttpConfig::Directory() const
    {
    if( iDirectory ) 
        {
        return *iDirectory;
        }
        
    return KNullDesC();
    }


// ---------------------------------------------------------------------------
// Filename setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetFilenameL( const TDesC& aFilename )
    {
    if( aFilename.Length() ) 
        {
        HBufC* temp = aFilename.AllocL();
        delete iFilename;
        iFilename = NULL;
        iFilename = temp;
        }
    else 
        {
        // aFilename is empty so we delete the old filename
        delete iFilename;
        iFilename = NULL;
        }
    }


// ---------------------------------------------------------------------------
// Filename getter
// ---------------------------------------------------------------------------
//	
const TDesC& CCatalogsHttpConfig::Filename() const
    {
    if( iFilename ) 
        {
        return *iFilename;
        }
    return KNullDesC();
    }


// ---------------------------------------------------------------------------
// Full path
// ---------------------------------------------------------------------------
//	
HBufC* CCatalogsHttpConfig::FullPathLC() const    
    {
    DLTRACEIN((""));
    if ( iDirectory == NULL
         || iDirectory->Length() == 0
         || iFilename == NULL
         || iFilename->Length() == 0 )
        {
        // Directory and filename should always be set.
        // NULL or empty values are not accepted.
        User::Leave( KErrPathNotFound );
        }
    
    HBufC* fullpath = HBufC::NewLC( iDirectory->Length() 
        + iFilename->Length() );
    TPtr ptr( fullpath->Des() );
    
    ptr.Append( *iDirectory );
    ptr.Append( *iFilename );
    DLTRACEOUT(( _L("Path: %S"), fullpath ));
    return fullpath;
    }


// ---------------------------------------------------------------------------
// Headers getter
// ---------------------------------------------------------------------------
//	
MCatalogsHttpHeaders& CCatalogsHttpConfig::RequestHeaders() const
    {
    return *iHeaders;
    }

        
// ---------------------------------------------------------------------------
// Accesspoint setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetConnectionMethod( 
    const TCatalogsConnectionMethod& aMethod )
    {
    iConnectionMethod = aMethod;
    }
        
        
// ---------------------------------------------------------------------------
// Access point getter
// ---------------------------------------------------------------------------
//	
const TCatalogsConnectionMethod& CCatalogsHttpConfig::ConnectionMethod() const
    {
    return iConnectionMethod;
    }
          

// ---------------------------------------------------------------------------
// Observer setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetObserver( MCatalogsHttpObserver* aObserver )
    {
    iObserver = aObserver;
    }
          

// ---------------------------------------------------------------------------
// Observer getter
// ---------------------------------------------------------------------------
//	
MCatalogsHttpObserver* CCatalogsHttpConfig::Observer() const
    {
    return iObserver;
    }


// ---------------------------------------------------------------------------
// Set HTTP method
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::SetHttpMethod( TCatalogsHttpMethod aMethod )
    {
    iHttpMethod = aMethod;    
    }


// ---------------------------------------------------------------------------
// Get HTTP method
// ---------------------------------------------------------------------------
//	
TCatalogsHttpMethod CCatalogsHttpConfig::HttpMethod() const
    {
    return iHttpMethod;
    }


// ---------------------------------------------------------------------------
// Externalize
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    aStream.WriteInt32L( iPriority );
    ExternalizeDesL( Directory(), aStream );
    ExternalizeDesL( Filename(), aStream );
    iHeaders->ExternalizeL( aStream );
    iConnectionMethod.ExternalizeL( aStream );    
    aStream.WriteInt32L( iHttpMethod );
    aStream.WriteUint32L( iOptions );
    }


// ---------------------------------------------------------------------------
// Internalize
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iPriority = static_cast<TCatalogsTransportPriority>( 
        aStream.ReadInt32L() );
    InternalizeDesL( iDirectory, aStream );
    InternalizeDesL( iFilename, aStream );
    
    delete iHeaders;
    iHeaders = NULL;
    
    iHeaders = CCatalogsHttpHeaders::NewL( aStream );
    iConnectionMethod.InternalizeL( aStream );
    iHttpMethod = static_cast<TCatalogsHttpMethod>( aStream.ReadInt32L() );
    iOptions = aStream.ReadUint32L();
    }


// ---------------------------------------------------------------------------
// Options getter
// ---------------------------------------------------------------------------
//  
TUint32 CCatalogsHttpConfig::Options() const
    {
    return iOptions;
    }


// ---------------------------------------------------------------------------
// Options setter
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpConfig::SetOptions( TUint32 aOptions )
    {
    iOptions = aOptions;
    }

    
// ---------------------------------------------------------------------------
// Cloner
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig* CCatalogsHttpConfig::CloneL() const
    {
    CCatalogsHttpConfig* copy = new( ELeave ) CCatalogsHttpConfig( *this );
    CleanupStack::PushL( copy );
    copy->ConstructL( *this );
    CleanupStack::Pop( copy );
    return copy;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig::CCatalogsHttpConfig( MCatalogsHttpConfigObserver* aObserver ) : 
    iOwnObserver( aObserver ), iPriority( ECatalogsPriorityMedium ), 
    iHttpMethod( ECatalogsHttpPost )    
    {
    }
        

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::ConstructL()
    {
    iHeaders = CCatalogsHttpHeaders::NewL();
    iFilename = KNullDesC().AllocL();
    iDirectory = KNullDesC().AllocL();
    }
        
        
// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConfig::CCatalogsHttpConfig( const CCatalogsHttpConfig& aOther ): 
    iOwnObserver( aOther.iOwnObserver ),
    iPriority( aOther.iPriority ), 
    iConnectionMethod( aOther.iConnectionMethod ), 
    iObserver( aOther.iObserver ), 
    iHttpMethod ( aOther.iHttpMethod ),
    iOptions( aOther.iOptions )
    {
    }
        
        
// ---------------------------------------------------------------------------
// 2nd phase of copy construction
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpConfig::ConstructL( const CCatalogsHttpConfig& aOther )
    {
    iFilename = aOther.Filename().AllocL();        
    iDirectory = aOther.Directory().AllocL();

    // Copy headers
    iHeaders = aOther.iHeaders->CloneL();    
    }

/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of OMA DM adapter, allows to configure SW Installer
*
*/


#include <smldmadapter.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>
#include "cswinstadapter.h"

_LIT8( KIntType, "int" ); // Leaf inputs
_LIT8( KTextType, "text/plain" ); // Leaf inputs
_LIT8( KSWInstDMAdapterVersion, "1.0" ); // Adapter version
_LIT8( KSWInstRootNode, "SWInstaller" ); // root URI
_LIT8( KSWInstRootURI, "SWInstaller" );
_LIT8( KAllowUnsignedNode, "AllowUnsigned" ); // URI postfix
_LIT8( KAllowUnsignedURI, "SWInstaller/AllowUnsigned" );
_LIT8( KAllowUnsignedDescription, "This node tells whether unsigned \
    applications are allowed to be installed" ); // Description
static const TUint32 KSizeOfSettingId = 0x8; // Contanst size declaration


// ============================= LOCAL FUNCTIONS  =============================

#ifdef _DEBUG
#define DEBUG( x ) x
#else
#define DEBUG( x )
#endif


// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSWInstAdapter::NewL
// Symbian 1st phase contructor
// ----------------------------------------------------------------------------
//
CSWInstAdapter* CSWInstAdapter::NewL( MSmlDmCallback* aCallback )
    {
    CSWInstAdapter* self = NewLC( aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::NewLC
// Symbian 1st phase contructor. Push object to cleanup-stack
// ----------------------------------------------------------------------------
//
CSWInstAdapter* CSWInstAdapter::NewLC( MSmlDmCallback* aCallback )
    {
    CSWInstAdapter* self = new ( ELeave ) CSWInstAdapter( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::CSWInstAdapter()
// C++ Constructor
// ----------------------------------------------------------------------------
CSWInstAdapter::CSWInstAdapter( TAny* aEcomArguments )
    : CSmlDmAdapter::CSmlDmAdapter( aEcomArguments )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter Constructor" ) ) );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::ConstructL
// 2nd phase contructor
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::ConstructL()
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::ConstructL" ) ) );
    iSettingsCenRep = CRepository::NewL( KCRUidSWInstallerSettings );
    iLocalVarCenRep = CRepository::NewL( KCRUidSWInstallerLV );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::~CSWInstAdapter()
// C++ Destructor
// ----------------------------------------------------------------------------
//
CSWInstAdapter::~CSWInstAdapter()
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter Destructor" ) ) );
    delete iSettingsCenRep;
    delete iLocalVarCenRep;
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::DDFVersionL
// Return DM plug-in version
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    // Insert version information
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::DDFVersionL" ) ) );
    aDDFVersion.InsertL( 0, KSWInstDMAdapterVersion );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::DDFStructureL
// Return DM plug-in structure
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    // Declare accesses
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::DDFStructureL" ) ) );
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();
    
    // Create root node 
    MSmlDmDDFObject& root = aDDF.AddChildObjectL( KSWInstRootNode );
    FillNodeInfoL( root,
                   accessTypes,
                   MSmlDmDDFObject::EPermanent,
                   MSmlDmDDFObject::ENode,
                   MSmlDmDDFObject::EOne,
                   KNullDesC8(),
                   KTextType );
                   
    accessTypes.SetReplace();

    MSmlDmDDFObject& allowUnsignedNode = root.AddChildObjectL( KAllowUnsignedNode );
    FillNodeInfoL( allowUnsignedNode,
                   accessTypes,
                   MSmlDmDDFObject::EPermanent,
                   MSmlDmDDFObject::EChr,
                   MSmlDmDDFObject::EOne,
                   KAllowUnsignedDescription,
                   KTextType );

    DEBUG( RDebug::Print( _L( "CSWInstAdapter::DDFStructureL done" ) ) );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::StreamingSupport
// Return streaming support status, set supported item size
// ----------------------------------------------------------------------------
//
TBool CSWInstAdapter::StreamingSupport( TInt& /* aItemSize */ )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::StreamingSupport" ) ) );
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::StreamCommittedL
// Commit stream buffer
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::StreamCommittedL()
    {        
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::StreamCommitted" ) ) );
    // Intentionally left empty 
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::CompleteOutstandingCmdsL
// Complete outstanding commands
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::CompleteOutstandingCmdsL()
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::CompleteOutstandingCmdsL" ) ) );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::FillNodeInfoL
// Fill node info
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::FillNodeInfoL( MSmlDmDDFObject& aDDFObject, 
                                    TSmlDmAccessTypes& aAccessTypes, 
                                    MSmlDmDDFObject::TScope aScope,
                                    MSmlDmDDFObject::TDFFormat aFormat, 
                                    MSmlDmDDFObject::TOccurence aOccurence,
                                    const TDesC8& aDescription,
                                    const TDesC8& aMIMEType )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FillNodeInfoL" ) ) );
    aDDFObject.SetAccessTypesL( aAccessTypes );
    aDDFObject.SetScopeL( aScope );
    aDDFObject.SetOccurenceL( aOccurence );
    aDDFObject.SetDFFormatL( aFormat );
    aDDFObject.SetDescriptionL( aDescription );
    if ( aFormat != MSmlDmDDFObject::ENode )
        {
        aDDFObject.AddDFTypeMimeTypeL( aMIMEType );
        }
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FillNodeInfoL done" ) ) );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::CopyCommandL
// Copy object
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                               const TDesC8& /*aTargetLUID*/,
                               const TDesC8& /*aSourceURI*/, 
                               const TDesC8& /*aSourceLUID*/,
                               const TDesC8& /*aType*/, 
                               TInt aStatusRef )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::CopyCommandL" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ----------------------------------------------------------------------------
// DeleteObjectL
// Delete object
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::DeleteObjectL( const TDesC8& /* aURI */, 
                                    const TDesC8& /* aLUID */,
                                    TInt aStatusRef )

    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::DeleteObjectL" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::FetchLeafObjectL
// Fetch leaf
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                   const TDesC8& /* aLUID */,
                                   const TDesC8& aType, 
                                   TInt aResultsRef,
                                   TInt aStatusRef )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FetchLeafObjectL" ) ) );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    if ( aURI.Compare( KAllowUnsignedURI ) == 0 )
        {
        TInt cenRepValue;
        TInt ret = iLocalVarCenRep->Get( KSWInstallerHideUntrustedIns, cenRepValue );
        DEBUG( RDebug::Print( _L("CSWInstAdapter: CenRep ret = %d, val = %d" ), ret, !cenRepValue ) );
        if ( ret == KErrNone )
            {
            TBuf8<KSizeOfSettingId> buf;
            buf.AppendNum( !cenRepValue );
            CBufBase* bufBase = CBufFlat::NewL( 1 );
            CleanupStack::PushL( bufBase );
            bufBase->InsertL( 0, buf );
            Callback().SetResultsL( aResultsRef, *bufBase, aType );
            CleanupStack::PopAndDestroy( bufBase );
            }
        else
            {
            retValue = CSmlDmAdapter::EError;          
            }
        }
    else
        {
        retValue = CSmlDmAdapter::EError;
        }
    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FetchLeafObjectL done" ) ) );
    }    
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::FetchLeafObjectSizeL
// Calculate leaf object size
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                           const TDesC8& /* aLUID */,
                                           const TDesC8& /* aType */, 
                                           TInt aResultsRef,
                                           TInt aStatusRef )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FetchLeafObjectSizeL" ) ) );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    if ( aURI.Compare( KAllowUnsignedURI ) == 0 )
        {
        TBuf8<KSizeOfSettingId> buf;
        CBufBase* bufBase = CBufFlat::NewL( KSizeOfSettingId );
        CleanupStack::PushL( bufBase );
        buf.AppendNum( sizeof(TInt) );
        bufBase->InsertL( 0, buf );
        Callback().SetResultsL( aResultsRef, *bufBase, KIntType );
        CleanupStack::PopAndDestroy( bufBase );
        }
    else
        {
        retValue = CSmlDmAdapter::ENotFound;
        }
    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::FetchLeafObjectSizeL done" ) ) );
    }    

// ----------------------------------------------------------------------------
// CSWInstAdapter::ChildURIListL
// Create child URI list
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::ChildURIListL( const TDesC8& aURI, 
                                const TDesC8& /* aLUID */,
                                const CArrayFix<TSmlDmMappingInfo>& /* aPreviousURISegmentList */,
                                TInt aResultsRef, 
                                TInt aStatusRef )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::ChildURIListL" ) ) );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    
    if ( aURI.Compare( KSWInstRootURI ) == 0)
        {
        CBufBase* buf = CBufFlat::NewL( 1 );
        CleanupStack::PushL( buf );
        buf->InsertL( 0, KAllowUnsignedNode );
        Callback().SetResultsL( aResultsRef, *buf, KNullDesC8 );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        retValue = CSmlDmAdapter::ENotFound;
        }

    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::ChildURIListL done" ) ) );
    }    

// ----------------------------------------------------------------------------
// CSWInstAdapter::AddNodeObjectL
// Add node
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::AddNodeObjectL( const TDesC8& /* aURI */,
                                     const TDesC8& /* aParentLUID */,
                                     TInt aStatusRef )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::AddNodeObjectL" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }    

// ----------------------------------------------------------------------------
// CSWInstAdapter::ExecuteCommandL
// Execute command
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                      const TDesC8& /* aLUID */,
                                      const TDesC8& /* aArgument */, 
                                      const TDesC8& /* aType */,
                                      TInt aStatusRef )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::ExecuteCommandL" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }    

// ----------------------------------------------------------------------------
// CSWInstAdapter::ExecuteCommandL
// Execute command, streaming enabled
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                      const TDesC8& /* aLUID */,
                                      RWriteStream*& /* aStream */,
                                      const TDesC8& /* aType */,
                                      TInt aStatusRef )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::ExecuteCommandL (streaming)" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }    
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::UpdateLeafObjectL
// Update leaf object
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                        const TDesC8& /* aLUID */,
                                        const TDesC8& aObject,
                                        const TDesC8& /* aType */,
                                        TInt aStatusRef )
    {
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::UpdateLeafObjectL" ) ) );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    if ( aURI.Compare( KAllowUnsignedURI ) == 0 )
        {
        TLex8 lex( aObject );
        TInt value;
        TInt ret;
        if ( lex.Val( value ) != KErrNone )
            {
            DEBUG( RDebug::Print( _L( "CSWInstAdapter: lex.Val failed" ) ) );
            retValue = CSmlDmAdapter::EInvalidObject;
            }
        else
            {
            TInt allow = !!value; // make sure it's 0 or 1
            TInt hide  = !value;
            ret = iSettingsCenRep->Set( KSWInstallerAllowUntrusted, allow );
            DEBUG( RDebug::Print( _L( "CSWInstAdapter: KSWInstallerAllowUntrusted set to %d, ret %d" ), allow, ret ) );
            ret |= iLocalVarCenRep->Set( KSWInstallerHideUntrustedIns, hide );
            DEBUG( RDebug::Print( _L( "CSWInstAdapter: KSWInstallerHideUntrustedIns set to %d, ret %d" ), hide, ret ) );
            if ( ret != KErrNone )
                {
                retValue = CSmlDmAdapter::EError;
                }
            }
        }
    else
        {
        retValue = CSmlDmAdapter::ENotFound;
        }
    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::UpdateLeafObjectL done" ) ) );
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::UpdateLeafObjectL
// Update leaf object, streaming enabled
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                    const TDesC8& /* aLUID */,
                                    RWriteStream*& /* aStream */, 
                                    const TDesC8& /* aType */,
                                    TInt aStatusRef
                                  )
    {
    // Not supported
    DEBUG( RDebug::Print( _L( "CSWInstAdapter::UpdateLeafObjectL (streaming)" ) ) );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::StartAtomicL
// Start atomic
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::StartAtomicL()
    {
    }
    
// ----------------------------------------------------------------------------
// CSWInstAdapter::CommitAtomicL
// Commit atomic commands
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::CommitAtomicL()
    {
    }

// ----------------------------------------------------------------------------
// CSWInstAdapter::RollbackAtomicL
// Lose all modifications after 'StartAtomicL' command
// ----------------------------------------------------------------------------
//
void CSWInstAdapter::RollbackAtomicL()
    {
    }    
    
// End of File  

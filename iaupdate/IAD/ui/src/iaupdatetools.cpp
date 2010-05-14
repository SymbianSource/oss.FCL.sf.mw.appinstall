/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of IAUpdateTools functions
*
*/






#include <s32mem.h>
#include <iaupdateparameters.h>

#include "iaupdatetools.h"
#include "iaupdatedebug.h"

// -----------------------------------------------------------------------------
// IAUpdateTools::ExternalizeParametersL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateTools::ExternalizeParametersL( HBufC8*& aTarget,
                                            const CIAUpdateParameters& aSource )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateTools::ExternalizeParametersL begin");
    // Notice!
    // If you make changes here. Also, remember to make corresponding
    // changes to the InternalizeParametersL

    // The size of the buffer will contain:
    // - 1 TInt for UIDs,
    // - 1 TBool for ShowProgress boolean
    // - 1 TUint for importance
    // - 1 TUint for type
    // - 1 TBool for Refresh boolean
    
    // - 3 TInt for descriptor lengths
    // --> bytes for 5 + 3 = 8 TInt values are needed 
    //     (sizeof( TInt ) * KNumberOfTInts)
    // - 3 descriptor TDesC texts
    const TInt KNumberOfTInts( 8 );

    // Notice, that because descriptor texts that are included
    // here are TDesC, their Size-information has to be used
    // instead of Length.
    HBufC8* tmp = 
        HBufC8::NewLC( sizeof( TInt ) * KNumberOfTInts
                       + aSource.SearchCriteria().Size()
                       + aSource.CommandLineExecutable().Size()
                       + aSource.CommandLineArguments().Size() );

    // Create stream that uses the buffer created above.
    // By using the stream, the values are easier to insert into the buffer.
    TPtr8 ptr( tmp->Des() );    
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );

    // These are integers for uids
    IAUPDATE_TRACE_1("[IAUPDATE] uid: %x", aSource.Uid().iUid );
    stream.WriteInt32L( aSource.Uid().iUid );

    // These are length and data pairs for descriptors
    IAUPDATE_TRACE_1("[IAUPDATE] search criteria: %S", &aSource.SearchCriteria() );
    ExternalizeDesL( aSource.SearchCriteria(), stream );
    IAUPDATE_TRACE_1("[IAUPDATE] command line executable: %S", &aSource.CommandLineExecutable() );
    ExternalizeDesL( aSource.CommandLineExecutable(), stream );
#ifdef _DEBUG
    HBufC* temp = HBufC::NewL( aSource.CommandLineArguments().Length() );
    temp->Des().Copy( aSource.CommandLineArguments()) ;
    IAUPDATE_TRACE_1("[IAUPDATE] command line arguments: %S", temp );
    delete temp;
#endif //_DEBUG
    ExternalizeDes8L( aSource.CommandLineArguments(), stream );
    
    // Boolean for ShowProgress
    stream.WriteInt32L( aSource.ShowProgress() );
    
    // TUint for importance
    stream.WriteInt32L( aSource.Importance() );
    
    // TUint for type
    stream.WriteInt32L( aSource.Type() );
    
    // Boolean for Refresh
    stream.WriteInt32L( aSource.Refresh() );

    // Stream is not needed anymore
    CleanupStack::PopAndDestroy( &stream );

    // Remove tmp from the stack but do not delete it because it will be
    // assigned to the aTarget below.
    CleanupStack::Pop( tmp );
    
    // Replace aTarget with new value
    delete aTarget;
    aTarget = tmp;
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateTools::ExternalizeParametersL end");
    }

// -----------------------------------------------------------------------------
// IAUpdateTools::InternalizeParametersL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateTools::InternalizeParametersL( CIAUpdateParameters& aTarget, 
                                            const TDesC8& aSource )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateTools::InternalizeParametersL begin");
    // Notice!
    // If you make changes here. Also, remember to make corresponding
    // changes to the ExternalizeParametersL

    // InternalizeDesL function will set values into this tmp descriptor.
    HBufC* tmp( NULL );
    
    // Create stream that uses the given buffer.
    RDesReadStream stream( aSource );
    CleanupClosePushL( stream );

    aTarget.SetUid( TUid::Uid( stream.ReadInt32L() ) );
    IAUPDATE_TRACE_1("[IAUPDATE] uid: %x", aTarget.Uid().iUid );
    
    InternalizeDesL( tmp, stream );
    CleanupStack::PushL( tmp );
    aTarget.SetSearchCriteriaL( *tmp );
    IAUPDATE_TRACE_1("[IAUPDATE] search criteria: %S", tmp );
    CleanupStack::PopAndDestroy( tmp );
    tmp = NULL;
        
    InternalizeDesL( tmp, stream );
    CleanupStack::PushL( tmp );
    aTarget.SetCommandLineExecutableL( *tmp );
    IAUPDATE_TRACE_1("[IAUPDATE] command line executable: %S", tmp );
    CleanupStack::PopAndDestroy( tmp );
    tmp = NULL;

    HBufC8* tmp8( NULL );        
    InternalizeDes8L( tmp8, stream );
    CleanupStack::PushL( tmp8 );
    aTarget.SetCommandLineArgumentsL( *tmp8 );
#ifdef _DEBUG
    tmp = HBufC::NewL( tmp8->Length() );
    tmp->Des().Copy( *tmp8 ) ;
    IAUPDATE_TRACE_1("[IAUPDATE] command line arguments: %S", tmp );
    delete tmp;
    tmp = NULL;
#endif //_DEBUG
    CleanupStack::PopAndDestroy( tmp8 );
    tmp8 = NULL;
    
    aTarget.SetShowProgress( stream.ReadInt32L() );

    // new parameters (in phase 2 IAD) do not always exist. Parameters may have written by old IAD in selfupdate case.
    TRAPD( err, aTarget.SetImportance( stream.ReadInt32L() ) );
    if ( err != KErrEof )
        {
        User::LeaveIfError( err );
        aTarget.SetType( stream.ReadInt32L() );
        aTarget.SetRefresh( stream.ReadInt32L() );
        }
     
    IAUPDATE_TRACE_1("[IAUPDATE] show progress: %d", aTarget.ShowProgress() );
    CleanupStack::PopAndDestroy( &stream );   
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateTools::InternalizeParametersL end");
    }


// -----------------------------------------------------------------------------
// IAUpdateTools::ExternalizeDesL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateTools::ExternalizeDesL( const TDesC& aDes, 
                                     RWriteStream& aStream )
    {
    aStream.WriteInt32L( aDes.Length() );
    aStream.WriteL( aDes );
    }

// -----------------------------------------------------------------------------
// IAUpdateTools::InternalizeDesL
// 
// -----------------------------------------------------------------------------
// 
TInt IAUpdateTools::InternalizeDesL( HBufC*& aDes, 
                                     RReadStream& aStream )
    {
    TInt length = aStream.ReadInt32L();
    if ( length > 0 ) 
        {
        HBufC* target = HBufC::NewLC( length );
        TPtr ptr( target->Des() );        
        aStream.ReadL( ptr, length );
        delete aDes;
        aDes = target;
        CleanupStack::Pop( target );
        }
    else
        {
        delete aDes;
        aDes = KNullDesC().AllocL();
        }
    return length;
    }

// -----------------------------------------------------------------------------
// IAUpdateTools::ExternalizeDes8L
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateTools::ExternalizeDes8L( const TDesC8& aDes, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aDes.Length() );
    aStream.WriteL( aDes );
    }

// -----------------------------------------------------------------------------
// IAUpdateTools::InternalizeDes8L
// 
// -----------------------------------------------------------------------------
// 
TInt IAUpdateTools::InternalizeDes8L( HBufC8*& aDes, RReadStream& aStream )
    {
    TInt length = aStream.ReadInt32L();
    if ( length > 0 ) 
        {
        HBufC8* target = HBufC8::NewLC( length );
        TPtr8 ptr( target->Des() );        
        aStream.ReadL( ptr, length );
        delete aDes;
        aDes = target;
        CleanupStack::Pop( target );
        }
    else
        {
        delete aDes;
        aDes = KNullDesC8().AllocL();
        }
    return length;
    }

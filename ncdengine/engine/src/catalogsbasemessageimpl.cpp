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
* Description:   Implementation of CatalogsBaseMessageImpl
*
*/


#include <e32std.h>
#include <f32file.h>

#include "catalogsbasemessageimpl.h"
#include "catalogsclientserverserversession.h"
#include "catalogsserverdefines.h"
#include "catalogsdebug.h"


const TInt KInputSlot = 2;
const TInt KOutputSlot = 3;


CCatalogsBaseMessageImpl::CCatalogsBaseMessageImpl(
    const RMessage2& aMessage,
    CCatalogsClientServerServerSession& aSession,
    TInt aHandle ) :
    iMessage( aMessage ),
    iSession( aSession ),
    iHandle( aHandle ),
    iSenderLost( EFalse )
    {
    }

void CCatalogsBaseMessageImpl::ConstructL()
    {
    DLTRACEIN(( "this: %x", this ));
    }


CCatalogsBaseMessageImpl* CCatalogsBaseMessageImpl::NewL( 
        const RMessage2& aMessage,
        CCatalogsClientServerServerSession& aSession,
        TInt aHandle )
    {
    CCatalogsBaseMessageImpl* self = 
        CCatalogsBaseMessageImpl::NewLC( aMessage, aSession, aHandle );
    CleanupStack::Pop( self );
    return self;
    }


CCatalogsBaseMessageImpl* CCatalogsBaseMessageImpl::NewLC( 
        const RMessage2& aMessage,
        CCatalogsClientServerServerSession& aSession,
        TInt aHandle )
    {
    CCatalogsBaseMessageImpl* self = 
        new( ELeave ) CCatalogsBaseMessageImpl( aMessage, aSession, aHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsBaseMessageImpl::~CCatalogsBaseMessageImpl()
    {
    }

TInt CCatalogsBaseMessageImpl::InputLength() const
    {
    return iMessage.GetDesLength( KInputSlot );
    }

TInt CCatalogsBaseMessageImpl::ReadInput( TDes8& aInputData ) const
    {
	return iMessage.Read( KInputSlot, aInputData );
    }

TInt CCatalogsBaseMessageImpl::ReadInput( TDes16& aInputData ) const
    {
	return iMessage.Read( KInputSlot, aInputData );
    }    

TInt CCatalogsBaseMessageImpl::Handle() const
    {
    return iHandle;
    }

void CCatalogsBaseMessageImpl::CompleteAndRelease( TInt aStatus ) const
    {
    DLTRACEIN(("TInt, handle: %d, this: %x", Handle(), this ));
    if ( iSenderLost )
        {
        DLINFO(("Sender was lost, deleting message"));
        delete this;
        DLTRACEOUT((""));
        return;
        }
    iMessage.Complete( aStatus );
    delete this;
    DLTRACEOUT((""));
    }

void CCatalogsBaseMessageImpl::CompleteAndReleaseL(
    const TDesC8& aOutputData,
    TInt aStatus ) 
    {
    DLTRACEIN(("TDesC8, TInt, handle: %d, this: %x", Handle(), this ));
    if ( iSenderLost )
        {
        DLINFO(("Sender was lost, deleting message"));
        delete this;
        DLTRACEOUT((""));
        return;
        }
    
    TInt clientDescLength( iMessage.GetDesMaxLengthL( KOutputSlot ) );

    if ( clientDescLength < aOutputData.Length() )
        {
        if( iMessage.Function() == ECatalogsExternalAllocMessage )
            {
            TInt incompMsgHandle( iSession.WriteToLargerDesL( aOutputData,
                                                              aStatus ) );
                                                              
            TBuf8<KCatalogsMinimumAllocSize> internalMsg(
                                               KCatalogsMinimumAllocLength );
            
            internalMsg.Num( incompMsgHandle );
            internalMsg.Append( KCatalogsTooSmallDescMsgElementDivider );
            internalMsg.AppendNum( aOutputData.Length() );

            TRAPD( error, iMessage.WriteL( KOutputSlot, internalMsg ) );
            if ( error != KErrNone )
                {
                iSession.RemoveIncompleteMessage( incompMsgHandle );
                User::Leave( error );
                }
            }
        iMessage.Complete( KCatalogsErrorTooSmallDescriptor );
        }
    else
        {
        iMessage.WriteL( KOutputSlot, aOutputData );
        iMessage.Complete( aStatus );
        }
        
    delete this;
    DLTRACEOUT((""));
    }

void CCatalogsBaseMessageImpl::CompleteAndReleaseL(
    TInt aOutputData,
    TInt aStatus )
    {
    DLTRACEIN(("TInt, TInt,handle: %d, this: %x", Handle(), this));
    if ( iSenderLost )
        {
        DLINFO(("Sender was lost, deleting message"));
        delete this;
        DLTRACEOUT((""));
        return;
        }

    TPckgBuf<TInt> handleBuf( aOutputData );
    iMessage.WriteL( KOutputSlot, handleBuf );
    iMessage.Complete( aStatus );        
    delete this;
    DLTRACEOUT((""));
    }

void CCatalogsBaseMessageImpl::CompleteAndReleaseL(
    const TDesC16& aOutputData,
    TInt aStatus ) 
    {
    DLTRACEIN(("TDesC16, TInt, handle: %d, this: %x", Handle(), this));
    if ( iSenderLost )
        {
        DLINFO(("Sender was lost, deleting message"));
        delete this;
        DLTRACEOUT((""));
        return;
        }

    TInt clientDescLength( iMessage.GetDesMaxLengthL( 3 ) );

    if ( clientDescLength < aOutputData.Length() )
        {
        if( iMessage.Function() == ECatalogsExternalAllocMessage )
            {
            TInt incompMsgHandle( iSession.WriteToLargerDesL( aOutputData,
                                                              aStatus ) );
                                                              
            TBuf16<KCatalogsMinimumAllocSize> internalMsg(
                                               KCatalogsMinimumAllocLength );
            
            internalMsg.Num( incompMsgHandle );
            internalMsg.Append( KCatalogsTooSmallDescMsgElementDivider );
            internalMsg.AppendNum( aOutputData.Length() );

            TRAPD( error, iMessage.WriteL( KOutputSlot, internalMsg ) );
            if ( error != KErrNone )
                {
                iSession.RemoveIncompleteMessage( incompMsgHandle );
                User::Leave( error );
                }
            }

        iMessage.Complete( KCatalogsErrorTooSmallDescriptor );
        }
    else
        {
        iMessage.WriteL( KOutputSlot, aOutputData );
        iMessage.Complete( aStatus );
        }
        
    delete this;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Completes the message
// ---------------------------------------------------------------------------
//	
void CCatalogsBaseMessageImpl::CompleteAndReleaseL( RFs& /*aFs*/, 
                                                    RFile& aFile )
    {
    DLTRACEIN(("RFs, RFile,handle: %d, this: %x", Handle(), this));
    if ( iSenderLost )
        {
        DLINFO(("Sender was lost, deleting message"));
        delete this;
        DLTRACEOUT((""));
        return;
        }

    //DLTRACE(("Sharing the file session"));
    //User::LeaveIfError( aFs.ShareProtected() );

    DLTRACE(("TransferToClient, completes the message if successful"));
    aFile.TransferToClient( iMessage, KOutputSlot );    
    DLTRACE(("Transferred"));
    // The message should be complete
    /**
     * How to handle errors?
     */
    DASSERT( iMessage.IsNull() );
    
    delete this;
    DLTRACEOUT((""));
    
    }

    
void CCatalogsBaseMessageImpl::Release()
    {
    DLTRACEIN(( "this: %x", this ));
    delete this;
    }

TBool CCatalogsBaseMessageImpl::CheckSecurityPolicy( const TSecurityPolicy& aSecurityPolicy )
    {
    return aSecurityPolicy.CheckPolicy( iMessage );
    }
    
MCatalogsSession& CCatalogsBaseMessageImpl::Session() const
    {
    return iSession;
    }

TBool CCatalogsBaseMessageImpl::CounterPartLost( 
    const MCatalogsSession& aSession )
    {
    DLTRACEIN((""));
    // We compare the pointers
    if ( &iSession == &aSession )
        {
        DLINFO(("Sender was lost"));
        iSenderLost = ETrue;
        return ETrue;
        }
    return EFalse;
    }

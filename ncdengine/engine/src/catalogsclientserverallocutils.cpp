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


#include "catalogsclientserverallocutils.h"

#include "catalogsserverdefines.h"

#include "catalogsutils.h"
#include "catalogsdebug.h"


TInt AllocReturnBuf( TInt aExpectedLength, HBufC8*& aAllocatedBuf )
    {        
    TInt allocationLength = aExpectedLength;

    // Minimum length required for the message to enable internal
    // messaging in the case of too small descriptor
    if ( allocationLength < KCatalogsMinimumAllocLength )
        {
        allocationLength = KCatalogsMinimumAllocLength;
        }
    
    HBufC8* tempReturnBuf( HBufC8::New( allocationLength ) );
    if ( !tempReturnBuf )
        {
        return KErrNoMemory;        
        }
    aAllocatedBuf = tempReturnBuf;
    return KErrNone;
    }

TInt AllocReturnBuf( TInt aExpectedLength, HBufC16*& aAllocatedBuf )
    {    
    TInt allocationLength = aExpectedLength;

    // Minimum length required for the message to enable internal
    // messaging in the case of too small descriptor
    if ( allocationLength < KCatalogsMinimumAllocLength )
        {
        allocationLength = KCatalogsMinimumAllocLength;
        }
    
    HBufC16* tempReturnBuf( HBufC16::New( allocationLength ) );
    if ( !tempReturnBuf )
        {
        return KErrNoMemory;        
        }
    aAllocatedBuf = tempReturnBuf;
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// Realloc could be converted to creation of a new buffer and
// deletion of old because the copying of the old contents
// is not needed.
// ---------------------------------------------------------------------------
// 
TInt RetrieveNewDescLengthAndReAlloc( HBufC8*& aTempReturnBuf,
                                      TInt& aIncompleteMessageHandle )
    {   
    TInt incompleteMessageHandle( -1 );
    TInt newDescriptorMaxLength( -1 );
    TInt error( 0 );                
    error = InterpretNewAllocInfo( *aTempReturnBuf,
                                   incompleteMessageHandle,
                                   newDescriptorMaxLength );
    DLTRACE(("new maxlength: %d", newDescriptorMaxLength ));                                   
    if ( error != KErrNone )
        {
        // We should never end up here because it means
        // that our internal messaging has done something wrong
            
        // Cannot delete server side incomplete message because
        // handle possible invalid.
        aIncompleteMessageHandle = -1;
        return error;           
        }
    else if ( newDescriptorMaxLength
              <= KCatalogsMinimumAllocLength )
        {
        // This should also not be able to happen
        aIncompleteMessageHandle = incompleteMessageHandle;
        return KErrGeneral;
        }
        
    aIncompleteMessageHandle = incompleteMessageHandle;

    
    HBufC8* temp = aTempReturnBuf->ReAlloc( newDescriptorMaxLength );
    
    if ( !temp ) 
        {
        return KErrNoMemory;
        }
    
    aTempReturnBuf = temp;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Realloc could be converted to creation of a new buffer and
// deletion of old because the copying of the old contents
// is not needed.
// ---------------------------------------------------------------------------
// 
TInt RetrieveNewDescLengthAndReAlloc( HBufC16*& aTempReturnBuf,
                                      TInt& aIncompleteMessageHandle )
    {
    TInt incompleteMessageHandle( -1 );
    TInt newDescriptorMaxLength( -1 );
    TInt error( 0 );                
    error = InterpretNewAllocInfo( *aTempReturnBuf,
                                   incompleteMessageHandle,
                                   newDescriptorMaxLength );
    if ( error != KErrNone )
        {
        // We should never end up here because it means
        // that our internal messaging has done something wrong
            
        // Cannot delete server side incomplete message because
        // handle possible invalid.
        aIncompleteMessageHandle = -1;
        return error;           
        }
    else if ( newDescriptorMaxLength
              <= KCatalogsMinimumAllocLength )
        {
        // This should also not be able to happen
        aIncompleteMessageHandle = incompleteMessageHandle;
        return KErrGeneral;
        }
        
    aIncompleteMessageHandle = incompleteMessageHandle;

    HBufC16* temp = aTempReturnBuf->ReAlloc( newDescriptorMaxLength );
    
    if ( !temp ) 
        {
        return KErrNoMemory;
        }
    
    aTempReturnBuf = temp;

    return KErrNone;    
    }
    
  
        
    
// ---------------------------------------------------------------------------
// At the moment this function expects aTempReturnBuf to contain
// elements delimited with KCatalogsTooSmallDescMsgElementDivider.
// <incomplete message's handle><KCatalogsTooSmallDescMsgElementDivider><new length>
// ---------------------------------------------------------------------------
//    
TInt InterpretNewAllocInfo( const HBufC8& aTempReturnBuf,
                            TInt& aIncompleteMessageHandle,
                            TInt& aNewDescriptorMaxLength )
    {
    
    TInt messageLength = aTempReturnBuf.Length();
    if ( messageLength < 3 )
        {
        // Too small descriptor to contain valid information for us.
        return KErrGeneral;
        }
        
    TInt dividerIndex( aTempReturnBuf.Locate( 
                        KCatalogsTooSmallDescMsgElementDivider ) );
    if ( dividerIndex == KErrNotFound )
        {
        return KErrGeneral;
        }
        
    // Message's handle is the string from beginning of the descriptor
    // to the previous char before the divider. (length of this is the same
    // as index of the divider)
    TPtrC8 descHandle = aTempReturnBuf.Left( dividerIndex );

    TInt intHandle = 0;
    TInt error = DesDecToInt( descHandle, intHandle );
    if ( error != KErrNone )
        {
        return error;
        }
    
    
    TInt lengthFromRight = messageLength - ( dividerIndex + 1 );
    TPtrC8 descNewLength = aTempReturnBuf.Right( lengthFromRight );    

    TInt intNewLength = 0;
    error = DesDecToInt( descNewLength, intNewLength );
    if ( error != KErrNone )
        {
        return error;
        }    
    
    aIncompleteMessageHandle = intHandle;
    aNewDescriptorMaxLength = intNewLength; 
    
    return KErrNone;
    }
    
 // ---------------------------------------------------------------------------
// At the moment this function expects aTempReturnBuf to contain
// elements delimited with KCatalogsTooSmallDescMsgElementDivider.
// <incomplete message's handle><KCatalogsTooSmallDescMsgElementDivider><new length>
// ---------------------------------------------------------------------------
//    
TInt InterpretNewAllocInfo( const HBufC16& aTempReturnBuf,
                            TInt& aIncompleteMessageHandle,
                            TInt& aNewDescriptorMaxLength )
    {
    
    TInt messageLength = aTempReturnBuf.Length();
    if ( messageLength < 3 )
        {
        // Too small descriptor to contain valid information for us.
        return KErrGeneral;
        }
        
    TInt dividerIndex( aTempReturnBuf.Locate( 
                        KCatalogsTooSmallDescMsgElementDivider ) );
    if ( dividerIndex == KErrNotFound )
        {
        return KErrGeneral;
        }
        
    // Message's handle is the string from beginning of the descriptor
    // to the previous char before the divider. (length of this is the same
    // as index of the divider)
    TPtrC16 descHandle = aTempReturnBuf.Left( dividerIndex );

    TInt intHandle = 0;
    TInt error = DesDecToInt( descHandle, intHandle );
    if ( error != KErrNone )
        {
        return error;
        }
    
    
    TInt lengthFromRight = messageLength - ( dividerIndex + 1 );
    TPtrC16 descNewLength = aTempReturnBuf.Right( lengthFromRight );    

    TInt intNewLength = 0;
    error = DesDecToInt( descNewLength, intNewLength );
    if ( error != KErrNone )
        {
        return error;
        }    
    
    aIncompleteMessageHandle = intHandle;
    aNewDescriptorMaxLength = intNewLength; 
    
    return KErrNone;
    }

/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements TCatalogsHttpResponseComposer
*
*/


#include "catalogshttpresponsecomposer.h"

#include "catalogshttpincludes.h"
#include "catalogsutils.h"
#include "catalogskeyvaluepair.h"
#include "catalogshttpmessageconstants.h"

#include "catalogsdebug.h"



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* TCatalogsHttpResponseComposer::ComposeResponseL( 
    const MCatalogsHttpOperation& aOperation,
    const TDesC8& aBody ) const
    {
    DLTRACEIN((""));
    
    RCatalogsBufferWriter writer;
    writer.OpenLC();
        
    WriteStatusLineL( aOperation, writer() );
    
    WriteHeadersL( aOperation, writer() );
    writer().WriteL( aBody );
        
    HBufC8* result = writer.PtrL().AllocL();
    CleanupStack::PopAndDestroy( &writer );
    DLINFO(("result: %S", result ));
    return result;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TCatalogsHttpResponseComposer::WriteStatusLineL(   
    const MCatalogsHttpOperation& aOperation,
    RWriteStream& aStream ) const
    {
    // HTTP-version SP Status-Code SP Reason-Phrase CRLF
    aStream.WriteL( CatalogsHttpMessageConstants::KHttpVersion );
    aStream.WriteL( CatalogsHttpMessageConstants::KSpace );
    
    TBuf8<3> statusBuf;
    statusBuf.Num( aOperation.StatusCode() );
    aStream.WriteL( statusBuf );
    
    aStream.WriteL( CatalogsHttpMessageConstants::KSpace );
    aStream.WriteL( aOperation.StatusText() );
    aStream.WriteL( CatalogsHttpMessageConstants::KCrLf );    
    
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TCatalogsHttpResponseComposer::WriteHeadersL(   
    const MCatalogsHttpOperation& aOperation,
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    const RPointerArray<CCatalogsKeyValuePair>& headers( 
        aOperation.ResponseHeadersL().Headers() );
        
    DLTRACE(("Writing %d headers", headers.Count() ));
    for ( TInt i = 0; i < headers.Count(); ++i ) 
        {
        WriteHeaderL( *headers[i], aStream );
        }
    
    // End of headers
    aStream.WriteL( CatalogsHttpMessageConstants::KCrLf );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TCatalogsHttpResponseComposer::WriteHeaderL(   
    const CCatalogsKeyValuePair& aHeader,
    RWriteStream& aStream ) const
    {
    aStream.WriteL( aHeader.Key() );
    aStream.WriteL( CatalogsHttpMessageConstants::KColon );
    aStream.WriteL( CatalogsHttpMessageConstants::KSpace );
    aStream.WriteL( aHeader.Value() );
    aStream.WriteL( CatalogsHttpMessageConstants::KCrLf );
    }

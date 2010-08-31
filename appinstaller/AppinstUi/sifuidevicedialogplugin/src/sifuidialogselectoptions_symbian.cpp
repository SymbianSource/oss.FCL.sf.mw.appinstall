/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Symbian specific functions in optional components selection dialog
*
*/

#include <QList>
#include <QVariant>
#include <s32mem.h>                     // RDesWriteStream

const TInt KBufferGranularity = 4;


// ----------------------------------------------------------------------------
// ConvertOptionalComponentIndexesL()
// ----------------------------------------------------------------------------
//
QByteArray ConvertOptionalComponentIndexesL( const QList<QVariant> &aIndexes )
{
    // Get aIndexes into symbianArray
    RArray<TInt> symbianArray;
    CleanupClosePushL( symbianArray );
    QListIterator<QVariant> iter( aIndexes );
    while( iter.hasNext() ) {
        QVariant item = iter.next();
        int i;
        bool ok;
        i = item.toInt( &ok );
        if( ok ) {
            symbianArray.AppendL( i );
        }
    }

    // Write symbianArray to a package buffer
    CBufFlat* buffer = CBufFlat::NewL( KBufferGranularity );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    TPckg< const RArray<TInt> > indexesPckg( symbianArray );
    writeStream.WriteL( indexesPckg );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );

    // Create byte array (copies data) from the package buffer
    const TInt KFromTheBeginning = 0;
    TPtr8 ptr = buffer->Ptr(KFromTheBeginning);
    QByteArray byteArray( reinterpret_cast<const char*>( ptr.Ptr() ), ptr.Length() );
    CleanupStack::PopAndDestroy( buffer );

    CleanupStack::PopAndDestroy( &symbianArray );
    return byteArray;
}


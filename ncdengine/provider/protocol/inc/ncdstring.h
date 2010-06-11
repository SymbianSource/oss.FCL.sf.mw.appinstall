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
* Description:   CNcdString declaration
*
*/


#ifndef NCDSTRING_H
#define NCDSTRING_H

#include <e32base.h>
#include <s32strm.h>

class CNcdString : public CBase
    {
public:
    static CNcdString* NewL( const TDesC& aKey = KNullDesC,
                             const TDesC& aData = KNullDesC );
    static CNcdString* NewLC( const TDesC& aKey = KNullDesC,
                              const TDesC& aData = KNullDesC);
    static CNcdString* NewL( RReadStream& aReadStream );
    static CNcdString* NewLC( RReadStream& aReadStream );
    static CNcdString* NewL( const CNcdString& aString );
    static CNcdString* NewLC( const CNcdString& aString );
    ~CNcdString();
    
    void ExternalizeL( RWriteStream& aWriteStream ) const;
    void InternalizeL( RReadStream& aReadStream );

private:
    void ConstructL( const TDesC& aKey, const TDesC& aData );
    void ConstructL( RReadStream& aStream );
    void ConstructL( const CNcdString& aString );
    CNcdString();
public:
    const TDesC& Key() const;
    const TDesC& Data() const;
    void SetDataL( const TDesC8& aData );
    void SetDataL( const TDesC16& aData );
    void SetKeyL( const TDesC8& aData );
    void SetKeyL( const TDesC16& aData );
private:
    HBufC* iKey;
    HBufC* iData;
    };

#endif

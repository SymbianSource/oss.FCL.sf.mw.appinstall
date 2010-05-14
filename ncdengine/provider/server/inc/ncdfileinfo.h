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
* Description:   CNcdFileInfo declaration
*
*/


#ifndef C_NCDFILEINFO_H	
#define C_NCDFILEINFO_H	

#include <e32base.h>
#include <s32strm.h>

#include "ncdsendable.h"
#include "ncditempurpose.h"

/**
 * Utility class used for containing content file specific info
 */
class CNcdFileInfo : public CBase, public MNcdSendable
    {
public:

    static CNcdFileInfo* NewLC( 
        const TDesC& aFilePath,
        const TDesC& aMimeType, 
        TNcdItemPurpose aPurpose = ENcdItemPurposeUnknown );
    
    static CNcdFileInfo* NewLC( RReadStream& aStream );
    
    virtual ~CNcdFileInfo();

    const TDesC& FilePath() const;
    const TDesC& MimeType() const;
    
    void SetMimeTypeL( const TDesC& aMime );
    
    TNcdItemPurpose Purpose() const;

    void ExternalizeL( RWriteStream& aStream ) const;
    
    void ExternalizeWithoutFilenamesL( 
        RWriteStream& aStream ) const;
    
    void InternalizeL( RReadStream& aStream );

    /**
     * Set general data
     * Ownership is transferred
     */
    void SetDataL( HBufC8* aData );
    
    const TDesC8& Data() const;

protected:

    CNcdFileInfo( TNcdItemPurpose aPurpose );
    void ConstructL( const TDesC& aFilePath,
        const TDesC& aMimeType );
    
private:

    TNcdItemPurpose iPurpose;
    HBufC* iFilePath;
    HBufC* iMimeType;    
    HBufC8* iData;
    };

#endif // C_NCDFILEINFO_H	

/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdaterIdentifier
*
*/




#ifndef IA_UPDATER_IDENTIFIER_H
#define IA_UPDATER_IDENTIFIER_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>


/**
 *
 */
class CIAUpdaterIdentifier : public CBase
    {

public:

    IMPORT_C static CIAUpdaterIdentifier* NewL();

    IMPORT_C static CIAUpdaterIdentifier* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdaterIdentifier();


    IMPORT_C void Reset();


    IMPORT_C const TDesC& Id() const;
    IMPORT_C void SetIdL( const TDesC& aId );
    
    IMPORT_C const TDesC& Namespace() const;
    IMPORT_C void SetNamespaceL( const TDesC& aNamespace );

    IMPORT_C const TUid& Uid() const;
    IMPORT_C void SetUid( const TUid& aUid );
    IMPORT_C void SetUid( const TInt aUid );

    IMPORT_C void InternalizeL( RReadStream& aStream );

    IMPORT_C void ExternalizeL( RWriteStream& aStream );


private:

    // Prevent these if not implemented
    CIAUpdaterIdentifier( const CIAUpdaterIdentifier& aObject );
    CIAUpdaterIdentifier& operator =( const CIAUpdaterIdentifier& aObject );


    CIAUpdaterIdentifier();
    
    void ConstructL();


private: // data
    
    HBufC* iId;
    HBufC* iNamespace;
    TUid iUid;
    
    };
		
#endif // IA_UPDATER_IDENTIFIER_H


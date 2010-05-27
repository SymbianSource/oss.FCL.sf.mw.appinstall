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
* Description:   CIAUpdaterResult
*
*/




#ifndef IA_UPDATER_RESULT_H
#define IA_UPDATER_RESULT_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>

class CIAUpdaterIdentifier;

/**
 *
 */
class CIAUpdaterResult : public CBase
    {

public:

    IMPORT_C static CIAUpdaterResult* NewL();

    IMPORT_C static CIAUpdaterResult* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdaterResult();

    IMPORT_C CIAUpdaterIdentifier& Identifier();

    IMPORT_C TInt ErrorCode() const;
    IMPORT_C void SetErrorCode( const TInt aErrorCode );

    IMPORT_C TBool Hidden() const;
    IMPORT_C void SetHidden( TBool aHidden );
    

    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private:

    // Prevent these if not implemented
    CIAUpdaterResult( const CIAUpdaterResult& aObject );
    CIAUpdaterResult& operator =( const CIAUpdaterResult& aObject );


    CIAUpdaterResult();
    
    void ConstructL();


private: // data
    
    CIAUpdaterIdentifier* iIdentifier;
    TInt iErrorCode;
    TBool iHidden;
    
    };
		
#endif // IA_UPDATER_RESULT_H


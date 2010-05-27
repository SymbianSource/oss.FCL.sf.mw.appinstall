/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Header file of CIAUpdateFirstTimeDateFile class
*
*/






#ifndef IA_UPDATE_FIRSTTIMEDATE_FILE_H
#define IA_UPDATE_FIRSTTIMEDATE_FILE_H

#include <e32std.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32const.h>


/**
 *
 */
class CIAUpdateFirstTimeDateFile : public CBase
    {

public:

    static CIAUpdateFirstTimeDateFile* NewL( const TDesC& aFile );

    static CIAUpdateFirstTimeDateFile* NewLC( const TDesC& aFile );
    
    
    ~CIAUpdateFirstTimeDateFile();


    TBool ReadDataL();

    void WriteDataL();
    
    const TTime& FirstTime() const;
    
    void SetCurrentFirstTime();
     

private:

    // Prevent these if not implemented
    CIAUpdateFirstTimeDateFile( const CIAUpdateFirstTimeDateFile& aObject );
    CIAUpdateFirstTimeDateFile& operator =( const CIAUpdateFirstTimeDateFile& aObject );


    CIAUpdateFirstTimeDateFile();
    
    virtual void ConstructL( const TDesC& aFile );


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private: // data

	// file server session
	RFs iFsSession;
	
	// file path
	TFileName iPath;


    // Time when IAD started first time
    TTime iFirstTime;

    };
		
#endif // IA_UPDATE_FIRSTTIMEDATE_FILE_H


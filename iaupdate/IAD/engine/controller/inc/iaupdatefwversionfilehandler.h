/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef IAUPDATE_FWVERSION_FILEHANDLER
#define IAUPDATE_FWVERSION_FILEHANDLER


#include <e32std.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32const.h>

NONSHARABLE_CLASS( CIAUpdateFwVersionFileHandler ) : public CBase 
    {

public:

    static CIAUpdateFwVersionFileHandler* NewL();

    static CIAUpdateFwVersionFileHandler* NewLC();
    
    virtual ~CIAUpdateFwVersionFileHandler();
    
           
    HBufC* FwVersionL();
    
    void SetFwVersionL( const TDesC& aFwVersion );
    
    
private:

    CIAUpdateFwVersionFileHandler();
    
    // Prevent these if not implemented
    CIAUpdateFwVersionFileHandler( const CIAUpdateFwVersionFileHandler& aObject );
    CIAUpdateFwVersionFileHandler& operator =( const CIAUpdateFwVersionFileHandler& aObject );
    
    void ConstructL();

    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );

    void ReadControllerDataL();

    void WriteControllerDataL();

private: // data

    // file server session
    RFs iFsSession;
    
    // controller data file path
    TFileName iPath;
       
    HBufC* iFwVersion;
 
    };
        
#endif // IAUPDATE_FWVERSION_FILEHANDLER


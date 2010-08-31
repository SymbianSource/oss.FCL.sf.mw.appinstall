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
* Description:    
*
*/



#ifndef IAUPDATEBGINTERNAL_FILE_HANDLER
#define IAUPDATEBGINTERNAL_FILE_HANDLER


#include <e32std.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32const.h>
#include "iaupdatebgcheckermode.h"

class CIAUpdateBGInternalFileHandler : public CBase
    {

public:

    static CIAUpdateBGInternalFileHandler* NewL();

    static CIAUpdateBGInternalFileHandler* NewLC();
    
    
    virtual ~CIAUpdateBGInternalFileHandler();


    TBool ReadControllerDataL();

    void WriteControllerDataL();
    
    
    const TTime& LastTimeShowNewFeatureDialog() const;
    
    void SetLastTimeShowNewFeatureDialog( const TTime& aTime );

    void SetLastTimeShowNewFeatureDialog( TInt64 aTime );

    TBool UserRejectNewFeatureDialog() const;
    
    void SetUserRejectNewFeatureDialog( TBool aUserDecision );
    
    void SetMode( TIAUpdateBGMode aMode );
    
    TIAUpdateBGMode Mode();
    
    HBufC* FwVersion();
    
    TInt SetFwVersionL( const TDesC& aFwVersion );
    
    TInt SoftNotificationID();
    
    void SetSoftNotificationID( TInt aID );
    
    TInt RetryTimes();
    
    void SetRetryTimes( TInt aRetry );
    
    TInt NrOfIndicatorEntries();
    
    void  SetNrOfIndicatorEntries( TInt aEntries );
    
protected:

    CIAUpdateBGInternalFileHandler();
    
    void ConstructL();


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private:

    // Prevent these if not implemented
    CIAUpdateBGInternalFileHandler( const CIAUpdateBGInternalFileHandler& aObject );
    CIAUpdateBGInternalFileHandler& operator =( const CIAUpdateBGInternalFileHandler& aObject );


private: // data

    // file server session
    RFs iFsSession;
    
    // controller data file path
    TFileName iPath;

    // Time when the content was refreshed.
    TTime iLastTimeShowNewFeatureDialog;
    
    TBool iUserRejectNewFeatureDialog;
    
    TIAUpdateBGMode iMode;
    
    HBufC* iFwVersion;
      
    TInt iSNID;
    
    TInt iRetryTimes;
    
    TInt iNrOfIndicatiorEntries;
    
    };
        
#endif // IAUPDATEBGINTERNAL_FILE_HANDLER


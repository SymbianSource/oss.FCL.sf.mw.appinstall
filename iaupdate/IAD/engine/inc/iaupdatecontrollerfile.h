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
* Description:   Header file of CIAUpdateControllerFile class
*
*/






#ifndef IA_UPDATE_CONTROLLER_FILE_H
#define IA_UPDATE_CONTROLLER_FILE_H


#include <e32std.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32const.h>


/**
 *
 */
class CIAUpdateControllerFile : public CBase
    {

public:

    IMPORT_C static CIAUpdateControllerFile* NewL( const TDesC& aFile );

    IMPORT_C static CIAUpdateControllerFile* NewLC( const TDesC& aFile );
    
    
    IMPORT_C virtual ~CIAUpdateControllerFile();


    IMPORT_C TBool ReadControllerDataL();

    IMPORT_C void WriteControllerDataL();
    
    
    IMPORT_C void SetCurrentData();

    
    IMPORT_C const TTime& RefreshTime() const;
    
    IMPORT_C void SetRefreshTime( const TTime& aTime );

    IMPORT_C void SetRefreshTime( TInt64 aTime );

    IMPORT_C void SetCurrentRefreshTime();
    

    IMPORT_C TLanguage Language() const;
    
    IMPORT_C void SetLanguage( TLanguage aLanguage );

    IMPORT_C void SetCurrentLanguage();
        

private:

    // Prevent these if not implemented
    CIAUpdateControllerFile( const CIAUpdateControllerFile& aObject );
    CIAUpdateControllerFile& operator =( const CIAUpdateControllerFile& aObject );


    CIAUpdateControllerFile();
    
    virtual void ConstructL( const TDesC& aFile );


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private: // data

	// file server session
	RFs iFsSession;
	
	// controller data file path
	TFileName iPath;


    // Time when the content was refreshed.
    TTime iRefreshTime;

    // The language that has been used.
    TLanguage iLanguage;
    };
		
#endif // IA_UPDATE_CONTROLLER_FILE_H


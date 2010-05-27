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




#ifndef IAUPDATEBGCONTROLLER_FILE_H
#define IAUPDATEBGCONTROLLER_FILE_H


#include <e32std.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32const.h>


class CIAUpdateBGControllerFile : public CBase
    {

    public:

        static CIAUpdateBGControllerFile* NewL();

        static CIAUpdateBGControllerFile* NewLC();
        
        virtual ~CIAUpdateBGControllerFile();

        TBool ReadControllerDataL();

        void WriteControllerDataL();
    
        const TTime& RefreshTime() const;
    
        void SetRefreshTime( const TTime& aTime );

        void SetRefreshTime( TInt64 aTime );

        void SetCurrentRefreshTime();
    
        TLanguage Language() const;
    
        void SetLanguage( TLanguage aLanguage );
        

    protected:

        CIAUpdateBGControllerFile();
    
        void ConstructL();

        void InternalizeL( RReadStream& aStream );

        void ExternalizeL( RWriteStream& aStream );


    private:

        // Prevent these if not implemented
        CIAUpdateBGControllerFile( const CIAUpdateBGControllerFile& aObject );
        CIAUpdateBGControllerFile& operator =( const CIAUpdateBGControllerFile& aObject );


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
        
#endif // IAUPDATEBGCONTROLLER_FILE_H

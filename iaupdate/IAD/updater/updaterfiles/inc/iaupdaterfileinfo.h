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
* Description:   CIAUpdaterFileInfo
*
*/




#ifndef IA_UPDATER_FILE_INFO_H
#define IA_UPDATER_FILE_INFO_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>


/**
 *
 */
class CIAUpdaterFileInfo : public CBase
    {

public:

    IMPORT_C static CIAUpdaterFileInfo* NewL();

    IMPORT_C static CIAUpdaterFileInfo* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdaterFileInfo();


    IMPORT_C const TDesC& FilePath() const;
    IMPORT_C void SetFilePathL( const TDesC& aPath );


    IMPORT_C virtual void InternalizeL( RReadStream& aStream );

    IMPORT_C virtual void ExternalizeL( RWriteStream& aStream );

    
protected:

    CIAUpdaterFileInfo();
    
    virtual void ConstructL();


private:

    // Prevent these if not implemented
    CIAUpdaterFileInfo( const CIAUpdaterFileInfo& aObject );
    CIAUpdaterFileInfo& operator =( const CIAUpdaterFileInfo& aObject );


private: // data
    
    HBufC* iFilePath;
    
    };
		
#endif // IA_UPDATER_FILE_INFO_H


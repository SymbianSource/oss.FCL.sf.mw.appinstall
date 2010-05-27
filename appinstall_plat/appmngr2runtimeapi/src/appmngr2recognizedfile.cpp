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
* Description:   Class that contains file name and it's MIME type
*
*/


#include "appmngr2recognizedfile.h"     // CAppMngr2RecognizedFile 
#include <apmstd.h>                     // TDataType


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2RecognizedFile* CAppMngr2RecognizedFile::NewL(
        const TDesC& aFileName, const TDataType& aDataType )
    {
    CAppMngr2RecognizedFile* self = new (ELeave) CAppMngr2RecognizedFile;
    CleanupStack::PushL( self );
    self->iFileName = aFileName.AllocL();
    self->iDataType = new (ELeave) TDataType( aDataType );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2RecognizedFile* CAppMngr2RecognizedFile::NewL(
        HBufC* aFileName, HBufC* aMimeType )
    {
    CAppMngr2RecognizedFile* self = new (ELeave) CAppMngr2RecognizedFile;
    CleanupStack::PushL( self );
    
    TBuf8<KMaxDataTypeLength> dataType;
    dataType.Copy( *aMimeType );
    self->iDataType = new (ELeave) TDataType( dataType );
    
    // takes ownership of the parameters, may not leave after this
    self->iFileName = aFileName;
    delete aMimeType;
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::FileName()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CAppMngr2RecognizedFile::FileName()
    {
    return *iFileName;
    }

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::
// ---------------------------------------------------------------------------
//
EXPORT_C const TDataType& CAppMngr2RecognizedFile::DataType()
    {
    return *iDataType;
    }

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::~CAppMngr2RecognizedFile()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2RecognizedFile::~CAppMngr2RecognizedFile()
    {
    delete iFileName;
    delete iDataType;
    }

// ---------------------------------------------------------------------------
// CAppMngr2RecognizedFile::CAppMngr2RecognizedFile()
// ---------------------------------------------------------------------------
//
CAppMngr2RecognizedFile::CAppMngr2RecognizedFile()
    {
    }


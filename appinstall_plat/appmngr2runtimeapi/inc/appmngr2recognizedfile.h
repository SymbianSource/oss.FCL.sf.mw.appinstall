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


#ifndef C_APPMNGR2RECOGNIZEDFILE_H
#define C_APPMNGR2RECOGNIZEDFILE_H

#include <e32base.h>                    // CBase

class TDataType;

/**
 * File and it's MIME type.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2RecognizedFile : public CBase
    {
public:     // constructor and destructor
    /**
     * Creates new CAppMngr2RecognizedFile.
     * 
     * File name and data type are copied into the new CAppMngr2RecognizedFile object.
     * 
     * @param aFileName  File name
     * @param aDataType  Data (MIME) type
     * @returns new CAppMngr2RecognizedFile object
     */
    IMPORT_C static CAppMngr2RecognizedFile* NewL( const TDesC& aFileName,
            const TDataType& aDataType );
    
    /**
     * Creates new CAppMngr2RecognizedFile by taking ownership of given parameters.
     *
     * Note the construction order - the caller must take care of deleting the given
     * parameters if this function leaves.
     * 
     * @param aFileName  File name
     * @param aMimeType  MIME type
     * @returns new CAppMngr2RecognizedFile object
     */
    IMPORT_C static CAppMngr2RecognizedFile* NewL( HBufC* aFileName, HBufC* aMimeType );

    /**
     * Destructor.
     */
    IMPORT_C ~CAppMngr2RecognizedFile();

public:     // new functions
    /**
     * Returns file name.
     * @return const TDesC&  File name 
     */
    IMPORT_C const TDesC& FileName();

    /**
     * Returns data (MIME) type as TDataType.
     * @return const TDataType&  Data type.
     */
    IMPORT_C const TDataType& DataType();
    
private:    // new functions
    CAppMngr2RecognizedFile();
    
private:    // data
    HBufC* iFileName;
    TDataType* iDataType;
    };

#endif  // C_APPMNGR2RECOGNIZEDFILE_H


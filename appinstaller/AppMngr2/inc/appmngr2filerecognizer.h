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
* Description:   Recognizes files to get corresponding MIME types
*
*/


#ifndef C_APPMNGR2FILERECOGNIZER_H
#define C_APPMNGR2FILERECOGNIZER_H

#include <e32base.h>                    // CActive

class CAppMngr2RecognizedFile;
class CDir;
class RFs;


class CAppMngr2FileRecognizer : public CActive
    {
public:     // constructor and destructor
    static CAppMngr2FileRecognizer* NewL( RFs& aFs );
    ~CAppMngr2FileRecognizer();

public:     // new functions
    /**
     * Gets the data (MIME) type for files in a specified directory.
     * Works like RApaLsSession::RecognizeFilesL() except that can be used to
     * recognize files in private directories too (with AllFiles capability).
     * @param aPath  A valid path. Note that the path must end with a backslash. 
     * @param aStatus  A request status object. 
     */
    void RecognizeFilesL( const TDesC& aPath, TRequestStatus& aStatus );
    
    /**
     * Cancels ongoing file recognition.
     */
    void CancelRecognizeFiles();
    
    /**
     * Returns file recognition results in array. Items can be removed
     * from the array. Next call to RecognizeFilesL() resets the array.
     * @return RPointerArray<CAppMngr2RecognizedFile>&  Recognized files
     */
    RPointerArray<CAppMngr2RecognizedFile>& Results();
    
protected:  // from CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );
    
private:    // new functions
    CAppMngr2FileRecognizer( RFs& aFs );
    void RecognizeNextFileL();

private:    // data
    RFs& iFs;
    TRequestStatus* iDirStatus;
    HBufC* iDirPath;
    CDir* iDirEntries;
    TInt iIndex;
    RPointerArray<CAppMngr2RecognizedFile> iResultArray;
    enum TState
        {
        EIdle,
        ERecognizing
        } iState;
    };

#endif  // C_APPMNGR2FILERECOGNIZER_H


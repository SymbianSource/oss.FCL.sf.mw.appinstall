/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Component which handles reg file parsing asynchronously
*
*/

/**
 @file
 @internalTechnology
*/

#ifndef __APPREGEXTRACTOR_H__
#define __APPREGEXTRACTOR_H__
 
#include <e32base.h>
#include <f32file.h> 
#include "log.h"
#include <usif/scr/appregentries.h>
#include "sislauncherclient.h"
namespace Swi
{
class RSisHelper;

/**
 * This class is an active object, which manages file extraction in chunks, using SISHelper
 */
class CAppRegExtractor : public CActive
    {
public:
    static CAppRegExtractor* NewLC(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData );
    
    static CAppRegExtractor* NewL(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData );
    
    /**
     * Extract the file, using provided file handle which must be set up correctly.
     */ 
    void ExtractAppRegInfoSizeL(const TDesC& aFileName, TRequestStatus& aStatus);
    
    /// Get the error code for the latest async parsing operation.
    TInt GetErrorCode() const; 
 
    ~CAppRegExtractor();
    
private:
    CAppRegExtractor(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData ); 
    void ConstructL();
    
// from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    

    /// Sets up the file handle, owned by the file extractor for the duration of the extraction
    void SetupFileHandleL(const TDesC& aFileName);

    /// finishes the extraction and notifies the client of the result
    TInt FinishAppRegExtraction(TInt aResult);    

private:    
    RFs&    iFs;   
    TBool iCancelled;      
    TRequestStatus* iClientStatus;
    RSisLauncherSession iLauncher;
    RFile* iCurrentFile;
    TBool iManagedFileHandle; // whether we manage the file handle used for file extraction. 
    RPointerArray<Usif::CApplicationRegistrationData>& iApparcRegFileData;
    TInt iErrCode;
    };
    
}

#endif // #ifndef __FILEEXTRACTOR_H__

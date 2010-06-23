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
* Description:   Manages and watches one directory in CAppMngr2DirScanner 
*
*/


#ifndef C_APPMNGR2SCANNERDIR_H
#define C_APPMNGR2SCANNERDIR_H

#include <e32base.h>                    // CActive
#include <f32file.h>                    // RFs
#include "appmngr2scannerdirobserver.h" // MAppMngr2ScannerDirObserver


class CAppMngr2ScannerDir : public CActive
    {
public:     // constructor and destructor
    static CAppMngr2ScannerDir* NewL( RFs& aFs, const TDesC& aDir,
            MAppMngr2ScannerDirObserver& aObs );
    ~CAppMngr2ScannerDir();
    
public:     // new functions
    const TDesC& DirName() const;
    void StartWatchingChanges();
    void StopWatchingChanges();

protected:  // from CActive
    void DoCancel();
    void RunL();
    
private:    // new functions
    CAppMngr2ScannerDir( RFs& aFs, MAppMngr2ScannerDirObserver& aObs );
    void ConstructL( const TDesC& aDir );
    TPtrC ChopTrailingString( const TDesC& aString, const TDesC& aTrailing );

private:    // data
    MAppMngr2ScannerDirObserver& iObserver;
    RFs& iFs;
    HBufC* iDir;
    };

#endif  // C_APPMNGR2SCANNERDIR_H


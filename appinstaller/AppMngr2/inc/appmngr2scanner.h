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
* Description:   Directory scanner
*
*/


#ifndef C_APPMNGR2SCANNER_H
#define C_APPMNGR2SCANNER_H

#include <e32base.h>                    // CActive
#include <f32file.h>                    // RFs
#include "appmngr2scannerdirobserver.h" // MAppMngr2ScannerDirObserver

class CAppMngr2ScannerDir;
class MAppMngr2ScannerObserver;
class CAppMngr2FileRecognizer;


class CAppMngr2Scanner : public CActive, public MAppMngr2ScannerDirObserver
    {
public:     // constructor and destructor
    static CAppMngr2Scanner* NewL( MAppMngr2ScannerObserver& aObs );
    ~CAppMngr2Scanner();
    
public:     // from CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

public:     // from MAppMngr2ScannerDirObserver
    void DirectoryChangedL( const TDesC& aDirName );
    
public:     // new functions
    void AddDirectoryL( const TDesC& aPath );
    void StartScanningL();

private:    // new functions
    CAppMngr2Scanner( MAppMngr2ScannerObserver& aObs );
    void ConstructL();
    void NextValidScanningIndex();
    void HandleScanningResultsL();
    void StartMonitoringL();
    void GetPresentDrivesL( TDriveList& aDriveList );
    void HandleMonitoringEventL();

private:    // data
    MAppMngr2ScannerObserver& iObserver;
    RPointerArray<CAppMngr2ScannerDir> iDirs;
    CAppMngr2FileRecognizer* iRecognizer;
    RFs iFs;
    TInt iScanningIndex;
    enum TScannerState
        {
        EIdle,
        EScanning,
        } iState;
    };

#endif  // C_APPMNGR2SCANNER_H


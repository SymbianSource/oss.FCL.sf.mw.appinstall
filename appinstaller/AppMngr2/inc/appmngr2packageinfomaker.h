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
* Description:   Creates PackageInfo objects asynchronously
*
*/


#ifndef C_APPMNGR2PACKAGEINFOMAKER_H
#define C_APPMNGR2PACKAGEINFOMAKER_H

#include "appmngr2infomaker.h"          // CAppMngr2InfoMaker

class CAppMngr2Runtime;
class CAppMngr2PackageInfo;
class CAppMngr2RecognizedFile;


class CAppMngr2PackageInfoMaker : public CAppMngr2InfoMaker
    {
public:     // constructor and destructor
    static CAppMngr2PackageInfoMaker* NewL( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    static CAppMngr2PackageInfoMaker* NewLC( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    ~CAppMngr2PackageInfoMaker();
    
public:     // from CActive, via CAppMngr2InfoMaker
    void DoCancel();
    void RunL();

public:     // new functions
    void AddFileL( CAppMngr2RecognizedFile* aFile );    // takes ownership
    void StartGettingInstallationFilesL();

private:    // new functions
    CAppMngr2PackageInfoMaker( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    void ConstructL();

private:    // data
    RPointerArray<CAppMngr2RecognizedFile> iFiles;
    RPointerArray<CAppMngr2PackageInfo> iPackageInfos;
    };

#endif  // C_APPMNGR2PACKAGEINFOMAKER_H


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


#ifndef C_APPMNGR2SCANNEROBSERVER_H
#define C_APPMNGR2SCANNEROBSERVER_H

class CAppMngr2RecognizedFile;


class MAppMngr2ScannerObserver
    {
public:
    // ScanningResultL() passes ownership of items in aResult array
    virtual void ScanningResultL( RPointerArray<CAppMngr2RecognizedFile>& aResult ) = 0;
    virtual void ScanningComplete() = 0;
    virtual void DirectoryChangedL( const TDesC& aChangedDir ) = 0;
    };

#endif  // C_APPMNGR2SCANNEROBSERVER_H


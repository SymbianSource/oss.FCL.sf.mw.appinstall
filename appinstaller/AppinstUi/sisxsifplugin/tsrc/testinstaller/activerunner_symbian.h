/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test installer that uses Usif::RSoftwareInstall API.
*
*/

#ifndef ACTIVERUNNERPRIVATE_H
#define ACTIVERUNNERPRIVATE_H

#include <e32base.h>            // CActive
#include <usif/sif/sif.h>       // Usif::RSoftwareInstall
#include <SWInstApi.h>          // RSWInstLauncher

class ActiveRunner;
class QString;


class ActiveRunnerPrivate : public CActive
{
public:     // constructor and destructor
    ActiveRunnerPrivate( ActiveRunner *aRunner );
    ~ActiveRunnerPrivate();

public:     // new functions
    TInt Initialize( bool aUseSif );
    TInt Install( const QString& aFileName, bool aSilent, bool aOpenFile, bool aOcsp );
    TInt Remove( const Usif::TComponentId& aComponentId, bool aSilent );
    TInt Remove( const TUid& aUid, const TDesC8& aMime, bool aSilent );

protected:  // from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:    // new functions
    void DoInitializeL( bool aUseSif );
    void DoInstallL( const QString& aFileName, bool aSilent, bool aOpenFile, bool aOcsp );
    void DoRemoveL( const Usif::TComponentId& aComponentId, bool aSilent );
    void DoRemoveL( const TUid& aUid, const TDesC8& aMime, bool aSilent );

private:    // data
    ActiveRunner *q_ptr;
    RFs iFs;
    HBufC* iFileName;
    TBool iUseSif;
    Usif::COpaqueNamedParams *iArguments;
    Usif::COpaqueNamedParams *iResults;
    Usif::RSoftwareInstall iSoftwareInstall;
    SwiUI::RSWInstLauncher iSWInstLauncher;
};

#endif  // ACTIVERUNNERPRIVATE_H


/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines for IAUpdater program.
*
*/



#ifndef IA_UPDATER_DEFS_H
#define IA_UPDATER_DEFS_H

#include <e32base.h>

/**
 * IA Updater defines for general use.
 */
namespace IAUpdaterDefs
    {
    // Name for the process.
    _LIT( KIAUpdaterName,"IAUpdater"); 

    // Minor-Component name
    _LIT( KIAUpdaterInstaller,"IAUpdater installer"); 

    // Name of updater exe
    _LIT( KIAUpdaterExe, "iaupdater.exe" );

    // Resource file name
    _LIT( KIAUpdaterResourceFile, "iaupdater.rsc" );

    // These constant paths are used for init and result files.
    // Notice, that instead of getting the paths as a commandline parameters
    // when application is started, it is much safer to hard code them here.
    // Then, malicious software can not give their own filepaths for initialization.
    _LIT( KInitFilePath, "c:\\private\\2000f85a\\iaupdaterfilelist" );
    _LIT( KResultFilePath, "c:\\private\\2000f85a\\iaupdaterresults" );

    // Executable name for iaupdate. The Application to be started after self update.
    _LIT( KIAUpdateLauncherExe, "iaupdatelauncher.exe" );
    
    // Name of iaupdate exe
    _LIT( KIAUpdateExe, "iaupdate.exe" );
    
    // Executable name for background checker. The executable to be started after self update.
    _LIT( KIAUpdateBgCheckerExe, "iaupdatebg.exe" );
    
    
    const TUid KUidIABgChecker = { 0x200211f4 };

    // UID3 for iaupdate. 
    const TUint KIADUpdateUid3( 0x2000F85A );
    
    // UID3 for iaupdate launcher 
    const TUint KIADUpdateLauncherUid3( 0x2001FE2F );

    // The Installation drive for self update.
    const TText KIAUpdaterDrive( 'C' ); 

    const TUint KIAUpdaterParamLen( 16 ); 
    
    // If rendezvous completed with shutdown request IAD will exit immediately     
    const TInt KIAUpdaterShutdownRequest( 1 );

    #ifdef __WINS__
    const static TInt KInstallRetryWaitTime( 10000000 ); // for WINS
    #else
    const static TInt KInstallRetryWaitTime( 1000000 );  // 1 sec.
    #endif
    }

#endif // IA_UPDATER_DEFS_H

// EOF

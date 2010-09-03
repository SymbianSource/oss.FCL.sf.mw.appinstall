/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef __DAEMONBEHAVIOUR_H__
#define __DAEMONBEHAVIOUR_H__

#include <e32base.h>
#include <f32file.h>
#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>

#include "daemon.h"
#include "daemoninstaller.h"
#include "programstatus.h"   //For uninstaller
#include "swidaemonplugin.h" //For SWI plugin support
#include "sisregistrywritablesession.h"

class CSwiDaemonPlugin;

namespace Swi
{
/**
 * This class implements the specific behaviour when a media insertion
 * or removal is detected, or the daemon is started.
 */
class CDaemonBehaviour : public CBase, public MDaemonBehaviour, public MDaemonInstallBehaviour
    {
public:
    static CDaemonBehaviour* NewL( CProgramStatus& aMainStatus ); //For uninstaller
    static CDaemonBehaviour* NewLC( CProgramStatus& aMainStatus ); //For uninstaller
    ~CDaemonBehaviour();

    // from MDaemonBahviour
public: 
    /**
     * Function called on startup of the daemon.
     * This currently performs no opeartions.
     */
     TBool StartupL();

    /**
     * Function called on a media change.
     * If media is inserted, then this processes pre-installed files on
     * the card, and notifies IAR. On removal IAR is notified only.
     */
    void MediaChangeL(TInt aDrive, TChangeType aChangeType);
  
    // Uninstaller help functions
    /**
      * Returns current process status. Process status is used for communication
      *  between installer and uninstaller. 
      * @return TInt Process status.     
      */            
    TInt& GetProcessStatus();  
    
    /**
     * Change current process status. Program status is used for communication
     * between installer and uninstaller.
     *   
     * @param TInt Current program status.     
     */        
    void SetProcessStatus( TInt aStatus );  

     // Plug-in support help functions
     /**
      * Returns ETure is SWI Daemon plug-in is created.
      * 
      * @return TBool ETure if plug-in is created, EFalse if not plug-in.     
      */    
     TBool IsPlugin();
   
     /**
      * This function does check if plugin is all ready notified that media
      * processing is started. If not it calls MediaProcessingStart for plug-in.
      */
     void NotifyPlugin();
         
     /**
      * Reguest plug-in to install given package.
      *  
      * @param aDrive aSisFile Package file.     
      */
     void RequestPluginInstall( TDesC& aSisFile );
    
    // from MDaemonInstallBehaviour
public: 
     
    // See MDaemonInstallBehaviour::DoNotifyMediaProcessingComplete
    void DoNotifyMediaProcessingComplete();    
    
   
private:
    void ConstructL( CProgramStatus& aMainStatus );
  
    /**
     * Processes any pre-installed files present on the drive.
     *
     * @param aDrive The drive to check for pre-installed SISX files.
     */
    void ProcessPreinstalledFilesL(TInt aDrive);
  
    /**
     * Processes any pre-installed files in the given directory on the drive.
     @param aDrive The drive to check for pre-installed SISX files.
     @param aDirectory The directory to search for PreInstalled Files   
     */
    void ProcessPreinstalledFilesL(TInt aDrive, const TDesC& aDirectory);
    
    
    void UpdateComponentStatusL( TChangeType aChangeType, TInt aDrive );
    
    void UpdateStatusL( Usif::RSoftwareComponentRegistry& iScrServer,
                        Usif::TComponentId componentId,
                        TChangeType aChangeType,
                        TInt aTargetDrive,
                        RArray<TApaAppUpdateInfo>& iAppInfoArray );
    
private:
    
    RFs iFs;        
    // The sisx file processor
    CSisInstaller* iSisInstaller;
    // Plug-in instance if found by the ECOM.
    CSwiDaemonPlugin* iSwiDaemonPlugin;
    // Has MediaProcessingStart already been called?
    TBool iStartNotified;
    TInt iDrive;
    // Contains inserted drives.
    RArray<TInt> iDriveArray;   
    // General program status. Note this do not affect to plug-in.
    TInt iGeneralProcessStatus; //For uninstaller  
    
    Swi::RSisRegistryWritableSession iRegistrySession;
    TBool iRegSessionConnected;
    };

} // namespace Swi

#endif //__DAEMONBEHAVIOUR_H__

//EOF

/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of CNcdInstallationService
*
*/


#ifndef C_NCD_INSTALLATION_SERVICE_IMPL_H
#define C_NCD_INSTALLATION_SERVICE_IMPL_H

#include <e32base.h>
#include <AknsSrvClient.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <apgcli.h>
#include <centralrepository.h>
#include <usif/sif/sif.h>
#include <usif/scr/scr.h>

#include "ncdinstallationservice.h"
#include "ncditempurpose.h"
#include "ncdasyncoperationobserver.h"
#include "ncdasyncsilentinstallobserver.h"

#include "ncdwidgetregistrydata.h"

class MNcdInstallationServiceObserver;
class TCatalogsVersion;
class CNcdActiveOperationObserver;
class CNcdSilentInstallActiveObserver;


/**
 *
 */
class CNcdInstallationService : public CBase,
                                public MNcdInstallationService,
                                public MNcdAsyncOperationObserver,
                                public MNcdAsyncSilentInstallObserver
    {
public: // Construction & destruction
    
    /**
     * Constructor.
     * @return CNcdInstallationService* Installation service.
     */
    static CNcdInstallationService* NewL();
    
    /**
     * Constructor.
     * @return CNcdInstallationService* Installation service.
     */
    static CNcdInstallationService* NewLC();
    
    /**
     * Destructor.
     */
    ~CNcdInstallationService();
    
private: // Construction

    /**
     * Constructor.
     */
    CNcdInstallationService();
    
    /**
     * Constructor.
     */
    void ConstructL();
    
public: // From MNcdInstallationService

    /**
     * @see MNcdInstallationService::InstallL
     */
    void InstallL( const TDesC& aFileName,
                   const TDesC& aMimeType,
                   const TNcdItemPurpose& aPurpose );

    /**
     * @see MNcdInstallationService::InstallL
     */
    void InstallL( RFile& aFile,
                   const TDesC& aMimeType,
                   const TNcdItemPurpose& aPurpose );

    /**
     * @see MNcdInstallationService::InstallJavaL
     */
    void InstallJavaL( const TDesC& aFileName,
                       const TDesC& aMimeType,
                       const TDesC8& aDescriptorData );

    
    /**
     * @see MNcdInstallationService::InstallJavaL
     */
    void InstallJavaL( RFile& aFile,
                       const TDesC& aMimeType,
                       const TDesC8& aDescriptorData );
    
    /**
     * @see MNcdInstallationService::SilentInstallL
     */
    void SilentInstallL( RFile& aFile,
                         const TDesC& aMimeType,
                         const TNcdItemPurpose& aPurpose,
                         const Usif::COpaqueNamedParams* aInstallOptionsPckg );
    
    /**
     * @see MNcdInstallationService::SilentInstallJavaL
     */
    void SilentInstallJavaL( RFile& aFile,
                             const TDesC& aMimeType,
                             const TDesC8& aDescriptorData,
                             const Usif::COpaqueNamedParams* aInstallOptionsPckg );
    
    /**
     * @see MNcdInstallationService::SilentInstallWidgetJavaL
     */
    void SilentInstallWidgetL( RFile& aFile,
                               const Usif::COpaqueNamedParams* aInstallOptionsPckg );
    
    /**
     * @see MNcdInstallationService::MNcdCancelSilentInstall
     */
    void CancelSilentInstall( HBufC*& aFileName,
                              TUid& aAppUid,
                              TInt& aError );

                   
    /**
     * @see MNcdInstallationService::SetObserver
     */
    void SetObserver( MNcdInstallationServiceObserver& aObserver );

    /**
     * @see MNcdInstallationService::ApplicationVersion
     */
    TInt ApplicationVersion( const TUid& aUid, TCatalogsVersion& aVersion );

    /**
     * @see MNcdInstallationService::IsApplicationInstalledL
     */
    TBool IsApplicationInstalledL( const TUid& aUid );


    /**
     * @see MNcdInstallationService::IsApplicationInstalledL
     */
    TNcdApplicationStatus IsApplicationInstalledL( 
        const TUid& aUid, const TCatalogsVersion& aVersion );
    
    /**
     * @see MNcdInstallationService::IsWidgetInstalledL
     */
    TNcdApplicationStatus IsWidgetInstalledL(
        const TDesC& aIdentifier, const TCatalogsVersion& aVersion);
    
    /**
     * @see MNcdInstallationService::WidgetUidL
     */
    TUid WidgetUidL( const TDesC& aIdentifier);
    
    /**
     * @see MNcdInstallationService::SidFromSisRegistryL
     */
    TUid SidFromSisRegistryL( const TUid& aUid );

    /**
     * @see MNcdInstallationService::AppendRightsL
     */
    void AppendRightsL(
        const TDesC8& aRightsObject,
        const TDataType& aMimeType );


    /**
     * @see MNcdInstallationService::PackageUidFromSisL()
     */    
    TUid PackageUidFromSisL( RFile& aFile ) const;


    /**
     * @see MNcdInstallationService::IsThemeInstalled()
     */
    TBool IsThemeInstalledL( const TDesC& aThemeName );


    /**
     * @see MNcdInstallationService::DeleteFile()
     */
    TInt DeleteFile( const TDesC& aFilename );
    
    
    /**
     * @see MNcdInstallationService::FileServerSession()
     */
    RFs& FileServerSession(); 


    /**
     * @see MNcdInstallationService::WriteJadL()
     */
    HBufC* WriteJadL( const TDesC& aJarFileName, const TDesC8& aJad  );
   
    
public: // From MNcdAsyncOperationObserver


    /**
     * This callback function is called when the async operation
     * has been completed. So this will call NotifyObserverL then.
     * 
     * @see MNcdAsyncOperationObserver
     */
    virtual void AsyncOperationComplete( TInt aError );


public: // From MNcdAsyncSilentInstallObserver


    /**
     * This callback function is called when the silent installation
     * has been completed. So this will call NotifyObserverL then.
     * 
     * @see MNcdAsyncSilentInstallObserver
     */
    virtual void AsyncSilentInstallComplete( TInt aError );
    
        
public: // New methods

    
    // Used to notify the observer after application installation
    void NotifyObserverL(); 


    // Recognizes the data from the file itself
    HBufC* RecognizeDataL( const TDesC& aFileName );
        
    // Recognizes the data from the file itself
    HBufC* RecognizeDataL( RFile& aFile );

private: // From MAknServerAppExitObserver

    /**
     * This function is called after application installer has finished.
     * @see MApaServerAppExitObserver::HandleServerAppExit
     */
    void HandleServerAppExit( TInt aReason );

   
private: // new methods

    // Updates the list of installed themes
    void UpdateInstalledThemesL();
    
    // Returns the name of the theme that was installed last
    const TDesC& LatestThemeL();
    
    // Create a RApaLsSession
    void ConnectApaLsL();


    /**
     * @see MNcdInstallationService::InstallL
     * @param aSilentInstallOptionsPckg If NULL, then installation is done normally.
     * If not NULL, then the installation is done silently by using the given 
     * options. Ownership is not transferred.
     */
    void InstallL( RFile& aFile,
                   const TDesC& aMimeType,
                   const TNcdItemPurpose& aPurpose,
                   const Usif::COpaqueNamedParams* aSilentInstallOptions );

    /**
     * @see MNcdInstallationService::InstallJavaL
     * @param aSilentInstallOptionsPckg If NULL, then installation is done normally.
     * If not NULL, then the installation is done silently by using the given 
     * options. Ownership is not transferred.
     */
    void InstallJavaL( RFile& aFile,
                       const TDesC& aMimeType,
                       const TDesC8& aDescriptorData,
                       const Usif::COpaqueNamedParams* aSilentInstallOptions );

    
    void InstallWidgetL( 
        RFile& aFile,
        const Usif::COpaqueNamedParams* aSilentInstallOptions );
        

    // Sets correct values for member variables after installation has finished
    // or if it has been cancelled.
    void InstallationFinishedSetup( TInt aReason );


    // Used when the silent install is cancelled. This function
    // handles situations when the installation was finished even though
    // cancellation was requested. Reference parameters are used to pass
    // information about the installed application and about the possible
    // error.
    void HandleSilentInstallSuccessAfterCancelL( HBufC*& aFileName,
                                                 TUid& aAppUid,
                                                 TInt& aError );
    
   
    void SisRegistryEntryLC( 
        Swi::RSisRegistryEntry& aEntry, const TUid& aUid );

    TBool JavaAppExistsL( const TUid& aUid );

    /**
     * Initializes installer
     */
    void InitializeInstallerL();
    
    /**
     * Cancels normal SIS/JAR installation
     */
    void CancelInstall();
    
    
    /**
     * Creates a list of applications that are in ROM
     * 
     * @note ROM apps that have a stub-SIS are ignored
     */
    void InitializeRomApplicationListL();
    
    
    /**
     * Get a list of installed Midlet UIDS
     * 
     * @param aUids Emptry array where UIDs are added
     */
    void MidletUidsL( RArray<TUid>& aUids );
    
    
    /**
     * Checks if the UID is found in the ROM UID list
     * 
     * @param aUid 
     * @return ETrue if UID was found in the ROM UID list
     */
    TBool IsRomApplication( const TUid& aUid ) const;
    
    static TBool MatchJava( const TDesC& aMime );
    
    TUid InstalledMidletUidL();
    
    void PopulateInstalledWidgetUidsL();
    
    void PopulateInstalledWidgetsL(RExtendedWidgetInfoArray& aWidgets);
    
    TUid InstalledWidgetUidL();
    
    HBufC* InstalledWidgetNameLC();
    
    static TBool MatchWidget( RFile& aFile, const TDesC& aMime );
    
    void HandleInstalledWidgetL();
    
    TBool WidgetExistsL( const TUid& aUid );
    
    TBool WidgetExistsL( const TDesC& aIdentifier, TCatalogsVersion& aVersion );
    
    TBool WidgetExistsL( const TUid& aUid, TCatalogsVersion& aVersion );
    
private:
    
    enum TInstallType
        {
        ESisInstall,
        EFileInstall,        
        EJavaInstall,
        EWidgetInstall
        };


private: // Data

    TBool iBusy;
    MNcdInstallationServiceObserver* iObserver;
    // Registry session 
    Swi::RSisRegistrySession iRegistrySession;
    CAsyncCallBack* iInstallationCompleteCallback;
    TUid iAppUid;
    TUid iPackageUid;
    TUid iThemePackageUid;
    TNcdItemPurpose iPurpose;

    // File session 
    RFs iFs;
    
    TInstallType iInstallType;
    
    // Java handling

    // When installing java with JAD, the JAD path is stored here
    HBufC* iJadFileName;
    

    // Theme handling
    // Server session
    RAknsSrvSession iAknsSrv;
    // Array of skin information packages
    CArrayPtr<CAknsSrvSkinInformationPkg>* iThemes;
    // Name of the theme
    HBufC* iThemeName;
    // Package ID
    TAknsPkgID iPID;
    
    TInt iInstallError;
    
    HBufC* iRecognizedMime;
    
    RApaLsSession iApaLs;
    
    // For silent installation
    // Observer
    CNcdSilentInstallActiveObserver* iSilentInstallActiveObserver;
    
    CNcdActiveOperationObserver* iInstallStatusObserver;

    Usif::RSoftwareInstall iInstaller;
    Usif::COpaqueNamedParams* iArguments; 
    Usif::COpaqueNamedParams* iResults;
    
    // ROM application UIDS
    RArray<TUid> iRomUids; 
    
    //RWidgetRegistryClientSession iWidgetRegistry; 
    Usif::RSoftwareComponentRegistry iScrSession;
    
    //RWidgetInfoArray iInstalledWidgets;    
    RPointerArray<CExtendedWidgetInfo> iInstalledWidgetsInfos; 
    };

#endif // C_NCD_INSTALLATION_SERVICE_IMPL_H

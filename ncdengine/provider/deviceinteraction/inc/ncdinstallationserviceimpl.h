/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknServerApp.h>
#include <AknsSrvClient.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <apgcli.h>
#include <centralrepository.h>
#include <SWInstApi.h>
#include <WidgetRegistryClient.h>

#include "ncdinstallationservice.h"
#include "ncditempurpose.h"
#include "ncdasyncoperationobserver.h"
#include "ncdasyncsilentinstallobserver.h"


/**
 * New Java registry API was introduced in 3.2. We start using it 
 * from 5.0 onwards because it seems that some 3.2 devices don't support
 * it entirely
 */
#if defined( __SERIES60_30__ ) || defined( __SERIES60_31__ ) || defined ( __S60_32__ )
    #define USE_OLD_JAVA_API
    class MJavaRegistrySuiteEntry;
    class MJavaRegistry;
#else
    namespace Java
        {
        class CJavaRegistry;
        }
#endif


class CDocumentHandler;

class MNcdInstallationServiceObserver;
class TCatalogsVersion;
class CNcdActiveOperationObserver;
class CNcdSilentInstallActiveObserver;


/**
 *
 */
class CNcdInstallationService : public CBase,
                                public MNcdInstallationService,
                                public MAknServerAppExitObserver,
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
                         const SwiUI::TInstallOptionsPckg& aInstallOptionsPckg );
    
    /**
     * @see MNcdInstallationService::SilentInstallJavaL
     */
    void SilentInstallJavaL( RFile& aFile,
                             const TDesC& aMimeType,
                             const TDesC8& aDescriptorData,
                             const SwiUI::TInstallOptionsPckg& aInstallOptionsPckg );
    
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

    
    /**
     * @see MNcdInstallationService::DocumentHandler()
     */
    CDocumentHandler& DocumentHandler();
    
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
                   const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg );

    /**
     * @see MNcdInstallationService::InstallJavaL
     * @param aSilentInstallOptionsPckg If NULL, then installation is done normally.
     * If not NULL, then the installation is done silently by using the given 
     * options. Ownership is not transferred.
     */
    void InstallJavaL( RFile& aFile,
                       const TDesC& aMimeType,
                       const TDesC8& aDescriptorData,
                       const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg );

    
    void InstallWidgetL( 
        RFile& aFile,
        const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg );
        

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
    
    
    /**
     * Gets the UID of the latest installed midlet by using the 
     * Java installer's P&S key
     * 
     * @param aJavaRegistry Java registry
     * @return Midlet UID
     */
#ifdef USE_OLD_JAVA_API
    TUid LatestMidletUidL( MJavaRegistry& aJavaRegistry ) const;
#else    
    TUid LatestMidletUidL( Java::CJavaRegistry& aJavaRegistry ) const;
#endif
    
    static TBool MatchJava( const TDesC& aMime );
    
    TUid InstalledMidletUidL();
    
    void PopulateInstalledWidgetUidsL();
    
    TUid InstalledWidgetUidL();
    
    HBufC* InstalledWidgetNameLC();
    
    static TBool MatchWidget( RFile& aFile, const TDesC& aMime );
    
    void HandleInstalledWidgetL();
    
    TBool WidgetExistsL( const TUid& aUid );
    
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
    CDocumentHandler* iDocHandler;
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
    
#ifdef USE_OLD_JAVA_API    
    // For comparing installed midlet uids to find out the uid for the newly 
    // installed java app.
    RArray<TUid> iMIDletUids;
#endif

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

    SwiUI::RSWInstLauncher iInstaller;
        
    // ROM application UIDS
    RArray<TUid> iRomUids; 
    
    // needed for cancelling install correctly
    SwiUI::TServerRequest iCancelCode;  
    
    
    RWidgetRegistryClientSession iWidgetRegistry;    
    RWidgetInfoArray iInstalledWidgets;    
    };

#endif // C_NCD_INSTALLATION_SERVICE_IMPL_H

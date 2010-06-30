/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CStateMachine class implementation.
*
*
*/

#include <e32base.h>
#include <aknappui.h>
#include <swinstdefs.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <httpdownloadmgrcommon.h>
#include <coeutils.h>
#include <SenDomFragment.h>
#include <SenXmlReader.h>
#include <SenElement.h>
#include <centralrepository.h>      // CRepository
#include <browseruisdkcrkeys.h>     // KCRUidBrowser

#include <wlanerrorcodes.h> 		//For download error handling
#include <etelpckt.h>       		//For download error handling
#include <exterror.h>       		//For download error handling
#include <httpdownloadmgrcommon.h>	//For download error handling (not in S60 5.0 SDK)

#include <es_enum.h>
#include <rconnmon.h>
#include <eikdef.h>

#include "bootstrapstatemachine.h"
#include "bootstrapappui.h"
#include "bootstrapconst.h"
#include "bootstrapinstallationobserver.h"
#include "smartinstallerdll.h"
#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "config.h"
#include "utils.h" // HideAppFromMenuL()

// ADM binary name
_LIT(KAdmExePath, "ADM.exe");
_LIT(KDownloadFolder,"?:\\system\\adm\\");
// Sis file extension
_LIT(KSisExtn,".SIS");
_LIT(KSisxExtn,".SISX");
// Xml file extension
_LIT(KXMLExtn,".XML");
// Dep file extension
_LIT(KDepExtn,"_DEP");
// ADM changes file name
_LIT(KAdmChangesFile, "adm_changes.xml");
// Private folder of SWI Daemon
_LIT(KSWIDaemonPrivateFolder, "\\private\\10202dce\\");

// ---------------------------------------------------------------------------
// CStateMachine::NewLC
// ---------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewLC(CBootstrapAppUi& aSIUiObj, TUint32 aWrapperUid)
	{
	CStateMachine* object = new ( ELeave ) CStateMachine(aSIUiObj, aWrapperUid);
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

// ---------------------------------------------------------------------------
// CStateMachine::NewL
// ---------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewL(CBootstrapAppUi& aSIUiObj, TUint32 aWrapperUid)
	{
	CStateMachine* object = CStateMachine::NewLC(aSIUiObj, aWrapperUid);
	CleanupStack::Pop();
	return object;
	}

// ---------------------------------------------------------------------------
// CStateMachine::CStateMachine
// C++ default constructor.
// ---------------------------------------------------------------------------
//
CStateMachine::CStateMachine(CBootstrapAppUi& aBsAppUiObj, TUint32 aWrapperUid) :
							CActive(CActive::EPriorityStandard),
							iAppUi(aBsAppUiObj),
							iWrapperPackageUid(aWrapperUid),
							iAdmChangesInfo(),
							iBootstrapFailed(EFalse)
#ifdef USE_LOGFILE
							, iLog(aBsAppUiObj.iLog)
#endif
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------------------------
// CStateMachine::ConstructL
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CStateMachine::ConstructL()
	{
	// Connect to the Download Manager
	TBool masterInstance(!(iAppUi.EikonEnv()->StartedAsServerApp()));
	iDownloadManager.ConnectL( KUidBootstrapApp, *this, masterInstance );

	User::LeaveIfError(iLauncher.Connect());

	iIsAdmInstalled = IsPackagePresentL(KUidAdm, iAdmInstalledVersion);

	ReadConfigurationL();
	}

// ---------------------------------------------------------------------------
// CStateMachine::~CStateMachine
// Virtual Destructor.
// ---------------------------------------------------------------------------
//
CStateMachine::~CStateMachine()
	{
	LOG( "~StateMachine()" );
	Cancel();
	DELETE_IF_NONNULL( iDepFilename );
	DELETE_IF_NONNULL( iSisFilename );
	DELETE_IF_NONNULL( iDownloadUrl );
	delete iInstallObsrv;
	delete iConfigUrl;
	delete iUrl;
	delete iDownloadFileName;
	iIapArray.Close();
	iDownloadManager.DeleteAll();
	iDownloadManager.Close();
	iLauncher.Close();
	}

// ---------------------------------------------------------------------------
// CStateMachine::RunL
// Called for each state transition
// ---------------------------------------------------------------------------
//
void CStateMachine::RunL()
	{
	switch (iAppState)
		{
		case EBSStateInit:
			{
			StateInitL();
			RequestState(EBSStateDownloadChanges);
			}
			break;
		case EBSStateDownloadChanges:
			{
			StateDownloadL();
			// No need to request a state change. HandleDMgrEventL() will
			// trigger a correct event after the download has completed.
			}
			break;
		case EBSStateParseChanges:
			{
			StateParseChangesL();
			if( IsAdmDownloadRequired() )
				{
				RequestState(EBSStateDownloadAdm);
				}
			else
				{
				RequestState(EBSStateLaunchAdm);
				}
			}
			break;
		case EBSStateDownloadAdm:
			{
			StateDownloadL();
			// No need to request a state change. HandleDMgrEventL() will
			// trigger a correct event after the download has completed.
			}
			break;
		case EBSStateInstallAdm:
			{
			StateInstallAdmL();
			// No need to request a state change. HandleInstallCompleteL() will
			// trigger a correct event after the installation has completed.
			}
			break;
		case EBSStateLaunchAdm:
			{
			StateLaunchAdmL();
			RequestState(EBSStateExit);
			}
			break;
		case EBSStateExit:
			{
			// Exit the application
			Cancel();
			iAppUi.ExitApp(iAppState);
			}
			break;
		case EBSStateExitWithError:
			{
			Cancel();
			//Error Id needs to be already set before the state transition.
			iAppUi.ExitApp(iAppState);
			}
			break;
		default:
			LOG2( "Ignoring state change %d", iAppState );
			break;
		}
	}

// ---------------------------------------------------------------------------
// CStateMachine::StateComplete
// Trigger the state transition.
// ---------------------------------------------------------------------------
//
void CStateMachine::RequestState(const EBSAppState aNextState)
	{
	LOG3( "+ RequestState(): %d->%d", iAppState, aNextState );
	iAppState = aNextState;
	iStatus = KRequestPending;

	if( !IsActive() )
		{
		SetActive();
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

// ---------------------------------------------------------------------------
// CStateMachine::RunError
// Call Handlefailure incase of error.
// ---------------------------------------------------------------------------
//
TInt CStateMachine::RunError(TInt aError)
	{
	LOG2( "+ CStateMachine::RunError(%d)", aError );
	if ( aError != KLeaveExit )
		{
		HandleFailure();
		return KErrNone;
		}
	else
		{
		return KLeaveExit;
		}
	}

// ---------------------------------------------------------------------------
// CStateMachine::DoCancel
// ---------------------------------------------------------------------------
//
void CStateMachine::DoCancel()
	{
	LOG( "+ CStateMachine::DoCancel()" );
	iLauncher.CancelAsyncRequest(SwiUI::ERequestSilentInstall);
	iDownloadManager.DeleteAll();
	LOG( "- CStateMachine::DoCancel()" );
	}

// ---------------------------------------------------------------------------
// CStateMachine::Start
// ---------------------------------------------------------------------------
//
void CStateMachine::Start()
	{
	SetActive();

	// send signal that this request has completed
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

// ---------------------------------------------------------------------------
// CStateMachine::Stop
// ---------------------------------------------------------------------------
//
void CStateMachine::Stop()
	{
	LOG( "+ CStateMachine::Stop()" );
	Cancel();
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::ReadConfigurationL
// Leaves if runs out of memory
// Called from ConstructL in InitState
// @return KErrNone if configuration was read correctly, error code otherwise
// ---------------------------------------------------------------------------
//
TInt CStateMachine::ReadConfigurationL()
	{
	LOG( "+ ReadConfiguration()" );

	TFileName configFile;
	RArray< TPtrC > lineBuffer;
	RFs& rfs = CEikonEnv::Static()->FsSession();

	// Find the configuration file from the private directory
	TInt ret = rfs.PrivatePath( configFile );
	if ( ret == KErrNone )
		{
		TBuf<2> appDrive;
		// Insert the drive of the running application
		appDrive.Copy(iAppUi.Application()->AppFullName().Left(2));
		configFile.Insert( 0, appDrive );
		// Append the configuration file name
		configFile.Append( KConfigFile );

		LOG2( "configFile = %S", &configFile );

		HBufC* buffer = ReadConfigFile( rfs, configFile, lineBuffer, ret );

		if ( buffer && ret == KErrNone )
			{
			// Get Url from the config file
			TPtrC urlLine = GetConfigValue( KCfgTagUrl, lineBuffer, ret );
			if ( ret == KErrNone )
				{
				iConfigUrl = HBufC8::NewL( urlLine.Length() );
				iConfigUrl->Des().Copy(urlLine );
				LOG8_2( "configUrl = %S", iConfigUrl );
				}
			delete buffer;
			}
		}
	if ( !iConfigUrl )
		{
		// Error occured while reading the configuration, use default URL
		iConfigUrl = HBufC8::NewL( KDefaultDepServerUrl().Length() );
		*iConfigUrl = KDefaultDepServerUrl;
		LOG8_2( "configUrl = %S (default)", iConfigUrl );
		}

	lineBuffer.Close();

	LOG2("-  ReadConfiguration(): %d", ret );
	return ret;
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::StateInitL
// Enter in EBSStateSetup,
//
// Read the sis file name and dep file name, based on Bootstrap version
// Set the ADM changes file to download and the url to download from.
// ---------------------------------------------------------------------------
//
void CStateMachine::StateInitL()
	{
	LOG2( "+ SetFileNameInfo(): ver = %d", KBootstrapVersion );

	if( ( KBootstrapVersion < 1 ) && ( KBootstrapVersion > 4 ) )
		{
		// Unsupported Bootstrap version
		User::Leave( KErrNotSupported );
		}

	// Connect to SisRegistry to fetch Package info
	Swi::RSisRegistrySession registrySession;
	User::LeaveIfError( registrySession.Connect() );
	CleanupClosePushL( registrySession );

	Swi::RSisRegistryEntry entry;

	// Open registry entry
	User::LeaveIfError( entry.Open( registrySession, TUid::Uid( iWrapperPackageUid ) ) );
	CleanupClosePushL( entry );

	RPointerArray<HBufC> packageFiles;
	entry.FilesL( packageFiles );
	const TInt filesCount( packageFiles.Count() );

	// look into the files installed as part of the package,
	// to find the dep file name and actuall application file name.
	for ( TInt i=0; i<filesCount; i++ )
		{
		TParsePtrC parse( *packageFiles[i] );
		TPtrC orgExtn = parse.Ext();

		if ( orgExtn.Length() == 0 )
			{
			continue;
			}

		HBufC* extn = orgExtn.AllocLC();
		TPtr extnPtr = extn->Des();
		extnPtr.UpperCase();

		if ( KBootstrapVersion != 3 )
			{
			if (( extnPtr.Compare(KSisExtn) == 0 ) || ( extnPtr.Compare(KSisxExtn) == 0 ) )
				{
				TPtrC privatePath( KNullDesC );
				const TInt KDriveLength = 2;
				if ( ( packageFiles[i]->Length() ) > ( KSWIDaemonPrivateFolder().Length() + KDriveLength ) )
					{
					privatePath.Set( packageFiles[i]->Des().Mid( KDriveLength ) );
					privatePath.Set( privatePath.Left( KSWIDaemonPrivateFolder().Length() ) );
					}

				// Ignore if its a Stub Sis file
				if ( privatePath.CompareF( KSWIDaemonPrivateFolder ) )
					{
					if ( iSisFilename == NULL )
						{
						iSisFilename = packageFiles[i]->AllocL();
						LOG2( "Application sis '%S'", iSisFilename );
						}
					else
						// More than one sis present in the Wrapper Pkg
						{
						LOG2( "! More than one sis file present: '%S'. LEAVE.", packageFiles[i] );
						//TODO:GM: should there be a new error code for this??
						User::Leave( KErrNotSupported );
						}
					}
				} // if sis extension
			} // if (KBoostraVersion != 3)

		if ( KBootstrapVersion != 4 )
			{
			if( extnPtr.Compare(KXMLExtn) == 0 )
				{
				TPtrC orgName = parse.Name();
				const TInt namelength = orgName.Length();
				const TInt depSuffixlength = KDepExtn().Length();

				if ( namelength > depSuffixlength )
					{
					TPtrC depSuffix = orgName.Mid( ( namelength-depSuffixlength ), depSuffixlength );

					HBufC* name = depSuffix.AllocLC();
					TPtr namePtr = name->Des();
					namePtr.UpperCase();

					if ( namePtr.Compare( KDepExtn ) == 0 )
						{
						if ( iDepFilename == NULL )
							{
							iDepFilename = packageFiles[i]->AllocL();
							LOG2( "Dep file name is '%S'", iDepFilename );
							}
						else
							// More than one dep file present in the Wrapper Pkg
							{
							LOG2( "! More than one dep file present: '%S'. LEAVE.", packageFiles[i] );
							//TODO:GM: should there be a new error code for this??
							User::Leave( KErrNotSupported );
							}
						}
					CleanupStack::PopAndDestroy( name );
					}
				} // if XML extn
			} // if (KBoostrapVersion != 4)
		CleanupStack::PopAndDestroy( extn );
		}

	packageFiles.ResetAndDestroy();
	packageFiles.Close();

	if (KBootstrapVersion != 3)
		{
		if ( iSisFilename == NULL )
			{
			User::Leave( KErrNotSupported );
			}
		}
	if (KBootstrapVersion != 4)
		{
		if ( iDepFilename == NULL )
			{
			User::Leave( KErrNotSupported );
			}
		}

	CleanupStack::PopAndDestroy(&entry);
	CleanupStack::PopAndDestroy(&registrySession);

	iFileName.Set(KAdmChangesFile);

	// URL is already set
	DELETE_IF_NONNULL(iUrl);
	iUrl = iConfigUrl->AllocL();

	LOG2( "Filename is '%S'",&iFileName );

#ifdef FEATURE_BOOTSTRAP_SETIAP
	SetAndValidateIAPL();
#endif

	LOG( "- SetFileNameInfo()" );
	}

// ---------------------------------------------------------------------------
// CStateMachine::::IsAdmDownloadRequired
// Enter after EBSStateParseChanges
//
// Verify whether ADM sis download is required. Based on this, the next state will be set.
// ---------------------------------------------------------------------------
//
TBool CStateMachine::IsAdmDownloadRequired()
	{
	const TBool isAdmDownloadRequired =
			CompareVersions(iAdmInstalledVersion, iAdmChangesInfo.iVersion)
				== EGreaterSecondVersion;

	if ( isAdmDownloadRequired )
		{
		LOG7( "ADM update required (v%d.%02d.%d -> v%d.%02d.%d)",
			iAdmInstalledVersion.iMajor, iAdmInstalledVersion.iMinor, iAdmInstalledVersion.iBuild,
			iAdmChangesInfo.iVersion.iMajor, iAdmChangesInfo.iVersion.iMinor, iAdmChangesInfo.iVersion.iBuild
			);

		iFileName.Set(*iAdmChangesInfo.iSisName);

		DELETE_IF_NONNULL(iUrl);

		iUrl = iConfigUrl->AllocL();
		}
	else
		{
		LOG4( "ADM is up-to-date (v%d.%02d.%d)",
			iAdmInstalledVersion.iMajor, iAdmInstalledVersion.iMinor, iAdmInstalledVersion.iBuild
			);
		}

	return isAdmDownloadRequired;
	}

// ---------------------------------------------------------------------------
// CStateMachine::::StateDownloadL
// Enter in case EBSStateDownloadChanges, EBSStateDownloadAdm
//
// Download the specified file from the given url.
// ---------------------------------------------------------------------------
//
void CStateMachine::StateDownloadL()
	{
	LOG( "+ StateDownloadL()" );

	// Create the full URL for download
	DELETE_IF_NONNULL( iDownloadUrl );
	// +1 for '/'
	iDownloadUrl = HBufC8::NewL(iUrl->Length() + iFileName.Length() + 1 );
	TPtr8 ptr(iDownloadUrl->Des());
	ptr.Copy(*iUrl);

	// Append forward slash if that's missing
	if (ptr.Right(1)[0] != '/')
		{
		ptr.Append('/');
		}
	ptr.Append(iFileName);

	iDownloadError = EDlErrNoError;
	iConnectionAttempt = 0;

	// Set download file name along with path
	TChar systemDrive;
	RFs::DriveToChar(RFs::GetSystemDrive(), systemDrive);

	DELETE_IF_NONNULL( iDownloadFileName );
	iDownloadFileName = HBufC::NewL(KDownloadFolder().Length() + iFileName.Length());
	TPtr responseFilenamePtr = iDownloadFileName->Des();

	responseFilenamePtr.Copy(KDownloadFolder);
	responseFilenamePtr[0] = systemDrive;
	responseFilenamePtr.Append(iFileName);

	// Create new download
	LOG2( "Downloading '%S'", iDownloadFileName );
	LOG8_2( "  from '%S'", iDownloadUrl );

	TBool isNewDl = ETrue, tried = EFalse;
retry:
	RHttpDownload& dl = iDownloadManager.CreateDownloadL( *iDownloadUrl, isNewDl );

	ConeUtils::EnsurePathExistsL(*iDownloadFileName);

	if (isNewDl)
		{
		LOG2( "Receiving '%S'", iDownloadFileName );
		dl.SetStringAttribute( EDlAttrDestFilename, *iDownloadFileName );
		User::LeaveIfError( dl.Start() );
		}
	else
		{
		LOG( "Dl exists." );
		dl.Delete();
		LOG( "Dl deleted." );
		if (tried)
			{
			LOG( "Tried already, leaving" );
			User::Leave(KErrAlreadyExists);
			}
		else
			{
			tried = ETrue;
			goto retry;
			}
		}

	LOG( "- Download()" );
	}

// ---------------------------------------------------------------------------
// CStateMachine::StateParseChangesL
// Enter in case EBSStateParseChanges
//
// Parse the given changes file.
// ---------------------------------------------------------------------------
//
void CStateMachine::StateParseChangesL()
	{
	LOG2( "+ ParseFile('%S')", iDownloadFileName );
	User::LeaveIfNull(iDownloadFileName);

	// Initialize the parser
	CSenXmlReader* XmlReader = CSenXmlReader::NewLC(EErrorOnUnrecognisedTags);
	CSenDomFragment* DomFragment = CSenDomFragment::NewL();
	CleanupStack::PushL(DomFragment);

	XmlReader->SetContentHandler(*DomFragment);
	DomFragment->SetReader(*XmlReader);

	// Parse the provided changes file
	XmlReader->ParseL(CEikonEnv::Static()->FsSession(), *iDownloadFileName);

	// Get package version
	CSenElement* packageElement;
	packageElement = DomFragment->AsElement().Element(KFieldVersion);
	if (packageElement == NULL)
		{
		User::Leave(EXmlErrorMissingVersion);
		}
	else
		{
		TPtrC8 versionPtr = packageElement->Content();
		TVersion ver;
		if (SetVersion(versionPtr, iAdmChangesInfo.iVersion) == EFalse)
			{
			User::Leave(EXmlErrorInvalidVersion);
			}
		}

	// Get the ADM sis file name
	packageElement = DomFragment->AsElement().Element(KFieldSisName);
	if (packageElement)
		{
		iAdmChangesInfo.iSisName = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EXmlErrorMissingSisFileName);
		}

	// URL read from ADM changes file.
	// ADM.sis will be fetched from this url, if present. Or else default config url will be used.
#if USE_ADMCHANGES_URL
	// Get the optional URL
	packageElement = DomFragment->AsElement().Element(KFieldUrl);
	if (packageElement)
		{
		TPtrC8 urlPtr = packageElement->Content();
		iAdmChangesInfo.iUrl = urlPtr.AllocL();
		}
#endif

	CleanupStack::PopAndDestroy( DomFragment );
	CleanupStack::PopAndDestroy( XmlReader );

	// Delete no longer required changes file
	DeleteFile(*iDownloadFileName);

	LOG( "- ParseFile()" );
	}

// ---------------------------------------------------------------------------
// CStateMachine::StateInstallAdmL
// Enter in case EBSStateInstallAdm
//
// Silent Install the given sis file. This is asynchronous and the next state transition happens only in the handle function.
// ---------------------------------------------------------------------------
//
void CStateMachine::StateInstallAdmL()
	{
	LOG( "+ InstallPackage()" );
	SwiUI::TInstallOptions Options;

	Options.iUpgrade = SwiUI::EPolicyAllowed;
	Options.iOCSP    = SwiUI::EPolicyNotAllowed;
	Options.iDrive   = 'c';
	Options.iUntrusted = SwiUI::EPolicyNotAllowed;
	Options.iCapabilities = SwiUI::EPolicyNotAllowed;
	Options.iOverwrite = SwiUI::EPolicyAllowed;

	iOptionsPckg = Options;

	if (iInstallObsrv == NULL)
		{
		iInstallObsrv = new (ELeave) CInstallObserver(this);
		}
	iInstallObsrv->iStatus = KRequestPending;
	iInstallObsrv->Start();
	// Start asynchronous installation
	iLauncher.SilentInstall(iInstallObsrv->iStatus, *iDownloadFileName, iOptionsPckg);

	LOG( "- InstallPackage()" );
	}

// ---------------------------------------------------------------------------
// CStateMachine::StateLaunchAdmL
// Enter in case EBSStateLaunchAdm
//
// Launch ADM. A few command line args are passed and this depends on the bootstrap version.
// ---------------------------------------------------------------------------
//
void CStateMachine::StateLaunchAdmL()
	{
	LOG( "+ LaunchAdm()" );

	TUint32 protocolVer = KBootstrapAdmProtocolVersion;
	TUint32 launchCtrl = 0;

	// Command line arguments, version 15 specification:
	//
	//31               9  8  7  6  5  4  3  2  1  0
	// +-------------+--+--+--+--+--+--+--+--+--+--+
	// | reserved    |  |  |  |  |  |  |           |
	// +-------------+--+--+--+--+--+--+--+--+--+--+
	//
	//      value
	// bits range description
	//  0-3  0-15 Command line parameter protocol version:
	//             0 = not supported
	//             1 = ADM.exe 1 <wrapper_uid> <iap> <depfile> <sisfile>
	//             2 = ADM.exe 2 <wrapper_uid> <iap> <depfile> <sisfile>
	//             3 = ADM.exe 3 <wrapper_uid> <iap> <depfile>
	//             4 = ADM.exe 4 <wrapper_uid> <iap> <sisfile> (this was used in beta1, beta2)
	//            15 = ADM.exe 15 [parameters as specified by other bits]
	//
	//         since
	//  bit  version type command line parameter present
	//    4      4     N  Bootstrap version number
	//    5      4     N  Wrapper UID
	//    6      4     N  IAP
	//    7      4     N  ADM launch condition code:
	//                    KCmdLineCancelAdm: ADM needs to cancel installation
	//                    KCmdLineLaunchAdm: ADM starts normally
	//    8      4     S  Full path to dependency XML file
	//    9      4     S  Full path to application SIS file
	//
	// Types:
	//  N  Natural decimal number (range 0 - 2^32-1, fits to TUint32)
	//  S  UTF-8 string, must NOT contains spaces or other whitespaces
	//
	// Command line parameters appear in the order of the bits set, i.e.
	// if bits 4, 8, 9 are set the command line parameters are:
	// ADM.exe 784 <bootstrap_version> <dep_file> <sis_file>
	//
	// If command line protocol version is less than 15, it is assumed that
	// old version command line parameter format is used.
	//

	// In case of approach 3 , cleanup of only BootStrap
	// is required.
	if (KBootstrapVersion == 3)
		{
		iWrapperPackageUid = KUidBootstrapApp.iUid;
		}

	// Set the launchCtrl based on Bootstrap Failure/Success
	if ( iBootstrapFailed )
		{
		if (iSisFilename)
			{
			// SisFile exists -> resuming is possible
			launchCtrl = KCmdLineCancelAdm;
			}
		else
			{
			launchCtrl = KCmdLineCancelAdmNoResume;
			}
		}
	else
		{
		launchCtrl = KCmdLineLaunchAdm;
		}

	LOG2( "LaunchCtrl: %d", launchCtrl );

	// By default, set the following bits:
	// 1. BootstrapVersion,
	// 2. WrapperUid
	// 3. Launchctrl
	// 4. IAP (If not set, the value is 0)
	protocolVer |= (KCmdLineBitBootstrapVersion | KCmdLineBitWrapperUid)
			| KCmdLineBitADMLaunchControl
			| KCmdLineBitIAP;

	// Set the mandatory command line parameters
	//
	// Size of 32-bit Bootstrap Version Value is 10 characters
	// Size of 32-bit UID when converted to numeric string is 10 characters (4294967295)
	// Size of 32-bit iIAP (TUint32) when converted to numeric string is 10 characters
	// Size of 2 spaces is 2 characters
	if ( (launchCtrl == KCmdLineCancelAdm) || (launchCtrl == KCmdLineLaunchAdm) )
		{
		switch (KBootstrapVersion)
			{
			case 1:
			case 2:
				{
				User::LeaveIfNull(iSisFilename);
				User::LeaveIfNull(iDepFilename);

				protocolVer |= (KCmdLineBitFileSis | KCmdLineBitFileDep);
				break;
				}
			case 3:
				{
				User::LeaveIfNull(iDepFilename);
				protocolVer |= (KCmdLineBitFileDep);
				break;
				}
			case 4:
				{
				User::LeaveIfNull(iSisFilename);
				protocolVer |= (KCmdLineBitFileSis);
				break;
				}
			default:
				// Unsupported Bootstrap Version
				User::Leave(KErrNotSupported);
				break;
			} // switch (KBootstrapVersion)
		} // if ( (launchCtrl == KCmdLineCancelAdm) || (launchCtrl == KCmdLineLaunchAdm) )

	// Build the command line parameter string
	HBufC *processArgs = NULL;
	TInt processArgsLen = 11; // obligatory protocol version number and ' '
	TUint32 tmp = (protocolVer & ~KCmdLineBitProtocolVerMask) >> KCmdLineProtocolVerBits;
	for (TInt i = 0; tmp != 0 || i < sizeof(KCmdLineParamLengths); tmp >>= 1, i++)
		{
		if (tmp & 1)
			{
			processArgsLen += KCmdLineParamLengths[i];
			}
		}
	if (iDepFilename)
		processArgsLen += iDepFilename->Length()+1; // +1 comes from the appended ' '
	if (iSisFilename)
		processArgsLen += iSisFilename->Length();
	LOG2( "len %d", processArgsLen );
	processArgs = HBufC::NewLC(processArgsLen);
	TPtr ptr = processArgs->Des();

	// append the obligatory protocol version number
	ptr.AppendNum(protocolVer);
	ptr.Append(' ');

	if (protocolVer & KCmdLineBitBootstrapVersion)
		{
		ptr.AppendNum(KBootstrapVersion);
		ptr.Append(' ');
		}
	if (protocolVer & KCmdLineBitWrapperUid)
		{
		ptr.AppendNum(iWrapperPackageUid);
		ptr.Append(' ');
		}
	if (protocolVer & KCmdLineBitIAP)
		{
		ptr.AppendNum(iIAP);
		ptr.Append(' ');
		}
	if (protocolVer & KCmdLineBitADMLaunchControl)
		{
		ptr.AppendNum(launchCtrl);
		ptr.Append(' ');
		}
	if (protocolVer & KCmdLineBitFileDep)
		{
		ptr.Append(*iDepFilename);
		ptr.Append(' ');
		}
	if (protocolVer & KCmdLineBitFileSis)
		{
		ptr.Append(*iSisFilename);
		}

	// Launch ADM
	LOG3( "Launching %S '%S'", &KAdmExePath, processArgs);

	RProcess process;
	CleanupClosePushL<RProcess>( process );

	const TInt err = process.Create( KAdmExePath, processArgs->Des() );
	if (err != KErrNone)
		{
		LOG2( "ADM launch failed with error: %d", err );
		CleanupStack::PopAndDestroy( &process );
		CleanupStack::PopAndDestroy( processArgs );
		User::Leave(err);
		}
	else
		{
		// ADM was launched properly, set it visible in the menu grid
		LOG( "Revealing ADM menu icon" );
		CUtils::HideApplicationFromMenuL( KUidAdm.iUid, EFalse );
		}

// Do not lower the priority: it will cause the "OK" note to be shown
// on faster, more recent S60 devices
//	process.SetPriority( EPriorityBackground );
	process.Resume();

	CleanupStack::PopAndDestroy( &process );
	CleanupStack::PopAndDestroy( processArgs );

	LOG2( "- LaunchAdmL(): %d", err );
	}

// ---------------------------------------------------------------------------
// CStateMachine::HandleDMgrEventL
// Callback to handle the download manager events
// Called in EBSStateDownloadChanges, EBSStateDownloadAdm
// ---------------------------------------------------------------------------
//
void CStateMachine::HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
	{
	TInt32 glErrId = KErrNone;
	TInt32 errId = ENoError;

	// Get error IDs
	aDownload.GetIntAttribute( EDlAttrGlobalErrorId, glErrId );
	aDownload.GetIntAttribute( EDlAttrErrorId, errId );

#ifdef DO_LOG
	_LIT(KFmt, ", GlErrId=%6d, ErrId=%d");
	TBuf<64> buf;

	if ( ((glErrId != KErrNone) || (errId != ENoError)) && glErrId != KErrUnknown )
		{
		buf.AppendFormat(KFmt, glErrId, errId);
		}

	if ( ( iDownloadState != aEvent.iDownloadState ) ||
		 ( iProgressState != aEvent.iProgressState ) )
		{
		iDownloadState = aEvent.iDownloadState;
		iProgressState = aEvent.iProgressState;

		LOG4( "DlSt=%5d, PrSt=%5d%S", iDownloadState, iProgressState, &buf );
		}
#endif

	switch ( aEvent.iDownloadState )
		{
		case EHttpDlCreated:
		case EHttpDlInprogress:
			{
			//Nothing done.
			break;
			}
		case EHttpDlPaused:
		case EHttpDlFailed:
// These two fill fold to EHttpDlFailed. We don't want to process failure events twice.
//		case EHttpDlNonPausableNetworkLoss:
//		case EHttpDlMultipleMOFailed:
			{
			//Continue download if paused in EHttpContentTypeReceived.
			//Pause is received even in ProgNone state!
			if( ( aEvent.iProgressState == EHttpContentTypeReceived ) ||
				( (aEvent.iProgressState == EHttpProgNone ) && ( errId == ENoError ) ) )
				{
				LOG( "Starting download" );
				aDownload.Start();
				break;
				}
			if ( ProcessDlErrors(glErrId, errId, iDownloadError) )
				{
				if ( (iIapArrayIndex+1) < iIapArray.Count() )
					{
					// Try the next IAP
					iIapArrayIndex++;
					LOG3( "Starting download with next IAP[%d]=%d.", iIapArrayIndex, iIapArray[iIapArrayIndex] );
					SetIAP( iIapArray[iIapArrayIndex] );
					aDownload.Start();
					break;
					}
				else if ( (iIapArrayIndex+1) == iIapArray.Count() )
					{
					// We've gone through all the IAPs in the SNAP, prompt the user for an access point
					SetIAP( 0 );
					// This ensures that this code path is taken only once
					iIapArrayIndex++;
					}
				if (++iConnectionAttempt < KDownloadConnectionRetries)
					{
					LOG4( "Restarting download due to network failure (%d: %d, %d)", iConnectionAttempt, glErrId, errId );
					// TODO: Do we need to do a Reset() before Start()?
					aDownload.Start();
					// We don't restart the download timer on purpose
					//
					// Clear the error id so it doesn't get caught below
					// as this is not an error situation.
					iDownloadError = EDlErrNoError;
					}
				else
					{
					LOG4( "Too many download retries, cancelling download (%d; %d, %d)", iDownloadError, glErrId, errId );
					// iDownloadError has been set properly by ProcessDlErrors()
					}
				}
			if ( iDownloadError != EDlErrNoError )
				{
				LOG2( "DlErr=%d", iDownloadError );

				if ( iDownloadError == EDlErrCancelled )
					{
					HandleFailure(ETrue); // If user pressed cancel, then iIsIapCancelled is 1
					}
				else
					{
					HandleFailure(EFalse);
					}
				}
			break;
			}
		case EHttpDlCompleted:
			{
			// TODO: Get the DownloadfileName??
			iDownloadManager.GetIntAttribute( EDlMgrIap, iIAP );
			LOG2( "IAP set to %d", iIAP );

			switch (iAppState)
				{
				case EBSStateDownloadChanges:
					RequestState(EBSStateParseChanges);
					break;
				case EBSStateDownloadAdm:
					RequestState(EBSStateInstallAdm);
					break;
				default:
					LOG2( "DlEvent from unknown state %d", iAppState );
					// TODO: Fix the panic code
					User::Panic(_L("SmartInst"), 42);
					break;
				}
			break;
			}
		default:
			{
			break;
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CStateMachine::ProcessDlErrors(const TInt32 aGlErrId, const TInt32 aErrId, EDownloadError& aDownloadError)
	{
	LOG4( "+ ProcessDlErrors(%d, %d, %d)", aGlErrId, aErrId, aDownloadError );
	TBool requestRestart = EFalse;
	const TUint count = iIapArray.Count();
	// Process the error codes
	switch (aErrId)
		{
		case KErrNone:
			// everything a-OK!
			break;
		case EConnectionFailed:
			// Request restart, if we haven't exhausted all the IAPs on the list.
			if (count > 0 && iIapArrayIndex < count)
				{
				requestRestart = ETrue;
				goto exit;
				}
			else
				{
				aDownloadError = EDlErrDownloadFailure;
				}
			break;
		case ETransactionFailed:
			// We set the download error to network failure as that's the
			// error propagated to TState::DownloadFailed().
			// The state machine will try to restart the download, if the
			// error code is EDlErrDownloadFailure, but NOT for EDlErrNetworkFailure.
			aDownloadError = EDlErrDownloadFailure;
			requestRestart = ETrue;
			break;
		case EObjectNotFound:
			aDownloadError = EDlErrFileNotFound;
			break;
		default:
			LOG2( "Unhandled ErrId %d", aErrId );
			aDownloadError = EDlErrDownloadFailure;
			break;
		}
	// Process the global error code
	switch (aGlErrId)
		{
		case KErrNone:
			// everything a-OK!
			break;
		case KErrCancel:
		case KErrAbort:
			// The user has cancelled the download / IAP selection box
			aDownloadError = EDlErrCancelled;
			break;
		case KErrDiskFull:
			// Disk full
			aDownloadError = EDlErrNotEnoughSpace;
			break;
		case KErrUnknown:
			{
			if( aErrId == KErrNone )
				{
				aDownloadError = EDlErrDownloadFailure;
				requestRestart = ETrue;
				}
			}
			break;
		case KErrNotFound:
		case KErrBadName:
		case KErrNotSupported:
		case KErrCommsLineFail:
		case KErrTimedOut:
		case KErrCouldNotConnect:
		case KErrDisconnected:
		case KErrGprsServicesNotAllowed:
		case KErrGsmMMNetworkFailure:
		case -8268: // KErrGsmOfflineOpNotAllowed:
		// WLAN network related errors:
		case KErrWlanNetworkNotFound:
		case KErrWlanRoamingFailed:
		case KErrWlanNetworkLost:
#if 0
		case KErrBadName:
		case KErrNotSupported:
		case KErrWlanOpenAuthFailed:
		case KErrWlanSharedKeyAuthRequired:
		case KErrWlanSharedKeyAuthFailed:
		case KErrWlanWpaAuthRequired:
		case KErrWlanIllegalEncryptionKeys:
		case KErrWlanPskModeRequired:
		case KErrWlanEapModeRequired:
		case KErrWlanSimNotInstalled:
		case KErrWlanNotSubscribed:
		case KErrWlanAccessBarred:
		case KErrWlanPasswordExpired:
		case KErrWlanNoDialinPermissions:
		case KErrWlanAccountDisabled:
		case KErrWlanRestrictedLogonHours:
		case KErrWlanServerCertificateExpired:
		case KErrWlanCerficateVerifyFailed:
		case KErrWlanNoUserCertificate:
		case KErrWlanNoCipherSuite:
		case KErrWlanUserRejected:
		case KErrWlanUserCertificateExpired:
		// less fatal, but still needs to fail the download:
		case KErrWlanWpaAuthFailed:
		case KErrWlan802dot1xAuthFailed:
		case KErrWlanIllegalWpaPskKey:
		case KErrWlanEapSimFailed:
		case KErrWlanEapTlsFailed:
		case KErrWlanEapPeapFailed:
		case KErrWlanEapMsChapv2:
		case KErrWlanEapAkaFailed:
		case KErrWlanEapTtlsFailed:
		case KErrWlanLeapFailed:
		case KErrWlanEapGtcFailed:
#endif
			// A fatal network error has occured, don't retry the download
			requestRestart = EFalse;
			aDownloadError = EDlErrNetworkFailure;
			break;
		default:
			if (!requestRestart)
				{
				// We assume all the other error codes to be 'hard' network errors
				LOG2( "Unhandled GlErrId %d", aGlErrId );
				aDownloadError = EDlErrNetworkFailure;
				}
			break;
		}
exit:
	LOG3( "- ProcessDlErrors(): %d, %d", requestRestart, aDownloadError );
	return requestRestart;
	}

// ---------------------------------------------------------------------------
// CStateMachine::HandleInstallCompleteL
// Callback to handle the Installation events
// Called in EBSStateInstallAdm
//
// If SWI is busy, re attempt installation 6 times
// ---------------------------------------------------------------------------
//
void CStateMachine::HandleInstallCompleteL(const TInt& aStatus)
	{
	LOG2("+ HandleInstallCompleteL(%d)", aStatus);
	TBool retrying = EFalse;

	User::LeaveIfNull(iDownloadFileName);

	// If the install server is busy , try connecting multiple times.
	if ( (aStatus == SwiUI::KSWInstErrBusy ||
			aStatus == KErrInUse ||
			aStatus == KErrServerBusy)
			&& ((++iInstallAttempt) <= KMaxInstallerConnectionAttempts))
		{
		LOG3( "Installer busy (%d), requesting retry (%d)", iInstallAttempt, aStatus );
		retrying = ETrue;
		User::After(KIterationTimer * iInstallAttempt);
		RequestState(EBSStateInstallAdm);
		}

	if (aStatus == KErrNone)
		{
		LOG( "ADM installed OK, deleting SIS." );
		// Delete no longer required sis file.
		DeleteFile(*iDownloadFileName);
		// ADM was installed succesfully, launch it!
		RequestState(EBSStateLaunchAdm);
		}
	else if (!retrying)
		{
		LOG3( "Installation unsuccessful (%d) after %d attempts, leaving.", aStatus, iInstallAttempt );
		// TODO: Better error handling
		User::Leave(aStatus);
		}

	LOG("- HandleInstallCompleteL()");
	}

// ---------------------------------------------------------------------------
// CStateMachine::IsPackagePresentL
// Check whether the given Package (uid & version) is already present in the device.
// Called in EBSStateInit to check for ADM Presence
// ---------------------------------------------------------------------------
//
TBool CStateMachine::IsPackagePresentL(const TUid& aUid, TVersion& aVersion)
	{
	LOG( "+ ADMPresent()");

	// Connect to SisRegistry to find if the package is
	// already installed.
	Swi::RSisRegistrySession registrySession;

	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);

	const TBool isPkgInstalled = registrySession.IsInstalledL(aUid);

	if (isPkgInstalled)
		{
		Swi::RSisRegistryEntry entry;

		// Open registry entry
		User::LeaveIfError(entry.Open(registrySession, aUid));
		CleanupClosePushL(entry);
		aVersion = entry.VersionL();

		CleanupStack::PopAndDestroy(&entry);
		}

	CleanupStack::PopAndDestroy(&registrySession);

	LOG3( "- ADMPresent(): 0x%x = %d", aUid.iUid, isPkgInstalled );

	return isPkgInstalled;
	}

// ---------------------------------------------------------------------------
// CStateMachine::DeleteFile
// Delete the given file
// ---------------------------------------------------------------------------
//
void CStateMachine::DeleteFile(const TDesC& aFileName)
	{
	CCoeEnv::Static()->FsSession().Delete(aFileName);
	}

// ---------------------------------------------------------------------------
// CStateMachine::HandleWaitNoteCancel
// Call HandleFailure.
// ---------------------------------------------------------------------------
//
//TODO: Can this be removed and call HandleFailure() directly?
void CStateMachine::HandleWaitNoteCancel()
	{
	HandleFailure(ETrue);
	}

// ---------------------------------------------------------------------------
// CStateMachine::HandleFailure
// Perform the state specific cleanup incase of failure.
// And then call AppUi Exit.
// ---------------------------------------------------------------------------
//
void CStateMachine::HandleFailure(TInt aError)
	{
	LOG2( "+ HandleFailure(): %d", aError );

	switch (iAppState)
		{
		case EBSStateInit:
		case EBSStateSetup:
		case EBSStateLaunchAdm:
			// No cleanUp required here
			break;
		case EBSStateDownloadChanges:
		case EBSStateDownloadAdm:
			// Deletion of all downloads is taken care in destructor
			break;
		case EBSStateInstallAdm:
			// Cancel the installation
			iLauncher.CancelAsyncRequest(SwiUI::ERequestSilentInstall);
			// fall-through
		case EBSStateParseChanges:
			{
			// Delete the downloaded dep file
			if (iDownloadFileName)
				{
				DeleteFile(*iDownloadFileName);
				}
			}
			break;
		default:
			break;
		}

	// Even in case of any failure, launch ADM, if present in device.
	// ADM needs to be launched in Cancel mode.
	if( (iAppState < EBSStateLaunchAdm) && iIsAdmInstalled )
		{
		// If it is user press cancel or Filenames are itself not set, then remove the whole wrapper package.
		// TODO: Should the failure reason be passed to ADM?
		// TODO: or show the error note in bootstrap, but launch ADM to remove the wrapper?
		if ( aError || ( iAppState <= EBSStateSetup) )
			{
			// No need to delete the files here, as the whole package will get removed.
			iBootstrapFailed = ETrue;
			}
		RequestState(EBSStateLaunchAdm);
		}
	else
		{
		// Delete the dep file is present
		if ( (KBootstrapVersion == 3) && iDepFilename )
			{
			DeleteFile(*iDepFilename);
			}
		else if ( (KBootstrapVersion == 4) && iSisFilename )
			{
			DeleteFile(*iSisFilename);
			}

		// Set the Error ID based on App State and User Cancel.
		if( aError || ((iAppState != EBSStateDownloadChanges) && (iAppState != EBSStateDownloadAdm)) )
			{
			iAppUi.SetErrorId(EInstallationFailed);
			}
		else
			{
			iAppUi.SetErrorId(EDownloadFailed);
			}

		// Now call App Exit to display the error note and exit bootstrap.
		RequestState(EBSStateExitWithError);
		}

	LOG( "- HandleFailure()" );
	}

#ifdef FEATURE_BOOTSTRAP_SETIAP
// ---------------------------------------------------------------------------
// Sets the used internet access point (IAP) to same that the Browser uses
// with additional verification that the selected IAP actually exist.
// ---------------------------------------------------------------------------
//
void CStateMachine::SetIAP(const TUint aIAP)
	{
	LOG2( "+ SetDlMgrIAP(): %d", aIAP );
	iIAP = aIAP;
	// Set the download manager to silent mode, if the IAP is valid
	iDownloadManager.SetBoolAttribute( EDlMgrSilentMode, aIAP != 0 );
	iDownloadManager.SetIntAttribute( EDlMgrIap, iIAP );
	}

// ---------------------------------------------------------------------------
// Sets the used internet access point (IAP) to same that the Browser uses
// with additional verification that the selected IAP actually exist.
// ---------------------------------------------------------------------------
//
void CStateMachine::SetAndValidateIAPL()
	{
	LOG( "+ CBootstrapAppUi::SetAndValidateIAPL()" );
	// Use the browser default IAP, if available
	TInt ret;
	TInt browserIAP, browserIAPmode, browserSNAP;
	TRequestStatus status;
	RConnectionMonitor connMon;

	// IsIapActive() requires this
	CleanupClosePushL(connMon);
	connMon.ConnectL();

	CRepository *repository = CRepository::NewLC( KCRUidBrowser );
	User::LeaveIfError( repository->Get(KBrowserAccessPointSelectionMode, browserIAPmode) );
	User::LeaveIfError( repository->Get(KBrowserDefaultAccessPoint, browserIAP) );
	ret = repository->Get( KBrowserNGDefaultSnapId, browserSNAP );
	CleanupStack::PopAndDestroy( repository );

	LOG5( "Browser IAP = %d, mode %d, SNAP %d (%d)", browserIAP, browserIAPmode, browserSNAP, ret );

	if ( OviStoreRunning() && IsIapActive(connMon, browserIAP) )
		{
		LOG2( "Using Ovi Store IAP %d", browserIAP );
		browserIAPmode = EBrowserCenRepApSelModeUserDefined;
		// browserIAP now contains the IAP ID that OviStore is using
		}

	// Get the IAP IDs for the browser SNAP, if we got the SNAP ID properly
	if (ret == KErrNone && browserSNAP > 0)
		{
		GetSnapIapsL(browserSNAP);
		}

	switch (browserIAPmode)
		{
		case EBrowserCenRepApSelModeDestination:
			//LOG( "Destination network ");
			break;
		case EBrowserCenRepApSelModeUserDefined:
			//LOG( "User defined IAP" );
			if (browserIAP > 0)
				{
				SetIAP(browserIAP);
				break;
				}
			// else fall-through
		case EBrowserCenRepApSelModeAlwaysAsk:
		default:
			LOG( "'Always ask'" );
			// if browser IAP is already active: then we utilise it directly, no prompting
			if ( IsIapActive(connMon, browserIAP) )
				{
				LOG2( "Connection already up (%d)", browserIAP );
				SetIAP(browserIAP);
				}
			else
				{
				LOG( "Prompting IAP" );
				// Set to always ask, we obey it as well
				}
			// need to set this, if we ended up here with default:
			browserIAPmode = EBrowserCenRepApSelModeAlwaysAsk;
			break;
		}
	// Found a proper IAP?
	if (iIAP != 0 || browserIAPmode == EBrowserCenRepApSelModeAlwaysAsk)
		{
#if 0
		TBuf<64> iapName;
		connMon.GetStringAttribute( iIAP, 0, KIAPName, iapName, status );
		User::WaitForRequest( status );
		if ( status.Int() != KErrNone )
			{
			// Try access point name
			connMon.GetStringAttribute( iIAP, 0, KAccessPointName, iapName, status );
			User::WaitForRequest( status );
			if ( status.Int() != KErrNone )
				{
				LOG2( "ConnMon.GetStrAttr() ret = %d, ignoring.", status.Int() );
				}
			}
		LOG3( "Using IAP %d ('%S')", iIAP, &iapName );
#else
		//LOG2( "Using IAP %d", iIAP );
#endif
		}
	else
		{
		// set the IAP from SNAP, if it's valid
		if (iIapArray.Count() > 0 && iIapArray[0] > 0)
			{
			LOG2( "Starting with IAP %d", iIapArray[0] );
			SetIAP(iIapArray[0]);
			}
		}

	CleanupStack::PopAndDestroy( &connMon );
	LOG3( "- CBootstrapAppUi::SetAndValidateIAPL(): %d, %d", ret, iIAP );
	}

// ---------------------------------------------------------------------------
// Get the default IAP for the selected SNAP. SNAP is available since 3.2
// ---------------------------------------------------------------------------
//
void CStateMachine::GetSnapIapsL(const TUint aSNAPId)
	{
	// Load the helper library dynamically
	_LIT(KLibHelper, "smartinstaller.dll");
	LOG( "Loading helper" );
	RLibrary lib;
	const TInt ret = lib.Load( KLibHelper() );
	if (ret == KErrNone)
		{
		CleanupClosePushL(lib);
		const TInt KHelperNewLC = 2;
		CHelper* helper = (CHelper*)lib.Lookup(KHelperNewLC)();
		TRAPD(ret, helper->GetSnapIapsL(aSNAPId, iIapArray) );
		LOG3( "Got %d IAPs (%d)", iIapArray.Count(), ret );
#ifdef DO_LOG
		for (TInt i = 0; i < iIapArray.Count(); i++)
			{
			LOG3( "IAP[%d]=%d", i, iIapArray[i] );
			}
#endif
		CleanupStack::PopAndDestroy(2, &lib); // helper, lib
		}
	else
		{
		LOG2( "Helper load failed %d", ret );
		}
	lib.Close();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CStateMachine::IsIapActive(RConnectionMonitor &aConnMon, TInt& aIAP)
	{
	LOG2( "+ IsIapActive(): %d", aIAP );

	TBool ret = EFalse;
	// Get the connection info for each connection
	TUint id, iapId, count, countSubConn;
	TRequestStatus status = KRequestPending;

	aConnMon.GetConnectionCount( count, status );
	User::WaitForRequest( status );

	if (status.Int() != KErrNone)
		{
		LOG2( "GetConnCount() failed %d.", status.Int() );
		return ret;
		}

	LOG2( "Active connections: %d", count );

	TInt oviStoreIap = KErrNotFound;
	for ( TInt i = 1; i <= count && !ret; i++ )
		{
		const TInt err = aConnMon.GetConnectionInfo( i, id, countSubConn );
		if ( err == KErrNone )
			{
			LOG3( "ConnID[%d] = %d" , i, id );
			// Get the IAP Id of the connection
			aConnMon.GetUintAttribute( id, 0, KIAPId, iapId, status );
			User::WaitForRequest( status );
			if ( status.Int() != KErrNone )
				{
				LOG2( "GetIapId failed (%d), trying next", status.Int() );
				// try next connection
				continue;
				}
			else
				{
				LOG2( "Active IAP = %d", iapId );
				}

			// Find the UIDs using this connection
			TConnMonClientEnumBuf clientBuf;
			aConnMon.GetPckgAttribute( id, 0, KClientInfo, clientBuf, status );
			User::WaitForRequest( status );
			if ( status.Int() == KErrNone )
				{
				//LOG3( "%d clients sharing IAP %d", clientBuf().iCount, iapId );
				for (TUint client = 0; client < clientBuf().iCount; client++)
					{
					//LOG2( "0x%08X", clientBuf().iUid[client].iUid );
					const TInt KUidSecureWidgetUi = 0x102829A0;
					if ( clientBuf().iUid[client].iUid == KUidSecureWidgetUi
							&& OviStoreRunning() )
						{
						if (oviStoreIap == KErrNotFound)
							{
							//LOG2( "Possible OviStore IAP %d", iapId );
							oviStoreIap = iapId;
							}
						else
							{
							// Another secure widget is running, we cannot determine
							// which one is ovi store, default to browser IAP
							oviStoreIap = 0;
							}
						}
					}
				}
			else
				{
				LOG2( "GetClientInfo failed (%d)", status.Int() );
				}
			// ignore any errors in KClientInfo query

			// If we found Ovi Store IAP, use that
			if (oviStoreIap > 0)
				{
				LOG2( "Sharing Ovi Store IAP %d", oviStoreIap );
				iapId = oviStoreIap;
				}

			if ( iIapArray.Count() > 0 )
				{
				// Go through all the SNAP's IAPs
				for (TInt iap = 0; iap < iIapArray.Count(); iap++)
					{
					if ( iapId == iIapArray[iap] )
						{
						LOG3( "Using %d (SNAP idx %d)", iapId, iap );
						// Update the IAP to match the found one
						aIAP = iapId;
						ret = ETrue;
						break;
						}
					}
				}
			else if ( iapId == aIAP )
				{
				LOG2( "Using %d", iapId );
				ret = ETrue;
				// we found matching IAP ID, break out of the loop
				break;
				}
			}
		else
			{
			LOG3( "GetConnInfo[%d] failed (%d), trying next", i, err );
			// try next connection
			}
		} // for
#if 0
	// If we didn't have any connections up, try to find the last used browser
	// IAP from the SNAP and use that:
	if (!ret && count == 0 && iIapArray.Count() > 0 )
		{
		// Go through all the SNAP's IAPs
		for (TInt iap = 0; iap < iIapArray.Count(); iap++)
			{
			if ( iapId == iIapArray[iap] )
				{
				LOG3( "Using %d (SNAP idx %d)", iapId, iap );
				// Update the IAP to match the found one
				aIAP = iapId;
				ret = ETrue;
				break;
				}
			}
		}
#endif
	LOG3( "- IsIapActive(): %d = %d", aIAP, ret );

	return ret;
	}
#endif // FEATURE_BOOTSTRAP_SETIAP

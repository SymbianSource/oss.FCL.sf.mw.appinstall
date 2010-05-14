/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __UNINSTALLATIONPROCESSOR_H__
#define __UNINSTALLATIONPROCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include "sisregistryfiledescription.h"
#include "processor.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "registrywrapper.h"
#else
#include "sisregistrywritablesession.h"
#endif

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CIntegrityServices;
#endif

namespace Swi
{
class CApplication;
class CSecurityManager;
class RSisHelper;
class RUiHandler;
class CHashContainer;
class CPlan;

/**
 * This class processes a CApplication created by the installation planner
 * @released
 * @internalTechnology 
 */
class CUninstallationProcessor : public CProcessor
	{
public:
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new CUninstallationProcessor
	 * @param aPlan              	Installation plan
	 * @param aUiHandler         	Interface for progress notification
	 * @param aStsSession 			Used to rollback installation process
	 * @param aRegistryWrapper		Used for registy operations
	 * @param aObserver			 	SWI Observer sessision handle which is used to log SWI events.
	 * @return                   	New application processor object
	 */
	static CUninstallationProcessor* NewL(const CPlan& aPlan, RUiHandler& aUiHandler, 
			Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
			RSwiObserverSession& aObserver);
#else
	/**
	 * Creates a new CUninstallationProcessor
	 * @param aPlan              Installation plan
	 * @param aUiHandler         Interface for progress notification
	 * @param aIntegrityServices Used to rollback installation process
	 * @param aObserver			 SWI Observer sessision handle which is used to log SWI events.
	 * @return                   New application processor object
	 */
	static CUninstallationProcessor* NewL(const CPlan& aPlan, RUiHandler& aUiHandler, 
			CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver);
#endif
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new CUninstallationProcessor and places it on the cleanup stack
	 * @param aPlan              	Installation plan
	 * @param aUiHandler         	Interface for progress notification
	 * @param aStsSession 			Used to rollback installation process
	 * @param aRegistryWrapper		Used for registy operations
	 * @param aObserver			 	SWI Observer sessision handle which is used to log SWI events.
	 * @return                   	New application processor object on the 
	 *                           	cleanup stack
	 */
	static CUninstallationProcessor* NewLC(const CPlan& aPlan, RUiHandler& aUiHandler, 
			Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
			RSwiObserverSession& aObserver);
#else
	/**
	 * Creates a new CUninstallationProcessor and places it on the cleanup stack
	 * @param aPlan              Installation plan
	 * @param aUiHandler         Interface for progress notification
	 * @param aIntegrityServices Used to rollback installation process
	 * @param aObserver			 SWI Observer sessision handle which is used to log SWI events.
	 * @return                   New application processor object on the 
	 *                           cleanup stack
	 */
	static CUninstallationProcessor* NewLC(const CPlan& aPlan, RUiHandler& aUiHandler, 
			CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver);
#endif

	static CUninstallationProcessor* NewL(CUninstallationProcessor& aProcessor);
	
	virtual ~CUninstallationProcessor();
	
private:
	CUninstallationProcessor(const CPlan& aPlan, RUiHandler& aUiHandler, 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
#else
			CIntegrityServices& aIntegrityServices,
#endif
			RSwiObserverSession& aObserver);
		
	void ConstructL();

	/**
	 * Resets member data ready for processing a new application
	 */
	void Reset();

	// Functions which get called by CProcessor::RunL to do the work
private:
	virtual TBool DoStateInitializeL();
	virtual TBool DoStateProcessEmbeddedL();
	virtual TBool DoStateExtractFilesL();
	virtual TBool DoStateVerifyPathsL();
	virtual TBool DoStateInstallFilesL();
	virtual TBool DoStateDisplayFilesL();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	virtual TBool DoParseApplicationRegistrationFilesL();	
#endif	
	virtual TBool DoStateUpdateRegistryL();
	virtual TBool DoStateProcessSkipFilesL();

	void DoCancel();

	CUninstallationProcessor& EmbeddedProcessorL();
private:
	/// Processes embedded applications
	CUninstallationProcessor* iEmbeddedProcessor;
		
	TInt iCurrent;
	};

} // namespace Swi

#endif

/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SWIS install state machine
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __SWIS_INSTALLMACHINE_H__
#define __SWIS_INSTALLMACHINE_H__

#include "statemachine.h"
#include "securitymanager.h"
#include "dessisdataprovider.h"
#include "siscontentprovider.h"
#include "swi/launcher.h"
#include "swi/sispackagetrust.h"
#include "swi/sistruststatus.h"


namespace Swi
{

class CInstallationProcessor;
class CInstallationPlanner;
class CPlan;
class RSisLauncherSession;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CNativeComponentInfo;
#endif


/**
 * State machine implementation for installation. Handles interactions between
 * SISHelper, UISS, Installation Planner, and Integrity Support.
 *
 * @internalComponent
 * @released
 */
class CInstallMachine : public CSwisStateMachine
	{
public:
	/**
	 * Base class for all other install states. Keeps a reference to the state
	 * machine object.
	 *
	 * @internalComponent
	 * @released
	 */
	class TInstallState : public CSwisStateMachine::TState
		{
	public:
		TInstallState(CInstallMachine& aInstallMachine);
	protected:
		CInstallMachine&	iInstallMachine;
		};
	
	/**
	 * This state retrieves raw controller data from SISHelper, which is
	 * necessary for the planner.
	 *
	 * @internalComponent
	 * @released
	 */
	class TGetControllerState : public TInstallState
		{
	public:
		TGetControllerState(CInstallMachine& aInstallMachine);
		/**
		 * Gets raw controller data from SISHelper, creates descriptor data 
		 * provider for that data, creates CController member 
		 * (CInstallMachine::iController) from the data provider. Using the 
		 * controller object determines whether or not the controller is signed,
		 * and sets up the following state accordingly:
		 * @li if the controller is signed, the next state will be  
		 *     TVerifyControllerState
		 * @li if the controller signed, the next state will be 
		 *     TPlanInstallationState
		 */
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This state asks user whether or not she wants to install the package.
	 */
	class TConfirmationState : public TInstallState
		{
	public:
		TConfirmationState(CInstallMachine& aInstallMachine);
		
		/**
		 * Retrieves logo, end certificates and calls Install Dialog
		 * using UISS.
		 */
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This state verifies controller signatures and performs OCSP check if
	 * necessary. This is done before planning.
	 *
	 * @internalComponent
	 * @released
	 */
	class TVerifyControllerState : public TInstallState
		{
	public:
		TVerifyControllerState(CInstallMachine& aInstallMachine);
		/// Starts asynchronous signature verification using security manager
		virtual void    EnterL();
		/// Handle any signature/certificate validation errors
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		
	private:
		/**
		 * Displays security alert to the user and optionally allows her to
		 * override the error/warning
		 *
		 * @param aCanOverride Flag that controls whether or not the user can 
		 *                     override the error or warning and continue 
		 *                     installation anyway
		 *                     ETrue  User can override the warning and 
		 *                                 continue installation anyway
		 *                     EFalse User cannot override the error, 
		 *                                 installation will be aborted
		 *
		 * @return ETrue  Continue installation (only if aCanOverride is not
		 *                EFalse, in which case the return value is ignored)
		 *		   EFalse Abort installation
		 */
		TBool SecurityAlertL(TBool aCanOverride);
		};
	
	/**
	 * This state verifies certificate chains using OCSP.
	 *
	 * @internalComponent
	 * @released
	 */
	class TOcspState : public TInstallState
		{
	public:
		TOcspState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
	private:
		TBool iNeedOcsp;
		};
	
	/**
	 * This state ensures that the SIS file is compatible with the device
	 * and the device state.
	 *
	 * @internalComponent
	 * @released
	 */
	class TCheckPrerequisitesState : public TInstallState
		{
	public:
		TCheckPrerequisitesState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
		};

	/**
	 * This state calls the planner to plan the installation.
	 */
	class TPlanInstallationState : public TInstallState
		{
	public:
		TPlanInstallationState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
		};
	
	/**
	 * This state ensures that the plan is compatible with the device
	 * and the device state.
	 *
	 * @internalComponent
	 * @released
	 */
	class TCheckPostrequisitesState : public TInstallState
		{
	public:
		TCheckPostrequisitesState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
		};


	/**
	 * This state communicates with Integrity Support to carry out the plan.
	 *
	 * @internalComponent
	 * @released
	 */
	class TIntegritySupportState : public TInstallState
		{
	public:
		TIntegritySupportState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
		};
	
	/**
	 * This is the final state that is entered when installation finishes.
	 *
	 * @internalComponent
	 * @released
	 */
	class TFinalState : public TInstallState
		{
	public:
		TFinalState(CInstallMachine& aInstallMachine);
		virtual void	EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void	Cancel();
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	private:	
		void SendBackComponentInfoL();
		
		// Populate the component information in to the NativeComponentInfo object.
		void PopulateNativeComponentInfoL(const CController& aController, 
							CNativeComponentInfo* aNativeComponentInfo);
							
		void PopulateInstallStatusL(const CController& aController, 
						Usif::RSoftwareComponentRegistry& aScrSession, 
						CNativeComponentInfo* aNativeComponentInfo);
	#endif
		};
	
	/**
	 * Utility class that holds current controller information.
	 */
	class TControllerInfo
		{
	public:
		TControllerInfo(const CController& aController, TInt64 aDataOffset);
		const CController& Controller() const;
		TInt64 DataOffset() const;			

	private:
		const CController& iController;
		TInt64 iDataOffset;	
		};
	
	/**
	 * This state registers to the SWI Observer and obtains the log file handle.
	 */
	class TRegistrationState : public TInstallState
		{
	public:
		TRegistrationState(CInstallMachine& aInstallMachine);

		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
		
	// The friend declarations are needed to access state machine's members
	// from the states.
	friend class TRegistrationState;
	friend class TGetControllerState;
	friend class TConfirmationState;
	friend class TVerifyControllerState;
	friend class TOcspState;
	friend class TCheckPrerequisitesState;
	friend class TPlanInstallationState;
	friend class TCheckPostrequisitesState;
	friend class TIntegritySupportState;
	friend class TFinalState;

public:
	/**
	 * Creates a new installation state machine.
	 *
	 * @param aMessage Message with the initial request.
	 *
	 * @return New installation state machine.
	 */
	static CInstallMachine* NewL(const RMessage2& aMessage);
	
	/**
	 * Creates a new installation state machine on the cleanup stack.
	 *
	 * @param aMessage Message with the initial request.
	 *
	 * @return New installation state machine on the cleanup stack.
	 */
	static CInstallMachine* NewLC(const RMessage2& aMessage);
	
	virtual ~CInstallMachine();
	virtual TInt Start();

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new installation state machine.
	 *
	 * @param aMessage Message with the initial request.
	 * 
	 * @param aInInfoMode to set Information Mode on
	 *
	 * @return New installation state machine.
	*/
	static CInstallMachine* NewL(const RMessage2& aMessage,const TBool aInInfoMode);
		
	/**
	 * Creates a new installation state machine on the cleanup stack.
	 *
	 * @param aMessage Message with the initial request.
	 * 
	 * @param aInInfoMode to set Information Mode on
	 *
	 * @return New installation state machine on the cleanup stack.
	 */
	static CInstallMachine* NewLC(const RMessage2& aMessage, const TBool aInInfoMode);
	#endif

protected:
	virtual TState* FirstState();
	virtual TState* ErrorOnStateEntryL(TInt aError);
	
	/**
	 * Checks if all capabilities requested by controller are signed for.
	 * Asks the user to grant user capabilities that were not signed for.
	 */
	void CheckAndGrantCapabilitiesL(RPointerArray<CFileDescription>& aFilesToCapabilityCheck);
	/**
	 * Checks if the device ID of the mobile is supported by 
	 * the Certs constaints
	 */	
	void CheckDeviceIdConstraintsL();

	virtual void SignalCompletedL();
	
	virtual void PostJournalFinalizationL(TInt aError);
	
	void SetProgressBarFinalValueL();

private:
	CInstallMachine(const RMessage2& aMessage);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CInstallMachine(const RMessage2& aMessage,const TBool aInInfoMode);
	#endif

	void ConstructL();
		
	/** Set the trust for the current controller
	@param aTrust Trust to be associated with the controller
	@deprecated in favour of new trust value - TSisTrustValue 
	*/
	void SetTrust(TSisPackageTrust aTrust);
	TSisPackageTrust Trust();

	const Sis::CController& CurrentController();
	
	/*
	* Get the set of capabilities required for the executables in the given component.
	*/	
	void GetRequestedCapabilities(TCapabilitySet& aRequestedCaps, RPointerArray<CFileDescription>& aFilesToCapabilityCheck);	
	
	/**
	Returns the main controller of the SIS file
	*/	
	const Sis::CController& MainController();
	
    /**
     * Assigns a validation status to the current controller.
     *
     * @param status The validation result to be assigned to the current
     * controller
     */
    void SetValidationStatus(TValidationStatus status);
    
    /**
     * Assigns a revocation status to the current controller.
     *
     * @param status The revocation result to be assigned to the current
     * controller
     */
    void SetRevocationStatus(TRevocationStatus status);
    
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    void SetUserGrantableCapabilities(TCapabilitySet aCapabilitySet);
    #endif

	void RunFileL(RSisLauncherSession& aLauncher, const TDesC& aFileName,
			const TDesC& aMimeType, Sis::TSISFileOperationOptions aFileOperationOption);

	void PublishPropertiesL();

    
    // States
    TRegistrationState			iRegistrationState;
	TGetControllerState			iGetControllerState;
	TConfirmationState          iConfirmationState;
	TVerifyControllerState		iVerifyControllerState;
	TOcspState                  iOcspState;
	TCheckPrerequisitesState	iCheckPrerequisitesState;
	TPlanInstallationState		iPlanInstallationState;
	TCheckPostrequisitesState	iCheckPostrequisitesState;
	TIntegritySupportState		iIntegritySupportState;
	TFinalState					iFinalState;
	
	// The rest
	CInstallPrefs*         iInstallPrefs; ///< Installation preferences
	RSisHelper             iSisHelper; ///< SISHelper session
	HBufC8*                iControllerData; ///< Raw uncompressed data
	Sis::CController*      iController; ///< Saved SISController
	CContentProvider*      iContentProvider; ///< Saved SIS content provider
	CContentProvider*      iCurrentContentProvider; ///< Point to the current Controller's ContentProvider.
	CPlan*                 iPlan; ///< Created by planner, passed to integrity
	CInstallationResult*   iResult;
	CInstallationProcessor* iProcessor; ///< Installs/removes/registers etc.
	CSecurityManager*      iSecurityManager;
	
	/// This gets inited by the security manager
	TSignatureValidationResult      iSigValidationResult;
	/// OCSP result dialog message.
	TRevocationDialogMessage        iOcspMsg;
	/// Certificate validation results
	RPointerArray<CPKIXValidationResultBase> iPkixResults;
	/// End certificates validated by security manager
	RPointerArray<CX509Certificate> iCertificates;
	/// Certificate info array for the UI.
	RPointerArray<CCertificateInfo> iCertInfos;
	/// OCSP outcomes.
	RPointerArray<TOCSPOutcome>     iOcspOutcomes;
	/// Capabilities associated by the security manager to the SISX file
	TCapabilitySet                  iGrantableCapabilitySet;
	/// Set by the security manager from policy.
	TBool                           iAllowUnsigned;
	// The installation planner
	CInstallationPlanner* iPlanner;
	// Initially set to ETrue for the main SIS controller, EFalse for all other controllers
	TBool iEmbedded;
	// The IAP to use for operation that involve talking to remote servers
	TUint32 iIap;
	// The set of languages that Device supports
	RArray<TInt> iDeviceSupportedLanguages;

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CNativeComponentInfo* iNativeComponentInfo;
	#endif

	}; // class CInstallMachine

} // namespace Swi

#endif

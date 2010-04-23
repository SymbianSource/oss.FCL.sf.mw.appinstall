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
* SWIS state machine
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __SWIS_STATEMACHINE_H__
#define __SWIS_STATEMACHINE_H__

#include <e32std.h>
#include <e32base.h>
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#include "registrywrapper.h"
#else
#include "integrityservices.h"
#include "sisregistrywritablesession.h"
#endif
#include "sishelperclient.h"
#include "sisuihandler.h"
#include "swi/sisinstallationresult.h"
#include "securitymanager.h"
#include "swiobserverclient.h"


namespace Swi
{
class CPlan;

/** Panic ID for panics raised in state machine */
enum StateMachinePanic
	{
	EStateError
	};

/**
 * State machine base absrtract class. A state machine in this implementation 
 * represents one task for SWIS (installation or uninstallation).
 *
 * @internalComponent
 * @released
 */
class CSwisStateMachine : public CActive
	{
public:
	virtual ~CSwisStateMachine();
	virtual TInt Start();
	virtual void PostJournalFinalizationL(TInt aError);
	/**
	 * Abstract base class for all states of a state machine.
	 *
	 * @internalComponent
	 * @released
	 */
	class TState
		{
	public:
		virtual void    EnterL()=0;
		virtual TState* CompleteL()=0;
		virtual TState* ErrorL(TInt aCode)=0;
		virtual void    Cancel()=0;
		};

	void CancelInstallation();
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Returns the mode in which state machine operates on.
	// Will return TRUE if it operates on component information extraction mode, 
	// FALSE when it operates on normal installtion/uninstallation mode ( By default ).
	TBool IsInInfoMode();
	
	// Sets the state machine to operate on information exteraction mode. 
	void SetIsInInfoMode(TBool aMode);
	#endif

protected:
	CSwisStateMachine(const RMessage2& aMessage);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CSwisStateMachine(const RMessage2& aMessage,const TBool aInInfoMode);
	#endif
	
	/** Must be called in derived classes */
	void ConstructL();
	void ChangeStateL(TState* aNextState);
	inline TState* CurrentState();
	void SetNextState(TState* aNextState);
	void CompleteSelf();

	void FinalizeJournalsL(TInt aError);
	void CompleteClientL(TInt aError);
	RUiHandler& UiHandler();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& TransactionSession();
	CRegistryWrapper& RegistryWrapper();
	void SetFinalProgressBarValue(TInt aValue);
	// Is the state machine runs in information extraction mode or normal installation/uninstallation mode?
	TBool iIsInInfoMode;
#else
	CIntegrityServices& IntegrityServicesL();
#endif

	virtual TState* FirstState()=0;
	virtual TState* ErrorOnStateEntryL(TInt aError)=0;

	const RMessage2& Message() const;	

// These are overridden and used to signal the UI	
	virtual void SignalCompletedL()=0;

// Used to do signalling to the UI, copes with the plan not having a valid
// TAppInfo yet
	void HandleInstallationEventL(CPlan* plan, TInstallEvent aEvent, TInt aValue = 0);
	RSwiObserverSession& Observer();
private:
	// from CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void InformUserOfFatalErrorL(TInt aError);

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void ResetRegistryCache(void);
#endif

protected:
	// The confirmation state; where the user elects to go ahead with
	// the installation/un-installation; has been entered. Therefore 
	// the registery MIGHT have been altered.
	TBool iOperationConfirmed;
	
private:
	TState*             iState;             ///< Curent state
	const RMessage2     iMessage;           ///< Message to complete when done
	RUiHandler          iUiHandler;         ///< UI handler server session

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession	iStsSession;		///< Transaction service session handle
	CRegistryWrapper* 	iRegistryWrapper;	///< Registry wrapper to access the registry
	CProgressBarValuePublisher* iProgressPublisher;	///< Pointer to the progress bar value publisher object..
#else
	CIntegrityServices* iIntegrityServices; ///< Shared integrity services.
#endif

	TBool iCancelled;
	RSwiObserverSession iObserver;			///< Swi Observer session handle.
	}; // class CSwisStateMachine

inline CSwisStateMachine::TState* CSwisStateMachine::CurrentState() 
	{
	return iState;
	};

inline RUiHandler& CSwisStateMachine::UiHandler()
	{
	return iUiHandler;
	}

inline const RMessage2& CSwisStateMachine::Message() const
	{
	return iMessage;
	}	
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline Usif::RStsSession& CSwisStateMachine::TransactionSession()
	{
	return iStsSession;
	}

inline CRegistryWrapper& CSwisStateMachine::RegistryWrapper()
	{
	return *iRegistryWrapper;
	}
#else
inline CIntegrityServices& CSwisStateMachine::IntegrityServicesL()
	{
	if (!iIntegrityServices)
		{
		User::Leave(KErrBadUsage);
		}

	return *iIntegrityServices;
	}
#endif

inline RSwiObserverSession& CSwisStateMachine::Observer()
	{
	return iObserver;
	}

} // namespace Swi

#endif

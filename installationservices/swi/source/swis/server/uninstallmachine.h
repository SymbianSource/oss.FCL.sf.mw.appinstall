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
* SWIS uninstall state machine
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __SWIS_UNINSTALLMACHINE_H__
#define __SWIS_UNINSTALLMACHINE_H__

#include "statemachine.h"

namespace Swi
{
class CUninstallationProcessor;
class CUninstallationPlanner;
class CSisRegistryPackage;
class CPlan;

/**
 * State machine implementation for uninstallation. Handles interactions 
 * between UISS, Installation Planner, and Integrity Support.
 *
 * @internalComponent
 * @released
 */
class CUninstallMachine : public CSwisStateMachine
	{
public:
	/**
	 * Base class for all other uninstall states. Keeps a reference to the 
	 * state machine object.
	 *
	 * @internalComponent
	 * @released
	 */
	class TUninstallState : public CSwisStateMachine::TState
		{
	public:
		TUninstallState(CUninstallMachine& aUninstallMachine);
	protected:
		CUninstallMachine& iUninstallMachine;
		};
	
	/**
	 * This state calls the uninstall dialog to confirm uninstallation with 
	 * the user.
	 */
	class TConfirmationState : public TUninstallState
		{
	public:
		TConfirmationState(CUninstallMachine& aUninstallMachine);
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This state calls the planner to plan the uninstallation.
	 */
	class TPlanUninstallationState : public TUninstallState
		{
	public:
		TPlanUninstallationState(CUninstallMachine& aUninstallMachine);
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This state communicates with Integrity Support to carry out the plan.
	 *
	 * @internalComponent
	 * @released
	 */
	class TIntegritySupportState : public TUninstallState
		{
	public:
		TIntegritySupportState(CUninstallMachine& aUninstallMachine);
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This is the final state that is entered when uninstallation finishes.
	 *
	 * @internalComponent
	 * @released
	 */
	class TFinalState : public TUninstallState
		{
	public:
		TFinalState(CUninstallMachine& aUninstallMachine);
		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
	
	/**
	 * This state registers to the SWI Observer and obtains the log file handle.
	 */
	class TRegistrationState : public TUninstallState
		{
	public:
		TRegistrationState(CUninstallMachine& aUninstallMachine);

		virtual void    EnterL();
		virtual TState* CompleteL();
		virtual TState* ErrorL(TInt aCode);
		virtual void    Cancel();
		};
		
	// The friend declarations are needed to access state machine's members 
	// from the states.
	friend class TRegistrationState;
	friend class TConfirmationState;
	friend class TPlanUninstallationState;
	friend class TIntegritySupportState;
	friend class TFinalState;

	CUninstallMachine(const RMessage2& aMessage);
	
	void ConstructL();
public:
	static CUninstallMachine* NewL(const RMessage2& aMessage);
	
	virtual ~CUninstallMachine();
	virtual TInt Start();
	
protected:
	virtual TState* FirstState();
	virtual TState* ErrorOnStateEntryL(TInt aError);

	virtual void SignalCompletedL();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	virtual void PostJournalFinalizationL(TInt aError);
#endif

private:
	// States
	TRegistrationState		 iRegistrationState;
	TConfirmationState       iConfirmationState;
	TPlanUninstallationState iPlanUninstallationState;
	TIntegritySupportState   iIntegritySupportState;
	TFinalState              iFinalState;
	
	// The rest
	CUninstallationPlanner* iPlanner;
	
	CSisRegistryPackage *iPackage; // the package to uninstall

	CPlan*     iPlan; ///< Created by planner

	CInstallationResult*   iResult;
	CUninstallationProcessor* iProcessor; ///< removes/unregisters etc.
	}; // class CUninstallMachine

} // namespace Swi

#endif

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
*     CBootstrapAppUi application UI class declaration.
*
*
*/


#ifndef __BOOTSTRAPAPPUI_H__
#define __BOOTSTRAPAPPUI_H__

// INCLUDES
#include <eikapp.h>
#include <aknappui.h>
#include <aknglobalconfirmationquery.h>
#include "debug.h"

// FORWARD DECLARATIONS
class CAknGlobalNote;
class CGlobalWaitNoteObserver;
class CStateMachine;

enum AppErrorId
		{
		ENoAppError = 0,					//0
		EInstallationFailed,
		EDownloadFailed				//2
		};

// CLASS DECLARATION
/**
* CBootstrapAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CBootstrapAppUi : public CAknAppUi
		{
public: // Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CBootstrapAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CBootstrapAppUi();

	/**
	 * ~CBootstrapAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CBootstrapAppUi();

	/**
	 * ExitApp
	 * Handle exit of App for success as well failures on all cases
	 */
	void ExitApp(TInt aAppState);

	/**
	 * HandleGlobalWaitNoteCancel
	 * Handle user press cancel
	 */
	void HandleGlobalWaitNoteCancel(const TInt& aStatus);

	/**
	 * SetErrorId
	 * Set the Error Id - to determine the type of error note displayed.
	 */
	inline void SetErrorId( AppErrorId aErrorId )
		{
		iErrorId = aErrorId;
		};

	inline CEikonEnv* EikonEnv() const {return iEikonEnv;}

private:

	/**
	 * InitializeSIL
	 * Init the app and start the SM.
	 */
	void InitializeSIL();

	/**
	 * WaitForInstallCompleteL
	 * Subscribe to the installer and get the wrapper UID (latest installation) once installer is free.
	 */
	void WaitForInstallCompleteL();

	/**
	 * HandleForegroundEventL
	 * Push the application to background.
	 * Called by the framework when the application switches
	 * to foreground.
	 */
	void HandleForegroundEventL(TBool aForeground);

	/**
	 * SendToBackgroundL
	 * Sends application to background.
	 */
	void SendToBackgroundL(TBool aBackground = ETrue);

	/**
	 * ShowGlobalWaitNoteL
	 * Show the Global wait note. (starts in EBSStateInit)
	 */
	void ShowGlobalWaitNoteL();

	/**
	 * StopGlobalWaitNoteL
	 * Stop the Global wait note incase of failure.
	 */
	void StopGlobalWaitNoteL();

	/**
	 * ShowNoteL
	 * Show an error note incase of failure.
	 */
	void ShowNoteL( const TAknGlobalNoteType& aType , const TInt& aResourceId );

	/**
	 * GetLatestInstalledL
	 * Get the UID of latest installation.
	 */
	TUint32 GetLatestInstalledL();

private: // Data
	friend class CStateMachine;

	CStateMachine* iStateMachine;  //< owns

	CAknGlobalNote* iGlobalWaitNote; //< owns

	CGlobalWaitNoteObserver* iGlobalWaitNoteObserver; //< owns

	/**
	 * ETrue, if Ovi Store client is running
	 */
	TBool iOviStoreRunning;

	/**
	 * Wrapper package UID
	 */
	TUint32 iWrapperPackageUid;

	/**
	 * Can Bootstrap UI be moved to foreground
	 */
	TBool iAllowForeground;

	/**
	 * WaitNoteId
	 */
	TInt iGlobalWaitNoteId;

	/**
	 * ErrorId - used to determine whether and type of error note to be displayed
	 */
	AppErrorId iErrorId;

	INIT_OBJECT_DEBUG_LOG
};

#endif // __BootstrapAPPUI_H__

// End of File


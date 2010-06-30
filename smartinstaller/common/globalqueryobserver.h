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
*     Declares CGlobalQueryObserver class for application.
*
*
*/

#ifndef __GLOBALQUERYOBSERVER_H__
#define __GLOBALQUERYOBSERVER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

/**
* CGlobalQueryObserver class
*/
class CGlobalQueryObserver :
	public CActive
	{
public:  // Constructors and destructor
	class MGlobalQueryClient
		{
	public:
		virtual void HandleGlobalQueryResponseL(TInt aStatus) = 0;
		};
	/**
	* NewL
	* Two-phased constructor.
	*/
	static CGlobalQueryObserver* NewL(MGlobalQueryClient* aClient, const TBool aIgnoreResponse);

	/**
	* ~CGlobalQueryObserver
	* Destructor.
	*/
	virtual ~CGlobalQueryObserver();

public:     // New function
	/**
	* Start
	* Start the activator.
	*/
	void Start();

private:    // New function
	/**
	* CGlobalQueryObserver
	* C++ default constructor.
	*/
	CGlobalQueryObserver(MGlobalQueryClient* aClient, const TBool aIgnoreResponse);

	/**
	* ConstructL
	* Symbian default constructor.
	*/
	void ConstructL();

private:    // From CActive
	/**
	* RunL
	* From CActive, handle an event.
	*/
	void RunL();

	/**
	* DoCancel
	* From CActive, implement the Cancel protocol.
	*/
	void DoCancel();

private: // Data
	/**
	 * Callback
	 */
	MGlobalQueryClient* iClient;

	/**
	 * Set to true to ignore query response.
	 */
	const TBool iIgnoreResponse;
};

#endif // __GLOBALQUERYOBSERVER_H__

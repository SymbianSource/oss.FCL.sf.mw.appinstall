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


#ifndef __BOOTSTRAPGLOBALQUERYOBSERVER_H__
#define __BOOTSTRAPGLOBALQUERYOBSERVER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CADMAppUi;

/**
* CGlobalQueryObserver class
*/
class CGlobalQueryObserver:
	public CActive
	{
	public:  // Constructors and destructor
		/**
		* NewL
		* Two-phased constructor.
		*/
		static CGlobalQueryObserver* NewL(CADMAppUi* aCBootstrapAppUiObj);

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

		/*
		* SetSelectItemFlag
		* Set iSelectItemFlag.
		*/
		void SetSelectItemFlag();

		/*
		* UnsetSelectItemFlag
		* Unset iSelectItemFlag.
		*/
		void UnsetSelectItemFlag();

	private:    // New function
		/**
		* CGlobalQueryObserver
		* C++ default constructor.
		*/
		CGlobalQueryObserver(CADMAppUi* aCBootstrapAppUiObj);

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
		* iSelectItemFlag
		*/
		TBool iSelectItemFlag;

		/**
		*Call Back
		*/
		CADMAppUi* iCBootstrapAppUiObj;
	};

#endif // __BOOTSTRAPGLOBALQUERYOBSERVER_H__


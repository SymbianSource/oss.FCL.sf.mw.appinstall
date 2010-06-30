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
*     Declaration of CBootstrapDocument class.
*
*
*/


#ifndef __BOOTSTRAPDOCUMENT_H__
#define __BOOTSTRAPDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CBootstrapAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CBootstrapDocument application class.
* An instance of class CBootstrapDocument is the Document part of the
* AVKON application framework for the Bootstrap application.
*/
class CBootstrapDocument : public CAknDocument
	{
	public: // Constructors and destructor

		/**
		* NewL.
		* Two-phased constructor.
		* Construct a CBootstrapDocument for the AVKON application aApp
		* using two phase construction, and return a pointer
		* to the created object.
		* @param aApp Application creating this document.
		* @return A pointer to the created instance of CBootstrapDocument.
		*/
		static CBootstrapDocument* NewL( CEikApplication& aApp );

		/**
		* NewLC.
		* Two-phased constructor.
		* Construct a CBootstrapDocument for the AVKON application aApp
		* using two phase construction, and return a pointer
		* to the created object.
		* @param aApp Application creating this document.
		* @return A pointer to the created instance of CBootstrapDocument.
		*/
		static CBootstrapDocument* NewLC( CEikApplication& aApp );

		/**
		* ~CBootstrapDocument
		* Virtual Destructor.
		*/
		virtual ~CBootstrapDocument();

	public: // Functions from base classes

		/**
		* CreateAppUiL
		* From CEikDocument, CreateAppUiL.
		* Create a CBootstrapAppUi object and return a pointer to it.
		* The object returned is owned by the Uikon framework.
		* @return Pointer to created instance of AppUi.
		*/
		CEikAppUi* CreateAppUiL();

	private: // Constructors

		/**
		* ConstructL
		* 2nd phase constructor.
		*/
		void ConstructL();

		/**
		* CBootstrapDocument.
		* C++ default constructor.
		* @param aApp Application creating this document.
		*/
		CBootstrapDocument( CEikApplication& aApp );

	};

#endif // __BOOTSTRAPDOCUMENT_H__

// End of File


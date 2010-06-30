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
*     Declares document class for application.
*
*
*/


#ifndef __ADMDOCUMENT_h__
#define __ADMDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CADMAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CADMDocument application class.
 * An instance of class CADMDocument is the Document part of the
 * AVKON application framework for the ADM example application.
 */
class CADMDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CADMDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CADMDocument.
	 */
	static CADMDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CADMDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CADMDocument.
	 */
	static CADMDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CADMDocument
	 * Virtual Destructor.
	 */
	virtual ~CADMDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CADMAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CADMDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CADMDocument(CEikApplication& aApp);

	};

#endif // __ADMDOCUMENT_h__
// End of File

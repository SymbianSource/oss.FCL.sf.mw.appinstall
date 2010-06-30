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
*     Main Application File for Bootstrap
*
*
*/


#ifndef __BOOTSTRAPAPPLICATION_H__
#define __BOOTSTRAPAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CBootstrapApplication application class.
* Provides factory to create concrete document object.
*/
class CBootstrapApplication : public CAknApplication
	{
	public: // Functions from base classes

		/**
		* From CApaApplication, AppDllUid.
		* @return Application's UID
		*/
		TUid AppDllUid() const;

	protected: // Functions from base classes

		/**
		* From CApaApplication, CreateDocumentL.
		* Creates CCBootstrapDocument document object. The returned
		* pointer in not owned by the CBootstrapApplication object.
		* @return A pointer to the created document object.
		*/
		CApaDocument* CreateDocumentL();
	};

#endif // __BOOTSTRAPAPPLICATION_H__

// End of File

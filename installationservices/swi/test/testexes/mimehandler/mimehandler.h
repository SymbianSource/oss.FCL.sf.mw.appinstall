/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tmimehandler.h
*
*/


#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coecntrl.h>
#include <e32base.h>
#include <eikstart.h>
#include <viewcli.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <s32file.h>

/**
 * Main application UI class. 
 */

class CMimeAppUi : public CEikAppUi
    {
    
	public:
		CMimeAppUi();
		~CMimeAppUi();
    	void ConstructL();
    	void DoExitL();
    
    	TBool ProcessCommandParametersL(TApaCommand aCommand,
    			TFileName& aDocumentName,const TDesC8& /*aTail*/);
    	TBool ProcessCommandParametersL(CApaCommandLine& aCommandLine);		
    }; 


// ===========================================================================
// CMimeDocument
// ===========================================================================

class CMimeApplication;

class CMimeDocument : public CEikDocument
/** 
 * Main application document
 */
	{
	public: // from CEikDocument
		CMimeDocument(CMimeApplication& aApp);
	    CEikAppUi* CreateAppUiL();
	};


// ===========================================================================
// CMimeApplication
// ===========================================================================

class CMimeApplication : public CEikApplication
/** 
 * Main application class
 */
	{
	private: // from CApaApplication
		TUid AppDllUid() const;
		CApaDocument* CreateDocumentL();
	};


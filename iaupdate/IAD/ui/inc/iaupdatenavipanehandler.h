/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Header file of CIAUpdateNaviPaneHandler class
*
*/



#ifndef IAUPDATENAVIPANEHANDLER_H
#define IAUPDATENAVIPANEHANDLER_H


//  INCLUDES
#include <e32base.h>
#include <aknnavide.h>

//FORWARD DECLARATIONS
class CEikStatusPane;


/**
* CIAUpdateNaviPaneHandler
* 
* This class is used for changing navipane title.
*/
class CIAUpdateNaviPaneHandler : public CBase
	{
    public:
        /**
        * Destructor.
        */
		virtual ~CIAUpdateNaviPaneHandler();

        /**
        * C++ default constructor.
        */
		CIAUpdateNaviPaneHandler(CEikStatusPane* aStatusPane);

	public:

		/**
		* Sets navi pane title.
		* @param aTitle.
		* @return None.
		*/
		void SetNaviPaneTitleL(const TDesC& aTitle);
		

	private:
		// status pane
		CEikStatusPane* iStatusPane;
	
	    // navi Pane
	    CAknNavigationControlContainer* iNaviPane;
        
        // navi decorator
        CAknNavigationDecorator* iNaviDecorator;
        
        // has Navidecorator been pushed into navi pane
        TBool iNavidecoratorPushed;
        
        // has navi pane been pushed into navigation pane's object stack
        TBool iNavipanePushed;
	};
#endif      // IAUPDATENAVIPANEHANDLER_H
            
// End of File

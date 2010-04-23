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
* Description:   Header file of CIAUpdateStatusPaneHandler class
*
*/



#ifndef IAUPDATESTATUSPANEHANDLER_H
#define IAUPDATESTATUSPANEHANDLER_H


//  INCLUDES
#include <e32base.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CIAUpdateNaviPaneHandler;



/**
* CIAUpdateStatusPaneHandler
* 
* This class is used for changing dialog title and image.
*/
class CIAUpdateStatusPaneHandler : public CBase
	{
    public:
        /**
        * Two-phased constructor.
        */
		static CIAUpdateStatusPaneHandler* NewL(CAknAppUi* aAppUi);

        /**
        * Destructor.
        */
		virtual ~CIAUpdateStatusPaneHandler();

    private:
        /**
        * By default Symbian OS constructor is private.
        */
		void ConstructL();

        /**
        * C++ default constructor.
        */
		CIAUpdateStatusPaneHandler(CAknAppUi* aAppUi);

	public:
		/**
		* Stores original title so it can be restored when dialog closes.
		* @param  None.
		* @return None.
		*/
        void StoreOriginalTitleL();

		/**
		* Restores original title.
		* @param  None.
		* @return None.
		*/
		void RestoreOriginalTitleL();

		/**
		* Sets dialog title.
		* @param  aText.
		* @return None.
		*/
		void SetTitleL(const TDesC& aText);

		/**
		* Sets dialog title.
		* @param  aResourceId.
		* @return None.
		*/
		void SetTitleL(TInt aResourceId);

	public:
		/**
		* Sets dialog title.
		* @param aAppUi.
		* @param aText.
		* @return Return code.
		*/
		static TBool SetTitleL(CAknAppUi* aAppUi, const TDesC& aText);

		/**
		* Gets dialog title.
		* @param aAppUi.
		* @param aText.
		* @return Return code.
		*/
		static TBool GetTitleL(CAknAppUi* aAppUi, HBufC*& aText);
		
		/**
		* Sets navi pane title.
		* @param aTitle.
		* @return None.
		*/
		void SetNaviPaneTitleL(const TDesC& aTitle);
		
	private:
		// access to app ui
		CAknAppUi* iAppUi;

		// original status pane title
		HBufC* iOriginalTitle;
		
		// is original status pane title stored
		TBool iOriginalTitleStored;

		// original context pane image
		CEikImage* iOriginalImage;
		
   	    CIAUpdateNaviPaneHandler* iNaviPaneHandler;
	};




#endif      // IAUPDATESTATUSPANEHANDLER_H
            
// End of File

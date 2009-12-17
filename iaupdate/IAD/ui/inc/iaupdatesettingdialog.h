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
* Description:   Header file of CIAUpdateSettingDialog class
*
*/




#ifndef IAUPDATESETTINGDIALOG_H
#define IAUPDATESETTINGDIALOG_H


//  INCLUDES
#include "iaupdatedialogutil.h"

#include <e32base.h>
#include <AknDialog.h>
#include <aknsettingitemlist.h>


// FORWARD DECLARATIONS
class CAknSettingItemList;
class CIAUpdateSettingItemList;
class CIAUpdateAccessPointHandler;
class CIAUpdateStatusPaneHandler;


// CLASS DECLARATION

/**
* CIAUpdateSettingDialog
*
* CIAUpdateSettingDialog is used for editing IA Update settings.
*/
class CIAUpdateSettingDialog : public CAknDialog, public MEikListBoxObserver
    {
    public:
    enum TAttributeKeys
        {
        EAccessPoint = 0,
        EAutoUpdateCheck,
        ERoamingWarning
        };
    
	public:
	
	    /**
        * Launches dialog.
        * @param aParam Class that contains dialog parameters.
        * @return Completion code.
        */
		static TBool ShowDialogL();
		
		
	public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
		static CIAUpdateSettingDialog* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateSettingDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateSettingDialog();
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

	private:
	    SEikControlInfo CreateCustomControlL( TInt aControlType );
	    
        /**
        * From MEikListBoxObserver, called by framework.
        * @param aListBox.
        * @param aEventType.
		* @return None
        */
		void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

        /**
        * From the base class.
		* Called by framework before the dialog is shown.
        * @param None
		* @return None
        */
        void PreLayoutDynInitL();

        /**
        * From the base class.
		* Called by framework before exiting the dialog.
        * @param aButtonId Button id.
		* @return ETrue to exit\ EFalse to not to exit.
        */
        TBool OkToExitL(TInt aButtonId);

        /**
        * From the base class.
		* Called by framework for key event handling.
        * @param aKeyEvent.
		* @param aType.
		* @return Return code.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * From the base class.
		* Called by framework when dialog is activated.
        * @param None.
		* @return None.
        */
		void ActivateL();
		
		/**
		* Method to get context sensitive help topic.
        * @param aContext Help topic to open.
		* @return None.
        */
		void GetHelpContext(TCoeHelpContext& aContext) const;

		/**
        * From the base class.
		* Called by framework before menu is shown.
        * @param aResourceId Menu resource id.
		* @param aMenuPane Pointer to the menu.
        * @return None.
        */
		void DynInitMenuPaneL(TInt aResourceID, CEikMenuPane* aMenuPane);

		/**
		* From the base class.
		* Handles menu events.
		* @param  aCommandId Command id.
		* @return None.
		*/
		void ProcessCommandL(TInt aCommandId);
		
		/**
		* From base class.
		* Called when UI layout changes. 
		* @param aType.
    	* @return None.
		*/
        void HandleResourceChange(TInt aType);


    private:
		/**
		* Save settings to CR.
		*/
		void SaveSettingsL();

		/**
		* Read settings from CR.
		*/
        void ReadSettingsL();
     
	private:
		/**
        * Opens setting editor.
        */
		void EditItemL();

		/**
        * Set setting item visibility (normal/readonly/hidden).
        * @param None.
		* @return None.
        */
		void SetVisibility();
		
        /**
        * Set all settings to read-only state.
        * @param None.
		* @return None.
        */
		void SetAllReadOnly();
		

	private:
		CIAUpdateSettingItemList* iList;

		// for title and icon handling
		CIAUpdateStatusPaneHandler* iStatusPaneHandler;

		// dialog edit mode
		TInt iEditMode;

    };


/**
* CIAUpdateSettingItemList
*
* Customized setting item list.
*/
class CIAUpdateSettingItemList : public CAknSettingItemList
    {
    public:
        TInt Attribute( TInt aKey );
        void CheckSettings();
        
        
    public:  // from CAknSettingItemList
        /**
        * Load settings.
        */
        void LoadSettingsL();
        
    	/**
    	* Launches the setting page for the current item by calling EditItemL on it
    	* @param aIndex	- current item's (Visible) index in the list
    	* @param aCalledFromMenu- ignored in most classes; may alter the behaviour
    	* of the setting page
    	*/
    	void EditItemL( TInt aIndex, TBool aCalledFromMenu );

        /**
        * Create setting item.
        * @param aSettingId Setting id.
        * @return Setting item.
        */
        CAknSettingItem* CreateSettingItemL( TInt aSettingId );

	private:
	    TInt iAccessPoint;
        TInt iAutoUpdateCheck;
        TInt iRoamingWarning;
        };


class CIAUpdateAccessPointSettingItem : public CAknSettingItem
	{
	public:
        static CIAUpdateAccessPointSettingItem* 
        CIAUpdateAccessPointSettingItem::NewL( TInt aSettingId, TInt& aAccessPointId );
	    ~CIAUpdateAccessPointSettingItem();
	private:
	    CIAUpdateAccessPointSettingItem ( TInt aSettingId, TInt& aAccessPointId );
	    void ConstructL();
	    

	public:	// from CAknSettingItem
		void EditItemL( TBool aCalledFromMenu );
		const TDesC& SettingTextL();
	
	private:
	    TBool EditAccessPointItemL();

    private:
        HBufC* iSettingText;
        
        TInt& iAccessPointId;
        
   		// for access point selection
		CIAUpdateAccessPointHandler* iApHandler;
	};



#endif      // IAUPDATESETTINGDIALOG_H
            
// End of File

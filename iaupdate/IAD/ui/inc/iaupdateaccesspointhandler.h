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
* Description:   Header file of CIAUpdateAccessPointHandler class
*
*/



#ifndef IAUPDATEACCESSPOINTHANDLER_H
#define IAUPDATEACCESSPOINTHANDLER_H


//  INCLUDES
#include <e32base.h>

// Access point selection
#include <cmmanager.h>

//FORWARD DECLARATIONS
class CCmApplicationSettingsUi;


/**
* CIAUpdateAccessPointHandler 
*
* CIAUpdateAccessPointHandler is for setting internet access points.
*/
class CIAUpdateAccessPointHandler : public CBase
    {
    public:
	enum TExitMode
		{
	    EDialogSelect,
	    EDialogCancel,
	    EDialogExit
		};
   
    public:
        /**
        * Two-phased constructor.
        */
        static CIAUpdateAccessPointHandler* NewL();

	    /**
        * Destructor.
        */
        virtual ~CIAUpdateAccessPointHandler();

    private:
        /**
        * C++ default constructor.
        */
	    CIAUpdateAccessPointHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    public: //functions
	    /**
        * Displays dialog for selecting one access point.
        * @return ETrue if user selected access point, EFalse otherwise.
        */
        TInt ShowApSelectDialogL( TInt& aItemUid, HBufC*& aItemName );
	          
        /**
        * Gets access point name
		* @param aItemUid Access point id
		* @paran aName Access point name
        * @return Error value.
        */
        TInt GetApNameL( TInt aItemUid, HBufC*& aItemName );
       
        /**
        * Get for Default Connection text
		* @param aLabelText Localised text for "Default connection" text
        */
        void GetDefaultConnectionLabelL( HBufC*& aLabelText ); 
    
    private:  //data
    	TBool iIdAppCsdSupport;
    	
    	CCmApplicationSettingsUi* iCmUi;
    	RCmManager iCmManager;
    };


#endif  //IAUPDATEACCESSPOINTHANDLER_H
            
// End of File

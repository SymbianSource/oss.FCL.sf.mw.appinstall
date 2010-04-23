/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateDetailsDialog
*
*/



#ifndef IAUPDATEDETAILSDIALOG_H
#define IAUPDATEDETAILSDIALOG_H


//  INCLUDES
#include <e32base.h>

#include "iaupdateversion.h"

// FORWARD DECLARATIONS
class TIAUpdateDialogParam;

/*
* CIAUpdateDetailsDialog
*
* CIAUpdateDetailsDialog is used for displaying update details.
*/
class CIAUpdateDetailsDialog : public CBase
    {
	public:
	
	    /**
        * Launches dialog.
        * @param aParam Class that contains dialog parameters.
        * @return Completion code.
        */
		static TBool ShowDialogL( TIAUpdateDialogParam* aParam );
		
	public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
		static CIAUpdateDetailsDialog* NewL( TIAUpdateDialogParam* aParam );
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateDetailsDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateDetailsDialog( TIAUpdateDialogParam* aParam );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
    private:
        void ConstructTextL();
        TInt BufferSize();
        HBufC* FileSizeTextLC( TInt aFileSize );
        HBufC* VersionTextLC( TIAUpdateVersion aVersion );
        TBool ShowDialogL();
        
    private:
        TIAUpdateDialogParam* iParam;
        
        HBufC* iBuf;
  
    };
#endif      // IAUPDATEDETAILSDIALOG_H
            
// End of File

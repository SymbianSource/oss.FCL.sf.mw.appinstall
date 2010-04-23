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



#ifndef IAUPDATEFWDETAILSDIALOG_H
#define IAUPDATEFWDETAILSDIALOG_H


//  INCLUDES
#include <e32base.h>

//FORWARD
class MIAUpdateFwNode;

/*
* CIAUpdateFwDetailsDialog
*
* CIAUpdateFwDetailsDialog is used for displaying update details.
*/
class CIAUpdateFwDetailsDialog : public CBase
    {
	public:
	
	    /**
        * Launches dialog.
        * @param aFwNode present FW node.
        * @return Completion code.
        */
		static TBool ShowDialogL( MIAUpdateFwNode* aFwNode );
		
	public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
		static CIAUpdateFwDetailsDialog* NewL( MIAUpdateFwNode* aFwNode );
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateFwDetailsDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateFwDetailsDialog( MIAUpdateFwNode* aFwNode );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
    private:
        void ConstructTextL();
        TInt BufferSize();
        HBufC* FileSizeTextLC( TInt aFileSize );
        TBool ShowDialogL();
        
    private:
        
        MIAUpdateFwNode* iFwNode;
        HBufC* iBuf;
  
    };
#endif      // IAUPDATEDETAILSDIALOG_H
            
// End of File

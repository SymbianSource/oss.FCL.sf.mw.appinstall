/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the 
*                CCUICertificateDetailsDialog class.
*
*                This dialog shows detailed information about certificates.
*
*/


#ifndef CUICERTIFICATEDETAILSDIALOG_H
#define CUICERTIFICATEDETAILSDIALOG_H

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CX509Certificate;

namespace SwiUI
{
namespace CommonUI
{

//  FORWARD DECLARATIONS
class CCUICertificateInfo;

/**
* This dialog shows detailed information about certificates.
*
* @lib swinstcommonui.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CCUICertificateDetailsDialog) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * Constructs the dialog.
        * @since 3.0
        * @return Pointer to the created certificate details dialog.
        */
        static CCUICertificateDetailsDialog* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CCUICertificateDetailsDialog();

    public: // New functions
        
        /**
        * Displays the dialog.
        * @since 3.0
        * @param aCertInfo - Certificate information object.
        */   
        void ExecuteLD( const CCUICertificateInfo& aCertInfo );      

    protected: // New functions
        
        /**
        * Adds a field (header and value) to the dialog.
        * @since 3.0
        * @param aMessage - Buffer where to append the field.
        * @param aHeaderResourceId - Resource id of the header string.
        * @param aValue - Field value.
        */     
        void AddFieldLC( HBufC*& aMessage, TInt aHeaderResourceId, const TDesC& aValue );
       
        /**
        * Converts date to local format.
        * @since 3.0
        * @param aDate - The date to be formatted.
        * @return Descriptor having the formatted date. Ownership transfers to caller.
        */    
        HBufC* DateToStringLC( const TDateTime& aDate );
	
        /**
        * Devides input descriptor to blocks.
        * @since 3.0
        * @param aInput - Input string.
        * @param aOutput - Pointer to output string.
        */
        void DevideToBlocks( const TDesC8& aInput, TPtr aOutput );

    private:
        
        /**
        * Constructor.
        */
        CCUICertificateDetailsDialog();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();       
    };
}
}


#endif      // CUICERTIFICATEDETAILSDIALOG_H
            
// End of File

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
* Description:   This file contains the header file of the CSisxUIAppInfo
*                class. 
*                
*                This class represents a native symbian package.
*
*/


#ifndef SISXUIAPPINFO_H
#define SISXUIAPPINFO_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <CUIDetailsDialog.h>
#include <swi/msisuihandlers.h>

// FORWARD DECLARATIONS

namespace SwiUI
{

// CLASS DECLARATION

/**
* This class represents a native symbian application
*
* @lib sisxui.lib
* @since 3.0
*/
class CSisxUIAppInfo : public CBase, public CommonUI::MCUIInfoIterator
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        * @param aInfo - Symbian engine application info object.
        * @param aIsTrusted - Indicate if the package is trusted or not
        */
        static CSisxUIAppInfo* NewL( const Swi::CAppInfo& aInfo, TBool aIsTrusted );
        
        /**
        * Destructor.
        */
        virtual ~CSisxUIAppInfo();

    public: // New functions

        /**
        * Return name of the package. 
        * @since 3.0
        * @return Name of the package.
        */
        const TDesC& Name() const;

        /**
        * Set the name of the package
        * @since 3.0
        * @param aName - Name of the package.
        */
        void SetNameL( const TDesC& aName );        

        /**
        * Return the package vendor. 
        * @since 3.0
        * @return Package vendor.
        */
        const TDesC& Vendor() const;   

        /**
        * Set the vendor of the package
        * @since 3.0
        * @param aVendor - Vendor of the package.
        */
        void SetVendorL( const TDesC& aVendor );       

        /**
        * Return the package version.
        * @since 3.0
        * @return Package vendor.
        */
        const TVersion& Version() const;   

        /**
        * Set the trusted status of the package. 
        * @since 3.0
        * @param aIsTrusted - Trusted status.
        */
        void SetTrusted( TBool aIsTrusted );
        
        /**
        * Indicates the trusted status of the package. 
        * @since 3.0
        * @return Trusted status.
        */
        TBool IsTrusted() const;

        /**
        * Sets the Uid of the package. 
        * @since 3.0
        * @param aUid - Package Uid.
        */
        void SetUid( const TUid& aUid );      

        /**
        * Return the Uid of the package. 
        * @since 3.0
        * @return Package Uid.
        */
        TUid Uid() const;    

        /**
        * Prepares the iterator to be shown in details dialog.
        * @since 3.0
        */
        void PrepareIteratorL( const Swi::CAppInfo& aInfo );
        
        /**
        * Frees the memory reserved for the iterator.
        * @since 3.0
        */
        void FreeIterator();        

    public: // Functions from base classes
        
        /**
        * From MCUIInfoIterator, Indicates if the iterator has any more 
        * fields left. 
        * @since 3.0
        * @return ETrue if there are some fields left, EFalse if not.
        */
        TBool HasNext() const;

        /**
        * From MCUIInfoIterator, Get the next field. 
        * @since 3.0
        * @param aKey - After successfull call contains the key / header of the field.
        * @param aKey - After successfull call contains the value of the field.
        */
        void Next( TPtrC& aKey, TPtrC& aValue );

        /**
        * From MCUIInfoIterator, Reset the iterator. After calling this, Next() 
        * will return the first value.
        * @since 3.0
        */
        virtual void Reset();
        
    private:
        
        /**
        * C++ default constructor.
        */
        CSisxUIAppInfo( TBool aIsTrusted );        
        
        /**
        * 2nd phase constructor.
        * @param aEntry - Sis registry entry.
        */
        void ConstructL( const Swi::CAppInfo& aInfo );  

        /**
        * Sets a new field for the iterator with given parameter values. 
        * @since 3.0
        * @param aResourceId - Resource id of the header text.
        * @param aValue - Value of the field.
        */
        void SetFieldL( TInt aResourceId, const TDesC& aValue );
        
    private:    // Data

        TInt iCurrentIndex;        
        CDesCArray* iKeys;  // Own
        CDesCArray* iValues;  // Own

        HBufC* iName;  // Own
        HBufC* iVendor; // Own
        TVersion iVersion;        
        TUid iUid;
        TBool iIsTrusted;
    };
}

#endif      // SISXUIAPPINFO
            
// End of File

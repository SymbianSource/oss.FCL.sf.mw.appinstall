/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains header files for the CCUIDetailsDialog
*                class.
*
*/


#ifndef CUIDETAILSDIALOG_H
#define CUIDETAILSDIALOG_H

// INCLUDES
#include <e32base.h>
#include <signed.h>
#include <badesca.h>
#include <f32file.h>


// FORWARD DECLARATIONS
class CCoeEnv;
class CX509Certificate;

namespace SwiUI
{

namespace CommonUI
{

// CLASS DECLARATIONS

/**
* This interface provides an iterator to application's detailed info.
* 
* @since 3.0
*/
class MCUIInfoIterator
    {
    public:

        /**
        * Indicates if the iterator has any more fields left. 
        * @since 3.0
        * @return ETrue if there are some fields left, EFalse if not.
        */
        virtual TBool HasNext() const = 0;
    
        /**
        * Get the next field. 
        * @since 3.0
        * @param aKey - On return contains the key / header of the field.
        * @param aKey - On return contains the value of the field.
        */
        virtual void Next( TPtrC& aKey, TPtrC& aValue ) = 0;

        /**
        * Reset the iterator. After calling this, Next() will return the first value.
        * @since 3.0
        */
        virtual void Reset() = 0;
    };

/**
* This class represents a user viewable certificate.
*
* @since 3.0
*/
class CCUICertificateInfo : public CBase
    {
    public:  // Constructors and destructor

        IMPORT_C static CCUICertificateInfo* NewL( const CX509Certificate& aCertificate );    

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCUICertificateInfo();
        
    public: // New functions

        /**
        * Returns pointer to X509Certificate where this info is based on.
        * @since 3.0
        * @return Pointer to the X509Certificate. Null if this is not a X509Certificate.
        */
        IMPORT_C CX509Certificate* X509Certificate();       

        /**
        * Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	virtual const TDesC& SubjectNameL() const;

	/**
        * Gets the issuer name of the certificate.
        * @since 3.0
        * @return The issuer name of the certificate.
        */
	virtual const TDesC& IssuerNameL() const;

	/**
        * Gets the date the certificate is valid from.
        * @since 3.0
        * @return The date the certificate is valid from.
        */
	virtual TDateTime ValidFromL() const;

	/**
        * Gets the date the certificate is valid until.
        * @since 3.0
        * @return The date the certificate is valid until.
        */
	virtual TDateTime ValidToL() const;

	/**
        * Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	virtual const TDesC8& FingerprintL() const;

	/**
        * Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	virtual const TDesC8& SerialNumberL() const;

        /**
        * Indicates wheter this certificate is self signed.
        * @since 3.0
        * @return ETrue if is self signed.
        */
        virtual TBool IsSelfSignedL() const;       

        /**
        * Gets the full encoding of this certificate.
        * @since 3.0
        * @return Encoding.
        */
        virtual const TDesC8& EncodingL() const;

        /**
        * Gets the signature of this certificate.
        * @since 3.0
        * @return Signature.
        */
        virtual const TDesC8& SignatureL() const;

        /**
        * Gets the public key algorithm of the certificate.
        * @since 3.0
        * @return Public key algorithm.
        */
        virtual TAlgorithmId PublicKeyAlgorithmL() const;

        /**
        * Gets the digest algorithm of the certificate.
        * @since 3.0
        * @return digest algorithm.
        */
        virtual TAlgorithmId DigestAlgorithmL() const;

    protected:

        /**
        * C++ default constructor.
        */
        IMPORT_C CCUICertificateInfo();

        /**
        * 2nd phase constructor.
        */
        IMPORT_C void BaseConstructL( const CX509Certificate& aCertificate );        

    private:  // Data

        HBufC* iIssuer;
        HBufC* iSubject;   
        HBufC8* iFingerprint;
        HBufC8* iSerialNumber;        
        HBufC8* iEncoding;
        HBufC8* iSignature;        
        CX509Certificate* iCertificate;        
    };


/**
* Dialog showing the application details. The dialog can also show a list
* of certificates.
*  
* @lib swinstcommonui.lib
* @since 3.0
*/
class CCUIDetailsDialog : public CBase
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.0
        * @return Pointer to the created details dialog.
        */
        IMPORT_C static CCUIDetailsDialog* NewL();

        /**
        * Two-phased constructor.
        * @since 3.0
        * @param aCbaResourceId - Resource id of the dialog CBAs.
        * @return Pointer to the created details dialog.
        */
        IMPORT_C static CCUIDetailsDialog* NewL( TInt aCbaResourceId );
        
        /**
        * Two-phased constructor.
        * @since 5.0
        * @param aCbaResourceId - Resource id of the dialog CBAs.
        * @param aWithInstallHeader - True value if "Install?" header to be shown. 
        * @return Pointer to the created details dialog.
        */
        IMPORT_C static CCUIDetailsDialog* NewL( TInt aCbaResourceId,
                                                 TBool aWithInstallHeader );
        
        /**
        * Installation confirmation header types
        * @since 5.0
        */
        enum TCUIDetailsHeader {
            EDetailsHeader,
            EInstallHeader,
            EInstallAndDownloadHeader
        };

        /**
        * Two-phased constructor.
        * @since 5.0
        * @param aCbaResourceId - Resource id of the dialog CBAs.
        * @param aHeaderType - Header to be used in details dialog.
        * @return Pointer to the created details dialog.
        */
        IMPORT_C static CCUIDetailsDialog* NewL( TInt aCbaResourceId,
                                                 TCUIDetailsHeader aHeaderType );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCUIDetailsDialog();

    public:
       
        /**
        * Displays the dialog.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        * @return Id of the selected CBA.
        */
        IMPORT_C TInt ExecuteLD( MCUIInfoIterator& aIterator );

        /**
        * Displays the dialog with given certificates.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        * @param aCertInfos - Array of certificate info objects.
        * @return Id of the selected CBA.
        */
        IMPORT_C TInt ExecuteLD( MCUIInfoIterator& aIterator,
                                 const RPointerArray<CCUICertificateInfo>& aCertInfos );

        /**
        * Displays the dialog with a link to DRM details.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        * @param aFile - Handle to the package. (for showing drm details)
        * @return Id of the selected CBA.
        */
        IMPORT_C TInt ExecuteLD( MCUIInfoIterator& aIterator,
                                 RFile& aFile );
        
        /**
        * Displays the dialog with given certificates and a link to DRM details.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        * @param aCertInfos - Array of certificate info objects.
        * @param aFile - Handle to the package. (for showing drm details)
        * @return Id of the selected CBA.
        */
        IMPORT_C TInt ExecuteLD( MCUIInfoIterator& aIterator,
                                 const RPointerArray<CCUICertificateInfo>& aCertInfos,
                                 RFile& aFile );
        
    private:

	/**
        * Constructor.
        */
        CCUIDetailsDialog();

        /**
        * Constructor.
        */
        CCUIDetailsDialog( TInt aCbaResourceId );
        
        /**
        * Constructor.
        * @since 5.0
        * @param aCbaResourceId - Resource id of the dialog CBAs.
        * @param aWithInstallHeader - True value if "Install?" header to be shown 
        */
        CCUIDetailsDialog( TInt aCbaResourceId, TCUIDetailsHeader aHeaderType );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Helper to show the details dialog.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        */
        TInt ShowDialogL( MCUIInfoIterator& aIterator );        

        /**
        * Sets a field in the details dialog.
        * @since 3.0
        * @param aItemArray - The constructed field is appended into this array.
        * @param aHeading - Heading of the field.
        * @param aValue - Value of the field.
        */
        void SetFieldL( CDesCArray& aItemArray,
                        const TDesC& aHeading, 
                        const TDesC& aValue );        

        /**
        * Helper to populate the field array.
        * @since 3.0
        * @param aIterator - Iterator for application details.
        */
        void PopulateArrayL( MCUIInfoIterator& aIterator );
        
        /**
        * Helper to get descriptor containing the fields of the dialog.
        * @since 3.0
        * @return Descriptor containing the fields.
        */
        HBufC* GetMessageDescriptorLC();

        /**
        * Helper to add certificate information to query text.
        * @since 3.0
        * @return Descriptor containing the fields.
        */
        void AddCertificatesLC( HBufC*& aMessage );    

        /**
        * Helper to add DRM information to query text.
        * @since 3.0
        * @return Descriptor containing the fields.
        */
        void AddDrmLC( HBufC*& aMessage );     

        /**
        * Used as a callback function in message query.
        * @since 3.0
        */
        static TInt ShowCertificates( TAny* ptr );

        /**
        * Used as a callback function in message query.
        * @since 3.0
        */
        static TInt ShowDrm( TAny* ptr );

        /**
        * Show CCUICertificateInfo certificates.
        * @since 3.0
        */
        void DoShowCertificatesL();        
        
        /**
        * Show detailed drm info.
        * @since 3.0
        */
        void DoShowDrmL(); 

        /**
        * Helper to construct a message query link from resource string.
        * @since 3.0
        * @param aResourceId - Id of the string.
        * @return Descriptor containing the link string.
        */
        HBufC* LoadLinkLC( TInt aResourceId );        

    private: // Data

        TInt iResourceFileOffset;
        CCoeEnv* iCoeEnv;  // not owned
        CDesCArray* iMessageArray;  
        TInt iCbaResourceId;        

        RPointerArray<CCUICertificateInfo> iCommonCertificates;

        RFile iFile;
        TBool iFileSet;        
        TCUIDetailsHeader iHeaderType;
    };
}
}

#endif // CUIDETAILSDIALOG_H

// End of file

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
* TSisTrustStatus declaration
*
*/


/**
 @file 
 @released
 @publishedPartner
*/


#ifndef __SISTRUSTSTATUS2_H__
#define __SISTRUSTSTATUS2_H__

#include <e32std.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/scr/scr.h>
#endif

class RReadStream;
class RWriteStream;

namespace Swi 
{
class TSisTrustStatus;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
// Forward declare the friend functions.
namespace ScrHelperUtil
	{
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TSisTrustStatus& aTrustStatus);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, TSisTrustStatus& aTrustStatus, RPointerArray<Usif::CPropertyEntry>* aCompPropertyArray);	
	}
#endif

/**
 * An enumeration providing information regarding the certificate validation
 * status of a controller. The order of the members is important and should 
 * not be changed as it is relied on elsewhere.
 */
enum TValidationStatus 
    {

		/** 	 
		 * The validation status is unknown,
		 * the associated registry entry has been populated from an old database
		 */
        EUnknown   = 0,   
		/**      
		 * Signed and validated at install time but one or more required certificate chains was expired at
		 * the time of a subsequent revocation check
		 */			
                                
        EExpired   = 10,        
		/**
		 * The validation status is invalid
		 * All required certificate chains failed validation
		 */   
                                
        EInvalid   = 20,        

		/**
		 * Application is unsigned, no certificates
		 */  
        EUnsigned  = 30,        
		
		/** 1 or more required chains validated but none to a matching 
		 *  issuer certificate in the certstore
		 */
                                
        EValidated = 40,        
		/** 1 or more certificate chains  validated to an issuer certificate
		 * in the certstore     
		 */
                                
        EValidatedToAnchor = 50,

		/**
		 * Package was installed in ROM 
		 */
                                
        EPackageInRom = 60       
    };
    
/**
 * An enumeration providing information regarding the certificate revocation
 * status of a controller. The order of the members is important and should 
 * not be changed as it is relied on elsewhere.
 */
enum TRevocationStatus 
    {

		/** The revocation status is  unknown, the associated registry 
		 * entry has been populated from an old database
		 */
        EUnknown2 = 0,           

		/**OCSP revocation check not performed due to user decision
		 * or failed validation
		 */
                                 
        EOcspNotPerformed = 10,	 

		/** 1 or more required certificate(s) were revoked.
		 */
                                 
        EOcspRevoked      = 20,  

		/** 1 or more certificates were 'unknown' to the OCSP server
		 */
                                 
        EOcspUnknown      = 30,  

		/** 1 or more checks resulted in transient error
		 */
                                 
        EOcspTransient    = 40,	 

		/** All certificate chains good	
		 */
                                 	
        EOcspGood         = 50,  
    };

/**
 * TSisTrustStatus provides trust information about the entity with which it
 * is associated.
 *
 * The trust status comprises of two main elements.
 *
 * - a validation status value
 * - a revocation status value
 *
 * The two status values are used to indicate the reason for the trust
 * designation.
 *
 * In addition there are two date fields, iResultDate and iLastCheckDate. The
 * first is the date upon which a revocation check was last made without
 * transient errors. The second is the date of the last attempt to check the
 * revocation status. Normally these dates will be the same, however if a
 * transient error has occured, then the last attempt value will be more
 * recent than the last result and the revocation status will indicate a
 * transient error.
 *
 * @note This file is persisted as part of the CSisRegistryObject class. For
 * this reason, changes to this class should change the version of the
 * CSisRegistryObject. Beware of unintentional changes to the SisRegistry file
 * format.
 *
 * @see Swi::CSisRegistryObject
 */
class TSisTrustStatus
    {
    public:
        /** 
         * Default constructor of TSisTrustStatus class. Constructs an object which contains the trust information of an entity with which it is associated.
         */
        IMPORT_C TSisTrustStatus();
        /**
         * Parameterised constructor of TSisTrustStatus class. Constructs an object which contains the trust information of an entity with the specified values.
           @param aValidation  The validation status of the certificate chains signing the associated entity.  
           @param aRevocation  The  revocation status of the certificate chains signing the associated entity.
           @param aResultDate  The date upon which a revocation check was last made without transient errors
           @param aCheckDate   The date of the last attempt to check the revocation status.
         */ 
        IMPORT_C TSisTrustStatus(TValidationStatus aValidation,
                                 TRevocationStatus aRevocation,
                                 TTime aResultDate,
                                 TTime aCheckDate);
         
        /**
          Used to check the trusted status of the associated entity
         @return the trusted status of the associated entity.
		 */
        IMPORT_C TBool IsTrusted() const; 
        
        /** 
            Gets the current validation status of the certificate chains signing the associated entity.
            @return the current validation status of the certificate chains signing the associated entity
        
		 */
        TValidationStatus ValidationStatus() const
        { return iValidationStatus; }

        /** Gets the current Revocation status of the certificate chains signing the associated entity.
            @return the current revocation status of the certificate chains signing the associated entity.  
		 */
        TRevocationStatus RevocationStatus() const 
        { return iRevocationStatus; }
        
        /**
            Finds out the last date of a successful revocation check that was performed.
            @return The last date a successful revocation check was performed.
		 */
        TTime ResultDate() const
        { return iResultDate; }
        
        /**
            Gets the last date of a revocation check that was performed 
            @return The last date a revocation check was performed.
		 */
        TTime LastCheckDate() const  
        { return iLastCheckDate; }
        
        /** Sets the validation status
            @param aStatus  It specifies the certificate validation status.

		 */
        void SetValidationStatus(TValidationStatus aStatus)
        { iValidationStatus = aStatus; }

        /** Sets the revocation status 
            @param aStatus  Specifies the certificate revocation status.
		 */
        void SetRevocationStatus(TRevocationStatus aStatus)
        { iRevocationStatus = aStatus; }

        /** sets the date of the last successful revocation check
            @param aDate  The date of the last successful revocation check. 
		 */
        void SetResultDate(TTime aDate) 
        { iResultDate = aDate; }

        /** sets the date of the last attempt at a revocation check
           @param aDate  The date of the last attempt at a revocation check.
		 */
        void SetLastCheckDate(TTime aDate ) 
        { iLastCheckDate = aDate; }

		/**
		 * initializes the object from a stream
		 * @param aStream in parameter contains the stream read.
		 */
        IMPORT_C void InternalizeL(RReadStream& aStream); 
        
		/** Externalizes the object to a stream
		 * @param aStream contains the stream to be written.
		 */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	public: // Friend Functions.
		friend void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TSisTrustStatus& aTrustStatus);
		friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, TSisTrustStatus& aTrustStatus, RPointerArray<Usif::CPropertyEntry>* aCompPropertyArray);
#endif
            
    private:
            
        TValidationStatus iValidationStatus;    ///< the results of the last
                                                ///<  validation check
        
        TRevocationStatus iRevocationStatus;    ///< the results of the last 
                                                ///< OCSP check
        
        TTime iResultDate;          ///< the last date a successful revocation
                                    ///< check was performed
        
        TTime iLastCheckDate;       ///< the last date a revocation check was
                                    ///< attempted
    
        TBool iQuarantined;         ///< true if quarantined.
                                    ///< Not used currently
    
        TTime iQuarantinedDate;     ///< date entry was quarantined.      
                                    ///< Not used currently
    };
    
}


#endif  // __SISTRUSTSTATUS2_H__

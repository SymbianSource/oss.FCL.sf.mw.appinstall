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
* The file constains the declaration of the certchainconstraints class.
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __CERTCHAINCONSTRAINTS_H__
#define __CERTCHAINCONSTRAINTS_H__

#include <e32base.h>
#include <pkixcertchain.h>

namespace Swi 
{

/**
The CCertChainConstraints class is to aggregate the constraints  
specified in certficate extensions across all valid certificate chains 
and then check them against information from the device and/or information
contained in the SIS file.
@released
@internalTechnology 
*/
class CCertChainConstraints : public CBase
	{
public:
		
	/**
	Constructs a new CCertChainConstraints object
	
	@param aValidCerts a valid PKIX certificate chain
	@return A Certificate Chain Constraint
	*/
	IMPORT_C static CCertChainConstraints* NewL(RPointerArray<CPKIXCertChainBase>& aValidCerts);
	
	/**
	Constructs a new CCertChainConstraints object and leaves it on the cleanup stack
	
	@param aValidCerts a valid PKIX certificate chain
	@return A Certificate Chain Constraint
	*/
	IMPORT_C static CCertChainConstraints* NewLC(RPointerArray<CPKIXCertChainBase>& aValidCerts);

	/**
	Constructs a new CCertChainConstraints object with no constraints

	@return A Certificate Chain Constraint
	*/
	IMPORT_C static CCertChainConstraints* NewL();
	
	~CCertChainConstraints();
	
	/**
	Determine if the request SID is valid.
	
	@param aRequestSID a request SID
	@return ETrue if the request SID is valid, EFalse if not
	*/	
	IMPORT_C TBool SIDIsValid(TSecureId aRequestSID) const;
	
	/**
	Determine if the request VID is valid.
	
	@param aRequestSID a request VID
	@return ETrue if the request VID is valid, EFalse if not
	*/		
	IMPORT_C TBool VIDIsValid(TVendorId aRequestVID) const;
	
	/**
	Determine if the request Capability set is valid.
	
	@param aRequestSID a request capability set
	@return ETrue if the request Capability Set is valid, EFalse if not
	*/		
	IMPORT_C TBool CapabilitiesAreValid(TCapabilitySet& aRequestCapabilities) const;
	
	/**
	Determine if the request Device ID is valid.
	
	@param aRequestSID a request device ID
	@return ETrue if the request Device ID is valid, EFalse if not.
	*/			
	IMPORT_C TBool DeviceIDIsValid(const HBufC* aRequestDeviceID) const;
	
	/**
	The method is tell if the SIDs are constrained or not 
	
	@return ETrue if the SID is constrained, EFalse if not
	*/				
	IMPORT_C TBool SIDsAreConstrained() const;
	
	/**
	The method is tell if the VIDs are constrained or not 
	
	@return ETrue the VID is constrained, EFalse if not
	*/					
	IMPORT_C TBool VIDsAreConstrained() const;
	
	/**
	The method is tell if the Device IDs are constrained or not 
	
	@return ETrue if the Device ID is constrained, EFalse if not
	*/						
	IMPORT_C TBool DeviceIDsAreConstrained() const;

	/**
	The method is tell if the Capabilities are constrained or not 
	
	@return ETrue if the Capabilities is constrained, EFalse if not
	*/						
	IMPORT_C TBool CapabilitiesAreConstrained() const;
	
	/**
	The method retrieves the valid Capability Set 
	
	@return valid capability set
	*/							
	IMPORT_C const TCapabilitySet& ValidCapabilities() const;
	
	/**
	Set the valid Capability set
	
	@param aValidCapabilities a capability set to be set in the constaints.
	@return none
	*/								
	IMPORT_C void SetValidCapabilities(const TCapabilitySet& aValidCapabilities);
	
private:

	CCertChainConstraints();
	//Second-phase construntor.
	void ConstructL(RPointerArray<CPKIXCertChainBase>& aValidCerts);
	/**
	Retrieve the constrained capability set from the certificate extension, and take
	the intersection of the retrieved capabilities and the existing capability
	constraints as the new capability constraints
	
	@param aValidCerts a CX509Certificate reference
	@return none
	*/							
	void RetrieveExtensionCapabilitySetL(const CX509Certificate& aCert);

	/**
	Retrieve the device IDs constraints from the certificate extension
	and take the intersection of the retrieved device ID constraints and 
	the existing device ID constraints as the new device ID constraints
	 
	@param aValidCerts a CX509Certificate reference
	@return none
	*/								
	void RetrieveExtensionDeviceIDListL(const CX509Certificate& aCert);
	
	/**
	Retrieve the SID constraints from the certificate extension
	and take the intersection of the retrieved SID constraints and 
	the existing SID constraints as the new VID constraints
	
	@param aValidCerts a CX509Certificate reference
	@return none
	*/									
	void RetrieveExtensionSIDListL(const CX509Certificate& aCert);

	/**
	Retrieve the VID constraints from the certificate extension
	and take the intersection of the retrieved VID constraints and 
	the existing VID constraints as the new VID constraints
	
	@param aValidCerts a CX509Certificate reference
	@return none
	*/										
	void RetrieveExtensionVIDListL(const CX509Certificate& aCert);	
	
	/**
	The list of valid secured IDs
	*/
	RArray<TSecureId> iValidSIDs;
	/**
	The list of valid vendor IDs
	*/	
	RArray<TVendorId> iValidVIDs;
	/**
	The list of valid device IDs
	*/	
	RPointerArray<HBufC> iValidDeviceIDs;
	/**
	The list of valid capability set
	*/	
	TCapabilitySet iValidCapabilities;
	/**
	The flag that represents if the SIDs are constrained.
	*/		
	TBool iSIDsAreConstrained;
	/**
	The flag that represents if the VIDs are constrained.
	*/			
	TBool iVIDsAreConstrained;
	/**
	The flag that represents if the Device IDs are constrained.
	*/			
	TBool iDeviceIDsAreConstrained;	
	/**
	The flag that represents if the Device IDs are constrained.
	*/			
	TBool iCapabilitiesAreConstrained;		
	};

} //namespace Swi

#endif // #ifndef __CERTCHAINCONSTRAINTS_H__

/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the CSISController
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISCONTROLLER_H__
#define __SISCONTROLLER_H__

#include <e32base.h>

#include "sisfield.h"
#include "sisdataindex.h"
#include "sissignaturecertificatechain.h"
#include "sispackagetrust.h"
#include <hash.h>
#include "nativecomponentinfo.h"

namespace Swi
{

class TSisTrustStatus;
class CCertChainConstraints;

namespace Sis
{

class CInfo;
class CPrerequisites;
class CSupportedLanguages;
class CSupportedOptions;
class CProperties;
class CLogo;
class CSignatures; 
class CInstallBlock;
class TPtrProvider;


/**
 * This class represents a Controller. This is a structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CController : public CField
	{
public:

	/**
	 * This creates a new CController object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.		 		 
	 */
	IMPORT_C static CController* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.
	 */
	IMPORT_C static CController* NewL(MSisDataProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.		 		 
	 */
	IMPORT_C static CController* NewLC(MSisDataProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.
	 */
	IMPORT_C static CController* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of TPtrProvider to read the controller from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.		 		 
	 */
	IMPORT_C static CController* NewLC(TPtrProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object.
	 *
	 * @param aDataProvider 	 An instance of TPtrProvider to read the controller from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.
	 */
	IMPORT_C static CController* NewL(TPtrProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CController object in place.
	 *
	 * @param aDataProvider 	 An instance of TPtrProvider to read the controller from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.		 		 
	 */
	
	IMPORT_C static CController* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CController object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of TPtrProvider to read the controller from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CController representing the data read.		 		 
	 */
	
	IMPORT_C static CController* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CController();
	
	/// adds the index to the list of chains checked at install time
    IMPORT_C void AddChainIndex(TInt aIndex) const;

    /// clears the list of chains checked at install time
    IMPORT_C void ClearCertChainIndices() const;

 	/**
 	* This sets the instance of the CCertChainConstraint.
	@param the CCertChainConstraints instance.
	*/	
 	IMPORT_C void SetCertChainConstraints(CCertChainConstraints* aConstraints);	
 	
 	/**
	 * Provide whether the package will be uninstalled after uninstalling last dependent
	 * @return the current remove with last dependent value for this controller
	 **/
	 IMPORT_C TInt RemoveWithLastDependent() const;
	
	/**
	 * Set the status which determines whether the package will be uninstalled 
	 * after uninstalling last dependent.
	 * @return the current remove with last dependent value for this controller
	 **/
	 IMPORT_C void SetRemoveWithLastDependent();	
	 
	 /**
	 * Generate a hash for the controller from SISInfo (without the length) to SISInstallBlock
	 * (i.e. except the SISSignatueCertificateChain and SISDataIndex)
	 * @param  aController SISController raw data
	 * @return Pionter to the new generated CMessageDigest object
	 **/
	 IMPORT_C CMessageDigest* GenerateControllerHashLC(const TDesC8& aController);

public: // Accessors

	/**
	 * Every SISController includes a SISInfo structure which can be
	 * accessed using this function.
	 *
	 * @return The Sis::CInfo object included in this controller.
	 */
	const CInfo& Info() const;

	/**
	 * Every SISController includes a SISInstallBlock structure which can be
	 * accessed using this function.
	 *
	 * @return The Sis::CInstallBlock object included in this controller.
	 */
	const CInstallBlock& InstallBlock() const;

	/**
	 * Every SISController includes a list of supported language encoded in
	 * a SISSupportedLanguages structure which can be
	 * accessed using this function. In a well formed SIS file this structure
	 * <b>cannot be</b> empty as there must always be a supported language.
	 *
	 * @return The Sis::CSupportedLanguages object included in this controller.
	 */
	const CSupportedLanguages& SupportedLanguages() const;

	/**
	 * Every SISController includes a list of supported options encoded in
	 * a SISSupportedOptions structure which can be
	 * accessed using this function. Note that, although a CSupportedOptions
	 * object is always constructed, this might be empty, e.g. no options 
	 * are present.
	 *
	 * @return The Sis::CSupportedOptions object included in this controller.
	 */
	const CSupportedOptions& SupportedOptions() const;

	/**
	 * Every SISController includes a list of supported properties encoded in
	 * a SISSupportedProperties structure which can be
	 * accessed using this function. Note that, although a CProperties
	 * object is always constructed, this might be empty, e.g. no properties 
	 * are present.
	 *
	 * @return The Sis::CProperties object included in this controller.
	 */
	const CProperties& Properties() const;

	/**
	 * Every SISController includes a list of prerequisites which must be met 
	 * for the installation to proceed. Note that, although a CPrerequisites
	 * object is always constructed, this might be empty, e.g. no prerequisites 
	 * are present.
	 *
	 * @return The Sis::CPrerequisites object included in this controller.
	 */
	const CPrerequisites& Prerequisites() const;

	/**
	 * A SISController may contain a SISLogo structure, it can be
	 * accessed using this function.
	 *
	 * @return A pointer to the Sis::CLogo object included in this controller,
	 *         NULL if no Logo was found in the SIS file.
	 */
	const CLogo* Logo() const;

	TUint32 DataIndex() const;

	TInt64 DataOffset() const;
	
	/**
	 * A controller can be <i>signed</i> multiple times. A signature/certificate
	 * for a given controller is encoded in a SISSignatureCertificateChain structure.
	 * This function returns a list of such structure as an array. Note that if
	 * the controller is not signed the returned array will be empty.
	 *
	 * @return An array of Sis::CSignatureCertificateChain objects.
	 */
	const RPointerArray<CSignatureCertificateChain>& SignatureCertificateChains() const;
	
	/** Get the trust associated the SIS package handled by this controller
	@return The TSisPackageTrust associated with the package
	*/
	TSisPackageTrust Trust() const;

	/** Set the trust associated the SIS package handled by this controller
	@param aTrust The TSisPackageTrust to be associated with the package
	*/
	void SetTrust(TSisPackageTrust aTrust);
	
	/** Get the System upgrade Cert status associated the SIS package handled by this controller
	@return The TBool associated with the package
	*/
	TBool IsSignedBySuCert() const;

	/** Set the System upgrade Cert validation status associated the SIS package handled by this controller
	@param aSignedBySuCert The TBool to be associated with the package
	*/
	void SetSignedBySuCert(TBool aSignedBySuCert);

	/**
     * Provide the trust status object describing the controller. 
     *
     * This member is not part of the SIS file structure but is information
     * about it that needs to travel through the SWIS. The object is returned
     * non-const from a const member as it is a mutable member of CController. 
     *
     * @return the current trust status for this controller.
     **/
	TSisTrustStatus& TrustStatus() const
	{ return *iTrustStatus; }

    /// @return the list of indexes of chains checked at install time
    const RArray<TInt>& CertChainIndices() const  
    { return iCertChainIndices; }
       
	/** Get the instance of the CCertChainConstraint.
	@return the CCertChainConstraints instance.
	*/
 	const CCertChainConstraints* CertChainConstraints() const;
 	
 	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TCapabilitySet UserGrantableCapabilities();
	void SetUserGrantableCapabilities(TCapabilitySet aCapabilitySet);
	
	/** Returns a flag 
	 @return iHasExe TBool flag
	 */
	inline TBool HasExecutable() const;
	
	/** Sets the status of flag 
	 @param aHasExe TBool
    */
	inline void SetHasExecutable(TBool aHasExe);
	
	/** Returns a flag 
	@return iIsDriveSelectionRequired TBool flag
	*/
	inline TBool CController::DriveSelectionRequired() const;
	
	/** Sets the status of flag 
	@param aIsDriveSelectionRequired TBool
    */
    inline void CController::SetDriveSelectionRequired(TBool aIsDriveSelectionRequired);
	
	/** Returns a pointer array of CNativeApplicationInfo 
	@return RPointerArray iApplicationInfo
	*/
    inline RCPointerArray<CNativeComponentInfo::CNativeApplicationInfo>& CController::GetApplicationInfo();
	
    /** Adds the application Info to the array of CNativeApplicationInfo 
	@param aApplicationInfo The info for a native application
	*/
    inline void CController::AddApplicationInfoL(const CNativeComponentInfo::CNativeApplicationInfo* aApplicationInfo);
    
    #endif
	
private:

	CController();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.	 
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	CInfo* iInfo;

	CSupportedLanguages* iLanguages;
	
	CSupportedOptions* iSupportedOptions;
	
	CProperties* iProperties;
	
	CInstallBlock* iInstallBlock;
	
	CPrerequisites* iPrerequisites;
	
	CLogo* iLogo;
	
	RPointerArray<CSignatureCertificateChain> iSignatureCertificateChains;
	
	CDataIndex* iDataIndex;
	
	TInt64 iDataOffset;
	
	TSisPackageTrust iTrust;
	
	TBool iSignedBySuCert;
	
	CCertChainConstraints* iCertChainConstraints;
	
	TBool iHasExe;						   ///< Indicates Whether the component has an exe or not
	
	TBool iIsDriveSelectionRequired;       ///< Indicates Whether drive selection is required or not.
		
	mutable TSisTrustStatus* iTrustStatus; ///< The install trust status

    mutable RArray<TInt> iCertChainIndices; ///< Contains the indexes of the chains
                                            ///< check for revocation at install time
	mutable TInt iRemoveWithLastDependent;  ///< Whether uninstall with last dependent
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RCPointerArray<CNativeComponentInfo::CNativeApplicationInfo> iApplicationInfo;  ///< Pointer Array of application info objects CNativeApplicationInfo.
	TCapabilitySet iUserGrantableCapabilities;
	#endif
	
	};

// inline functions from CController

inline const CInfo& CController::Info() const
	{
	return *iInfo;
	}

inline const CInstallBlock& CController::InstallBlock() const
	{
	return *iInstallBlock;		
	}

inline const CSupportedLanguages& CController::SupportedLanguages() const
	{
	return *iLanguages;
	}

inline const CSupportedOptions& CController::SupportedOptions() const
	{
	return *iSupportedOptions;
	}

inline const CProperties& CController::Properties() const
	{
	return *iProperties;
	}

inline const CPrerequisites& CController::Prerequisites() const
	{
	return *iPrerequisites;	
	}

inline const CLogo* CController::Logo() const
	{
	return iLogo;	
	}

inline TUint32 CController::DataIndex() const
	{
	return iDataIndex->Index();
	}

inline const RPointerArray<CSignatureCertificateChain>& CController::SignatureCertificateChains() const
	{
	return iSignatureCertificateChains;
	}

inline TInt64 CController::DataOffset() const
	{
	return iDataOffset;
	}

inline void CController::SetTrust(TSisPackageTrust aTrust)
	{
	iTrust = aTrust;
	}

inline TSisPackageTrust CController::Trust() const
	{
	return iTrust;
	}

inline void CController::SetSignedBySuCert(TBool aSignedBySuCert)
	{
	iSignedBySuCert = aSignedBySuCert;
	}

inline TBool CController::IsSignedBySuCert() const
	{
	return iSignedBySuCert;
	}

inline const CCertChainConstraints* CController::CertChainConstraints() const
 	{
 	return iCertChainConstraints;
 	} 
 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline void CController::SetUserGrantableCapabilities(TCapabilitySet aCapabilitySet)
	{
	iUserGrantableCapabilities = aCapabilitySet;
	}
	
inline TCapabilitySet CController::UserGrantableCapabilities()
	{
	return iUserGrantableCapabilities;
	}

inline TBool CController::HasExecutable() const
	{
	return iHasExe;
	}

inline void CController::SetHasExecutable(TBool aHasExe)
	{
	iHasExe = aHasExe;
	}

inline TBool CController::DriveSelectionRequired() const
    {
    return iIsDriveSelectionRequired;
    }

inline void CController::SetDriveSelectionRequired(TBool aIsDriveSelectionRequired)
    {
    iIsDriveSelectionRequired = aIsDriveSelectionRequired;
    }

inline RCPointerArray<CNativeComponentInfo::CNativeApplicationInfo>& CController::GetApplicationInfo()
    {
    return iApplicationInfo;
    }

inline void CController::AddApplicationInfoL(const CNativeComponentInfo::CNativeApplicationInfo* aApplicationInfo)
    {
    iApplicationInfo.AppendL(aApplicationInfo);
    }

#endif

} // namespace Sis

} // namespace Swi

#endif

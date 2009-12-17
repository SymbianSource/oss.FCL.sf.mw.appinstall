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
* This class wraps up the security policy for native software installation.
*
*/


/**
 @file
 @internalTechnology
*/

#ifndef __SECURITYPOLICY_H__
#define __SECURITYPOLICY_H__

#include <e32base.h>

namespace Swi
{

_LIT(KSwisPolicyFile, "z:\\system\\data\\swipolicy.ini");

_LIT8(KAllowUnsigned, "AllowUnsigned");
_LIT8(KMandatePolicies, "MandatePolicies");
_LIT8(KDRMEnabled, "DRMEnabled");
_LIT8(KDRMIntent, "DRMIntent");
_LIT8(KMandateCodeSigningExtension, "MandateCodeSigningExtension");
_LIT8(KOid, "Oid");
_LIT8(KOcspMandatory, "OcspMandatory");
_LIT8(KOcspEnabled, "OcspEnabled");
_LIT8(KAllowGrantUserCaps, "AllowGrantUserCapabilities");
_LIT8(KUserCapabilities, "UserCapabilities");
_LIT8(KAllowOrphanedOverwrite, "AllowOrphanedOverwrite");	// This flag allows orphaned files to be overwritten when set
_LIT8(KAllowPackagePropagate, "AllowPackagePropagate");  // Allows SWI to write stub SIS files to removable media cards
_LIT8(KApplicationShutdownTimeout, "ApplicationShutdownTimeoutSeconds");  // Timeout for shutting down Apps during install/uninstall
_LIT8(KRunWaitTimeout, "RunWaitTimeoutSeconds");  // Timeout to allow completion of apps/exes launched with RunWait option
_LIT8(KAllowRunOnInstallUninstall, "AllowRunOnInstallUninstall");
_LIT8(KReplacePath, "ReplacePath");
_LIT8(KDeletePreinstalledFilesOnUninstall, "DeletePreinstalledFilesOnUninstall");
_LIT8(KPhoneTsyName, "PhoneTsyName");
_LIT8(KAllowOverwriteOnRestore, "AllowOverwriteOnRestore");
/** When true this flag allows orphaned files residing in private/xxxxxxxx/import, /resource or /sys/bin directories to be
	overwritten or eclipsed under user permission.
 */	
_LIT8(KAllowProtectedOrphanOverwrite, "AllowProtectedOrphanOverwrite");
/**
When true this flag allows to delay the uninstallation of embedded packages 
until all their dependent packages have been uninstalled.
*/
_LIT8(KRemoveOnlyWithLastDependent, "RemoveOnlyWithLastDependent");
/**
	If defined in the ini file, this specifies the ECOM plugin which is used
	to filter the OCSP request HTTP headers.
 */
_LIT8(KOcspHttpHeaderFilter, "OcspHttpHeaderFilter");

// This flag defines whether a SIS file will be considered compatible with the device if no target devices are listed in the package.
_LIT8(KSISCompatibleIfNoTargetDevices, "SISCompatibleIfNoTargetDevices");
_LIT8(KAlternativeCodeSigningOid, "AlternativeCodeSigningOID");

_LIT8(KMaxNumOfLogEntries , "MaxNumOfLogEntries");


NONSHARABLE_CLASS(CPathReplaceEntry) : public CBase
/**
	Used internally by CSecurityPolicy to manage replacement
	key, value pairs.

	@internalComponent
 */
	{
public:
	CPathReplaceEntry(HBufC* aFindPath, HBufC* aReplacePath);
	virtual ~CPathReplaceEntry();
	
public:
	/** Drive-qualified directory to match on target LHS. */
	HBufC*const iFindPath;
	/** Drive-qualified replacement directory. */
	HBufC*const iReplacePath;
	};

/**
 * This class represents some settings that control the behaviour
 * when installing a SIS file. The settings are stored in a resource
 * files and are retrieved at construction time.
 *
 * This class is implemented as a Singleton class. So only one instance 
 * of this class will be created per thread (Per Thread because TLS is used to
 * store the instance of this class)
 */
class CSecurityPolicy : public CBase
	{
public:
	~CSecurityPolicy();	
	
	/**
	 * This function checks if the installer should allow unisgned SIS files
	 * to be installed.
	 * @return ETrue Unsigned SIS file may be installed.
	 *         EFalse Unsigned SIS files must not be installed.
	 */
	IMPORT_C TBool AllowUnsigned() const;
	
	/**
	 * This function checks if the end-entity certificates corresponding to
	 * the private keys used to sign the SIS file must contain the 
	 * codesigning extension.
	 * @return ETrue The certificates must contain the codesigning extension. 
	 * If they don't the SIS file will not be installed.
	 *		   EFalse It doesn't matter whether the certificates contain
	 * this extension or not.
	 */
	IMPORT_C TBool MandateCodesigningExtension() const;
	
	/**
	 * This function checks if all the certificates (except the root) in the
	 * chains corresponding to the private keys used to sign the SIS file must
	 * have a Policy extension containing one of the OID defined in the resource 
	 * file. 
     * @return ETrue The certificates must contain one of the OID defined in
     * the resource file. If they don't the SIS file will not be installed.
     *		   EFalse The presence or absence of the OID don't affect
     * the installation. The installer may still display this 
     * information to the user.
	 */
	IMPORT_C TBool MandateCertificatePolicies() const;

	/**
	 * ROM time specified extension OIDs which are compulsory for SWI
	 * certificates.
	 * 
	 * @return A list of OIDs which are mandatory for SWI certificates
	 */
	IMPORT_C const RPointerArray<HBufC> Oids() const;

	/**
	 * Returns a boolean flag to indicate whether DRM is enabled.
	 *
	 * @return ETrue if DRM is enabled, EFalse otherwise 
	 */
	IMPORT_C TBool DrmEnabled() const;
	
	/**
	 * Returns an integer indicating which DRM intent should be used
	 * for evaluation and execution of rights.
	 */
	IMPORT_C TInt DrmIntent() const;

	/**
	 * This function returns whether OCSP is enabled during Software Install.
	 *
	 * @return ETrue if OSCP is enabled, EFalse otherwise.
	 */
	IMPORT_C TBool OcspEnabled() const;
	
	/**
	 * This function returns whether OCSP is mandatory during Software Install.
	 *
	 * @return ETrue if OSCP is enabled, EFalse otherwise.
	 */
	IMPORT_C TBool OcspMandatory() const;
	
	/**
	 * This function returns whether Software Install should allow the
	 * granting of user capabilities, during install.
	 *
	 * @return ETrue if user capabilities are allowed to be granted,
	 *			  EFalse otherwise.
	 */
	IMPORT_C TBool AllowGrantUserCaps() const;

	/**
	 * This function returns a list of capabilities which Software Install
	 * considers to be user capabilities. This list is read from the 
	 * policy ini file.
	 *
	 * @return The list of capabilities Software Install considers to be 
	 *			  user capabilities.
	 */
	 IMPORT_C TCapabilitySet UserCapabilities() const;

	/**
	 *	This function returns whether Software Install should ask the user
	 * if orphaned files should be overwritten during install.
	 *
	 * @return ETrue if the user should be asked about orphaned files,
	 *			  EFalse otherwise.
	 */
	IMPORT_C TBool AllowOrphanedOverwrite() const;
	
	/** This function returns whether Software Install should create
	SIS stub files on removable media when a SIS file is installed to the 
	removable media.
	@return ETrue if stubs are to be created, EFalse otherwise
	*/
	IMPORT_C TBool AllowPackagePropagate() const;

	/**
	 * This function returns an integer indicating the maximum allowed
	 * shutdown time for applications which are shut down during an upgrade
	 * or uninstall.
	 * @return Timeout in microseconds for apps shut down during upgrade
	 * 			or uninstall.
	 */
	IMPORT_C TInt ApplicationShutdownTimeout() const;
	
	/**
	 * This function returns an integer indicating the maximum time to wait
	 * for completion of execution of files launched with the RunWait file
	 * option during an install/uninstall.
	 * @return Timeout in microseconds for apps shut down during upgrade
	 * 			or uninstall.
	 */
	IMPORT_C TInt RunWaitTimeout() const;
	
	/**
	 * This function returns a boolean indicating whether a SIS file with
	 * no target devices listed should be considered compatible with
	 * the device.
	 * @return ETrue if SIS files with no target devices listed are to
	 *			be considered compatible, EFalse otherwise.
	 */
	IMPORT_C TBool SISCompatibleIfNoTargetDevices() const;

	/**
	 * This function returns a boolean indicating whether the applications contained in
	 * the SIS file are allowed to run when being installed or uninstalled.
	 *
	 * @return ETrue if SIS files with no target devices listed are to
	 *			be considered compatible, EFalse otherwise.
	 */
	IMPORT_C TBool AllowRunOnInstallUninstall() const;

	IMPORT_C TUint32 OcspHttpHeaderFilter() const;

	IMPORT_C HBufC* ResolveTargetFileNameLC(
		const TDesC& aMmpName, TText aDrive) const;
	IMPORT_C HBufC* ResolveTargetFileNameL(
		const TDesC& aMmpName, TText aDrive) const;

	IMPORT_C TBool DeletePreinstalledFilesOnUninstall() const;

	/** When true this flag allows orphaned files residing in
	 * /private/xxxxxxxx/import, /resource or /sys/bin directories to be
	 * overwritten or eclipsed (under user permission).
	 * A file is considered orphaned if it does not belong to any installed
	 * package in the registry. This value defaults to true;
	 *
	 * @return ETrue if the Software Install should overwrite or eclipse
	 * orphaned files in protected directories, EFalse otherwise.
	 */
	IMPORT_C TBool AllowProtectedOrphanOverwrite() const;

 	/**
	This function is used to retrieve a list of alternate code signing 
 	OIDs. These values are in addition to the OID existing in 
 	securitydefs.h. Allows additional user defined OID to be 
 	specified for code signing.
 	@return A list of the alternative code oid references
 	*/	
	IMPORT_C const RPointerArray<HBufC> AlternativeCodeSigningOid() const;
	
	/**
	This function is used to retrieve the value of iRemoveOnlyWithLastDependent member.
	When true an embedded package can be uninstalled only if the last dependent has been uninstalled  
	*/
	IMPORT_C TBool RemoveOnlyWithLastDependent() const;
	
	IMPORT_C static void ReleaseResource();
	
	/**
  	 * This function is used to access the instance of this class. If
  	 * the instance is not created then this will create the instance
  	 * and return. TLS is used to store pointer of the instance.
  	 *
  	 * ReleaseResource function should be by the owner of this instance
  	 * after the use of this class.
  	 */
	IMPORT_C static CSecurityPolicy* GetSecurityPolicyL();
	
	/**
	 * This function gets the maximum number of entries  allowed in the log file 
	 * SisRegistryServer produce logfile , during install,Uninstall and Upgrade.
	 *
	 * @return maximum number of entries in logfile, 0 if no entries or 
	 * 		   logging is disabled,
	 *		   
	 */
	
	IMPORT_C TInt MaxNumOfLogEntries() const;

	IMPORT_C const TDesC& PhoneTsyName() const;
	
	/**
	Indicates whether the restore process should overwrite an application that is already installed
	if the same item appears in the backup e.g. if the system drive has not been reformatted.
	@return ETrue, if the restore process should overwrite applications; otherwise, EFalse is returned.
	*/
	IMPORT_C TBool AllowOverwriteOnRestore() const;
private:	
	CSecurityPolicy();
	
	/**
	 * The second-phase constructor. It reads the settings from
	 * the resource file.
	 */
	void ConstructL();

	/**
	Retrieves the next line of text from a buffer. Blank lines are skipped.

	@param aBuffer The buffer to parse.
	@param aPos    The position to start reading from. This IN/OUT parameter
	               should be initialised to zero on the first call.
    @param aLine   An out parameter that will be set to point to the next line
                   of text IF found.
    @return        Whether a line of text was successfully read.
	*/
	TBool ReadLineL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine) const; 

	/**
	 * This function adds a user capability to iUserCapabilities from
	 * a descriptor containing the name of the capability to add.
	 */
	void AddUserCapability(const TDesC8& aCapabilityName);

	void ReadReplacementPathsL(const TDesC8& aLine);
	
	/**
	 * This function adds an alternative oid to iAlternativeCodeSigningOids from
	 * a descriptor containing the alternative oid to add. Does not allow any duplication.
	 */
	void AddAlternativeCodeSigningOidL(const TDesC8& aAlternativeCodeSigningOid);
	 
private:
	TBool iIsInitialized;

	/**
	 * This is set to ETrue if the installer must accept to install unsigned
	 * SIS file. It is set to EFalse if only SIS files that are signed are
	 * allowed to be installed.
	 */
	TBool iAllowUnsigned;
	
	/**
	 * This is set to ETrue if the end-entity certificates corresponding 
	 * to the private keys used to sign the SIS files must have a codesigning
	 * extension. If this is set and the certificates don't have this extension
	 * the SIS file will not be installed.
	 * If this is set to EFalse then the certificates may or may not have a
	 * a codesigning extension.
	 */
	TBool iMandateCodesigningExtension;
	
	/**
	 * This is set to ETrue if all the certificates (except the root) in the
	 * chains corresponding to the private keys used to sign the SIS file must
	 * have a Policy extension containing one of the OID defined in the resource 
	 * file. 
	 * This is set to EFalse if the presence of OID doesn't affect the 
	 * installation.
	 */
	TBool iMandateCertificatePolicies;
	
	/**
	 * A List of custom OIDs which are mandatory for certificates used to
	 * sign applications.
	 *
	 */
	RPointerArray<HBufC> iCustomOids;
	
	/**
	 * A boolean flag to indicate whether DRM is enabled.
	 */
	TBool iDrmEnabled;
	
	/**
	 * The DRM intent to execute and evaluate if DRM is enabled. 
	 */
	TInt iDrmIntent;

	/**
	 * This is set to ETrue if OCSP is enabled, and software Install will
	 * carry out an OCSP check during the installation. Failure of the OCSP
	 * check does not necessarily cause installation to fail.
	 */
	TBool iOcspEnabled;
	
	/**
	 * This is set to ETrue if ocsp check success is mandatory before install
	 * can be carried out.	
	 */
	TBool iOcspMandatory;
	
	/**
	 * This is set to ETrue if Software Install will allow the user to grant
	 * user capabilities to an executable, which are not signed for.
	 */
	TBool iAllowGrantUserCaps;
	
	/**
	 *	User capabilities, read from the policy file.
	 */
	TCapabilitySet iUserCapabilities;

	/**
	 * This flag if set indicates that an orphaned file can be overwritten 
	 * during the install. The user is asked before the old file is deleted.
	 * A file is considered orphaned if it does not belong to any installed
	 * package in the registry.
	 */
	TBool iAllowOrphanedOverwrite;
	
	/**
	* This flag is set if SWI is allows to write sis stub files to removable
	* media cards. If a normal SIS file is installed on a removable device
	* appending the stub allows it to be in-place installed when the media
	* card is placed into another Symbian OS device 
	*/
	TBool iAllowPackagePropagate;
	
	/**
	 * The time in microseconds to allow applications to shutdown during
	 * uninstall or upgrade operations.  Applications which handle shut down
	 * events are sent such an event; the upgrade/uninstall operation will
	 * abort if they fail to shut down within this time.
	 */
	TInt iApplicationShutdownTimeout;
	
	/**
	 * The time in microseconds to wait for the completion of execution of
	 * files launched with the RunWait file option during an install/uninstall.
	 */
	TInt iRunWaitTimeout;

	/**
	 * This is set to ETrue if  SIS files with no target devices listed are to
	 * be considered compatible.
	 */
	TBool iSISCompatibleIfNoTargetDevices;

	/**
 	 * This is set to ETrue if applications are allowed to run when being installed 
 	 * or uninstalled.
 	 */	
 	TBool iAllowRunOnInstallUninstall;

	/** Set of replacement paths. */
	RPointerArray<CPathReplaceEntry> iReplEntries;
	
	
	/**
		ECOM plugin which implements HTTP header filter for OCSP requests.
		Zero means no plugin was specified.
	 */
	TUint32 iOcspHttpHeaderFilter;

	/**
	 * This is set to ETrue if pre-installed files are to be deleted on
	 * on uninstall, if the stub sis file used to install the package
	 * is not read-only.
	 */
	TBool iDeletePreinstalledFilesOnUninstall;

	/** Reference count used by RSecPolHandle. */
	mutable TInt iRefCount;
	
	/**
	 * This member indicates whether Software Install should allow 
	 * orphaned files in private directories to be overwritten or
	 * eclipsed during install.
	 *
	 */
	TBool iAllowProtectedOrphanOverwrite;
	
	/**
	A list of alternative OIDs for code signing.
	*/
	RPointerArray<HBufC> iAlternativeCodeSigningOids;
	
	/**
	Set this option to ETrue to delay the uninstallation of embedded packages 
	until all their dependent packages have been uninstalled.
	*/
	TBool iRemoveOnlyWithLastDependent;

	/**
	* The maximum number of log entries allowed.  Zero if logging disabled.
	*/
	TInt iMaxNumOfLogEntries; 
	
	/**
	Whether the restore process should overwrite applications on writeable media if they
	already exist.
	*/
	TBool iAllowOverwriteOnRestore;
	
	// 50 equivalent to KCommsDbSvrMaxFieldLength
	TBuf<50> iPhoneTsyName;

	friend class RSecPolHandle;

#ifdef _DEBUG	
	enum TPanic
		{
		ERRPFindPathNoDelim = 0x10, ERRPFindTooShort, ERRPFindNoLeadBackslash,
			ERRPFindNoTrailBackslash, ERRPRepTooShort, ERRPRepInvalidDrive,
			ERRPNoRepColonBackslash, ERRPRepNoTrailBackslash
		};
	static void Panic(TPanic aPanic);
#endif
	};

class RSecPolHandle
/**
	This handle class provides access to a single
	immutable instance of CSecurityPolicy.  This
	instance is reference counted and stored in
	sisregistryclient's thread-local storage.
	
	An instance of this object can be used in place
	of CSecurityPolicy::NewL(), but has the advantage
	that, providing it is opened once, the policy
	object can be accessed without continually re-reading
	and parsing the ini file, or maintaining a complex
	web of references or pointers.
 */
	{
public:
	IMPORT_C RSecPolHandle();
	IMPORT_C void OpenL();
	IMPORT_C void OpenLC();
	IMPORT_C void Close();
	
	inline const CSecurityPolicy& operator()() const;
	
private:
#ifdef _DEBUG
	enum TPanic
		{
		EOpnAlreadyOpen = 0x10
		};
	static void Panic(TPanic aPanic);
#endif
private:
	/** Reference counted policy object. */
	const CSecurityPolicy* iSecPol;
	};


inline const CSecurityPolicy& RSecPolHandle::operator()() const
/**
	Accessor function provides access to the security policy
	object which this handle reference counts.

	@return					const reference to underlying
							CSecurityPolicy object.
 */
	{
	return *iSecPol;
	}

} // namespace SWI

#endif // __SECURITYPOLICY_H__

/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines common types for the SIF APIs.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFCOMMON_H
#define SIFCOMMON_H

#include <ct/rcpointerarray.h>
#include <s32strm.h>
#include <usif/usifcommon.h>

namespace Usif
    {
    class MOpaqueParam;

    /**
        This enumeration defines the possible values of the status of an installation package. This status
        can be obtained from a @see CComponentInfo object using the @see CComponentInfo::InstallStatusL()
        method.
     */
    enum TInstallStatus
        {
        ENewComponent,                  ///< New installation; the component is not present in the system
        EUpgrade,                       ///< This is a valid upgrade of an existing component
        EAlreadyInstalled,              ///< Component with the same version is already installed
        ENewerVersionAlreadyInstalled,  ///< A newer version of the component is already installed
        EInvalid                        ///< Package is not valid and cannot be installed, for example,
                                        ///< it may be an upgrade of an uninstalled component or the
                                        ///< package may be corrupt
        };

    /**
        This enumeration defines the possible values of the authenticity of an installation package. This authenticity
        can be obtained from a @see CComponentInfo object using the @see CComponentInfo::Authenticity()
        method.
     */
    enum TAuthenticity
        {
        ENotAuthenticated,          ///< Component could not be authenticated by the installer, e.g. unsigned or self-signed
        EAuthenticated,             ///< Component is authenticated
        };

    /**
        The TSecurityContext class allows SIF plug-ins to verify the capabilities and secure Ids of SIF clients.
        For example, when a SIF client requests silent installation a SIF plug-in may verify that that the client
        has the TrustedUI capability.
     */
    class TSecurityContext
        {
    public:
        /**
            Checks if the SIF client has a given capability.

            @param aCapability The capability to test.
            @return ETrue if the SIF client has the capability, EFalse otherwise.
        */
        IMPORT_C TBool HasCapability(TCapability aCapability) const;

        /**
            Checks if the SIF client has a given capability.

            @param aCapability The capability to test.
            @leave KErrPermissionDenied If the SIF client does not have the capability.
        */
        IMPORT_C void HasCapabilityL(TCapability aCapability) const;

        /**
            Returns the secure Id of a SIF client.
        */
        IMPORT_C TSecureId SecureId() const;

    private:
        friend class CSifTransportRequest;
        IMPORT_C TSecurityContext(const RMessage2& aMessage);

        const RMessagePtr2 iMsg;
        };

    /**
        This class provides information about a software package. Returned by RSoftwareInstall::GetComponentInfo()."
     */
    class CComponentInfo : public CBase
        {
    public:

        class CApplicationInfo : public CBase
            {
        public:
            IMPORT_C static CApplicationInfo* NewLC(const TUid& aAppUid, const TDesC& aName, const TDesC& aGroupName, const TDesC& aIconFileName);
            static CApplicationInfo* NewL(RReadStream& aStream);
            void ExternalizeL(RWriteStream& aStream) const;
                        
            IMPORT_C const TUid& AppUid() const;
            IMPORT_C const TDesC& Name() const;
            IMPORT_C const TDesC& GroupName() const;
            IMPORT_C const TDesC& IconFileName() const;                       
            IMPORT_C virtual ~CApplicationInfo();
                        
        private:                  
            CApplicationInfo();           
            
            TUid iAppUid; // Application UID
            HBufC* iName; // Name of the application
            HBufC* iGroupName; // Group folder name where the application will be present
            HBufC* iIconFileName; // Full path of the icon file
            };
               
        class CNode : public CBase
            {
        public:
            /**
                Initializes the object with the details of a component.

                @param aSoftwareTypeName The name of the software type of a component.
                @param aComponentName The name of a component.
                @param aVersion The version of a component.
                @param aVendor The name of a component vendor.
                @param aScomoState The SCOMO state of a component.
                @param aInstallStatus The installation status of a component.
                @param aComponentId The Id of a component.
                @param aGlobalComponentId The global Id of a component.
                @param aAuthenticity The authenticity of a component.
                @param aUserGrantableCaps User grantable capabilities of a component.
                @param aHasExe Whether component has an executable.
                @param aIsDriveSelectionRequired Whether drive selection is required or not.
                @param aApplications Information of the applications in the package.
                @param aChildren Child nodes describing embedded components of a component. This method
                        passes the ownership of the child nodes to this new node and empties the aChildren array.
                However, if the leave occurs the caller must delete the child nodes in the array.
                @leave KErrOverflow it the total length of aName, aVersion and aVendor exceeds 512 characters.
                @leave Or other system wide error code
            */
            IMPORT_C static CNode* NewLC(const TDesC& aSoftwareTypeName, const TDesC& aComponentName,
                                const TDesC& aVersion, const TDesC& aVendor, TScomoState aScomoState,
                                TInstallStatus aInstallStatus, TComponentId aComponentId,
                                const TDesC& aGlobalComponentId, TAuthenticity aAuthenticity,
                                const TCapabilitySet& aUserGrantableCaps, TInt aMaxInstalledSize,
                                TBool aHasExe, TBool aIsDriveSelectionRequired, 
                                RPointerArray<CApplicationInfo>* aApplications = NULL, 
                                RPointerArray<CNode>* aChildren = NULL);

            /**
                Adds a child node to this node. This method passes the ownership of the child node
                to this node so the caller must pop it from the cleanup stack but not delete.

                @return The pointer to the child node to be added.
                @leave System wide error code
            */
            IMPORT_C void AddChildL(CNode* aChild);

            /**
                Returns the name of software type of a component.

                @return The name of the software type of a component.
                @leave System wide error code
            */
            IMPORT_C const TDesC& SoftwareTypeName() const;

            /**
                Returns the name of a component.

                @return The name of a component.
                @leave System wide error code
            */
            IMPORT_C const TDesC& ComponentName() const;

            /**
                Returns the version of a component.

                @return The version of a component.
                @leave System wide error code
            */
            IMPORT_C const TDesC& Version() const;

            /**
                Returns the name of the component vendor.

                @return The name of the component vendor.
                @leave System wide error code
            */
            IMPORT_C const TDesC& Vendor() const;

            /**
                Returns the SCOMO state of a component. This has meaning only if the installStatus of the component
                is not EInvalid or ENewComponent.

                @return The SCOMO state of a component.
                @leave System wide error code
            */
            IMPORT_C TScomoState ScomoState() const;

            /**
                Returns the installation status of a component.

                @return The installation status of a component.
                @leave System wide error code
            */
            IMPORT_C TInstallStatus InstallStatus() const;

            /**
                Returns the Id of a component.

                @return The Id of a component.
                @leave System wide error code
            */
            IMPORT_C TComponentId ComponentId() const;

            /**
                Returns the global Id of a component.

                @return The global Id of a component.
                @leave System wide error code
            */
            IMPORT_C const TDesC& GlobalComponentId() const;

            /**
                Returns the authenticity of a component.

                @return The authenticity of a component.
                @leave System wide error code
            */
            IMPORT_C TAuthenticity Authenticity() const;

            /**
                Returns user capabilities required by a component.

                @return The user grantable capabilities.
                @leave System wide error code
            */
            IMPORT_C const TCapabilitySet& UserGrantableCaps() const;

            /**
                Returns the maximum size of a component after installation.

                @return The maximum size of a component.
                @leave System wide error code
            */
            IMPORT_C TInt MaxInstalledSize() const;

            /**
                Returns the array of CComponentInfo objects for the components embedded
                in the component being queried.

                @return The array of the embedded CComponentInfo objects.
                @leave System wide error code
            */
            IMPORT_C const RPointerArray<CNode>& Children() const;

            /**
                Destructor.
            */
            IMPORT_C virtual ~CNode();
            /**
                Returns the HasExecutable Flag which indicates whether the
                component has an executable.

                @return The HasExecutable TBool Flag .
            */
            IMPORT_C TBool HasExecutable() const;
            
            /**
            Returns the IsDriveSelectionRequired Flag which indicates whether the
            drive selection is required or not.

            @return The IsDriveSelectionRequired TBool Flag .
            */
            IMPORT_C TBool DriveSeletionRequired() const;
            
            /**
            Returns the applications in the component being queried.

            @return The array of the CApplicationInfo objects.
            @leave System wide error code
            */
            IMPORT_C const RPointerArray<CApplicationInfo>& Applications() const;

        private:
            friend class CComponentInfo;

            CNode();
            CNode& operator=(const CNode& other);
            CNode(const CNode& other);

            static CNode* NewL(RReadStream& aStream);
            void ExternalizeL(RWriteStream& aStream) const;

            HBufC* iSoftwareTypeName;                           ///< The name of software type of the component
            HBufC* iComponentName;                              ///< The name of the software component
            HBufC* iVersion;                                    ///< The Version of the software component 
            HBufC* iVendor;                                     ///< The of the vendor of the software component
            HBufC* iGlobalComponentId;                          ///< The global id of the component if already installed
            TScomoState iScomoState;                            ///< The SCOMO status of the component
            TInstallStatus iInstallStatus;                      ///< Informs whether the component is already installed or upgradeable
            TComponentId iComponentId;                          ///< The unique id of the component if already installed
            TAuthenticity iAuthenticity;                        ///< The authenticity of the component
            TCapabilitySet iUserGrantableCaps;                  ///< User grantable capabilities required be the component
            TInt iMaxInstalledSize;                             ///< The maximum size of the component after installation
            TBool iHasExe;                                      ///< Indicates Whether the component has an executable
            TBool iIsDriveSelectionRequired;                    ///< Indicates Whether drive selection is required or not.
            RCPointerArray<CApplicationInfo> iApplications;     ///< The array of application info of the component being queried
            RCPointerArray<CNode> iChildren;                    ///< The array of components embedded in the component being queried            
            };

    public:

        /**
            Creates a new instance of the CComponentInfo class.

            @leave System wide error code
         */
        IMPORT_C static CComponentInfo* NewL();

        /**
            Creates a new instance of the CComponentInfo class.

            @leave System wide error code
         */
        IMPORT_C static CComponentInfo* NewLC();

        /**
            Returns the root node of this CComponentInfo object.

            @return The root node of this CComponentInfo object.
            @leave System wide error code
         */
        IMPORT_C const CNode& RootNodeL() const;

        /**
            Sets a node as the root node of this component info. This method passes the
            ownership of the root node to this object so the caller must pop it from the
            cleanup stack but not delete.
            
            @return The pointer to the root node to be set.
            @leave System wide error code
         */
        IMPORT_C void SetRootNodeL(CNode* aRootNode);

        /**
            Returns the pointer to the root node of this object. The caller takes the ownership
            of this node and must delete it when no longer needed. After this call the CComponentInfo
            object doesn't contain a root node. Hence, calling RootNodeL() results in the leave with
             KErrNotFound.
            
            @return The pointer to the root node.
            @leave System wide error code
         */
        IMPORT_C void SetRootNodeAsChildL(CNode& aParentNode);

        /**
            Externalises an object of this class to a write stream.

            @param aStream Stream to which the object should be externalised.
            @leave System wide error code
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
            Allocates a buffer for serialized CComponentInfo and registers it in the @see TIpcArgs class.
            The SIF server serializes a @see CComponentInfo object into this buffer.

            @param aIpcArgs Arguments to be sent to the SIF Server.
            @param aIndex An index value that identifies the slot in the array of arguments into which
            the argument value is to be placed. This must be a value in the range 0 to 3.
            @leave System wide error code
        */
        IMPORT_C void PrepareForIpcL(TIpcArgs& aIpcArgs, TInt aIndex);

        /**
            Destructor.
        */
        IMPORT_C virtual ~CComponentInfo();

    private:
        CComponentInfo();
        CComponentInfo& operator=(const CComponentInfo& other);
        CComponentInfo(const CComponentInfo& other);

        void InternalizeFromExternalBufferL() const;
        void ConstInternalizeL(RReadStream& aStream) const;
        void CleanupExternalBuffer() const;

        CNode* iRootNode;

        mutable HBufC8* iExternalBuffer;
        mutable TPtr8 iExternalBufferPtr;
        mutable TBool iDeferredInternalization;

        enum
            {
            // An arbitrary limit for the length of a single descriptor, for example aComponentName, aVersion or aVendor.
            KMaxDescriptorLength = 256,
            // An arbitrary limit for the total length of iSoftwareTypeName, iComponentName, iVersion, iVendor and iGlobalComponentId.
            KMaxHeapBufLength = 2048,
            // An arbitrary size of the internal buffer for sending the component info across the process boundary
            KMaxComponentInfoStreamedSize = 2*KMaxHeapBufLength+6*sizeof(TInt)+sizeof(TInt8),
            };
        };


    /**
        The COpaqueNamedParams class is a container for integer and string values identified by name.

        This class is designed for sending opaque params across the process boundary. For example, a SIF
        client may use it to send custom arguments to the SIF server or receive custom results.
    */
    NONSHARABLE_CLASS(COpaqueNamedParams) : public CBase
        {
    public:
        /**
            Creates a new instance of the COpaqueNamedParams class.

            @leave System wide error code
         */
        IMPORT_C static COpaqueNamedParams* NewL();

        /**
            Creates a new instance of the COpaqueNamedParams class.

            @leave System wide error code
         */
        IMPORT_C static COpaqueNamedParams* NewLC();

        /**
            Externalises an object of this class to a write stream.

            @param aStream Stream to which the object should be externalised.
            @leave System wide error code
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
            Internalises an object of this class from a read stream.

            @param aStream Stream from which the object should be internalised.
            @leave System wide error code
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
            Allocates a buffer and serializes the params of this object into it. The buffer is registered
            in the @see TIpcArgs class in order to be sent to the SIF Server.

            @param aIpcArgs Ipc arguments to be sent to the SIF Server.
            @param aIndex An index value that identifies the slot in the array of arguments into which
            the argument value is to be placed. This must be a value in the range 0 to 3.
            @leave System wide error code
        */
        IMPORT_C void PrepareArgumentsForIpcL(TIpcArgs& aIpcArgs, TInt aIndex) const;

        /**
            Allocates a buffer for serialized opaque results and registers it in the TIpcArgs class.
            The SIF server serializes opaque results into this buffer.

            @param aIpcArgs Arguments to be sent to the SIF Server.
            @param aIndex An index value that identifies the slot in the array of arguments into which
            the argument value is to be placed. This must be a value in the range 0 to 3.
            @leave System wide error code
        */
        IMPORT_C void PrepareResultsForIpcL(TIpcArgs& aIpcArgs, TInt aIndex);

        /**
            Returns the number of the opaque params added to this object.
            @leave System wide error code
        */
        IMPORT_C TInt CountL() const;

        /**
            Adds a string param to this object. If a param with the same name already exists it gets overwritten.

            @param aName The name of the string param to be added.
            @param aValue The value of the string param to be added.
            @leave KErrOverflow if the size of the value of the string param being added exceeds 1024 bytes or if the name of the string param being added 
            exceeds 128 bytes or the memory allocated for all the params added to this container exceeds 4096 bytes.
            @leave System wide error code
        */
        IMPORT_C void AddStringL(const TDesC& aName, const TDesC& aValue);
        
        /**
            Adds a string array param to this object. If a param with the same name already exists it gets overwritten.

            @param aName The name of the string param to be added.
            @param aValueArray The array of value string params to be added.
            @leave KErrOverflow if the size of the value of the string param being added exceeds 1024 bytes or if the name of the string param being added 
            exceeds 128 bytes or the memory allocated for all the params added to this container exceeds 4096 bytes.
            @leave System wide error code.
        */
        IMPORT_C void AddStringArrayL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray);        

        /**
            Adds an integer param to this object. If a param with the same name already exists it gets overwritten.

            @param aName The name of the integer param to be added.
            @param aValue The value of the integer param to be added.
            @leave KErrOverflow if the size of the name of the string param being added exceeds 128 bytes or
            the memory allocated for all the params added to this container exceeds 4096 bytes.
            @leave System wide error code
        */
        IMPORT_C void AddIntL(const TDesC& aName, TInt aValue);
        
        /**
            Adds an integer array param to this object. If a param with the same name already exists it gets overwritten.

            @param aName The name of the integer param to be added.
            @param aValueArray The array of value integer params to be added.
            @leave KErrOverflow if the size of the name of the string param being added exceeds 128 bytes or
            the memory allocated for all the params added to this container exceeds 4096 bytes.
            @leave System wide error code.
        */
        IMPORT_C void AddIntArrayL(const TDesC& aName, const RArray<TInt>& aValueArray);
        

        /**
            Returns the list of the names of params added to the object.

            @param aNames The list of the names of params. The caller is responsible for the instantiation
            and cleanup of RPointerArray<HBufC>. The caller must also destroy all the elements of aNames added
            by this method.
            @leave System wide error code
        */
        IMPORT_C void GetNamesL(RPointerArray<HBufC>& aNames) const; 

        /**
            Gets a reference to the string param identified by aName.

            @param aName The name of the string param to be obtained.
            @return The value of the string param or KNullDesC if not found.
            @leave System wide error code
        */
        IMPORT_C const TDesC& StringByNameL(const TDesC& aName) const;
        
        /**
            Gets a reference to the string array param identified by aName.

            @param aName The name of the string param to be obtained.
            @return The string array param.
            @leave KErrNotFound if aName is not found.
			@leave System wide error code.
        */
        IMPORT_C const RPointerArray<HBufC>& StringArrayByNameL(const TDesC& aName) const;        

        /**
            Returns the integer param identified by aName.

            @param aName The name of the integer param to be obtained.
            @param aValue The value of the integer param returned by this method.
            @return Indicates if the requested param has been found.
            @leave System wide error code
        */
        IMPORT_C TBool GetIntByNameL(const TDesC& aName, TInt& aValue) const;

        /**
            Returns the integer param identified by aName.

            @param aName The name of the integer param to be obtained.
            @return The value of the integer param.
            @leave KErrNotFound if not found
            @leave Or other system-wide error code.
        */
        IMPORT_C TInt IntByNameL(const TDesC& aName) const;
        
        /**
            Returns the integer array param identified by aName.

            @param aName The name of the integer param to be obtained.
            @return The integer array param.
            @leave KErrNotFound if aName is not found.
            @leave Or other system-wide error code.
        */
        IMPORT_C const RArray<TInt>& IntArrayByNameL(const TDesC& aName) const;        

        /**
            Empties the container and frees all memory allocated to the params.
        */
        IMPORT_C void Cleanup();

        /**
            Destructor.
        */
        IMPORT_C virtual ~COpaqueNamedParams();

    private:
        COpaqueNamedParams();
        COpaqueNamedParams& operator=(const COpaqueNamedParams& other);
        COpaqueNamedParams(const COpaqueNamedParams& other);
        
        void InternalizeFromExternalBufferL() const;
        void ConstInternalizeL(RReadStream& aStream) const;
        void ConstCleanup() const;
        void CleanupExternalBuffer() const;
        void VerifyExternalizedSizeForNewParamArrayL(TInt aNameSize, TInt aValueSize) const;
        void VerifyExternalizedSizeForExistingParamArrayL(TInt aOldValueSize, TInt aNewValueSize) const;
        void AddOpaqueParamL(MOpaqueParam* aItemBase);
private:
        RPointerArray<MOpaqueParam> iParams;
        
        mutable HBufC8* iExternalBuffer;
        mutable TPtr8 iExternalBufferPtr;
        mutable TBool iDeferredInternalization;
        mutable TInt iExternalizedSize;

        };

	/**
        Input Opaque Parameters:

		Pre-defined opaque arguments and results:
		"InstallInactive" - Yes/No 
		If provided in the opaque installation parameters, requests the installation of
		a component without activating it. Otherwise, components are activeted after installation by default.
		
		"InstallSilently" - Yes/No 
		If provided in the opaque installation parameters, requests a silent installation,
		i.e. one which avoids interaction with the user whenever possible. Please note that this may involve
		additional security checks, and is not necessarily supported by all installers.

		"Drive" -  Array of TUInt 
		Preffered drive in the order of priority with zero index being the highest priority
		
		"Languages" - Array of TLanguage
		List of languages to use
		
		"OCSPUrl" - TDesC
		The OSCP responder's URL

		"PerformOCSP" - TSifPolicy
		Whether OCSP should be performed or not or Request user

		"IgnoreOCSPWarnings" - TSifPolicy
		Action in case of OCSP warnings 

		"AllowUpgrade" - TSifPolicy
		Allow an upgrade (if yes, upgrade of the application) 

		"AllowUpgradeData" - TSifPolicy
		Whether private data of the application to be retained or removed during the upgrade

		"InstallOptionalItems" - TSifPolicy
		Install optional items in package 

		"AllowUntrusted" - TSifPolicy
		Check if installation of untrusted (uncertified) package is allowed 

		"GrantCapabilities" - TSifPolicy
		Whether to grant user capabilities 

		"AllowAppShutdown" - TSifPolicy
		Should we silently kill an application if needed 

		"AllowAppBreakDependency" - TSifPolicy
		Allow uninstall of dependent packages 

		"AllowOverwrite" - TSifPolicy
		Can files be overwritten 

		"PackageInfo" - TSifPolicy
		Whether to skip infos 

		"AllowDownload" - TSifPolicy
		Is it ok to download 

		"UserName" - TDesC
		UserName for Download request

		"Password" - TDesC
		Password for Download request

		"SourceUrl" - TDesC
		SourceUrl or Directory

		"IAP" - TInt
		Internet Access Point

		"SNAP" -  TInt
		Service Network Access Point, alternative to passing iap 

  		"MimeType" -  TDesC
		Represents MIME Type to be used during HTTP download

  		"Charset" -  TDesC
		Represents charset to be used during HTTP download 

		"AllowIncompatible" -  TSifPolicy
		Check if installation of incompatible packages is allowed 

        

		Output Opaque parameters:

		"ComponentId" - Array of TUInt
		If returned in the custom results, provides the id's of the newly added components
		after an install. Please note that not all installers (i.e. SIF Plugins) may support this.
		Eg: Includes embedded sis packages component ID's as well for native installer

		"ErrCategory" - TInt
		If returned in the custom results gives the error category, that is defined by USIF

		"ErrCode" - TInt
		If returned in the custom results gives one of the USIF error code.
		
		"ExtendedErrCode" - TInt
		If returned in the custom results gives the extended error information, that is specific to 
		a software component type.

		"ErrMessage" - TDesC
		If returned in the custom results gives the localized error message

		"ErrMessageDetails" - TDesC
		If returned in the custom results gives the localized error message details about error

	*/
	_LIT(KSifInParam_InstallInactive, "InstallInactive");
	_LIT(KSifInParam_InstallSilently, "InstallSilently");
	_LIT(KSifInParam_Drive, "Drive");
	_LIT(KSifInParam_Languages, "Languages");
	_LIT(KSifInParam_OCSPUrl, "OCSPUrl");
	_LIT(KSifInParam_PerformOCSP, "PerformOCSP");
	_LIT(KSifInParam_IgnoreOCSPWarnings, "IgnoreOCSPWarnings");
	_LIT(KSifInParam_AllowUpgrade, "AllowUpgrade");
	_LIT(KSifInParam_AllowUpgradeData, "AllowUpgradeData");
	_LIT(KSifInParam_InstallOptionalItems, "InstallOptionalItems");
	_LIT(KSifInParam_AllowUntrusted, "AllowUntrusted");
	_LIT(KSifInParam_GrantCapabilities, "GrantCapabilities");
	_LIT(KSifInParam_AllowAppShutdown, "AllowAppShutdown");
	_LIT(KSifInParam_AllowAppBreakDependency, "AllowAppBreakDependency");
	_LIT(KSifInParam_AllowOverwrite, "AllowOverwrite");
	_LIT(KSifInParam_PackageInfo, "PackageInfo");
	_LIT(KSifInParam_AllowDownload, "AllowDownload");
	_LIT(KSifInParam_UserName, "UserName");
	_LIT(KSifInParam_Password, "Password");
	_LIT(KSifInParam_SourceUrl, "SourceUrl");
	_LIT(KSifInParam_IAP, "IAP");
	_LIT(KSifInParam_SNAP, "SNAP");
	_LIT(KSifInParam_MimeType, "MimeType");
	_LIT(KSifInParam_Charset, "Charset");
	_LIT(KSifInParam_AllowIncompatible, "AllowIncompatible");

	_LIT(KSifOutParam_ComponentId, "ComponentId");
	_LIT(KSifOutParam_ErrCategory, "ErrCategory");
	_LIT(KSifOutParam_ErrCode, "ErrCode");
	_LIT(KSifOutParam_ExtendedErrCode, "ExtendedErrCode");
	_LIT(KSifOutParam_ErrMessage, "ErrMessage");
	_LIT(KSifOutParam_ErrMessageDetails, "ErrMessageDetails");

    /**
        SIF Server Secure ID
    
        Installers should use this id to verify that installation requests come from the SIF Server.
    */
    _LIT_SECURE_ID(KSifServerUid,0x10285BCB);

    } // namespace Usif

#endif // USIF_SIF_COMMON_H

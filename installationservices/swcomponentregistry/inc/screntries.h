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
* Declares entry classes for Software Components Registry interface.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SCRENTRIES_H
#define SCRENTRIES_H

#include <e32base.h>
#include <s32strm.h>
#include <f32file.h> 
#include <usif/usifcommon.h>

namespace Usif
    {
    // CONSTANTS
    
    /**
        KUnspecifiedLocale means that the locale is not specified and locale resolving will be done by SCR.
        First, the current locale with its downgraded languages (@see BaflUtils::GetDowngradePathL) is tried 
        by SCR. If no value is found, then non-localized values are searched for. 
     */
    static const TLanguage KUnspecifiedLocale = TLanguage(-1);
    
    /**
        KNonLocalized means that only non-localized values are looked up by SCR.
     */
    static const TLanguage KNonLocalized = TLanguage(0); 
    
    // CLASSES
    
        
    NONSHARABLE_CLASS(CGlobalComponentId) : public CBase
    /**
        Encapsulates the global software component identifier which is a named, component level and nullable string.
        that an installer may use to specify a deterministic identifier for the component that is unique within 
        the context of that software type. This global component id would be the package UID for native applications 
        or the midlet-names for Java etc 
        
        The global component id is consistent and equal on all devices. It is used to state dependencies 
        and to check for the presence of installed software components of arbitrary types without each installer having 
        to be aware of every other installers naming scheme.        
     */
        {
    public:
        /**
            Creates an empty global software component ID object.
            @return A pointer to the newly allocated global software component ID object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CGlobalComponentId* NewLC();
                
        /**
            Creates a global software component ID object.
            @param aGlobalIdName A deterministic identifier for the component that is unique within the context of that software type.
            @param aUniqueSwTypeName Non-localized unique software type name, such as @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava.
            @return A pointer to the newly allocated global software component ID object, if creation is successful.
         */ 
        IMPORT_C static CGlobalComponentId* NewL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName);
        
        /**
            Creates a global software component ID object.
            @param aGlobalIdName A deterministic identifier for the component that is unique within the context of that software type.
            @param aUniqueSwTypeName Non-localized unique software type name, such as @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava.
            @return A pointer to the newly allocated global software component ID object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */ 
        IMPORT_C static CGlobalComponentId* NewLC(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName);
        
        /**
            Creates a global software component ID object from a stream.
            @param aStream The stream to read the software component entry object from
            @return A pointer to the newly allocated global software component ID object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CGlobalComponentId* NewL(RReadStream& aStream);
                
        /**
            Externalises this CGlobalComponentId object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
                
        /**
            Internalises this CGlobalComponentId object from a read stream.
            @param aStream The stream from which the object is internalised.
            @internalComponent
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);
                
        /** @return The global id name part (@see CGlobalComponentId::NewLC) of the global component id.*/
        IMPORT_C const TDesC& GlobalIdName() const;
            
        /** @return The software type name part (@see CGlobalComponentId::NewLC) of the global component id. */
        IMPORT_C const TDesC& SoftwareTypeName() const;
        
        IMPORT_C ~CGlobalComponentId();
        
    private:
        CGlobalComponentId();
        void ConstructL(const TDesC& aGlobalIdName, const TDesC& aSoftwareTypeName);
        
    private:
        HBufC* iGlobalIdName;       ///< The global id name part of the global component id.
        HBufC* iSoftwareTypeName;   ///< The software type name part of the global component id.
        };
    
    
    NONSHARABLE_CLASS(CVersionedComponentId) : public CBase
    /**     
        Class that represents a continuous set of versions for a particular component.
        
        This class is used for specifying dependencies, where a dependant component requires 
        specific version(s) of the supplier to be present on the system. The version information 
        is optional, i.e. this class can be used to represent any version of a component.
        
        If this class is used to represent a dependant component, it specifies the range of versions
        of the dependency relationship defined by the dependant component. 
     */
        {
    public:
        
        /**
            Creates an empty version component identifier object.
            @return A pointer to the newly allocated version component identifier object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CVersionedComponentId* NewLC();
                
        /**
            Creates a versioned component identifier object.
            
            N.B. The version parameters are optional and used to set the lower and upper limit of the version range. 
                 It is also possible to set only an upper limit or a lower limit or none. If only one version 
                 parameter is supplied then a lower limit will be set. In this case, there is no upper limit. 
                 If the first version parameter is upplied with KNullDesC, then the second version parameter
                 will be used to set an upper limit.
            
            @param aGlobalId The global software component identifier.
            @param aVersionFrom The first version of the specified version range.
            @param aVersionTo The last version of the specified version range.
                    
            @return A pointer to the newly allocated versioned component identifier object, if creation is successful.
         */ 
        IMPORT_C static CVersionedComponentId* NewL(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom=NULL, const TDesC* aVersionTo=NULL);

        /**
            Creates a versioned component identifier object.
            For more information about the version parameters look at @see CVersionedComponentId::NewLC().
            
            @param aGlobalId The global software component identifier.
            @param aVersionFrom The first version of the specified version range.  
            @param aVersionTo The last version of the specified version range.
            
            @return A pointer to the newly allocated versioned component identifier object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */ 
        IMPORT_C static CVersionedComponentId* NewLC(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom=NULL, const TDesC* aVersionTo=NULL);
        
        /**
            Creates a versioned component identifier object from a stream.
            @param aStream The stream to read the versioned component identifier object from
            @return A pointer to the newly allocated versioned component identifier object, if creation is successful.
            @internalComponent
          */
        IMPORT_C static CVersionedComponentId* NewL(RReadStream& aStream);

        /**
            Internalises this CVersionedComponentId object from a read stream.
            @param aStream The stream from which the object is internalised.
            @internalComponent
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);
        
        /**
            Externalises this CVersionedComponentId object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        
        /** @return The global id of the versioned component. */
        IMPORT_C const CGlobalComponentId& GlobalId() const;
        
        /** @return The first version of the specified version range. NULL, if there is no lower limit for the version range. */
        IMPORT_C const TDesC* VersionFrom() const;
        
        /** @return  The last version of the specified version range. NULL, if there is no upper limit for the version range. */
        IMPORT_C const TDesC* VersionTo() const;
        
        IMPORT_C ~CVersionedComponentId();
                
    private:
        CVersionedComponentId();
        void ConstructL(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom, const TDesC* aVersionTo);
        
    private:
        CGlobalComponentId* iGlobalId;  ///< The global id of the versioned component identifier.
        HBufC* iVersionFrom;        ///< The first version of the specified version range.
        HBufC* iVersionTo;      ///< The last version of the specified version range.
        };

    
    class CComponentEntry : public CBase
    /**
        Class that represents a software component entry stored in the SCR.
     */
        {
        friend class CScrRequestImpl;
    public:
        /**
            Creates an empty software component entry object.
            @return A pointer to the newly allocated software component object, if creation is successful.
         */
        IMPORT_C static CComponentEntry* NewL();
        
        /**
            Creates an empty software component entry object.
            @return A pointer to the newly allocated software component object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CComponentEntry* NewLC();
        
        /**
            Creates a software component entry object from a stream.
            @param aStream The stream to read the software component entry object from
            @return A pointer to the newly allocated software component entry object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CComponentEntry* NewL(RReadStream& aStream);
        
        /**
            Creates a software component entry object.
            
            @param aId  The unique ID of the software component.
            @param aName The name of of the software component.
            @param aVendor The vendor name of the software component.
            @param aSwType The software type of the component.
            @param aGlobalId The globally unique identifier of the component.
            @param aRemovable Whether the component is removable. ETrue means the component is removable.
            @param aSize The total size of the component at installation time in bytes.
            @param aScomoState The SCOMO state of the component.
            @param aInstalledDrives The set of drives that files for the software component were installed to. 
            @param aVersion The version identifier of the software component.
            @param aInstallTime The formatted install time string. The format is as following: YYYYMMDD:HHMMSS
            @param aDrmProtected Whether the component is DRM protected. ETrue means the component is DRM protected.
            @param aHidden Whether the component is displayed in the list of installed components. ETrue means the component is hidden.
            @param aKnownRevoked Whether the component has been revoked. ETrue means that a revocation check was
                                   performed during or after the installation and the component was found to be revoked.
            @param aOriginVerified Whether the installer has verified the originator of the component. ETrue means the installer 
                                    has verified the originator of the software component by using appropriate checking mechanisms.
            @return A pointer to the newly allocated software component object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CComponentEntry* NewL(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType,
                                              const TDesC& aGlobalId, TBool aRemovable, TInt64 aSize, const TScomoState& aScomoState,
                                              const TDriveList& aInstalledDrives, const TDesC& aVersion, const TDesC& aInstallTime,
                                              TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified);
        
        /**
            Creates a software component entry object.
            
            @param aId  The unique ID of the software component.
            @param aName The name of of the software component.
            @param aVendor The vendor name of the software component.
            @param aSwType The software type of the component.
            @param aGlobalId The globally unique identifier of the component.
            @param aRemovable Whether the component is removable. ETrue means the component is removable.
            @param aSize The total size of the component at installation time in bytes.
            @param aScomoState The SCOMO state of the component.
            @param aInstalledDrives The set of drives that files for the software component were installed to. 
            @param aVersion The version identifier of the software component.
            @param aInstallTime The formatted install time string. The format is as following: YYYYMMDD:HHMMSS  
            @param aDrmProtected Whether the component is DRM protected. ETrue means the component is DRM protected.
            @param aHidden Whether the component is displayed in the list of installed components. ETrue means the component is hidden.
            @param aKnownRevoked Whether the component has been revoked. ETrue means that a revocation check was
                                   performed during or after the installation and the component was found to be revoked.
            @param aOriginVerified Whether the installer has verified the originator of the component. ETrue means the installer 
                                    has verified the originator of the software component by using appropriate checking mechanisms.
            @return A pointer to the newly allocated software component object, if creation is successful.
                           The pointer is also put onto the cleanup stack.
            @internalComponent
         */
        IMPORT_C static CComponentEntry* NewLC(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType, 
                                               const TDesC& aGlobalId, TBool aRemovable, TInt64 aSize, const TScomoState& aScomoState, 
                                               const TDriveList& aInstalledDrives, const TDesC& aVersion, const TDesC& aInstallTime,
                                               TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified);
        
        IMPORT_C ~CComponentEntry();
        
        /**
            Externalises this CComponentEntry object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        
        /**
            Internalises this CComponentEntry object from a read stream.
            @param aStream The stream from which the object is internalised.
            @internalComponent
        */
        IMPORT_C void InternalizeL(RReadStream& aStream);
        
        /** @return The unique ID of the software component. */
        IMPORT_C const TComponentId& ComponentId() const; 
        
        /** @return The name of the software component. */
        IMPORT_C const TDesC& Name() const;
        
        /** @return The vendor name of the software component. */
        IMPORT_C const TDesC& Vendor() const;
        
        /** @return The software type name of the software component, e.g @see Usif::KSoftwareTypeNative. */
        IMPORT_C const TDesC& SoftwareType() const;
        
        /** @return Globally unique component identifier. */
        IMPORT_C const TDesC& GlobalId() const;
        
        /** @return Whether the software component is removable. ETrue means it is removable. */
        IMPORT_C TBool IsRemovable() const;
        
        /** @return The total size of the component at installation time in bytes. */
        IMPORT_C TInt64  ComponentSize() const;
        
        /** @return The SCOMO state of the software component. @see Usif::TScomoState */
        IMPORT_C const TScomoState& ScomoState() const;
        
        /** @return The set of drives that files for the software component were installed to. */
        IMPORT_C const TDriveList& InstalledDrives() const;
        
        /** @return The version identifier of the software component. */
        IMPORT_C const TDesC& Version() const;
        
        /** @return The install time of the component. */
        IMPORT_C TTime InstallTimeL() const;
        
        /** @return  Whether the component is DRM protected. ETrue means the component is DRM protected.*/
        IMPORT_C TBool IsDrmProtected() const;
        
        /** @return  Whether the component is hidden in the list of installed components. ETrue means the component is hidden. */
        IMPORT_C TBool IsHidden() const;
        
        /** @return  Whether the component has been revoked. ETrue means that a revocation check was performed
                  during or after the installation and the component was found to be revoked.
         */
        IMPORT_C TBool IsKnownRevoked() const;
        
        /** @return  Whether the installer has verified the originator of the component. ETrue means the installer 
                         has verified the originator of the software component by using appropriate checking mechanisms.
         */
        IMPORT_C TBool IsOriginVerified()const;
        
    private:
        CComponentEntry();
        void ConstructL(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType, const TDesC& aGlobalId, TBool aRemovable, 
                        TInt64 aSize, const TScomoState& aScomoState, const TDriveList& aInstalledDrives, const TDesC& aVersion, const TDesC& aInstallTime,
                        TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified);
    private:
        TComponentId iComponentId;   ///< The unique id of the software component.
        HBufC* iName;                ///< Name of the software component.
        HBufC* iVendor;              ///< Vendor of the software component.
        HBufC* iSwType;              ///< Software type name of the component.
        HBufC* iGlobalId;            ///< Globally unique component identifier.
        TBool iRemovable;            ///< Whether the component is removable.
        TInt64 iSize;                ///< The total size of the component at installation time in bytes.
        TScomoState iScomoState;     ///< SCOMO state of the component.
        TDriveList iInstalledDrives; ///< The set of drives that files for the software component were installed to.
        HBufC* iVersion;             ///< Version identifier of the software component
        HBufC* iInstallTime;         ///< The install time of the component. 
        TBool iDrmProtected;         ///< Whether the component is DRM protected.
        TBool iHidden;               ///< Whether the component is hidden in the list of installed components.
        TBool iKnownRevoked;         ///< Whether the component has been revoked.
        TBool iOriginVerified;       ///< Whether the installer has verified the originator of the component.
        };
        
    class CPropertyEntry : public CBase
    /**
        Abstract base class for a software component property entry. 
        Specifies the type of a software component property. Before getting a property value, 
        the base property object must be converted to a derived property object (e.g. CIntPropertyEntry). 
     */
        {
    public:
    /** 
        Enumeration to indicate the type of the property.
     */
    enum TPropertyType
        {
        EBinaryProperty = 1,    ///< The property value is binary data. See @see CBinaryPropertyEntry.
        EIntProperty,       ///< The property value is integer. See @see CIntPropertyEntry.
        ELocalizedProperty,  ///< The property is a localizable string. See @see CLocalizablePropertyEntry.
        };  
    
    public:
        /**
            Creates a base software component property object from a stream.
            @param aStream The stream to read the software property object from.
            @return A pointer to the newly allocated software property object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CPropertyEntry* NewL(RReadStream& aStream);
        
        /** @return The name of the property. */
        IMPORT_C const TDesC& PropertyName() const;
        
        /** Virtual destructor for the component property object*/
        IMPORT_C virtual ~CPropertyEntry();
        
        /**
            Externalises a base/derived property object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;         
        
        /** @return The type of the property object. Implemented by derived classes. */
        virtual CPropertyEntry::TPropertyType PropertyType() const = 0;
        
    protected:
        CPropertyEntry();
        void ConstructL(const TDesC& aPropertyName);
    
    private:
        virtual void DoExternalizeL(RWriteStream &aStream) const = 0;
        virtual void DoInternalizeL(RReadStream& aStream) = 0;
        
    private:    
        HBufC* iPropertyName;         ///< The name of the property entry.
        };
    
    class CIntPropertyEntry : public CPropertyEntry
    /**
        Class that represents an integer component property. 
     */
        {
    public:
        /**
            Creates an integer component property object.
            
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
         */
        IMPORT_C static CIntPropertyEntry* NewL(const TDesC& aPropertyName, TInt64 aPropertyValue);
        
        /**
            Creates an integer component property object.
            
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CIntPropertyEntry* NewLC(const TDesC& aPropertyName, TInt64 aPropertyValue);
        
        IMPORT_C ~CIntPropertyEntry();
        
        /**
            The implementation of pure virtual function from @see Usif::CPropertyEntry.
            @return The type of the property object.
         */
        IMPORT_C CPropertyEntry::TPropertyType PropertyType() const;
        
        /** @return The 32-bit integer value. */ 
        IMPORT_C TInt IntValue();
        
        /** @return The 64-bit integer value. */ 
        IMPORT_C TInt64 Int64Value();
        
    private:
        void DoExternalizeL(RWriteStream& aStream) const;
        void DoInternalizeL(RReadStream& aStream);
        
        CIntPropertyEntry();
        CIntPropertyEntry(TInt64 aPropertyIntValue);
        void ConstructL(const TDesC& aPropertyName);
        static CIntPropertyEntry* NewLC();
        friend CPropertyEntry* CPropertyEntry::NewL(RReadStream& aStream);
        
    private:
        TInt64 iIntValue;   ///< The integer property value.
        };
    
    class CBinaryPropertyEntry : public CPropertyEntry
    /**
        Class that represents a binary non-localizable 8-bit descriptor component property.     
    */
        {
    public:
        /**
            Creates a binary component property object.
                
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
         */
        IMPORT_C static CBinaryPropertyEntry* NewL(const TDesC& aPropertyName, const TDesC8& aPropertyValue);
        
        /**
            Creates a binary component property object.
                        
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CBinaryPropertyEntry* NewLC(const TDesC& aPropertyName, const TDesC8& aPropertyValue);
        
        IMPORT_C ~CBinaryPropertyEntry(); 
        
        /**
            The implementation of pure virtual function from @see Usif::CPropertyEntry.
            @return The type of the property object.
         */
        IMPORT_C TPropertyType PropertyType() const;
        
        /** @return The binary property value. */ 
        IMPORT_C const TDesC8& BinaryValue();
    
    private:
        CBinaryPropertyEntry();
        void ConstructL(const TDesC& aPropertyName, const TDesC8& aPropertyValue);
        void DoExternalizeL(RWriteStream& aStream) const;
        void DoInternalizeL(RReadStream& aStream);
    
        static CBinaryPropertyEntry* NewLC();
        friend CPropertyEntry* CPropertyEntry::NewL(RReadStream& aStream);
        
    private:
        HBufC8* iValue; ///< The value of the binary property.
        };
    
    NONSHARABLE_CLASS(CLocalizablePropertyEntry) : public CPropertyEntry
    /**
        Class that represents a localizable string component property.
        
        A localized component property is a string property which can have different values 
        for different locales. A particular instance of a localized component property includes 
        both the localized string value and the locale in question.
     */
        {
    public:
        /**
            Creates a localizable component property object.
                    
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @param aLocale The locale of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
         */
        IMPORT_C static CLocalizablePropertyEntry* NewL(const TDesC& aPropertyName, const TDesC& aPropertyValue, TLanguage aLocale);
        
        /**
            Creates a localizable component property object.
                            
            @param aPropertyName The name of the property.
            @param aPropertyValue The value of the property.
            @param aLocale The locale of the property.
            @return A pointer to the newly allocated component property object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CLocalizablePropertyEntry* NewLC( const TDesC& aPropertyName, const TDesC& aPropertyValue, TLanguage aLocale);
        
        IMPORT_C ~CLocalizablePropertyEntry();
        
        /** The locale of the localizable property object. */
        IMPORT_C TLanguage LocaleL() const;
        
        /**
            The implementation of pure virtual function from @see Usif::CPropertyEntry.
            @return The type of the property object.
         */
        IMPORT_C TPropertyType PropertyType() const;
        
        /** @return The string property value. */
        IMPORT_C const TDesC& StrValue() const;     
        
    private:
        CLocalizablePropertyEntry();
        CLocalizablePropertyEntry(TLanguage aLocale);
        void DoExternalizeL(RWriteStream &aStream) const;
        void DoInternalizeL(RReadStream& aStream);
        static CLocalizablePropertyEntry* NewLC();
        friend CPropertyEntry* CPropertyEntry::NewL(RReadStream& aStream);
        void ConstructL(const TDesC& aPropertyName, const TDesC& aPropertyValue);
        
    private:
        TLanguage iLocale; ///< The language code of the locale.
        HBufC* iStrValue; ///< The string value of the property
        };
    
    NONSHARABLE_CLASS(CComponentFilter) : public CBase
    /**
        A filter class which is used to filter the software components stored in the SCR.
        An empty filter object is first created and then required filter keys are set separately.
     */
        {
        friend class CScrRequestImpl;
    public:
        
        enum TDbOperator
            {
            EEqual = 1,
            ELike
            };
        
        /**
            Creates an empty component filter object.
            @return A pointer to the newly allocated component filter object, if creation is successful.
         */
        IMPORT_C static CComponentFilter* NewL();
        
        /**
            Creates an empty component filter object.
            @return A pointer to the newly allocated component filter object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CComponentFilter* NewLC();
        
        /**
            Creates a component filter object from a stream.
            @param aStream The stream to read the component filter object from
            @return A pointer to the newly allocated component filter object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CComponentFilter* NewL(RReadStream& aStream);
        
        ~CComponentFilter();
        
        /**
            Restricts the returned component names in the filter.
            Please note that wildcards are not supported and matching is case-sensitive.
            @param aName The name of the software component.
         */
        IMPORT_C void SetNameL(const TDesC& aName);
        
        /**
            Restricts the returned component vendor names in the filter.
            Please note that wildcards are not supported and matching is case-sensitive.
            @param aVendor The vendor name of the software component.
         */
        IMPORT_C void SetVendorL(const TDesC& aVendor);
        
        /**
            Restricts the returned component software types in the filter.
            @param aSwType The software type name of the software component (e.g. @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava). 
        */
        IMPORT_C void SetSoftwareTypeL(const TDesC& aSwType);
        
        /**
            Sets whether the software component which is being searched for is removable.
            @param aValue ETrue, if the software component is removable. Otherwise, EFalse.
         */
        IMPORT_C void SetRemovable(TBool aValue);
        
        /**
            Sets whether the software component which is being searched for is DRM protected.
            @param aValue ETrue, if the software component is DRM protected. Otherwise, EFalse.
         */
        IMPORT_C void SetDrmProtected(TBool aValue);
        
        /**
            Sets whether the software component which is being searched for is hidden.
            @param aValue ETrue, if the software component is hidden. Otherwise, EFalse.
         */
        IMPORT_C void SetHidden(TBool aValue);
        
        /**
            Sets whether the software component which is being searched for is known-revoked.
            @param aValue ETrue, if the software component is known-revoked. Otherwise, EFalse.
         */
        IMPORT_C void SetKnownRevoked(TBool aValue);
        
        /**
            Sets whether the software component which is being searched for is origin-verified.
            @param aValue ETrue, if the software component is origin-verified. Otherwise, EFalse.
         */
        IMPORT_C void SetOriginVerified(TBool aValue);
        
        /**
            Restricts the returned component SCOMO states in the filter.
            See @see Usif::TScomoState for the list of SCOMO states.
            @param aScomoState The SCOMO state of the software component.
         */
        IMPORT_C void SetScomoStateL(TScomoState aScomoState);
        
        /**
            Sets the set of drives that files of the software components were installed to.
            If more than one drive is supplied, all software components which files were installed to
            any of these drives are returned.
         */
        IMPORT_C void SetInstalledDrivesL(const TDriveList& aDrives);
        
        /**
            Adds a localizable property to the filter. Only the software components having 
            this property with the specified value are returned by the filter. It is possible to 
            add more than one property. In this case, the software components possessing all provided 
            properties with the specified values are returned by the filter.
            
            For more information about searching for current locale variant of properties look at 
            @see RSoftwareComponentRegistry::GetComponentPropertyL.
            
            If the locale is provided, the component will be returned only if the property exists 
            for the specified locale. If the locale is not provided, or is equal to KUnspecifiedLocale,
            then all software components which have the property with the specified value for any locale will
            match this condition.
            
            @param aName The name of the property. Please note that matching is case-insensitive.
            @param aValue The value of the property.    
            @param aLocale Optional. The language code of the property value.
            @param aNameOperator The comparison operator (TDbOperator type) to be associated with the name.
            @param aValueOperator The comparison operator (TDbOperator type) to be associated with the value.          
                    
         */
        IMPORT_C void AddPropertyL(const TDesC& aName, const TDesC& aValue, TLanguage aLocale=KUnspecifiedLocale, 
                TDbOperator aNameOperator = EEqual, TDbOperator aValueOperator = EEqual);
        
        /**
            Adds an integer property to the filter. The software components having this
            property with the specified value are enumerated. It is possible to add more than one property. 
            In this case, the software components possessing all provided properties with the specified 
            values are enumerated.
            
            @param aName The name of the property. Please note that matching is case-insensitive.
            @param aValue The value of the property.
            @param aNameOperator The comparison operator (TDbOperator type) to be associated with the name.
            @param aValueOperator The comparison operator (TDbOperator type) to be associated with the value.          
            
         */
        IMPORT_C void AddPropertyL(const TDesC& aName, TInt64 aValue, TDbOperator aNameOperator = EEqual, 
                TDbOperator aValueOperator = EEqual);
        
        /**
            Adds a binary 8-bit descriptor property to the filter. The software components having this
            property with the specified value are enumerated. It is possible to add more than one property. 
            In this case, the software components possessing all provided properties with the specified 
            values are enumerated.
            
            @param aName The name of the property. Please note that matching is case-insensitive.
            @param aValue The value of the property.
            @param aNameOperator The comparison operator (TDbOperator type) to be associated with the name.
            @param aValueOperator The comparison operator (TDbOperator type) to be associated with the value.          
            
         */
        IMPORT_C void AddPropertyL(const TDesC& aName, const TDesC8& aValue, TDbOperator aNameOperator = EEqual, 
                TDbOperator aValueOperator = EEqual);       
        
        /**
            Sets a file name to filter the software components having registered this file.
            Please note that wildcards are not supported and matching is case-insensitive.
            
            N.B. This call can be invoked multiple times for the same filter in order to request components which register a particular set of files.
            @param aFile The fully qualified file path.
         */
        IMPORT_C void SetFileL(const TDesC& aFile);
        
        /**
            Externalises a component filter object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        
    private:
        CComponentFilter();
        void ConstructL();
        void InternalizeL(RReadStream& aStream);
        
        enum TFieldsFlag
            {
            EName           = 0x0001,
            EVendor         = 0x0002,
            ESoftwareType   = 0x0004,
            ERemovable      = 0x0008,
            EScomoState     = 0x0010,
            EInstalledDrive = 0x0020,
            EProperty       = 0x0040,
            EFile           = 0x0080,
            EDrmProtected   = 0x0100,
            EHidden         = 0x0200,
            EKnownRevoked   = 0x0400,
            EOriginVerified = 0x0800
            };
        
        class TPropertyOperator
            {
        public:
            IMPORT_C TPropertyOperator(TDbOperator aNameOperator = EEqual, TDbOperator aValueOperator = EEqual);
            void ExternalizeL(RWriteStream& aStream) const;
            IMPORT_C void InternalizeL(RReadStream& aStream);  
            IMPORT_C TDbOperator NameOperator() const;
            IMPORT_C TDbOperator ValueOperator() const;
        
        private:
            TDbOperator iNameOperator;
            TDbOperator iValueOperator;
            };
        
    private:
        TUint16 iSetFlag;            ///< Flag variable that shows what fields have been set
        HBufC* iName;                ///< Name for the software component
        HBufC* iVendor;              ///< Vendor for the software component
        HBufC* iSwType;              ///< Software type of the component.
        TBool iRemovable;            ///< Whether the software component is removable
        TScomoState iScomoState;     ///< SCOMO state for the software component
        TDriveList iInstalledDrives; ///< Installed drives for the software component
        RPointerArray<CPropertyEntry> iPropertyList; ///< The list of key properties
        HBufC* iFile;               ///< The full name of the file owned by the component
        TBool iDrmProtected;         ///< Whether the component is DRM protected.
        TBool iHidden;               ///< Whether the component is displayed in the list of installed components.
        TBool iKnownRevoked;         ///< Whether the component has been revoked.
        TBool iOriginVerified;       ///< Whether the installer has verified the originator of the component.
        RArray<TPropertyOperator> iPropertyOperatorList; ///< The list of operators associated with each property. 
        };

    
    class CLocalizableCommonData : public CBase
    /**
        Base class which provides common functionality for localized information classes (e.g. CLocalizableComponentInfo). 
        Please note that this class cannot be instantiated.
     */
        {
    public:
        ~CLocalizableCommonData();  
        
        /** @return The locale of the data. */
        IMPORT_C TLanguage Locale() const;
        
    protected:
        CLocalizableCommonData();
        CLocalizableCommonData(TLanguage aLocale);
        
        void ExternalizeL(RWriteStream& aStream) const;
        void InternalizeL(RReadStream& aStream);
        
    private:
        TLanguage iLocale; ///< The language code of the locale.
        };
        
    NONSHARABLE_CLASS(CLocalizableComponentInfo) : public CLocalizableCommonData
    /**
        This class contains the localizable information of a software component.
        Component names and vendor names are localizable names and transferred to the SCR 
        with the objects of this class.
     */
        {
    public:
        /**
            Creates a localizable component information object.
            @param aName The name of the component.
            @param aVendor The vendor of the component.
            @param aLocale The language code of the object's locale.
            @return A pointer to the newly allocated localizable component info object, if creation is successful.
         */
        IMPORT_C static CLocalizableComponentInfo* NewL(const TDesC& aName, const TDesC& aVendor, TLanguage aLocale);
        
        /**
            Creates a localizable component information object.
            @param aName The name of the component.
            @param aVendor The vendor of the component.
            @param aLocale The language code of the object's locale.
            @return A pointer to the newly allocated component information object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CLocalizableComponentInfo* NewLC(const TDesC& aName, const TDesC& aVendor, TLanguage aLocale);
        
        /**
            Creates a localizable component information from a stream.
            @param aStream The stream to read the component information object from.
            @return A pointer to the newly allocated component information object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CLocalizableComponentInfo* NewL(RReadStream& aStream);
        
        ~CLocalizableComponentInfo();
        
        /**
            Externalises a localizable component information object to a write stream.
            @param aStream The stream to which the object is externalised.
            @internalComponent
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;    
        
        /** @return The localizable name of the component. */
        IMPORT_C const TDesC& NameL() const;
        
        /** @return The localizable vendor name of the component. */
        IMPORT_C const TDesC& VendorL() const;
        
    private:
        CLocalizableComponentInfo();
        CLocalizableComponentInfo(TLanguage aLocale);
        void ConstructL(const TDesC& aName, const TDesC& aVendor);
        void InternalizeL(RReadStream& aStream);
        
    private:
        HBufC* iName;      ///< The localizable component name.
        HBufC* iVendor;    ///< The localizable component vendor name.
        };
    
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS    
    NONSHARABLE_CLASS(CLocalizedSoftwareTypeName) : public CLocalizableCommonData
        /**
            This class contains localized software type names.
            Software type names are localizable names and transferred to the SCR 
            with the objects of this class.
            
            @internalTechnology
         */
            {
        public: 
            /**
                Creates a localized software type name object.
                @param aName The name of the software type.
                @param aLocale The language code of the object's locale.
                @return A pointer to the newly allocated localized software type name object, if creation is successful.
             */
            IMPORT_C static CLocalizedSoftwareTypeName* NewL(const TDesC& aName, TLanguage aLocale);
            
            /**
                Creates a localized software type name object.
                @param aName The name of the software type.
                @param aLocale The language code of the object's locale.
                @return A pointer to the newly allocated localized software type name object, if creation is successful.
                        The pointer is also put onto the cleanup stack.
             */
            IMPORT_C static CLocalizedSoftwareTypeName* NewLC(const TDesC& aName, TLanguage aLocale);
            
            /**
                Creates a localized software type name object from a stream.
                @param aStream The stream to read the localized software type name object from.
                @return A pointer to the newly allocated localized software type name object, if creation is successful.
                @internalComponent
             */
            IMPORT_C static CLocalizedSoftwareTypeName* NewL(RReadStream& aStream);
            
            ~CLocalizedSoftwareTypeName();
            
            /**
                Externalises a localized software type name object to a write stream.
                @param aStream The stream to which the object is externalised.
                @internalComponent
             */
            IMPORT_C void ExternalizeL(RWriteStream& aStream) const;    
            
            /** @return The localized software type name. */
            IMPORT_C const TDesC& NameL() const;
            
        private:
            CLocalizedSoftwareTypeName();
            CLocalizedSoftwareTypeName(TLanguage aLocale);
            void ConstructL(const TDesC& aName);
            void InternalizeL(RReadStream& aStream);
            
        private:
            HBufC* iName;      ///< The localized software type name.
            };
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS   
    
    enum TScrComponentOperationType
    /**
        The type of the operation being performed on an SCR component. 
     */
        {
        EScrCompInstall =0,     ///< A new SCR component is added.
        EScrCompUnInstall,      ///< A existing SCR component is removed.
        EScrCompUpgrade,        ///< An existing SCR component is updated.
        EScrCompHidden          ///< The component operation is hidden to the user. N.B. SCR doesn't save logs for this type of operation.
        };
    
    NONSHARABLE_CLASS (CScrLogEntry) : public CBase
    /**
        Class that represents a software component log record generated by the SCR.
     */ 
        {
        friend class CScrRequestImpl;
    public:
        /**
            Creates a log entry object.
            @param aComponentName The name of the component.
            @param aUniqueSwTypeName The non-localized, unique software type name, such as @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava.
            @param aGlobalId The global Id of the component.
            @param aVersion The version of the component.
            @param aCompOpType The type of the component operation. @see Usif::TScrComponentOperationType
            @return A pointer to the newly allocated log entry object, if creation is successful.
            @internalComponent
         */
        IMPORT_C static CScrLogEntry* NewL(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType);
        
        /**
            Creates a log entry object.
            @param aComponentName The name of the component.
            @param aUniqueSwTypeName The non-localized, unique software type name, such as @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava.
            @param aGlobalId The global Id of the component.
            @param aVersion The version of the component.
            @param aCompOpType The type of the component operation. @see Usif::TScrComponentOperationType
            @return A pointer to the newly allocated log entry object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
            @internalComponent      
         */
        IMPORT_C static CScrLogEntry* NewLC(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType);

        /**
            Creates a log entry object from a stream.
            @param aStream The stream to read the log entry object from.
            @return A pointer to the newly allocated log entry object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
            @internalComponent
        */
        IMPORT_C static CScrLogEntry* NewLC(RReadStream& aStream);
        
        ~CScrLogEntry();
        
        /**
            Write the object to a stream 
         
            @param aStream The stream to write to
            @internalComponent
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
         
        /**
            Read the object from a stream
         
            @param aStream The stream to read from.
            @internalComponent
         */
        IMPORT_C void InternalizeL(RReadStream& aStream) ;
        
        /**
            @return The name of the component for which the log was generated.
         */
        IMPORT_C const TDesC& ComponentName() const; 
        
        /**
            @return The non-localized, unique software type name, such as @see Usif::KSoftwareTypeNative and @see Usif::KSoftwareTypeJava.
         */
        IMPORT_C const TDesC& SoftwareTypeName() const;
        
        /**
            The UID.
            @return The global ID of the component.
         */
        IMPORT_C const TDesC& GlobalId() const; 
                
        /**
            @return The version of the component.
         */
        IMPORT_C const TDesC& ComponentVersion() const; 
        
        /**
            @return The time when the operation occured.
         */
        IMPORT_C const TTime OperationTime() const; 
                
        /**
            @return The type of the component operation for the log record.
         */
        IMPORT_C TScrComponentOperationType OperationType() const;
        
        /**
            @return The component id of the log record.
            @internalComponent
         */
        IMPORT_C TComponentId ComponentId() const;
    
    private:
        CScrLogEntry();
        void ConstructL(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType);
        
    private:    
        HBufC* iComponentName ;
        HBufC* iSwTypeName;
        HBufC* iVersion;
        TScrComponentOperationType iOpType ; 
        TTime  iRecordTime ;   
        HBufC* iGlobalId;
        TComponentId iComponentId; 
        };

    } // End of namespace Usif

#endif // SCRENTRIES_H

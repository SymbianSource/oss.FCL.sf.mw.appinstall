/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef APPREGENTRIES_H
#define APPREGENTRIES_H

#include <e32base.h>
#include <s32strm.h>
#include <f32file.h> 
namespace Usif
    {

    class CScrRequestImpl;
    class CDataType;
    class CServiceInfo;
    class CLocalizableAppInfo;
    class CAppViewData;
    class CCaptionAndIconInfo;
     
    
    const TInt KSCRMaxAppGroupName=0x10; // Length of App's logical groupname
    typedef TBuf<KSCRMaxAppGroupName> TAppGroupName;
    
    NONSHARABLE_CLASS (TApplicationCharacteristics)
    /**
     Used to store application characteristics like its embeddabilty, group name etc.
     */
         {
     public:
         /**
          Creates an empty TApplicationCharacteristics object.
          */
         IMPORT_C TApplicationCharacteristics();
         
         IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
         
         IMPORT_C void InternalizeL(RReadStream& aStream);
             
     public:
         /** 
           Defines an application's support for embeddability. 
          */
         enum TAppEmbeddability {
             /** 
              An application cannot be embedded. 
             */
             ENotEmbeddable=0,
             /** 
              An application can be run embedded or standalone and can read/write embedded document-content. 
             */
             EEmbeddable=1,
             /** 
              An application can only be run embedded and can read/write embedded document-content. 
             */
             EEmbeddableOnly=2,
             /** 
              An application can be run embedded or standalone and cannot read/write embedded document-content. 
             */
             EEmbeddableUiOrStandAlone=5,
             /** 
              An application can only be run embedded and cannot read/write embedded document-content. 
             */
             EEmbeddableUiNotStandAlone=6 
             };         
         /** 
          Indicates the extent to which the application can be embedded.
         */
         TAppEmbeddability iEmbeddability;
         /** 
          Indicates whether the application is document-based and supports being asked 
          to create a new file. 
         */
         TBool iSupportsNewFile;
         /** 
          Indicates whether the existence of the application should be advertised to the 
          user. If this is set to ETrue, the application does not appear on the Extras 
          Bar (or equivalent). 
         */
         TBool iAppIsHidden; // not shown in the Shell etc.
         /** 
          Allows the application to be launched in the foreground (when set to EFalse) 
          or in the background (when set to ETrue). 
         */
         TBool iLaunchInBackground;  // Series 60 extension to allow apps to be launched in the background   
         /** 
          Stores the application's logical group name. 
         */
         TAppGroupName iGroupName; // Series 60 extension to allow apps to be categorized according a logical group name, e.g. 'games'
         /** 
          Indicates the application attributes. One or more values from TCapabilityAttribute may be specified. 
         */
         TUint iAttributes;
         };    
    
    class TEmbeddableFilter
       {
    public:
       /** Constructs an empty embeddability filter. 
        */
       IMPORT_C TEmbeddableFilter();
       
       /** Adds aEmbeddability value to the EmbeddabilityFlag. 
        */
       IMPORT_C void AddEmbeddability(TApplicationCharacteristics::TAppEmbeddability aEmbeddability);
       
       /** Compares aEmbeddability with the filter.
        * @param aEmbeddability TAppEmbeddability value to compare.
        * @see TAppEmbeddability for more details.
        * @return True, if aEmbeddability is included in the filter; false, otherwise. 
        */
       IMPORT_C TBool MatchesEmbeddability(TApplicationCharacteristics::TAppEmbeddability aEmbeddability) const;
       
       /** Sets the value of aEmbeddability with EmbeddabilityFlag's value.
        */
       void  SetEmbeddabilityFlags(TUint aEmbeddabilityFlags);
       
       /** Return the value of iEmbeddability.
        */
       IMPORT_C TUint EmbeddabilityFlags() const;
    private:
       TUint iEmbeddabilityFlags;
       };
    
    
    NONSHARABLE_CLASS(CAppInfoFilter) : public CBase
    /**
        A filter class which is used to filter the Application registration Info stored in the SCR.
        An empty filter object is first created and then required filter keys are set separately.
     */
        {
        friend class CScrRequestImpl;
      
    public:
        /**
            Creates an empty AppRegistrationInfo filter object.
            @return A pointer to the newly allocated AppRegistrationInfo filter object, if creation is successful.
         */
        IMPORT_C static CAppInfoFilter* NewL();
        
        /**
            Creates an empty AppRegistrationInfo filter object.
            @return A pointer to the newly allocated AppRegistrationInfo filter object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */
        IMPORT_C static CAppInfoFilter* NewLC();
        
        /**
            Creates a AppRegistrationInfo filter object from a stream.
            @param aStream The stream to read the AppRegistrationInfo filter object from
            @return A pointer to the newly allocated AppRegistrationInfo filter object, if creation is successful.
         */
        IMPORT_C static CAppInfoFilter* NewL(RReadStream& aStream);
        
        IMPORT_C void SetAllApps();
        IMPORT_C void SetAllApps(const TInt aScreenMode);
        
        IMPORT_C void SetEmbeddableApps();
        IMPORT_C void SetEmbeddableApps(const TInt aScreenMode);
        
        IMPORT_C void SetEmbeddabilityFilter(TEmbeddableFilter& aEmbeddabilityFilter);    
        IMPORT_C void SetEmbeddabilityFilterWithScreenMode(TEmbeddableFilter& aEmbeddabilityFilter, const TInt aScreenMode);  
        
        IMPORT_C void SetCapabilityAttributeMaskAndValue(const TInt aCapabilityAttributeMask, const TInt aCapabilityAttributeValue);
        IMPORT_C void SetCapabilityAttributeMaskAndValue(const TInt aCapabilityAttributeMask, const TInt aCapabilityAttributeValue, const TInt aScreenMode );
                
        IMPORT_C void SetServerApps(const TUid aServiceUid);
        IMPORT_C void SetServerApps(const TUid aServiceUid, const TInt aScreenMode);

        ~CAppInfoFilter();
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        void InternalizeL(RReadStream& aStream);
        
    private:
        CAppInfoFilter();
        void ConstructL();
               
        enum TAppRegInfoFlag
            {
            EAllApps                                                    = 0x0001,
            EAllAppsWithScreenMode                                      = 0x0002,
            EGetEmbeddableApps                                          = 0x0004,
            EGetEmbeddableAppsWithSreenMode                             = 0x0008,
            EGetFilteredAppsWithEmbeddabilityFilter                     = 0x0010,
            EGetFilteredAppsWithEmbeddabilityFilterWithScreenMode       = 0x0020,
            EGetFilteredAppsWithCapabilityMaskAndValue                  = 0x0040,
            EGetFilteredAppsWithCapabilityMaskAndValueWithScreenMode    = 0x0080,
            EGetServerApps                                              = 0x0100,
            EGetServerAppsWithScreenMode                                = 0x0200,
            };
        
    private:
        TUint16               iSetFlag;                                 ///< Flag variable that shows what fields have been set
        TInt                  iScreenMode;                              ///< Sreen Mode of the application
        TEmbeddableFilter     iEmbeddabilityFilter;                     ///< Embeddabilty Filter
        TUint                 iCapabilityAttributeMask;                 ///< Capability attribute bit mask
        TUint                 iCapabilityAttributeValue;                ///< Capability attribute bit value
        TUid                  iServiceUid;                              ///< Service Uid
        TUid                  iAppUid;                                  ///< Application Uid
        };

    
    NONSHARABLE_CLASS(CAppServiceInfoFilter) : public CBase
    /*
     * A filter class that determines the information fields that need to be set before a GetServiceInfoL is invoked.
     * Initially an empty filter is created and subsequently the Set methods are used to set the filter based on the input parameters 
     * with which the GetServiceInfoL query is made.
     */
    	{
    	friend class CScrRequestImpl;
    	      
    public:
    	    	
       	/**
    	Creates an empty CAppServiceInfoFilter filter object.
    	@return A pointer to the newly allocated CAppServiceInfoFilter filter object, if creation is successful.
    	*/
    	IMPORT_C static CAppServiceInfoFilter* NewL();
    	    	        
    	/**
    	Creates an empty CAppServiceInfoFilter filter object.
    	@return A pointer to the newly allocated CAppServiceInfoFilter filter object, if creation is successful.
    	The pointer is also put onto the cleanup stack.
    	*/
    	IMPORT_C static CAppServiceInfoFilter* NewLC();
    	    	        
    	/**
    	Creates a CAppServiceInfoFilter filter object from a stream.
    	@param aStream The stream to read the CAppServiceInfoFilter filter object from
    	@return A pointer to the newly allocated CAppServiceInfoFilter filter object, if creation is successful.
    	*/
    	IMPORT_C static CAppServiceInfoFilter* NewL(RReadStream& aStream);
    	    	  
    	IMPORT_C void SetAppUidForServiceInfo (const TUid aAppUid);
    	IMPORT_C void SetServiceUidForServiceImplementations (const TUid aServiceUid);
    	IMPORT_C void SetServiceUidAndDatatTypeForServiceImplementationsL (const TUid aServiceUid, const TDesC& aDataType);
    	IMPORT_C void SetAppUidAndServiceUidForOpaqueData (const TUid aAppUid, const TUid aServiceUid);
    	
    	~CAppServiceInfoFilter();
    	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    	void InternalizeL(RReadStream& aStream);
    	    	               
    private:
    	CAppServiceInfoFilter();
    	void ConstructL();
    	enum TServiceInfoFlag
            {
            EGetServiceInfoForApp                                       = 0x0001,
            EGetServiceImplementationForServiceUid                      = 0x0002,
            EGetServiceImplementationForServiceUidAndDatatType          = 0x0004,
            EGetOpaqueDataForAppWithServiceUid                          = 0x0008,
            };
    	
    private:
    	TUint16 iSetFlag;  
    	TUid iAppUid;
    	TUid iServiceUid;
    	HBufC* iDataType; 
    	};
    
    
    NONSHARABLE_CLASS(COpaqueData) : public CBase
        {
    friend class CScrRequestImpl;
    public:
        /**
            Creates an empty COpaqueData object.
            @return A pointer to the newly allocated COpaqueData object, if creation is successful.
         */            
        IMPORT_C static COpaqueData* NewL();
        
        /**
            Creates an empty COpaqueData object.
            @return A pointer to the newly allocated COpaqueData object, if creation is successful.
                    The pointer is also put onto the cleanup stack.
         */          
        IMPORT_C static COpaqueData* NewLC();
        
        /**
            Creates an empty COpaqueData object.
            
            @param aOpaqueData  A 8-bit buffer containing the opaque data.
            @param aLanguage    The language associated with the opaque data. 
            @return A pointer to the newly allocated COpaqueData object, if creation is successful.
         */       
        IMPORT_C static COpaqueData* NewL(TDesC8& aOpaqueData, TLanguage aLanguage);
        
        /**
            Creates an empty COpaqueData object.
            
            @param aOpaqueData  A 8-bit buffer containing the opaque data.
            @param aLanguage    The language associated with the opaque data. 
            @return A pointer to the newly allocated COpaqueData object, if creation is successful.
                     The pointer is also put onto the cleanup stack.
         */           
        IMPORT_C static COpaqueData* NewLC(TDesC8& aOpaqueData, TLanguage aLanguage);
        
        /**
            Creates a COpaqueData object from the stream.
                 
            @param  aStream   The stream to read the COpaqueData object from.
            @return A pointer to the newly allocated COpaqueData object, if creation is successful.
         */           
        IMPORT_C static COpaqueData* NewL(RReadStream& aStream);
        
        /**
         @return The buffer containing the opaque data.
         */
        IMPORT_C HBufC8& OpaqueData();
       
        /**
         @return The language associated with the opaque data.
         */       
        IMPORT_C TLanguage Language();
        
        ~COpaqueData();
        
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
        
        IMPORT_C void InternalizeL(RReadStream& aStream);
        
    private:
        COpaqueData();
        void ConstructL(const TDesC8& aOpaqueData, TLanguage aLanguage);

    private:
        HBufC8* iOpaqueData;
        TLanguage iLanguage;
        };

    class CPropertyEntry;
    NONSHARABLE_CLASS(CApplicationRegistrationData) : public CBase
    /*
     * This contains the basic information of an application such as application's uid, filename, 
     * attributes, embeddability, groupname, default screen number, opaque data, 
     * an array of pointers to localizable info, properties, file ownership info etc.
     *  
     * N.B. Mandatory fields associated to an application are AppUid and AppFile
     *      AppFile should contain the abosulte file path, along with the drive and extension (if any)
     */
          {
          friend class CScrRequestImpl;
      public:
          /**
              Creates an empty CApplicationRegistrationData object.
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
           */            
          IMPORT_C static CApplicationRegistrationData* NewL();
          
          /**
              Creates an empty CApplicationRegistrationData object.
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
                      The pointer is also put onto the cleanup stack.
           */                 
          IMPORT_C static CApplicationRegistrationData* NewLC();
          
          /**
              Creates a CApplicationRegistrationData object from the stream.
                   
              @param  aStream   The stream to read the CApplicationRegistrationData object from.
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
           */                        
          IMPORT_C static CApplicationRegistrationData* NewL(RReadStream& aStream);
          
          /**
              Creates a CApplicationRegistrationData object.
              
              @param aOwnedFileArray          An array of files owned by the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aServiceArray            An array of services related information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aLocalizableAppInfoList  An array of localizable information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppPropertiesArray      An array of properties associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aOpaqueDataArray         An array of opaque data associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppUid                  The Uid of the application.
              @param aAppFile                 The executable file associated with the application.
              @param aCharacteristics         The attributes associated with the application. @see Usif::TApplicationCharacteristics
              @param aDefaultScreenNumber
              @param aTypeId
              
              
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
           */          
          IMPORT_C static CApplicationRegistrationData* NewL(
                        RPointerArray<HBufC>& aOwnedFileArray,
                        RPointerArray<Usif::CServiceInfo>& aServiceArray,
                        RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                        RPointerArray<CPropertyEntry>& aAppPropertiesArray,
                        RPointerArray<COpaqueData>& aOpaqueDataArray,
                        TUid aAppUid, const TDesC& aAppFile, 
                        TApplicationCharacteristics& aCharacteristics,
                        TInt aDefaultScreenNumber, TInt aTypeId = 0);
          /**
              Creates a CApplicationRegistrationData object.
              
              @param aOwnedFileArray          An array of files owned by the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aServiceArray            An array of services related information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aLocalizableAppInfoList  An array of localizable information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppPropertiesArray      An array of properties associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aOpaqueDataArray         An array of opaque data associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppUid                  The Uid of the application.
              @param aAppFile                 The executable file associated with the application.
              @param aCharacteristics         The attributes associated with the application. @see Usif::TApplicationCharacteristics
              @param aDefaultScreenNumber
              @param aTypeId
              
              
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
                      The pointer is also put onto the cleanup stack.
           */            
          IMPORT_C static CApplicationRegistrationData * NewLC(
                        RPointerArray<HBufC>& aOwnedFileArray,
                        RPointerArray<CServiceInfo>& aServiceArray,
                        RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                        RPointerArray<CPropertyEntry>& aAppPropertiesArray,
                        RPointerArray<COpaqueData>& aOpaqueDataArray,
                        TUid aAppUid, const TDesC& aAppFile,
                        TApplicationCharacteristics& aCharacteristics,
                        TInt aDefaultScreenNumber, TInt aTypeId = 0);

           /**
              Creates a CApplicationRegistrationData object.
              
              @param aOwnedFileArray          An array of files owned by the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aServiceArray            An array of services related information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aLocalizableAppInfoList  An array of localizable information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppPropertiesArray      An array of properties associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppUid                  The Uid of the application.
              @param aAppFile                 The executable file associated with the application.
              
              
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
           */            
          IMPORT_C static CApplicationRegistrationData* NewL(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<CServiceInfo>& aServiceArray, RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                                                             RPointerArray<CPropertyEntry>& aAppPropertiesArray, TUid aAppUid, 
                                                             const TDesC& aAppFile);
           /**
              Creates a CApplicationRegistrationData object.
              
              @param aOwnedFileArray          An array of files owned by the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aServiceArray            An array of services related information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aLocalizableAppInfoList  An array of localizable information associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppPropertiesArray      An array of properties associated with the application. The ownership is also transferred to CApplicationRegistrationData.
              @param aAppUid                  The Uid of the application.
              @param aAppFile                 The executable file associated with the application.
              
              
              @return A pointer to the newly allocated CApplicationRegistrationData object, if creation is successful.
			          The pointer is also put onto the cleanup stack.
           */             
          IMPORT_C static CApplicationRegistrationData* NewLC(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<CServiceInfo>& aServiceArray, RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                                                             RPointerArray<CPropertyEntry>& aAppPropertiesArray, TUid aAppUid, 
                                                             const TDesC& aAppFile);
          ~CApplicationRegistrationData();
          IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		  IMPORT_C void InternalizeL(RReadStream& aStream);
              
          IMPORT_C const TUid AppUid() const;
          IMPORT_C const TDesC& AppFile() const;
          IMPORT_C TInt TypeId() const;
          IMPORT_C TUint Attributes() const;
          IMPORT_C TBool Hidden() const;
          IMPORT_C TApplicationCharacteristics::TAppEmbeddability Embeddability() const;
          IMPORT_C TBool NewFile() const;
          IMPORT_C TBool Launch() const;
          IMPORT_C const TDesC& GroupName() const;
          IMPORT_C TInt DefaultScreenNumber() const;
          IMPORT_C const RPointerArray<HBufC>& OwnedFileArray() const;
          IMPORT_C const RPointerArray<CServiceInfo>& ServiceArray() const;
          IMPORT_C const RPointerArray<CLocalizableAppInfo>& LocalizableAppInfoList() const;
          IMPORT_C const RPointerArray<CPropertyEntry>& AppProperties() const;
          IMPORT_C const RPointerArray<COpaqueData>& AppOpaqueData() const;

      private:
          CApplicationRegistrationData();
          void ConstructL(RPointerArray<HBufC>& aOwnedFileArray,
                RPointerArray<CServiceInfo>& aServiceArray, 
                RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                RPointerArray<CPropertyEntry>& aAppPropertiesArray,
                RPointerArray<COpaqueData>& aOpaqueDataArray, TUid aAppUid,
                const TDesC& aAppFile, TApplicationCharacteristics& aCharacteristics,
                TInt aDefaultScreenNumber, TInt aTypeId);
          
          void ConstructL(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<CServiceInfo>& aServiceArray,
                                    RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList, RPointerArray<CPropertyEntry>& aAppPropertiesArray,
                                    TUid aAppUid, const TDesC& aAppFile);
          
          void ConstructL(const TDesC& aAppFile);
      private:
      
          RPointerArray<HBufC> iOwnedFileArray;                               // FileOwnershipInfo
          RPointerArray<CServiceInfo> iServiceArray;                    // Service Info and Service Data Type
          RPointerArray<CLocalizableAppInfo> iLocalizableAppInfoList;   //LocalizableAppInfo
          RPointerArray<CPropertyEntry> iAppPropertiesArray;            // AppProperties
          RPointerArray<COpaqueData> iOpaqueDataArray;
          
          // AppRegistrationInfo Table
          TUid iAppUid;
          HBufC* iAppFile;
          TInt iTypeId;
          TApplicationCharacteristics iCharacteristics;
          TInt iDefaultScreenNumber;                          
          };

    
  NONSHARABLE_CLASS(CDataType) : public CBase
  /*
   * It contains the priority and type information associated to each service provided by the application.
   */
      {
      friend class CScrRequestImpl;
  public:
      /**
          Creates an empty CDataType object.
          @return A pointer to the newly allocated CDataType object, if creation is successful.
       */       
      IMPORT_C static CDataType* NewL();
      
      /**
          Creates an empty CDataType object.
          @return A pointer to the newly allocated CDataType object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */          
      IMPORT_C static CDataType* NewLC();
      
      /**
          Creates a CDataType object from the stream.
               
          @param  aStream   The stream to read the CDataType object from.
          @return A pointer to the newly allocated CDataType object, if creation is successful.
       */         
      IMPORT_C static CDataType* NewL(RReadStream& aStream);
      
      /**
          Creates a CDataType object.
          
          @param aPriority The priority to be associated with the service.
          @param aType     The type information to be associated with the service.
          @return A pointer to the newly allocated CDataType object, if creation is successful.
       */         
      IMPORT_C static CDataType* NewL(TInt aPriority, const TDesC& aType);
      
      /**
          Creates a CDataType object.
          
          @param aPriority The priority to be associated with the service.
          @param aType     The type information to be associated with the service.
          @return A pointer to the newly allocated CDataType object, if creation is successful.
                  The pointer is also put onto the cleanup stack.          
       */       
      IMPORT_C static CDataType* NewLC(TInt aPriority, const TDesC& aType);
      
      ~CDataType();
      
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	  
      IMPORT_C void InternalizeL(RReadStream& aStream);
      
      /**
       @return The priority associated with the service.
       */
      IMPORT_C TInt Priority() const;
 
      /**
       @return The type information associated with the service.
       */      
      IMPORT_C const TDesC& Type() const;
  private:
          CDataType();
          void ConstructL(TInt aPriority, const TDesC& aType);
  private:    
          TInt iPriority;
          HBufC* iType;
      };

  
  NONSHARABLE_CLASS(CServiceInfo) : public CBase
  /*
   * This contains the service related information such as service Uid, opaque data and data type information associated with an applciation.
   */
      {
      friend class CScrRequestImpl;
  public:
      /**
          Creates an empty CServiceInfo object.
          @return A pointer to the newly allocated CServiceInfo object, if creation is successful.
       */       
      IMPORT_C static CServiceInfo* NewL();
     
      /**
          Creates an empty CServiceInfo object.
          @return A pointer to the newly allocated CServiceInfo object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */       
      IMPORT_C static CServiceInfo* NewLC();
      
      /**
          Creates a CServiceInfo object from the stream.
               
          @param  aStream   The stream to read the CServiceInfo object from.
          @return A pointer to the newly allocated CServiceInfo object, if creation is successful.
       */        
      IMPORT_C static CServiceInfo* NewL(RReadStream& aStream);
      
      /**
          Creates a CServiceInfo object.
          @param  aUid              The Uid associated with the service.
          @param  aOpaqueDataArray  An array of opaque data associated with the service. The ownership is also transferred to 
                                    the CServiceInfo object.
          @param  aDataTypes        An array of data types associated with the service. The ownership is also transferred to 
                                    the CServiceInfo object.            
          @return A pointer to the newly allocated CServiceInfo object, if creation is successful.
       */       
      IMPORT_C static CServiceInfo* NewL(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes);
      
      /**
          Creates a CServiceInfo object.
          @param  aUid              The Uid associated with the service.
          @param  aOpaqueDataArray  An array of opaque data associated with the service. The ownership is also transferred to 
                                    the CServiceInfo object.
          @param  aDataTypes        An array of data types associated with the service. The ownership is also transferred to 
                                    the CServiceInfo object.           
          @return A pointer to the newly allocated CServiceInfo object, if creation is successful.
       */         
      IMPORT_C static CServiceInfo* NewLC(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes);
      ~CServiceInfo();
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	  IMPORT_C void InternalizeL(RReadStream& aStream);
	  
	  /**
	   @return The Uid associated with the service.
	   */
      IMPORT_C const TUid Uid() const;
      
      /**
       @return The array of data types associated with the service.
       */      
      IMPORT_C const RPointerArray<CDataType>& DataTypes() const;
      
      /**
       @return The array of opaque data associated with the service.
       */          
      IMPORT_C const RPointerArray<COpaqueData>& OpaqueData() const;
  private:
          CServiceInfo();
          void ConstructL(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes);
  private:    
          TUid iUid;
          RPointerArray<CDataType> iDataTypes;
          RPointerArray<COpaqueData> iOpaqueDataArray;
      };

  
  NONSHARABLE_CLASS(CLocalizableAppInfo) : public CBase
  /*
   * This contains localizable information associated with an application.
   */
      {
      friend class CScrRequestImpl;
  public:
      /**
          Creates an empty CLocalizableAppInfo object.
          @return A pointer to the newly allocated CLocalizableAppInfo object, if creation is successful.
       */         
      IMPORT_C static CLocalizableAppInfo* NewL();
      
      /**
          Creates an empty CLocalizableAppInfo object.
          @return A pointer to the newly allocated CLocalizableAppInfo object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */         
      IMPORT_C static CLocalizableAppInfo* NewLC();
      
      /**
          Creates a CLocalizableAppInfo object from the stream.
               
          @param  aStream   The stream to read the CLocalizableAppInfo object from.
          @return A pointer to the newly allocated CLocalizableAppInfo object, if creation is successful.
       */               
      IMPORT_C static CLocalizableAppInfo* NewL(RReadStream& aStream);
      
      /**
          Creates a CLocalizableAppInfo object.
          
          @param  aShortCaption          A locale specific short caption associated with the application. 
          @param  aApplicationLanguage   A language that the application is localized in. 
          @param  aGroupName             A group name associated with the application. 
          @param  aCaptionAndIconInfo    A locale specific caption and icon information associated with the application. The ownership is also transferred
                                         to the CLocalizableAppInfo object.
          @param  aViewDataList          An array of view information associated with the application. The ownership is also transferred
                                         to the CLocalizableAppInfo object.                  
          @return A pointer to the newly allocated CLocalizableAppInfo object, if creation is successful.
       */       
      IMPORT_C static CLocalizableAppInfo* NewL(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<CAppViewData>& aViewDataList);
      
      /**
          Creates a CLocalizableAppInfo object.
          
          @param  aShortCaption          A locale specific short caption associated with the application.
          @param  aApplicationLanguage   A language that the application is localized in.
          @param  aGroupName             A group name associated with the application.
          @param  aCaptionAndIconInfo    A locale specific caption and icon information associated with the application. The ownership is also transferred
                                         to the CLocalizableAppInfo object.     
          @param  aViewDataList          An array of view information associated with the application. The ownership is also transferred
                                         to the CLocalizableAppInfo object.                       
          @return A pointer to the newly allocated CLocalizableAppInfo object, if creation is successful.
                  The pointer is also put onto the cleanup stack.     
       */        
      IMPORT_C static CLocalizableAppInfo* NewLC(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<CAppViewData>& aViewDataList);
      
      ~CLocalizableAppInfo();
      
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	  
      IMPORT_C void InternalizeL(RReadStream& aStream);
     
      /**
       @return The short caption associated with the locale for the application.
       */
      IMPORT_C const TDesC& ShortCaption() const;
      
      /**
       @return The language that the application is localized in.
       */      
      IMPORT_C TLanguage ApplicationLanguage() const;
      
      /**
       @return The group name associated with the application.
       */        
      IMPORT_C const TDesC& GroupName() const;
      
      /**
       @return The array of Application view information associated with the application.
       */        
      IMPORT_C const RPointerArray<CAppViewData>& ViewDataList() const;
      
      /**
       @return The locale specific caption and icon information associated with the application.
       */        
      IMPORT_C const CCaptionAndIconInfo* CaptionAndIconInfo() const;

  private:
      CLocalizableAppInfo();
      
      void ConstructL(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<Usif::CAppViewData> aViewDataList);

      void ConstructL(const TDesC& aShortCaption, const TDesC& aGroupName);

  private:
      HBufC* iShortCaption;                               
      TLanguage iApplicationLanguage;                     
      HBufC* iGroupName;                                  
      CCaptionAndIconInfo* iCaptionAndIconInfo;
      RPointerArray<CAppViewData> iViewDataList;
      };

  
  NONSHARABLE_CLASS(CCaptionAndIconInfo) : public CBase
  /*
   * This contains the caption and icon information associated to an application such as caption, icon filename and number of icons.
   * 
   * N.B. Icon Filename, if present, should contain the absolute file path, along with the drive and extension (if any).
   */
      {
      friend class CScrRequestImpl;
          
  public:
      /**
          Creates an empty CCaptionAndIconInfo object.
          @return A pointer to the newly allocated CCaptionAndIconInfo object, if creation is successful.
       */           
      IMPORT_C static CCaptionAndIconInfo* NewL();
      
      /**
          Creates an empty CCaptionAndIconInfo object.
          @return A pointer to the newly allocated CCaptionAndIconInfo object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */           
      IMPORT_C static CCaptionAndIconInfo* NewLC();
      
      /**
          Creates a CCaptionAndIconInfo object from the stream.
               
          @param  aStream   The stream to read the CCaptionAndIconInfo object from.
          @return A pointer to the newly allocated CCaptionAndIconInfo object, if creation is successful.
       */          
      IMPORT_C static CCaptionAndIconInfo* NewL(RReadStream& aStream);
      
      /**
          Creates a CCaptionAndIconInfo object.
          
          @param  aCaption       The caption associated with the application. The ownership is transferred to the CCaptionAndIconInfo object.
          @param  aIconFileName  The absolute filename of the icon file. The ownership is transferred to the CCaptionAndIconInfo object.
          @param  aNumOfAppIcons The number of icons associated with the application.
          @return A pointer to the newly allocated CCaptionAndIconInfo object, if creation is successful.
       */         
      IMPORT_C static CCaptionAndIconInfo* NewL(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons);
      
      /**
          Creates a CCaptionAndIconInfo object.
          
          @param  aCaption       The caption associated with the application. The ownership is transferred to the CCaptionAndIconInfo object.
          @param  aIconFileName  The absolute filename of the icon file. The ownership is transferred to the CCaptionAndIconInfo object.
          @param  aNumOfAppIcons The number of icons associated with the application.
          @return A pointer to the newly allocated CCaptionAndIconInfo object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */         
      IMPORT_C static CCaptionAndIconInfo* NewLC(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons);
     
      ~CCaptionAndIconInfo();
      
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	  
      IMPORT_C void InternalizeL(RReadStream& aStream);
      
      /**
       @return The caption associated with the application.
       */
      IMPORT_C const TDesC& Caption() const;
      
      /**
       @return The absolute name of the icon file associated with the application.
       */      
      IMPORT_C const TDesC& IconFileName() const;
      
      /**
       @return The number of icons associated with the application.
       */      
      IMPORT_C TInt NumOfAppIcons() const;
  private:
      CCaptionAndIconInfo();
      void ConstructL(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons);
      
  private:
      HBufC* iCaption;                                    
      HBufC* iIconFileName;                                
      TInt iNumOfAppIcons;                                
      };

  
  NONSHARABLE_CLASS(CAppViewData) : public CBase
  /*
   * This contains view information of an application such as screenmode and an array of caption and icon info associated with each view.
   */
      {
      friend class CScrRequestImpl;

  public:
      /**
          Creates an empty CAppViewData object.
          @return A pointer to the newly allocated CAppViewData object, if creation is successful.
       */       
      IMPORT_C static CAppViewData* NewL();
      
      /**
          Creates an empty CAppViewData object.
          @return A pointer to the newly allocated CAppViewData object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */       
      IMPORT_C static CAppViewData* NewLC();
      
      /**
          Creates a CAppViewData object from the stream.
               
          @param  aStream   The stream to read the CAppViewData object from.
          @return A pointer to the newly allocated CAppViewData object, if creation is successful.
       */       
      IMPORT_C static CAppViewData* NewL(RReadStream& aStream);
      
      /**
          Creates a CAppViewData object.
          
          @param aUid                The Uid of the application.
          @param aScreenMode         The screen mode associated with the view.
          @param aCaptionAndIconInfo The caption and icon info associated with the view. The ownership is
                                     also transferred to the CAppViewData object.
          @return A pointer to the newly allocated CAppViewData object, if creation is successful.
       */         
      IMPORT_C static CAppViewData* NewL(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo);
      
      /**
          Creates a CAppViewData object.
          
          @param aUid                The Uid of the application.
          @param aScreenMode         The screen mode associated with the view.
          @param aCaptionAndIconInfo The caption and icon info associated with the view. The ownership is
                                     also transferred to the CAppViewData object.
          @return A pointer to the newly allocated CAppViewData object, if creation is successful.
                  The pointer is also put onto the cleanup stack.     
       */       
      IMPORT_C static CAppViewData* NewLC(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo);
      
      ~CAppViewData();
      
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	  
      IMPORT_C void InternalizeL(RReadStream& aStream);

      /**
       @return The Uid of the application.
       */
      IMPORT_C const TUid Uid() const;
      
      /**
       @return The screen mode associated with the view.
       */      
      IMPORT_C TInt ScreenMode() const;
     
      /**
       @return The caption and icon info associated with the view.
       */  
      IMPORT_C const CCaptionAndIconInfo* CaptionAndIconInfo() const;
      
  private:
      CAppViewData();
      void ConstructL(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo);
      
  private:
      TUid iUid;
      TInt iScreenMode;
      CCaptionAndIconInfo* iCaptionAndIconInfo;
      };

  
  NONSHARABLE_CLASS(CLauncherExecutable) : public CBase
  /**
   * This contains information regarding the executable used to launch the application.
   */
      {
      friend class CScrRequestImpl;
  public:
      /**
          Creates an empty CLauncherExecutable object.
          @return A pointer to the newly allocated CLauncherExecutable object, if creation is successful.
       */
      IMPORT_C static CLauncherExecutable* NewL();
      
      /**
          Creates an empty CLauncherExecutable object.
          @return A pointer to the newly allocated CLauncherExecutable object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */
      IMPORT_C static CLauncherExecutable* NewLC();
      
      /**
          Creates a CLauncherExecutable object.
          
          @param  aTypeId   The Application type id.
          @param  aLauncher The native executable used to launch the application
          @return A pointer to the newly allocated CLauncherExecutable object, if creation is successful.

       */
      IMPORT_C static CLauncherExecutable* NewL(TInt aTypeId, const TDesC& aLauncher);
      
      /**
          Creates a CLauncherExecutable object.
          
          @param  aTypeId   The Application type id.
          @param  aLauncher The native executable used to launch the application
          @return A pointer to the newly allocated CLauncherExecutable object, if creation is successful.
                  The pointer is also put onto the cleanup stack.
       */
      IMPORT_C static CLauncherExecutable* NewLC(TInt aTypeId, const TDesC& aLauncher);
      
      /**
          Creates a CLauncherExecutable object from the stream.
               
          @param  aStream   The stream to read the CLauncherExecutable object from.
          @return A pointer to the newly allocated CLauncherExecutable object, if creation is successful.
       */
      IMPORT_C static CLauncherExecutable* NewL(RReadStream& aStream);
      
      IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
      
      IMPORT_C void InternalizeL(RReadStream& aStream);
      
      /**
         @return The Application type id.
       */
      IMPORT_C TInt TypeId() const;
      
      /**
         @return The executable used to launch the application.
       */      
      IMPORT_C const TDesC& Launcher() const;
      
      IMPORT_C ~CLauncherExecutable();
        
  private:
      CLauncherExecutable();
      void ConstructL(TInt aTypeId, const TDesC& aLauncher);
        
  private:
      TInt iTypeId;       ///< Application type id (same as the software type id of the component)
      HBufC* iLauncher;   ///< The native executable used to launch the application
      };

    
} // End of USIF Namespace

#endif // APPREGENTRIES_H

/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* sifnotification.h - Interfaces to publish (by installers) and get notified (for clients) to an SIF operation progress
*
*/

/**
 @file
 @released
 @publishedPartner
*/

#ifndef __SIFNOTIFICATION_H__
#define __SIFNOTIFICATION_H__

#include <e32std.h>
#include <e32base.h>
#include <s32strm.h>
#include <usif/usiferror.h>
#include <e32property.h> 

namespace Usif
{
// Forward declaration.
class CSifNotifierBase;
class CDeleteKeyTimer;

//Identifies the type of operation
enum TSifOperationPhase
    {
    EInstalling = 1,
    EUninstalling,
    EUpgrading
    };

//Identifies a stage in type of operation
enum TSifOperationSubPhase 
    {
    ENoSubPhase = 1,
    EOCSPCheck,
    EDownload,
    EFileOperation
    };

enum TSifOperation
    {
    ESifOperationStart = 1,
    ESifOperationEnd
    };


/*
 * This contains the basic information of package going to be installed/uninstalled (provided at the start of the operation)
 */
NONSHARABLE_CLASS(CSifOperationStartData) : public CBase    
    {
public:
    /**
        Creates a CSifOperationStartData object from the stream.
             
        @param  aStream   The stream to read the CSifOperationStartData object from.
        @return A pointer to the newly allocated CSifOperationStartData object, if creation is successful.
     */ 
    IMPORT_C static CSifOperationStartData* NewL(RReadStream& aStream);
    
    /**
        Creates a CSifOperationStartData object.

		@deprecated  Use the overload with aOperationPhase instead.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aComponentName		The name of the software component
        @param  aApplicationNames	The list of applications associated with the software component, an empty array in case of no applications
        @param  aApplicationIcons   The list of corresponding application icon names(without path) for every application, an empty array in case of no applications                 
        @param  aComponentSize		The total size of the component in bytes
        @param  aIconPath			The path where all application and component icons are found. It must have a trailing backslash.
        @param  aComponentIcon		The name of the component icon(without path)
        @param  aSoftwareType		The software type name of the software component
                                                   
        @return A pointer to the newly allocated CSifOperationStartData object, if creation is successful.
     */      
    IMPORT_C static CSifOperationStartData* NewL(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,
                                                  const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, 
                                                      const TDesC& aSoftwareType);
    
    /**
        Creates a CSifOperationStartData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aComponentName      The name of the software component
        @param  aApplicationNames   The list of applications associated with the software component, an empty array in case of no applications
        @param  aApplicationIcons   The list of corresponding application icon names(without path) for every application, an empty array in case of no applications                 
        @param  aComponentSize      The total size of the component in bytes
        @param  aIconPath           The path where all application and component icons are found. It must have a trailing backslash.
        @param  aComponentIcon      The name of the component icon(without path)
        @param  aSoftwareType       The software type name of the software component
        @param  aOperationPhase     The type of SIF operation
                                                   
        @return A pointer to the newly allocated CSifOperationStartData object, if creation is successful.
     */      
    IMPORT_C static CSifOperationStartData* NewL(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,
                                                  const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, 
                                                      const TDesC& aSoftwareType, TSifOperationPhase aOperationPhase);

    /**
        Creates a CSifOperationStartData object.

  		@deprecated  Use the overload with aOperationPhase instead.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aComponentName      The name of the software component
        @param  aApplicationNames   The list of applications associated with the software component, an empty array in case of no applications
        @param  aApplicationIcons   The list of corresponding application icon names(without path) for every application, an empty array in case of no applications                 
        @param  aComponentSize      The total size of the component in bytes
        @param  aIconPath           The path where all application and component icons are found. It must have a trailing backslash.
        @param  aComponentIcon      The name of the component icon(without path)
        @param  aSoftwareType       The software type name of the software component
                                                           
        @return A pointer to the newly allocated CSifOperationStartData object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */     
    IMPORT_C static CSifOperationStartData* NewLC(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,
                                                      const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, 
                                                          const TDesC& aSoftwareType);
    

    /**
        Creates a CSifOperationStartData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aComponentName      The name of the software component
        @param  aApplicationNames   The list of applications associated with the software component, an empty array in case of no applications
        @param  aApplicationIcons   The list of corresponding application icon names(without path) for every application, an empty array in case of no applications                 
        @param  aComponentSize      The total size of the component in bytes
        @param  aIconPath           The path where all application and component icons are found. It must have a trailing backslash.
        @param  aComponentIcon      The name of the component icon(without path)
        @param  aSoftwareType       The software type name of the software component
        @param  aOperationPhase     The type of SIF operation
                                                           
        @return A pointer to the newly allocated CSifOperationStartData object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */     
    IMPORT_C static CSifOperationStartData* NewLC(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,
                                                      const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, 
                                                          const TDesC& aSoftwareType, TSifOperationPhase aOperationPhase);
    
    /**
     @return The global component Id associated with the operation.
     */     
    IMPORT_C const HBufC& GlobalComponentId() const;
    
    /**
     @return The component name associated with the operation.
     */     
    IMPORT_C const HBufC& ComponentName() const;
    
    /**
     @return An array of application names.
     */     
    IMPORT_C const RPointerArray<HBufC>& ApplicationNames() const;
    
    /**
     @return An array of application icons.
     */     
    IMPORT_C const RPointerArray<HBufC>& ApplicationIcons() const;
    
    /**
     @return The size of the component in bytes.
     */      
    IMPORT_C TInt ComponentSize() const;    
    
    /**
     @return The path to the application icons.
     */     
    IMPORT_C const HBufC& IconPath() const; 
   
    /**
     @return The icon associated with the component.
     */      
    IMPORT_C const HBufC& ComponentIcon() const;
    
    /**
     @return The software type associated with the operation.
     */      
    IMPORT_C const HBufC& SoftwareType() const;    
    
    IMPORT_C ~CSifOperationStartData();          

    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    
    /**
     @return The type of SIF operation.
     */   
    IMPORT_C TSifOperationPhase OperationPhase() const;

private:
    CSifOperationStartData();
    void ConstructL(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,  
                        const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, 
                            const TDesC& aComponentIcon, const TDesC& aSoftwareType, TSifOperationPhase aOperationPhase);
    void InternalizeL(RReadStream& aStream);

private:
    HBufC* iGlobalComponentId;                                    
    HBufC* iComponentName;
    HBufC* iComponentIcon;
    RPointerArray<HBufC> iApplicationNames;
    RPointerArray<HBufC> iApplicationIcons;          
    TInt iComponentSize;
    HBufC* iIconPath;      
    HBufC* iSoftwareType;
    TSifOperation iOperationType;
    TSifOperationPhase iOperationPhase;
    };


/*
 * This contains the error status information after an install/uninstall operation
 */
NONSHARABLE_CLASS(CSifOperationEndData) : public CBase
    {      
public:
    /**
        Creates a CSifOperationEndData object from the stream.
             
        @param  aStream   The stream to read the CSifOperationEndData object from.
        @return A pointer to the newly allocated CSifOperationEndData object, if creation is successful.
     */     
    IMPORT_C static CSifOperationEndData* NewL(RReadStream& aStream);
    
    /**
        Creates a CSifOperationEndData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aErrCategory		A SIF error category
        @param  aErrCode			SIF plugin (or runtime) specific error code
        @param  aErrMsg				SIF plugin (or runtime) specific localized error message         
        @param  aErrMsgDetails		SIF plugin (or runtime) specific localized message details
                                                        
        @return A pointer to the newly allocated CSifOperationEndData object, if creation is successful.
     */     
    IMPORT_C static CSifOperationEndData* NewL(const TDesC& aGlobalComponentId, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails);    

    /**
        Creates a CSifOperationEndData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation          
        @param  aErrCategory		A SIF error category
        @param  aErrCode			SIF plugin (or runtime) specific error code
        @param  aErrMsg				SIF plugin (or runtime) specific localized error message         
        @param  aErrMsgDetails		SIF plugin (or runtime) specific localized message details
                                                        
        @return A pointer to the newly allocated CSifOperationEndData object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */         
    IMPORT_C static CSifOperationEndData* NewLC(const TDesC& aGlobalComponentId, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails);

    /**
     @return The global component Id associated with the operation.
     */     
    IMPORT_C const HBufC& GlobalComponentId() const;
    
    /**
    @return The category to which the error belongs.
    */     
    IMPORT_C TErrorCategory ErrorCategory() const;
       
    /**
    @return An error code associated with the operation.
    */     
    IMPORT_C TInt ErrorCode() const;  
       
    /**
     @return An error message.
     */     
    IMPORT_C const HBufC& ErrorMessage() const;
    
    /**
     @return More details regarding the error message. 
     */     
    IMPORT_C const HBufC& ErrorMessageDetails() const;
       
    IMPORT_C ~CSifOperationEndData();
  
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
    CSifOperationEndData();
    void ConstructL(const TDesC& aGlobalComponentID, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails);
    void InternalizeL(RReadStream& aStream);

private:     
    HBufC* iGlobalComponentId;   
    HBufC* iErrMsg;                                    
    HBufC* iErrMsgDetails;
    TErrorCategory iErrCategory; 
    TInt iErrCode;      
    TSifOperation iOperationType;
    };


/*
 * This class provides the operation progress.
 */
NONSHARABLE_CLASS(CSifOperationProgressData) : public CBase
    {
    public:
    /**
        Creates a CSifOperationProgressData object from the stream.
             
        @param  aStream   The stream to read the CSifOperationProgressData object from.
        @return A pointer to the newly allocated CSifOperationProgressData object, if creation is successful.
     */          
    IMPORT_C static CSifOperationProgressData* NewL(RReadStream& aStream);
    
    /**
        Creates a CSifOperationProgressData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation           
        @param  aPhase				The type of SIF operation
        @param  aSubPhase			The current stage in the type of SIF operation
        @param  aCurrentProgress    The amount of operation which is completed in current subphase
        @param  aTotal				The total amount of operation required by this subphase operation
                                                        
        @return A pointer to the newly allocated CSifOperationProgressData object, if creation is successful.
     */      
    IMPORT_C static CSifOperationProgressData* NewL(const TDesC& aGlobalcomponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal);      

    /**
        Creates a CSifOperationProgressData object.
        
        @param  aGlobalComponentId  The global component id which uniquely identifies the operation           
        @param  aPhase				The type of SIF operation
        @param  aSubPhase			The current stage in the type of SIF operation
        @param  aCurrentProgress    The amount of operation which is completed in current subphase
        @param  aTotal				The total amount of operation required by this subphase operation
                                                        
        @return A pointer to the newly allocated CSifOperationProgressData object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */     
    IMPORT_C static CSifOperationProgressData* NewLC(const TDesC& aGlobalcomponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal);

    /**
     @return The global component id associated with the operation.
     */ 
    IMPORT_C const HBufC& GlobalComponentId() const;
    
    /**
     @return The current phase of operation.
     */     
    IMPORT_C TSifOperationPhase Phase() const;
    
    /**
     @return The current sub-phase of operation.
     */    
    IMPORT_C TSifOperationSubPhase SubPhase() const;
    
    /**
     @return The current progress value.

	 @deprecated  Use CurrentProgress() instead.
     */     
    IMPORT_C TInt CurrentProgess() const;
    
    /**
     @return The current progress value.
     */     
    IMPORT_C TInt CurrentProgress() const;
    
    /**
     @return The total value to achieve completion of the subphase.
     */     
    IMPORT_C TInt Total() const;  
    
    IMPORT_C ~CSifOperationProgressData();               

    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
    CSifOperationProgressData();
    void ConstructL(const TDesC& aGlobalcomponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal);
    void InternalizeL(RReadStream& aStream);

private:   
    HBufC* iGlobalComponentId;       
    TSifOperationPhase iPhase;
    TSifOperationSubPhase iSubPhase;
    TInt iCurrentProgress;
    TInt iTotal;
    };

/**
 * This class will be used by installers to publish a single install/uninstall/update operation information.
 * A scheduler needs to be in started when using this class.
 */
NONSHARABLE_CLASS(CPublishSifOperationInfo) : public CBase
    {
public:
    
    /**
        Creates a CPublishSifOperationInfo object.
                                                        
        @return A pointer to the newly allocated CPublishSifOperationInfo object, if creation is successful.
     */       
    IMPORT_C static CPublishSifOperationInfo* NewL();
  
    /**
        Creates a CPublishSifOperationInfo object.
                             
        @return A pointer to the newly allocated CPublishSifOperationInfo object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */       
    IMPORT_C static CPublishSifOperationInfo* NewLC();
    
    /**
		Publish the start of an operation. The corresponding PublishCompletionL() method should be called later
		else an opeartion place holder is always blocked for this operation which has started.       
       
		@capability WriteDeviceData
		@param aStartData Start operation related information.
		@leave KErrInUse No more slots are available to publish.
     */
    IMPORT_C void PublishStartL(CSifOperationStartData& aStartData);

    /**
		Publish the operation progress information
       
		@capability WriteDeviceData
		@param aProgressData Progress operation related information.
     */
    IMPORT_C void PublishProgressL(CSifOperationProgressData& aProgressData);

    /**
		Publish the error status of the operation (at the end)
       
		@capability WriteDeviceData
		@param aEndData End operation related information.
     */
    IMPORT_C void PublishCompletionL(CSifOperationEndData& aEndData);
    
    IMPORT_C ~CPublishSifOperationInfo(); 
    
private:
    void SetKeyToDelete(TUint aStartEndKey);
    void SetStartEndKey(TUint aStartEndKey);
    CPublishSifOperationInfo();
    void ConstructL();
    TUint AvailableKeyL();
    TUint KeyForGlobalComponentIdL(const TDesC& aGlobalComponentId);
    
private:
    CDeleteKeyTimer* iDeleteTimer;
    TInt iBufferSize;
    TUint iStartEndKey;
    };

/**
 * MSifOperationsHandler
 * Clients must implement this class in order to receive notifications.
 */
class MSifOperationsHandler
    {
public:
    /**
       Called when a new operation is started. The client should use the SubscribeL() method 
       (@see CSifOperationsNotifier) to register for progress and end notification for this operation.
       This function should return quickly as this is run as part of an active object request completion handler.

       @param aKey The Start End key of the operation.
       @param aStartData Start operation related information.
     */
    virtual void StartOperationHandler(TUint aKey, const CSifOperationStartData& aStartData) = 0;
    
    /**
       Called when an operation completes.
       This function should return quickly as this is run as part of an active object request completion handler.

       @param aEndData End operation related information.
     */
    virtual void EndOperationHandler(const CSifOperationEndData& aEndData) = 0;
   
    /**
       Called whenever a progress update is available.
       This function should return quickly as this is run as part of an active object request completion handler.

       @param aProgressData Progress operation related information.
     */   
    virtual void ProgressOperationHandler(const CSifOperationProgressData& aProgressData) = 0;
    virtual ~MSifOperationsHandler() {}
    };

/**
 * CSifOperationsNotifier
 *
 * Clients must create a CSifOperationsNotifier object by passing in the handler object.
 *
 * For every new Sif Operation, the StartOperationHandler() method of the handler would be called along
 * with some related information. The client can choose to subscribe to this operation by calling the 
 * SubscribeL() method.
 * 
 * A scheduler needs to be started when using this class.
 */
NONSHARABLE_CLASS(CSifOperationsNotifier) : public CActive
    {
public:    
    
    /**
        Creates a CSifOperationsNotifier object.
        
        @param  aHandler A handle to the implementation of the MSifOperationsHandler class.      
                                        
        @return A pointer to the newly allocated CSifOperationsNotifier object, if creation is successful.
     */         
    IMPORT_C static CSifOperationsNotifier* NewL(MSifOperationsHandler& aHandler);
    
    /**
        Creates a CSifOperationsNotifier object.
        
        @param  aHandler A handle to the implementation of the MSifOperationsHandler class.     
                                                        
        @return A pointer to the newly allocated CSifOperationsNotifier object, if creation is successful.
                The pointer is also put onto the cleanup stack. 
     */         
    IMPORT_C static CSifOperationsNotifier* NewLC(MSifOperationsHandler& aHandler);
    
    /**
       Subscribe for progress and end notifications or only end notification.
       
       @param aKey The StartEnd key associated with the operation.
       @param aSubscribeForProgressNotifications Boolean to specify optional subscription to progreess information.
     */
    IMPORT_C void SubscribeL(TUint aKey, TBool aSubscribeForProgressNotifications = ETrue);
    
    /**
       Cancel Subscription for a specific key.
       
       @param aKey The StartEnd key associated with the operation.
     */    
    IMPORT_C void CancelSubscribeL(TUint aKey);
    
    /**
       Cancel all subscriptions.
     */      
    IMPORT_C void CancelSubscribeL();
    
    IMPORT_C ~CSifOperationsNotifier();

private:
    enum TNotifierState
        {
        EFirstStart,
        EWaitForChange
        };
    CSifOperationsNotifier(MSifOperationsHandler& aHandler);
    void GetKeysL(RArray<TUint>& aKeys);
    void ActiveOperationsNotificationL();
    void WaitForChangeL();
    void ConstructL();
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    MSifOperationsHandler& iHandler;
    TInt iBufferSize;   
    RArray<TUint> iKeyArray;
    RProperty iProperty;
    RPointerArray<CSifNotifierBase> iNotifierArray;
    TNotifierState iNotifierState;
    };


} // namespace

#endif

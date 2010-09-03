/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_CATALOGSHTTPDOWNLOAD_H
#define C_CATALOGSHTTPDOWNLOAD_H

// HLA: #include <DownloadMgrClient.h>
#include <download.h>
#include <downloadevent.h>
#include <s32file.h>

#include "catalogshttpoperation.h"
#include "catalogstransportoperationid.h"
#include "catalogshttpconfigobserver.h"
#include "catalogshttpobserver.h"

class MCatalogsHttpConfig;
class CCatalogsHttpConfig;
class CCatalogsHttpHeaders;
class CCatalogsKeyValuePair;
class CCatalogsHttpDownloadManager;
class CCatalogsHttpQTDownload;
using namespace WRT;
/**
* HTTP file download implementation
*
* @note Due to insufficient access to HTTP response headers through the 
* Download manager, Download uses CCatalogsHttpTransaction with HTTP HEAD
* to get the response headers if the target filename cannot be figured out
* from the given URL
* @note Getting all of the response headers can be forced through SetHeaderMode()
* 
*/
class CCatalogsHttpDownload : public CBase,
    public MCatalogsHttpOperation, 
    public MCatalogsHttpConfigObserver,
    public MCatalogsHttpObserver
    {
public:

    enum TDownloadDeleteState
        {
        /**
         * RHttpDownload object can be deleted if necessary
         */
        EDownloadCanBeDeleted = 0,
        
        /**
         * Download was paused by the client and RHttpDownload must
         * not be deleted
         */
        EDownloadPaused
        };
        
public: // Constructors & destructor    


    /**
    * Creator
    * 
    * @param aDownload Platform download
    * @param aConfig Configuration
    * @return A new HTTP download
    */
    static CCatalogsHttpDownload* NewLC( 
        CCatalogsHttpDownloadManager& aOwner, 
        Download* aDownload,
        const CCatalogsHttpConfig& aConfig );


    /**
    * Destructor
    */
    ~CCatalogsHttpDownload();


public: // MCatalogsTransportOperation
          
    TInt AddRef();		
    
    TInt Release();
            
    TInt RefCount() const;    
    
    TInt Cancel();

    TCatalogsTransportProgress Progress() const;		


public: // MCatalogsHttpOperation

    /**
    * @see MCatalogsHttpOperation::Start()
    */
    TInt Start( TResumeStartInformation aResumeOrigin = EResumeFromUser );
    
    
    /**
    * @see MCatalogsHttpOperation::Pause()
    */
    TInt Pause();


    /**
    * @see MCatalogsHttpOperation::NotifyCancel()
    */
    virtual void NotifyCancel();


    // Body and headers
    
    /**
    * @see MCatalogsHttpOperation::SetBodyL( const TDesC8& )
    * @note Downloads do not support this
    */
    void SetBodyL( const TDesC8& aBody );


    /**
    * @see MCatalogsHttpOperation::SetBodyL( const TDesC16&	)    
    */
    void SetBodyL( const TDesC16& aBody );
    
    
    /**	    
    * @see MCatalogsHttpOperation::Body()
    * @note For downloads, this always returns an empty descriptor.
    */
    const TDesC8& Body() const;


    /**
    * @see MCatalogsHttpOperation::SetUriL()
    */
    void SetUriL( const TDesC8& aUri );
    
    
    /**
    * @see MCatalogsHttpOperation::SetUriL()
    */
    void SetUriL( const TDesC16& aUri );

    /**
    * @see MCatalogsHttpOperation::Uri()
    */
    const TDesC8& Uri() const;


    /**
    * Returns the current configuration for the download
    *
    * The changes to the configuration must be made before
    * the download is started. Changes in priority can be made even
    * if the download is running but they have effect only if the download
    * is paused.        
    *
    * @return Download configuration
    * @see MCatalogsHttpOperation::Config()
    */
    MCatalogsHttpConfig& Config() const;


    /**
    * @see MCatalogsHttpOperation::RequestHeadersL()
    */
    MCatalogsHttpHeaders& RequestHeadersL() const;
    
    /**
    * @see MCatalogsHttpOperation::ResponseHeadersL()
    */
    const MCatalogsHttpHeaders& ResponseHeadersL() const;
    	    	    
    	    
    /**
    * @see MCatalogsHttpOperation::OperationType()
    */
    TCatalogsHttpOperationType OperationType() const;

    
    /**
    * @see MCatalogsHttpOperation::OperationId()
    */
    const TCatalogsTransportOperationId& OperationId() const;

    /**
    * @see MCatalogsHttpOperation::SetContentType( const TDesC8&)
    * @note Downloads do not support this
    */
    void SetContentTypeL( const TDesC8& aContentType );        

    /**
    * @see MCatalogsHttpOperation::SetContentType( const TDesC16&)
    * @note Downloads do not support this
    */
    void SetContentTypeL( const TDesC16& aContentType );


    /**
    * @see MCatalogsHttpOperation::ContentType()
    */
    const TDesC8& ContentType() const;


    /**
    * @see MCatalogsHttpOperation::ContentSize()
    */
    TInt32 ContentSize() const;
    
    
    /**
    * @see MCatalogsHttpOperation::TransferredSize()
    */
    TInt32 TransferredSize() const;
    
    
    /**
    * @see MCatalogsHttpOperation::IsPausable()
    */
    TBool IsPausable() const;
    

    /**
     * Not supported
     * @see MCatalogsHttpOperation::StatusCode()
     */
    TInt StatusCode() const;        
    
    TDownloadDeleteState GetStatusState();
    Download* GetDownload();
    /**
     * Not supported
     * @see MCatalogsHttpOperation::StatusText()
     */
    const TDesC8& StatusText() const;
       


    /** 
     * @see MCatalogsHttpOperation::SetConnection()
     */
    void SetConnection( CCatalogsConnection& aConnection );
    
    
    /** 
     * @see MCatalogsHttpOperation::ReportConnectionError()
     */
    void ReportConnectionError( TInt aError );
    
    
    /** 
     * @see MCatalogsHttpOperation::ConnectionManager()
     */
    CCatalogsHttpConnectionManager& ConnectionManager();
    
    void ExternalizeL( RWriteStream& aStream ) const;
    void InternalizeL( RReadStream& aStream );
    
    TCatalogsHttpOperationState State() const;    

    
public: // From MCatalogsHttpConfigObserver

    TInt HandleHttpConfigEvent( MCatalogsHttpConfig* aConfig,
        const TCatalogsHttpConfigEvent& aEvent );

public: // From MCatalogsHttpObserver

    void HandleHttpEventL( MCatalogsHttpOperation& aOperation,
        TCatalogsHttpEvent aEvent );
        
    TBool HandleHttpError( MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpError aError );

     void HandledownloadEventL(DownloadEvent& aEvent);   
              
public:     

    /**
    * Handles events from Download Manager
    *
    * @param aEvent Download Manager event
    */
     /* HLa
    void HandleEventL( THttpDownloadEvent aEvent );
    */
    
    /**
     * Set file server session
     *
     * @param aFs File server session
     */
    void SetFileServerSession( RFs& aFs );
    
    
#ifdef __SERIES60_31__
    TBool IsCancelled() const;
#endif    
       
public:

    /**
    * Sets the header mode
    * @param aMode Mode
    */
    void SetHeaderMode( TCatalogsHttpHeaderMode aMode );

        
protected:  
    
    /**
    * Constructor
    */
    CCatalogsHttpDownload( 
        CCatalogsHttpDownloadManager& aOwner, 
        Download* aDownload );
    
    /**
    * 2nd phase constructor
    */
    void ConstructL( const CCatalogsHttpConfig* aConfig );

protected:

    /**
    * Updates the content type for the download
    */
    void UpdateContentType();
    
    TInt GetDeleteState( TDownloadDeleteState& aStatus );

    TInt SetDeleteState( const TDownloadDeleteState aStatus );    

private: // New methods

    /**
    * Handles event progress
    * @param aEvent The event that has progressed
    */
    /* HLa
    void HandleEventProgressL( const THttpDownloadEvent& aEvent );
    */
    
    /**
    * Updates the target filename from Content-Disposition -header
    * @throw KErrNotFound if the header doesn't exist
    */
    void UpdateFilenameFromContentDispositionL();

    /**
    * Updates the target filename from DL manager
    */
    void UpdateFilenameL();


    /** 
    * Updates the platform download to match the current configuration
    */
    void UpdateDownloadConfigurationL();    


    /**
    * Updates the iUri to match the currently used uri 
    */
    void UpdateUriL();

    
    /**
    * Updates the request headers to the platform DL manager
    */
    void UpdateRequestHeadersL();
    
    
    /**
    * Update the response headers from DL manager to the download
    */        
    void UpdateResponseHeadersL();


    /**
   * Update the response headers from the transaction to the download
    */        
    void UpdateResponseHeadersL( MCatalogsHttpOperation& aOperation );


    /** 
    * Checks if the given header matches any of the request headers that have
    * an predefined enumeration in the platform DL manager
    *
    * @param aHeader Header
    * @return Header enumeration
    */        
    TUint MatchWithPredefinedRequestHeader( 
        const TDesC8& aHeader ) const;


    /** 
    * Checks if the given header matches any of the general headers that have
    * an predefined enumeration in the platform DL manager
    *
    * @param aHeader Header
    * @return Header enumeration
    */        
    TUint MatchWithPredefinedGeneralHeader( 
        const TDesC8& aHeader ) const;

    
    /** 
    * Checks if the given header matches any of the entity headers that have
    * an predefined enumeration in the platform DL manager
    *
    * @param aHeader Header
    * @return Header enumeration
    */
    TUint MatchWithPredefinedEntityHeader( 
        const TDesC8& aHeader ) const;

    
    /**
    * Adds the header from the pair to the target in the format used by
    * platform's DL manager
    *
    * @param aTarget Target descriptor. Allocated/resized if necessary
    * @param aPair Key-value -pair holding the header key and value
    */
    void AddRequestHeaderL( HBufC8*& aTarget, 
        const CCatalogsKeyValuePair& aPair );
    
    /**
     * Updates transferring flag if the state changes. Also notifies
     * owner of the change
     */
    void SetTransferring( TBool aTransferring );
    
    TInt StartDownload();
    
    /**
     * Starts a OMA DD download if necessary
     * 
     * @return ETrue if a DD download was started
     */
    TBool StartDescriptorDownloadL();
    
    /**
     * Deletes downloaded files
     * 
     * Errors are ignored
     */
    void DeleteFiles();
    
    /**
     * Moves/renames the temp file as the target file
     */
    void MoveFileL();
    
    /**
     * Tries to parse the filename from the URI
     * 
     * @note Filename from this kind of an URI "http://www.doh.com/dl=bigfile"
     * is "dl" so beware
     */ 
    void ParseFilenameFromUriL();
    
    /**
     * Returns ETrue if aDes != NULL and aDes->Length() > 0
     */
    TBool ContainsData( const HBufC* aDes ) const;
    
    /**
     * Gets the correct extension for iTempFilename 
     * and ContentType() and sets it to iConfig
     */
    void UpdateExtensionL();
    
    /**
     *  Replace current extension at aName with extension given (eExt).
     */
    void ReplaceExtension( TDes& aName, const TDesC& aExt );
    
    /**
     * Checks the preset content-type and forces HEAD request if
     * content is OMA DD, JAD or some DRM file
     */
    void UpdateHeadRequirement();
    
    void InitializeDownloadL();
    
    void DeletePlatformDownload();
    
    /**
     * Updates the secondary id from platform download
     * Doesn't check if the download exists
     */
    void UpdateSecondaryIdL();
    
    void EncodeUriL();
    
    const TDesC8& EncodedUri() const;
    
    
private:

    CCatalogsHttpDownloadManager& iOwner;
    TCatalogsTransportOperationId iId;  // Operation id
    Download* iDownload;           // Platform download
    CCatalogsHttpConfig* iConfig;       // Configuration
    MCatalogsHttpObserver* iObserver;   // Observer        
    TCatalogsHttpEvent iState;          // State of the download
    HBufC8* iUri;                       // Current URI
    HBufC8* iEncodedUri;                // Current URI as encoded
    CCatalogsHttpHeaders* iResponseHeaders; // Headers from HTTP response
    CCatalogsHttpQTDownload* iQTDownload;
    
    // added request headers not supported by DL man
    HBufC8* iAddedRequestHeaders;       
    TInt iRefCount;        
    TBool iNormalDelete;

    // HLa: TBuf8<KMaxContentTypeLength> iContentType;
    TBuf8<256> iContentType;
        
    // Transaction for retrieving all of the response headers
    MCatalogsHttpOperation* iTransaction;
    TCatalogsHttpHeaderMode iMode;    
        
    TBool iDdDownload;  // True if downloading a DD        
    RFile iDdFile;      // Target file for DD
    RFs iFs;
    TInt iReconnectCount;  // number of reconnect attempts
   
    TBool iPaused;
      
    TBool iReconnectWhenFail;
    
    TBool iTransferring;
    
    CCatalogsConnection* iConnection; // not owned    
    TBool iFileIsSet;
    HBufC* iTempFilename;
    
    // mutable because this is only used as an optimization and is
    // updated in ContentSize() const
    mutable TInt32 iContentSize; 
    
    TInt32 iTransferredSize; 

#ifdef __SERIES60_31__
    TBool iCancelled;
#endif    
    
    // For managing async pause with dlmgr.
    TBool iPausePending;
    TBool iQueuedResume;
    TDownloadDeleteState iStatus;
protected:

    // Grant access to UpdateContentType
    friend class CCatalogsHttpDownloadManager;
    
    };

class  CCatalogsHttpQTDownload: public QObject
	{
		 Q_OBJECT
		 	public:
		 		CCatalogsHttpQTDownload(CCatalogsHttpDownload* aHttpDownload,Download* aDownload);
	    public slots:
    	void downloadEventHandler(DownloadEvent*);
    	void downloadErrorHandler(Error);
	    public:
	    	CCatalogsHttpDownload* iCatalogsHttpDownload;
	    	Download* iDownload;           // Platform download
	};
	
	
#endif // C_CATALOGSHTTPDOWNLOAD_H

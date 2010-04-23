/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_CATALOGSHTTPOPERATION_H
#define M_CATALOGSHTTPOPERATION_H

#include "catalogstransportoperation.h"
#include "catalogshttptypes.h"

class RWriteStream;
class RReadStream;

class MCatalogsHttpObserver;    // Observer for HTTP operations
class MCatalogsHttpConfig;
class MCatalogsHttpHeaders;
class TCatalogsTransportOperationId;
class CCatalogsConnection;
class CCatalogsHttpConnectionManager;

/**
* Interface for HTTP operations
*
*
* Calling Cancel() also releases (deletes if the reference count reaches 0) 
* the interface. Only Release() should be called after a Cancel() if
* necessary.
*
* Operations have priorities that determine their execution order in case
* there are too many concurrent operations. There are also separate limits 
* for concurrent downloads and transactions. If these limits are exceeded, the
* operations are queued until some operation completes.
*/
class MCatalogsHttpOperation : public MCatalogsTransportOperation
    {
    public:
        /**
         * Determines how the download was started, by user or
         * automatically by the engine
         */
        enum TResumeStartInformation
            {
            EAutomaticResume = 0,
            EResumeFromUser
            };
            
    public: // From MCatalogsTransportOperation
    
        /**
        * @return Reference count after cancel
        */    
        TInt Cancel() = 0;        
        
    public: // New methods
    
        /**
        * Starts the operation or resumes a paused operation
        *
        * The operation is queued if the limit for concurrently executing 
        * operations would be otherwise exceeded. In that case, the state 
        * of the operation will be ECatalogsHttpOpQueued. The operation
        * will be automatically resumed when it can be executed.
        *
        * @param aResumeOrigin Determines who started the download
        * @return Symbian error code
        * @see TCatalogsHttpOperationState::ECatalogsHttpOpQueued
        */
	    virtual TInt Start( TResumeStartInformation aResumeOrigin = EResumeFromUser ) = 0;
	    
	    
	    /**
	    * Pauses the operation
	    *
	    * The operation can be resumed with Start().
	    * 
	    * @return Error code
	    * @see TCatalogsHttpOperationState::ECatalogsHttpOpPaused
	    */
	    virtual TInt Pause() = 0;

		    
	    /**
	     *
	     */
	    virtual void NotifyCancel() = 0;
	    
	    
        // Body and headers
        
        /**
        * Sets the HTTP request body if the operation supports it
        * 
        * @param aBody HTTP body 
        */
	    virtual void SetBodyL( const TDesC8& aBody ) = 0;


        /**
        * Sets the HTTP request body if the operation supports it
        * 
        * @param aBody HTTP body 
        */	    
	    virtual void SetBodyL( const TDesC16& aBody ) = 0;
	    
	    
	    /**
	    * HTTP body part getter. 
	    * 
	    * @return Latest HTTP body part
	    * @note This method should be called only when handling 
	    * ECatalogsHttpResponseBodyReceived. If called outside the event
	    * handler, returns empty body
	    */
	    virtual const TDesC8& Body() const = 0;


        /**
        * Operation URI setter
        * @param aUri URI to set
        */
        virtual void SetUriL( const TDesC8& aUri ) = 0;

        /**
        * Operation URI setter
        * @param aUri URI to set
        */
        virtual void SetUriL( const TDesC16& aUri ) = 0;
        
        /**
        * Operation URI getter
        *
        * @return URI
        */
        virtual const TDesC8& Uri() const = 0;
      

        /**
        * Returns the current configuration for the download
        *
        * The changes to the configuration must be made before
        * the operation is started.
        *
        * @return Operation configuration
        */
        virtual MCatalogsHttpConfig& Config() const = 0;


        /**
        * Request headers getter
        *
        * The returned headers are the same that can be queried
        * from the current configuration through Config()
        *
        * @return Request headers
        * @exception KErrNotSupported if the operation doesn't
        * support request headers
        */
        virtual MCatalogsHttpHeaders& RequestHeadersL() const = 0;
        
        /**
        * Response headers getter
        * 
        * @return Response headers
        * @exception KErrNotSupported if the operation doesn's
        * support response headers
        * @note "Date" header is formatted: day.month.year hour.minute.second
        */
        virtual const MCatalogsHttpHeaders& ResponseHeadersL() const = 0;
        
	    	    
	    /**
	    * Returns the type of the operation
	    * 
	    * @return Operation type
	    */
	    virtual TCatalogsHttpOperationType OperationType() const = 0;

	    
        /**
        * Returns operation's ID
        *
        * @return Operation ID
        */
        virtual const TCatalogsTransportOperationId& OperationId() const = 0;
        

        /**
        * Sets the request content type if the operation supports it
        * @param aContentType Content type
        */
        virtual void SetContentTypeL( const TDesC8& aContentType ) = 0;  


        /**
        * Sets the request content type if the operation supports it
        * @param aContentType Content type
        */
        virtual void SetContentTypeL( const TDesC16& aContentType ) = 0;


        
        /**
        * Content type getter
        *
        * @return Content type
        */
        virtual const TDesC8& ContentType() const = 0;
                        
        
        /**
        * Content size getter
        * 
        * This is the size of the whole file/buffer
        * 
        * @return Content size
        */
        virtual TInt32 ContentSize() const = 0;
        
        
        /**
        * 
        * This is the size of the data transferred so far
        *
        * @return Size of the transferred data
        */
        virtual TInt32 TransferredSize() const = 0;
        
        
        /**
        * Returns ETrue if the operation can be paused
        * else returns EFalse.
        *
        * @return Is the operation pausable
        */
        virtual TBool IsPausable() const = 0;
        

        /**
        * Sets the header mode for file downloads.
        * @param aMode Mode
        * @note This setting has no effect on transactions.
        */
        virtual void SetHeaderMode( TCatalogsHttpHeaderMode aMode ) = 0;
        
        
        /**
         * Response status code
         */
        virtual TInt StatusCode() const = 0;        
        
        
        /**
         * Response status text
         */
        virtual const TDesC8& StatusText() const = 0;
        
        
        /** 
         * Connection setter
         */
        virtual void SetConnection( CCatalogsConnection& aConnection ) = 0;
        
        
        /**
         * Report connection error to the operation
         */
        virtual void ReportConnectionError( TInt aError ) = 0;

        
        /**
         * Connection manager getter
         */
        virtual CCatalogsHttpConnectionManager& ConnectionManager() = 0;
        
        
        /**
         * Externalize operation data
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;
        
        
        /**
         * Internalize operation data
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;
        
        
        /**
         * Operation state
         * 
         * @return Operation state. 
         * @note This is same as Progress().iState but without any 
         * client-server communication
         */
        virtual TCatalogsHttpOperationState State() const = 0;
    };
	
#endif // M_CATALOGSHTTPOPERATION_H

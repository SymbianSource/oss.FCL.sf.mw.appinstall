/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CATALOGSTRANSPORT_H
#define C_CATALOGSTRANSPORT_H

#include <e32base.h>
#include <e32cmn.h>

class MCatalogsTransportSession;
class CCatalogsHttpSessionManager;

/**
* Catalogs Transport implementation
*/
class CCatalogsTransport : public CBase 
	{
	public: // Constructors and destructor
	
	    /**
	    * Creator
	    *
	    * @return A new CCatalogsTransport-object
	    */
        static CCatalogsTransport* NewL();
	    
	    /**
	    * Destructor
	    */
	    virtual ~CCatalogsTransport();
	    
	public:  // From MCatalogsTransport
	
	    /**
	    *  Queries an interface from Transport.
	    *
	    *  @note The caller is responsible for releasing the interface
	    *  @note Interface ID's are listed in tcatalogstransportinterfaces.h
	    *
	    *  @param aSessionId An ID for the session. Should be the caller's UID
	    *  @param aInterfaceId The ID of the required interface
	    *  @return An instance of the required interface
	    *  @throws KErrArgument if the given interface ID is invalid	    
	    */
	    MCatalogsTransportSession* QueryInterfaceL( 
	        TInt32 aSessionId, 
		    TInt aInterfaceId,
		    TBool aCleanupSession = EFalse );
        
        
	public: // Methods used by Transport sessions 
	
	    /**
	    * Removes the session from CCatalogsTransport
	    *
	    * This is called by CCatalogsSmsSession and other sessions in 
	    * their destructors
	    *
	    * @param aSession Session to remove
	    * @note The session MUST be found from Transport
	    */
	    void RemoveSession( MCatalogsTransportSession* aSession );	    


    protected:
    
        /** 
        * 2nd phase constructor
        */
	    void ConstructL();
	    
	private:
	
	    /**
	    * Array type for storing Transport sessions
	    */
	    typedef RPointerArray<MCatalogsTransportSession> RCatalogsSessionArray;
	
	private:  // Private methods
	
	    /**
	    * Searches for the session that matches the given ID in the given array
	    *
	    * @param aSessionId Session id
	    * @param aSessions Session array
	    * @return SMS session or NULL
	    */
	    MCatalogsTransportSession* FindSession( TInt32 aSessionId, 
	        RCatalogsSessionArray& aSessions ) const;


	private:
	
	    RCatalogsSessionArray iHttpSessions;
	    RCatalogsSessionArray iSmsSessions;
	    CCatalogsHttpSessionManager* iHttpSessionManager;
	    TBool iRemovingAll;	    	    
	};


#endif // C_CATALOGSTRANSPORT_H

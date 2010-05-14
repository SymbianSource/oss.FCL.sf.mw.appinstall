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
* Description:   MNcdSessionHandler interface
*
*/


#ifndef C_NCDSESSIONHANDLER_H
#define C_NCDSESSIONHANDLER_H

#include <e32base.h>

/**
 * Session handler interface
 */
class MNcdSessionHandler
    {
public:
    /**
     * Creates a session for the given server and namespace. If there is an
     * existing session for the server and namespace, it is overwritten.
     * 
     * @param aServerURI Server URI. Case insensitive!
     * @param aNamespace Name space of the session.
     * @param aSessionId Session id from server.
     */
    virtual void CreateSessionL( const TDesC& aServerUri,
        const TDesC& aNameSpace,
        const TDesC& aSessionId ) = 0;
    
    /**
     * Removes a session for a given server.
     *
     * @param aServerURI Server URI. Case insensitive!
     * @param aNamespace Name space of the session.
     */
    virtual void RemoveSession( const TDesC& aServerUri,
        const TDesC& aNameSpace ) = 0;
    
    /**
     * Removes all sessions.
     */
    virtual void RemoveAllSessions() = 0;
    
    /**
     * Returns the session ID for the server URI
     *
     * @param aServerUri Server URI
     * @param aNamespace Name space of the session.
     * @return Session ID
     */
    virtual const TDesC& Session( const TDesC& aServerUri,
        const TDesC& aNameSpace ) = 0;
    
    /**
     * Checks whether a session exists for a given server.
     *
     * @param aServerURI Server URI. Case insensitive!
     * @param aNamespace Name space of the session.
     * @return TBool ETrue if a session exists, EFalse otherwise.
     */
    virtual TBool DoesSessionExist( const TDesC& aServerUri,
        const TDesC& aNameSpace ) = 0;
    
protected:

    virtual ~MNcdSessionHandler()
        {
        }
    };


class CNcdKeyValuePair;

/**
 * Server session handling interface.
 * This is used by the protocol parser.
 */
class CNcdSessionHandler : public CBase,
    public MNcdSessionHandler
    {
public:

    /**
     * Creates a new session handler
     * 
     * @return A new session handler
     */
    static CNcdSessionHandler* NewL();


    /**
     * Destructor
     */
    ~CNcdSessionHandler();
    
public:

    /**
     * @see MNcdSessionHandler::CreateSessions()
     */
    void CreateSessionL( const TDesC& aServerUri, const TDesC& aNameSpace,
         const TDesC& aSessionId );
    
    /**
     * @see MNcdSessionHandler::RemoveSession()
     */
    void RemoveSession( const TDesC& aServerUri,
        const TDesC& aNameSpace  );
    
    /**
     * @see MNcdSessionHandler::RemoveAllSessions()
     */
    virtual void RemoveAllSessions();
    
    /**
     * @see MNcdSessionHandler::Session()
     */
    const TDesC& Session( const TDesC& aServerUri,
        const TDesC& aNameSpace );
    
    /**
     * @see MNcdSessionHandler::DoesSessionExist()
     */
    TBool DoesSessionExist( const TDesC& aServerUri,
        const TDesC& aNameSpace );

private:

    // Search for the session
    TInt FindSession( const TDesC& aServerUri,
        const TDesC& aNameSpace );
        
private:
    class CNcdServerSession : public CBase
        {
        
    public: // Constructor & destructor
        
        /**
         * Creates a new server session
         * 
         * @param aServerURI Server URI. Case insensitive!
         * @param aNamespace Name space of the session.
         * @param aSessionId Session id from server.
         * @return A new server session object.
         */
        static CNcdServerSession* NewL( const TDesC& aServerUri,
            const TDesC& aNameSpace,
            const TDesC& aSessionId );
        
        /**
         * Creates a new server session and leaves it on the cleanup stack.
         * 
         * @param aServerURI Server URI. Case insensitive!
         * @param aNamespace Name space of the session.
         * @param aSessionId Session id from server.
         * @return A new server session object.
         */
        static CNcdServerSession* NewLC( const TDesC& aServerUri,
            const TDesC& aNameSpace,
            const TDesC& aSessionId );

        /**
         * Destructor
         */
        ~CNcdServerSession();
        
    public: // New functions
    
        const TDesC& ServerUri();
        const TDesC& NameSpace();
        const TDesC& SessionId();
        
    private: // Constructors
    
        CNcdServerSession();
        
        void ConstructL( const TDesC& aServerUri,
            const TDesC& aNameSpace,
            const TDesC& aSessionId );
        
    private: // Member data
    
        HBufC* iServerUri;
        HBufC* iNameSpace;
        HBufC* iSessionId;
        
        };
    
private:
    
    // Session array
    RPointerArray<CNcdServerSession> iSessions;
    };

#endif // C_NCDSESSIONHANDLER_H

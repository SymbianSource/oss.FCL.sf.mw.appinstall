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
* Description:   MNcdProtocol declaration
*
*/


#ifndef M_NCDPROTOCOL_H
#define M_NCDPROTOCOL_H

#include <e32base.h>

class MNcdParser;
class CNcdRequestBase;
class CNcdRequestConfiguration;
class MCatalogsContext;
class MNcdProtocolDefaultObserver;
class MNcdSessionHandler;

/**
 * Interface for protocol entry point.
 *
 * @note Requests are created with NcdRequestGenerator
 *
 */
class MNcdProtocol
                     
    {
public:

    /**
     * Creates a parser for parsing protocol responses.
     *
     * @param aContext Current context
     * @param aServerURI Server where the protocol response originates. 
     * This is used for session bookkeeping.
     */
    virtual MNcdParser* CreateParserL( MCatalogsContext& aContext,
        const TDesC& aServerUri ) = 0;
    

    /**
     * Process a preminet request
     *
     * @note This method must be called for all requests before they are
     * sent.
     *
     * @param aRequest Request to process
     * @param aServerUri Server URI for the request
     * @param aForceConfigurationData If true, configuration data is always included in the
     * request, otherwise only if there is no existing session for the request.
     * @return Final request data that can be sent to a server
     */
    virtual HBufC8* ProcessPreminetRequestL( const MCatalogsContext& aContext, 
        CNcdRequestBase& aRequest, 
        const TDesC& aServerUri,
        TBool aForceConfigurationData = EFalse ) = 0;


    /**
     * Process a configuration request
     *
     * @note This method must be called for all requests before they are
     * sent.
     *
     * @param aRequest Request to process
     * @param aServerUri Server URI for the request
     * @return Final request data that can be sent to a server
     */
    virtual HBufC8* ProcessConfigurationRequestL( 
        const MCatalogsContext& aContext, 
        CNcdRequestConfiguration& aRequest ) = 0;
     
    /**
     * Retrieves the default handler for protocol events.
     * All events not needed by the parser observer should be passed to
     * the default observer.
     * @return Default protocol events observer
     */
    //virtual MNcdProtocolDefaultObserver& ProtocolDefaultObserver() const = 0;


    /**
     * Session handler getter
     * @throw KErrNotFound if the context is not found
     */
    virtual MNcdSessionHandler& SessionHandlerL( 
        const MCatalogsContext& aContext ) const = 0;


protected:

    virtual ~MNcdProtocol()
        {
        }
    };



#endif

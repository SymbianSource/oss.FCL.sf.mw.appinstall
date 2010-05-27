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
* Description:   Contains MNcdNodeUriContent interface
*
*/


#ifndef M_NCD_NODE_URI_CONTENT_H
#define M_NCD_NODE_URI_CONTENT_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 *  This interface provides functions to handle uri content.
 *  There can be only one URI content in a node. 
 *  URI item may have to be purchased before it can
 *  be used. So, purchase interface may have to be used before
 *  this content is available. 
 *
 *  @note URI items are not installed before usage. 
 *  They will be launched directly from the given URI 
 *  by using some application that Symbian OS sees fit
 *  when the URI is opened. 
 *
 *  @see MNcdNodePurchase
 *
 *  
 */
class MNcdNodeUriContent : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeUriContentUid };


    /** 
     * This function gives the URI where the actual 
     * content is located. So, the actual content has not 
     * been given directly here. Instead the content may 
     * be used by some other application which is
     * started by the Symbian OS when the uri is opened.
     *
     * @return The URI that informs where the actual
     *  content is located. If the protocol has not defined any value, 
     *  then an empty string is returned.
     */
    virtual const TDesC& ContentUri() const = 0;

    /** 
     * Validity time in minutes for how long the target of the URI
     * received by calling ContentUri() is accessible. Notice that
     * the delta is not validity from the current time.
     *
     * @return The validity time in minutes for how long the target
     *  of the URI is accessible. If the protocol has not defined
     *  any value, then -1 is returned. Validity is counted from
     *  the time that the validity is sent in the protocol to
     *  the client.
     */
    virtual TInt ContentValidityDelta() const = 0;

protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdNodeUriContent() {}

    };


#endif // M_NCD_NODE_URI_CONTENT_H

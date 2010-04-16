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
* Description:   Contains MNcdNodeContent interface
*
*/


#ifndef M_NCD_NODE_CONTENT_INFO_H
#define M_NCD_NODE_CONTENT_INFO_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "ncditempurpose.h"

/**
 *  This interface provides functions to get general information
 *  about the content of the node.
 *
 *  @note This interface is available after node load if the server
 *  sent the information.
 *  
 *  @note The data is updated only from the server. Content downloads
 *  and installs do not affect this interface.
 *
 *  @note This interface is internalized from purchase history if possible.
 */
class MNcdNodeContentInfo : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeContentUid };


    /**
     * The purpose of the node.
     * 
     * 
     * @return Bit field describing the purpose(s) of this node, a combination
     *         of TNcdItemPurpose flags.
     * @see TNcdItemPurpose
     */
    virtual TUint Purpose() const = 0;    
    

    /**
     * Gives the mime type of the content data.
     *
     * @return The mime type of the download content data. The type is
     * given as a text according to the standards that define MIME types.
     */
    virtual const TDesC& MimeType() const = 0;


    /**
     * This function is the indicative Symbian application UID for the
     * contents that are applications. This can be used e.g. for checking
     * if the application has already been installed to the phone.
     *
     * @return The UID of the application item.
     */
    virtual TUid Uid() const = 0;

    /**
     * Different versions of the content items may exist. Thus,
     * a version identifier may be defined for the item. When installing
     * applications the version may be required.
     *
     * @return Version string of this application item.  
     * If the protocol has not defined any value, 
     * an empty string is returned.
     */
    virtual const TDesC& Version() const = 0;


    /**
     * Downloadable content size in bytes. 
     * In the protocol, multiple contents may be included for one content download.
     * Therefore, if there are multiple possibilities, the worst case scenario size 
     * is given here.
     *
     * @return Content size in bytes.
     * If the protocol has not defined any value, zero is returned.
     */
    virtual TInt Size() const = 0;

    /**
     * This function is the indicative Identifier for the
     * contents that are widgets. This can be used e.g. for checking
     * if the widget has already been installed to the phone.
     *
     * @return The Identifer of the application item.
     */
    virtual const TDesC& Identifier() const = 0;


protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeContentInfo() {}

    };


#endif // M_NCD_NODE_CONTENT_INFO_H

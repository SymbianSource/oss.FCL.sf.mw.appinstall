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
* Description:   Contains MNcdNodeMetadata interface
*
*/


#ifndef M_NCD_NODE_METADATA_H
#define M_NCD_NODE_METADATA_H


#include <e32cmn.h>
#include <e32std.h>

#include "catalogsbase.h"
#include "catalogsarray.h"
#include "ncdinterfaceids.h"

class MNcdNodeContainer;
class MNcdLoadNodeOperation;
class MNcdQuery;
class CNcdKeyValuePair;


/**
 *  Node metadata contains the basic information about the content.
 *  Mainly this interface provides functions that can be used to show
 *  various information about the node in the UI.
 *
 *  This interface can be queried from a node object after its metadata
 *  has been downloaded to the terminal. Metadata can be downloaded by
 *  calling the node object's MNcdNode::LoadL() method.
 *
 *  
 */
class MNcdNodeMetadata : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeMetadataUid };


    /**
     * Gets the metadata id.
     *
     * 
     * @return Id of the node.
     */
    virtual const TDesC& Id() const = 0;


    /**
     * All nodes and their content are part of a namespace. Within the
     * namespace, identifiers are unique. This function returns the namespace
     * of this metadata and its contents. This namespace should always be same
     * as the namespace of the node that may own this metadata.
     *
     * 
     * @return The namespace identifier. If the protocol has not defined
     *         any value, an empty descriptor is returned.
     */
    virtual const TDesC& Namespace() const = 0;


    /**
     * Retrieves the node name in text format.
     *
     * 
     * @return Name of the node. If the protocol has not defined any value
     *         for name, an empty descriptor is returned.
     */
    virtual const TDesC& Name() const = 0;


    /**
     * Retrieves the node description in text format.
     * 
     * 
     * @return Description for the node. If the protocol has not defined
     *         any value for description, an empty descriptor is returned.
     */
    virtual const TDesC& Description() const = 0;


    /**
     * Gets the disclaimer for the node in a query format.
     *
     * @note Query is used here just to give disclaimer information. The metadata
     * object does not care how the query is handled by the caller. SetResponseL
     * of the query may be used but the given response is not saved into the database.
     * Instead the set response information is availabe only while the metadata object
     * is alive. Use MNcdNodeUserData interface object if information about acception 
     * or rejection of the disclaimer should be saved to the database.
     *
     * 
     * @return Disclaimer query for the node. If the protocol has not defined
     *         any value, then NULL is returned. When a disclaimer query is returned,
     *         the user should Release the query when the object is not needed anymore  
     *         because the Disclaimer reference count is increase by one.
     */
    virtual MNcdQuery* Disclaimer() const = 0;


    /**
     * Gets the action name for the item.
     *
     * For example, the action for an item could be "Open", "Launch" or "Play"
     * etc.
     *
     * 
     * @return Action name string for the item.
     */
    virtual const TDesC& ActionName() const = 0;


    /**
     * Determines if the node is specified to be "always visible" in the client
     * UI. What this actually means is determined by the UI client
     * implementation. The idea is to give maximum visibility and availability
     * to the item for the user.
     *
     * 
     * @return ETrue if the item should be always visible, EFalse otherwise.
     */
    virtual TBool IsAlwaysVisible() const = 0;
    
    /**
     * Retrieves the layout type in text format.
     *
     * 
     * @return The layout type or empty string if not defined.
     */
    virtual const TDesC& LayoutType() const = 0;

    /**
     * Gets the more info for the node in a query format. This is a message
     * with a body of text to be shown.
     *
     * @note Query is used here just to give more info information. The metadata
     * object does not care how the query is handled by the caller. SetResponseL
     * of the query may be used but the given response is not saved into the database.
     *
     * 
     * @return More info query for the node. If the protocol has not defined
     *         any value, then NULL is returned. When a query is returned,
     *         the user should Release the query when the object is not needed anymore  
     *         because the reference count is increased by one.
     */
    virtual MNcdQuery* MoreInfo() const = 0;


    /**
     * Gets the details of this node. Details are additional information that
     * can be used e.g. to categorize nodes. Details are represented as key-value
     * pairs, for example: "artist:artist_name" or "album:album_name".
     * 
     * 
     * @return An array containing all the details of this node.
     */
    virtual const RPointerArray< CNcdKeyValuePair >& Details() const = 0;
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeMetadata() {}
    
    };


#endif // M_NCD_NODE_METADATA_H

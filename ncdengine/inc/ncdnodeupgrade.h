/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains MNcdNodeUpgrade interface
*
*/


#ifndef M_NCD_NODE_UPGRADE_H
#define M_NCD_NODE_UPGRADE_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "catalogsarray.h"


class MNcdNode;


/**
 *  This interface is provided for the node if this node
 *  contains an upgrade to the content of some other node.
 *  UI may use this interface to check if the node contains
 *  an upgrade.
 *
 *  This interface is available:
 *
 *  If the content of this node has been updated to server after the 
 *  node was bought or this node upgrades some installed application, 
 *  see EUpgradeContent
 *
 *  If this node upgrades some other node, see EUpgradeNode
 *
 *  This interface is NOT available if there is nothing to upgrade even if
 *  the upgrades-dependency is defined in the protocol.
 *
 *  
 */
class MNcdNodeUpgrade : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeUpgradeUid };


    enum TUpgradeType
        {
                
        /**
         * Upgrades some other node
         * @see UpgradeableNodeL()
         */
        EUpgradeNode,
        
        /**
         * The server contains an upgrade for the content of this
         * node
         *
         * This is used if the id in <downloadableContent> 
         * matches some installed application and the application's version
         * number is lower than the one in <downloadableContent> or
         * if the protocol defines an upgrades-dependency to
         * some application and that application is installed on the device
         *
         * @see MNcdNodeContentInfo
         */
        EUpgradeContent,
                
        /**
         * Upgrade type is set to EUpgradeNotAvailable when the upgrade
         * is installed.
         */
        EUpgradeNotAvailable
        };

    /**
     * Retrieves the name of the upgrade content in text format.
     *
     * 
     * @return Name of the upgrade content. If the protocol has not defined any value
     *         for name, an empty descriptor is returned.
     */
    virtual const TDesC& Name() const = 0;

    /**
     * This function is the indicative Symbian application UID for the
     * upgrade content that is an application. This can be used e.g. for checking
     * if the application has already been installed to the phone.
     *
     * @return The UID of the upgrade application item.
     */
    virtual TUid Uid() const = 0;

    /**
     * Different versions of the content items may exist. Thus,
     * a version identifier may be defined for the item. When installing
     * applications the version may be required.
     *
     * @return Version string of the upgrade application item.  
     * If the protocol has not defined any value, an empty string is returned.
     */
    virtual const TDesC& Version() const = 0;

    /**
     * Gives the target node that this node may upgrade.
     *
     * @note The reference count of the returned node object 
     * is increased by one. So, Release function of the node 
     * should be called when the node is not needed anymore.
     *
     * @note If the upgrade target has been directly gotten as a content
     * instead of as a node, then NULL pointer is returned here. Name, Uid and Version
     * information about the upgrade target content can still be requested by using
     * the other functions of this interface.
     *
     * @return Pointer to the target node that this node upgrades.
     *  NULL if the upgrade target has been given as a content intead of as a node.
     *  Counted, Release() must be called after use.
     * @exception Leave System wide error codes.
     */
    virtual MNcdNode* UpgradeableNodeL() const = 0;
    

    /**
     * Upgrade type getter
     *
     * @return Type of the upgrade
     * @see TUpgradeType
     */     
    virtual TUpgradeType UpgradeType() const = 0;
    
    
protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdNodeUpgrade() {}

    };


#endif // M_NCD_NODE_UPGRADE_H

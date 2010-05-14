/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_NODE_FACTORY_H
#define IA_UPDATE_NODE_FACTORY_H


#include <e32base.h>

class MNcdNode;
class CIAUpdateController;
class CIAUpdateNode;
class CIAUpdateFwNode;


/**
 * IAUpdateNodeFactory is used to create the correct iaupdate nodes
 * from the NCD nodes.
 *
 */
namespace IAUpdateNodeFactory
    {

    /**
     * @note Ownership of aNode is transferred. And if this function leaves,
     * then it automatically deletes the aNode.
     *
     * @param aNode NCD Engine node that is used for the iaupdate node.
     * Ownership is transferred. So, do not release the node after
     * giving it here. This class object supposes that the reference
     * count of the given node is at least one when it is given.
     * @param aController Update controller that is required by the iaupdate node.
     * @return CIAUpdateNode* IAUpdate node that is created. 
     * Ownership is transferred.
     * @exception Leaves with KErrArgument if aNode is NULL. Else leaves with
     * system wide error code.
     **/
    CIAUpdateNode* CreateNodeLC( MNcdNode* aNode,
                                 CIAUpdateController& aController );
    
    /**
     * @see IAUpdateNodeFactory::CreateNodeLC
     **/
    CIAUpdateNode* CreateNodeL( MNcdNode* aNode,
                                CIAUpdateController& aController );

    
    /**
     * @param aUid Uid of the node. This UID is checked agains the known UIDs that
     * require self update.
     * @return TBool ETrue if the node corresponding the given UID requires
     * self update. EFalse if the self update is not required.
     **/
    TBool IsSelfUpdate( const TUid& aUid );


    /**
     * @param aUid Uid of the node. This UID is checked agains the IAD UIDs.
     * @return TBool ETrue if the aUid describes IAD. Else EFalse.
     **/
    TBool IsIad( const TUid& aUid );
    

    /**
     * @param aUid Uid of the node. This UID is checked agains the NCD UIDs.
     * @return TBool ETrue if the aUid describes NCD. Else EFalse.
     **/
    TBool IsNcd( const TUid& aUid );


    /**
     * @param aUid Uid of the node. This UID is checked agains the IAD self 
     * updater UIDs.
     * @return TBool ETrue if the aUid describes IAD self updater. Else EFalse.
     **/
    TBool IsUpdater( const TUid& aUid );


    /**
     * @note Ownership of aNode is transferred. And if this function leaves,
     * then it automatically deletes the aNode.
     *
     * @param aNode NCD Engine node that is used for the iaupdate firmware node.
     * Ownership is transferred. So, do not release the node after
     * giving it here. This class object supposes that the reference
     * count of the given node is at least one when it is given.
     * @param aController Update controller that is required by the iaupdate node.
     * @return CIAUpdateNode* IAUpdate node that is created. 
     * Ownership is transferred.
     * @exception Leaves with KErrArgument if aNode is NULL. Else leaves with
     * system wide error code.
     **/
    CIAUpdateFwNode* CreateFwNodeLC( MNcdNode* aNode,
                                     CIAUpdateController& aController );
    
    /**
     * @see IAUpdateNodeFactory::CreateFwNodeLC
     **/
    CIAUpdateFwNode* CreateFwNodeL( MNcdNode* aNode,
                                    CIAUpdateController& aController );


    /**
     * @param aNode Node whose information is used to check if
     * the node describes a firmware update.
     * @return TBool ETrue if the given node is for firmware updates.
     * @exception Leaves with system wide error code.
     */
    TBool IsFwUpdateL( MNcdNode& aNode );


    /**
     * @param aMime MIME type is used to check if it describes 
     * a firmware update.
     * @return TBool ETrue if the given MIME is for firmware updates.
     */
    TBool IsFwUpdate( const TDesC& aMime );


    /**
     * @param aMime MIME type is used to check if it describes 
     * service pack.
     * @return TBool ETrue if the given MIME is for service pack.
     */
    TBool IsServicePack( const TDesC& aMime );


    /**
     * @param aMime MIME type is used to check if it describes 
     * a hidden item.
     * @return TBool ETrue if the given MIME is for a hidden item.
     * Else, EFalse.
     */
    TBool IsHidden( const TDesC& aMime );
    
    }
    
    
#endif  //  IA_UPDATE_NODE_FACTORY_H

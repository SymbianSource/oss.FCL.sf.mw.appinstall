/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of MIAUpdateAnyNode class 
*
*/



#ifndef IA_UPDATE_ANY_NODE_H
#define IA_UPDATE_ANY_NODE_H


class MIAUpdateBaseNode;


/**
 * MIAUpdateAnyNode
 * This interface provides enumerations for the node types.
 * Classes that implement this interface provide information
 * about their type. By using the type information, 
 * a MIAUpdateAnyNode object can be static casted to a more
 * specific object if necessary. This can be usefull, for example,
 * when arrays for MIAUpdateAnyNode objects are used. Also,
 * a function to get the base node is provided. So, common
 * information for all the nodes can be gotten through this
 * interface without casting the object.
 *
 * @see MIAUpdateBaseNode
 * @see MIAUpdateNormalNode
 * @see MIAUpdateFwNode
 */
class MIAUpdateAnyNode
    {

public:

    /**
     * These enumerations provide known node types that
     * can be interpreted as known more specific interfaces.
     */
    enum TNodeType
        {
        /**
         * Node type is not defined.
         * Static casting of the node should be avoided.
         */
        ENodeTypeNotDefined,
        
        /**
         * Node is normal node.
         * @see MIAUpdateNode
         */
        ENodeTypeNormal,

        /**
         * Node is firmware node.
         * @see MIAUpdateFwNode
         */        
        ENodeTypeFw
        };

    /**
     * @return TNodeType Describes the type of the node. 
     */
    virtual TNodeType NodeType() const = 0;
    
    /**
     * @return MIAUpdateBaseNode& Base interface for nodes.
     * This interface provides common details for all the different nodes.
     */
    virtual MIAUpdateBaseNode& Base() = 0;


protected:

    /**
     * Protected destructor to prevent deletion.
     */    
    virtual ~MIAUpdateAnyNode() { }

    };


#endif // IA_UPDATE_ANY_NODE_H

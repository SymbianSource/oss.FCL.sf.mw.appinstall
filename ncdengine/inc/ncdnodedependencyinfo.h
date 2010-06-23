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
* Description:   MNcdDependencyInfo
*
*/


#ifndef NCD_NODE_DEPENDENCY_INFO
#define NCD_NODE_DEPENDENCY_INFO

#include <e32cmn.h>

#include "ncdnodedependency.h"

class MNcdNode;

/**
 *  This interface provides functions to get information about dependency
 *  nodes or dependency content.
 *
 *  This interface is also present when the node depends on some free
 *  downloadable content. In that case, DependencyNodeL() will return 
 *  a NULL pointer.
 *
 *  @see MNcdNodeDependency
 *  @see MNcdNode
 *
 *  
 */
class MNcdNodeDependencyInfo
{
public:

    /** 
     * @return const TDesC& Name of the dependency content.
     *         If the protocol has not defined any value, 
     *         an empty string is returned.
     */
    virtual const TDesC& Name() const = 0;

    /** 
     * @return const TDesC& Version of the dependency content.
     *         If the protocol has not defined any value, 
     *         an empty string is returned.
     */
    virtual const TDesC& Version() const = 0;
    
    /** 
     * This function is the indicative Symbian application UID for the
     * contents that are applications. This can be used e.g. for checking
     * if the application has already been installed to the phone.
     *    
     * @return TUid Uid of the dependency content.
     */
    virtual TUid Uid() const = 0;


    /** 
     * @return MNcdNode* Node that contains the dependency content. 
     *         If the dependency content has been given as a 
     *         downloadable content, then NULL is returned.
     *         Counted, Release() must be called after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdNode* DependencyNodeL() const = 0;


    /**
     * Returns the state of the dependency
     *
     * @return Dependency's state
     */
    virtual TNcdDependencyState State() const = 0;


protected:

    /**
     * The destructor of an interface is set virtual to make sure that
     * the destructors of derived classes are called appropriately when the
     * object is destroyed.
     *
     * Destructor is defined as protected to prevent direct use of 
     * delete on interface.
     *
     * 
     */
    virtual ~MNcdNodeDependencyInfo() { };

};

#endif // NCD_NODE_DEPENDENCY_INFO

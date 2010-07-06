/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateDepUtils class 
*
*/



#ifndef IAUPDATEDEPUTILS_H_
#define IAUPDATEDEPUTILS_H_

// INCLUDES
#include <e32base.h> 

// FORWARD DECLARATIONS
class MIAUpdateNode;

// CLASS DECLARATION
/**
* Static class to retrieve dependencies/dependants of a node
*
*/
class IAUpdateDepUtils 
    {
public:
    /**
    * Get all dependency nodes which are not marked 
    *
    * @param aNode            Dependant node 
    * @param aAllNodes        All visible nodes in UI 
    * @param aDependencyNodes List of dependencies (to be marked). 
    */ 
    static void GetDependenciesL( const MIAUpdateNode& aNode, 
                                  const RPointerArray<MIAUpdateNode>& aAllNodes, 
                                  RPointerArray<MIAUpdateNode>& aDependencyNodes );
     
    /**
    * Get all dependant nodes which are marked 
    * 
    * @param aNode            Dependency node 
    * @param aAllNodes        All visible nodes in UI 
    * @param aDependencyNodes List of dependendants (to be unmarked). 
    */
    static void GetDependantsL( const MIAUpdateNode& aNode, 
                                const RPointerArray<MIAUpdateNode>& aAllNodes, 
                                RPointerArray<MIAUpdateNode>& aDependantNodes );
    
    };
#endif /* IAUPDATEDEPUTILS_H_ */

// End of File

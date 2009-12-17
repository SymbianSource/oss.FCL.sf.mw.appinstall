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
* Description:   Contains item purpose definitions
*
*/


#ifndef NCD_SCHEME_NODE_TYPE_H
#define NCD_SCHEME_NODE_TYPE_H

/**
 * Defines the flag values indicating the different
 * available scheme node types.
 *
 * 
 */
enum TNcdSchemeNodeType
    {
    /** Item node */
    ENcdSchemeItem,
    
    /** Normal folder node */
    ENcdSchemeFolder,
    
    /** Bundle folder node that must be requested from CDB */
    ENcdSchemeBundleFolder
    };

#endif // NCD_SCHEME_NODE_TYPE_H

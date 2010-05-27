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



#ifndef IA_UPDATE_CTRL_NODE_CONSTS_H
#define IA_UPDATE_CTRL_NODE_CONSTS_H

/**
 * IAUpdateCtrlNodeConsts
 * Gives constants that are used with node handling.
 */
namespace IAUpdateCtrlNodeConsts
    {
    
    /**
     * MIME post fix for the items that are hidden and should
     * not be shown in history view. But, still their information
     * should be included in purchase history. The postfix can be 
     * used to identify hidden items. This information is needed 
     * for the history data.
     *
     * Pattern constant is used for MIME comparisons.
     * So, strings should be equal and only difference is 
     * wildcard characters.
     */
    _LIT( KMimeHiddenPostfix, "/iad-hidden");
    _LIT( KMimeHiddenPostfixPattern, "*/iad-hidden" );


    // Special error codes
    // Make sure that this value does not overlap ncderrors.h 
    // or catalogserrors.h error codes.

    /**
     * Base error codes fore service pack operations.
     * Because service pack operation type information can not be
     * set into the purchase history through the NCD API, these
     * error code bases can be used to check what operation was
     * actually last handled. Purchse history Type information
     * would always give EStatePurchased for service packs.
     * The correct error code is inserted on top of the base
     * error code.
     */
    const TInt KErrBaseRange( 100000 );
    const TInt KErrBaseServicePackDownload( -1300000 );
    const TInt KErrBaseServicePackInstall( 
        KErrBaseServicePackDownload - KErrBaseRange );


    // Notice, if values below are changed, then make sure it is still
    // in the correct range for KErrBaseRange.

    // This error value is used if error has occurred in a dependency
    // chain and the current node that is a dependant should be skipped
    // without putting any information to the purchase history.
    const TInt KErrSkipNode( -27182 );

    }
    
#endif // IA_UPDATE_CTRL_NODE_CONSTS_H

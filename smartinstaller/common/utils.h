/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*   Static utility methods declaration
*/

#ifndef __UTILS_H__
#define __UTILS_H__

class CUtils : public CBase
	{
public:
	/**
	 * Returns ETrue if Ovi Store client is running
	 */
	static TBool OviStoreRunning();

	/**
	 * Returns ETrue if process with given UIDs is running
	 */
	static TBool IsProcessRunning(const TInt aUid);

	/**
	 * Returns ETrue if any of the listed processes with given UIDs are running
	 */
	static TBool AreProcessesRunning(const RArray<TInt>& aUids);

	/**
	 * Hides specified application icon from the menu grid. This version
	 * does not leave.
	 *
	 * @returns Error code
	 */
	static TInt HideApplicationFromMenu( TInt aUid, TBool aHidden );

	/**
	 * Hides specified application icon from the menu grid.
	 */
	static void HideApplicationFromMenuL( TInt aUid, TBool aHidden );

	/**
	 * Helper function to create a string presentation of the UID.
	 * Used by HideApplicationFromMenu()
	 */
	static void UIDText( TDes& aUid, TInt aInt );
	};

#endif

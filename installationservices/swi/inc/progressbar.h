/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __PROGRESSBAR_H__
#define __PROGRESSBAR_H__

namespace Swi
	{
	/// How many increments to update the bar on the uninstall of a file
	const static TInt KProgressBarUninstallAmount=1;
	
	/// How many bytes to copy before updating the progress bar by one increment
	const static TInt KProgressBarInstallChunkSize=1048576;  //1MB

	/// The extra increment we add onto the end to make there always be some progress.
	const static TInt KProgressBarEndIncrement=1;
	
	/// Percentage of completion required before updating the progress bar.
    const static TInt KProgressBarIncrement=1;

	/**
	 * Utility function to get how much to update the progress bar for a certain
	 * size of file copied. The smallest increment is KProgressBarEndIncrement since we always want to show that 
	 * some progress has occurred.
	 */
	TInt ProgressBarFileIncrement(TInt64 aSize);

	/**
	 * Utility function to get how much to update the progress bar for a certain
	 * size of chunk copied. 
	 */
	TInt ProgressBarChunkIncrement(TInt64 aSize);

	inline TInt ProgressBarChunkIncrement(TInt64 aSize)
		{
		return I64LOW(aSize) / KProgressBarInstallChunkSize;
		}

	inline TInt ProgressBarFileIncrement(TInt64 aSize)
		{
		return KProgressBarEndIncrement + ProgressBarChunkIncrement(aSize);
		}
	}


#endif

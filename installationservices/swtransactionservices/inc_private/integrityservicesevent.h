/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of TIntegrityServicesEvent
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __INTEGRITYSERVICESEVENT_H__
#define __INTEGRITYSERVICESEVENT_H__

namespace Usif
{
	
/**
 * Defines the events recorded in the journal file
 */
enum TIntegrityServicesEvent
	{
	ENone,
	ERemovedFile,
	EBackupFile,
	ETempFile,
	EAddedFile,
	ECommitted,
	EBackupFilesRemoved,
	EAddedFilesRemoved,
	ERemovedFilesRestored,
	ETempFilesRemoved
	};

} //namespace
#endif

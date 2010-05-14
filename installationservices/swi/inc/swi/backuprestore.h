/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef	__SWI_BACKUPRESTORE_H__
#define	__SWI_BACKUPRESTORE_H__

/**
 * @file
 * Definition of the interface between Secure Backup and Restore and Software Install.
 *
 * @internalTechnology
 * @released
*/

#include <e32std.h>
#include <f32file.h>

#include <swi/sisregistrysession.h>


namespace Swi
	{
	
	class RInstallServerSession;
	
	/**
     * @internalTechnology
	 * @released
	 */
	class RRestoreSession
		{
		
	public:
		IMPORT_C TInt Connect();
		IMPORT_C void StartPackageL(TUid aPackageUid, const HBufC8& aPackageMetadata);
		IMPORT_C void RestoreFileL(RFile& aFile, TDesC& aDestinationFileName);
		IMPORT_C void CommitPackageL();
		IMPORT_C void Close();
		
	private:
		RInstallServerSession* iSession;
		
		};
		
	/**
     * @internalTechnology
	 * @released
	 */
	class RBackupSession
		{
		
	public:
		IMPORT_C TInt Connect();
		IMPORT_C HBufC8* GetMetaDataL(TUid aPackageUid, RPointerArray<HBufC>& aFilesArray);
		IMPORT_C void Close();
	
	private:
		RSisRegistrySession iSession;
		
		};
		
	}
	
	
#endif	/* __SWI_BACKUPRESTORE_H__ */

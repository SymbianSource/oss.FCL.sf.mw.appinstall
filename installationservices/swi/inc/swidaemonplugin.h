/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef SWI_DAEMON_PLUGIN_H
#define SWI_DAEMON_PLUGIN_H
#include <ecom/ecom.h>

class CSwiDaemonPlugin : public CBase
	{
public:
	static CSwiDaemonPlugin *NewL();

	/**
	 * Media has been inserted with one, or more, valid (ie. correct
	 * header), uninstalled, stub-SIS files on it. This will be
	 * followed by one or more calls to RequestInstall for each SIS
	 * file on this media.
	 *
	 * @param aDrive Drive being processed
	 */
	virtual void MediaProcessingStart( TInt aDrive ) = 0;

	/**
	 * @param aFile - RFile handle, valid until this function returns.
	 *
	 * The FS associated with the RHandle is marked for sharing (so
	 * the RHandle can be passed to a server using
	 * RFile::TransferToServer).
	 * 
	 * Note that immediately after this call, the RFile handle will be
	 * closed. This means that the plugin must either duplicate it, or
	 * synchronously send it to a server which adopts it before this
	 * function returns.
	 */
	virtual void RequestInstall( RFile& aFile ) = 0;

	/**
	 * Normally a sequence of RequestInstall calls for each SIS file
	 * on a media will be followed by a call to this function.
	 *
	 * Alternatively MediaRemoved might be called to abort the sequence.
	 */
	virtual void MediaProcessingComplete() = 0; 


	/**
	 * Media has been removed from drive.
	 *
	 * @param aDrive Drive number
	 *
	 * If the media in the drive specified by the last call to
	 * MediaProcessingStart is removed, then this function will be
	 * called, regardless of whether or not MediaProcessingComplete
	 * has been called.
	 *
	 * This function may also be called when media is removed from any
	 * drive.
	 */
	virtual void MediaRemoved( TInt aDrive ) = 0;

	virtual ~CSwiDaemonPlugin();

private:
	TUid iDtor_ID_Key;
	};

_LIT8(KX,"X");
inline CSwiDaemonPlugin *CSwiDaemonPlugin::NewL()
	{
	const TUid KSwiDriveWatcherPlugin = { 0x10274D07 };
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KX());
	TAny *ptr = REComSession::CreateImplementationL(KSwiDriveWatcherPlugin,
													_FOFF(CSwiDaemonPlugin, iDtor_ID_Key),
													resolverParams,
													KRomOnlyResolverUid);
	return (CSwiDaemonPlugin *)ptr;
	}


inline CSwiDaemonPlugin::~CSwiDaemonPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}


#endif // SWI_DRIVEWATCHER_PLUGIN_H

/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the UI Support Server command class
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __UISUPPORTCOMMAND_H__
#define __UISUPPORTCOMMAND_H__

#include <e32std.h>
#include <e32base.h>

namespace Swi
{
class MUiHandler;

/**
 * Base class for all command handlers, which are used to handle messages received
 * by the server. This class provides utility functions for the derived classes
 * to utilise.
 */
class CUissCmdHandler  : public CBase
	{
public:
	/**
	 * Called to handle the message received by the server.
	 *
	 * @param aInBuf  Contains input data for the message
	 * @param aOutBuf Receives output data from the message handler
	 */
	virtual void HandleMessageL(const TDesC8& aInBuf, TDes8& aOutBuf)=0;
	
protected:
	/**
	 * Constructor.
	 *
	 * @param aUiHandler The UI Handler used to display the messages.
	 */
	IMPORT_C CUissCmdHandler(MUiHandler& aUiHandler);

	/**
	 * Helper function to complete the message, passing back a descriptor in argument 
	 * one of the message.
	 *
	 * @param aMessage	The message to complete.
	 * @param aResult	The result to complete with.
	 * @param aData		The data to write back in argument one of the message.
	 */
	void CompleteL(const RMessage2& aMessage, TInt aError, const TDesC8& aData) const;
	
protected:
	/// The UI Handler, used to call UI functions while handling the message.
	MUiHandler& iUiHandler;
	};

} // namespace Swi

#endif // #ifndef __UISUPPORTCOMMAND_H__

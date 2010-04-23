/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines server-side classes which are used to read SWI events from 
* the observation log files.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SWIOBSERVATIONREADER_H
#define SWIOBSERVATIONREADER_H

#include "swiobservedinfo.h"
#include "log.h"

namespace Swi
	{
	class CSwiObservationReader : public CBase
	/**
		Class for reading observed records from a SWI observation log file. 
		The log file is opened with OpenFileL and the set of records may be 
		enumerated by repeatedly calling the ReadRecordL function. 
	 */
		{
	public:
		static CSwiObservationReader* NewL();
		static CSwiObservationReader* NewLC();
				
		void OpenFileL(RFs& aFs, const TDesC& aLogFileName);
		CObservationData* ReadRecordL();
		CObservationHeader* ReadHeaderL();
		TUint8 ReadRecordTypeL();
		void SetStartPosL();
		void CloseFile();
				
		~CSwiObservationReader();
			
	private:
		CSwiObservationReader();
		void ConstructL(RFs& aFs, const TDesC& aLogFileName);
			
	private:
		RFileReadStream iStream; ///< Stream to read from the observation log file.
		TStreamPos iPosEnd;      ///< The end position of the observation log file.
		};
	} // End of namespace Swi
#endif
 
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
* Implements server-side classes which are used to read SWI events from 
* the observation log files.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#include "swiobservationreader.h"

using namespace Swi;

//
//CSwiObservationReader
//

CSwiObservationReader::CSwiObservationReader()
/**
	Constructor.
 */
 	{
 	//empty
 	}
 	
CSwiObservationReader::~CSwiObservationReader()
/**
	Destructor.
*/
	{
	iStream.Close();
	}
	
CSwiObservationReader* CSwiObservationReader::NewL()
/**
	Factory function allocates a new, initialized CSwiObservationReader object.
	
	@param 	aFs				File server session handle.
	@param	aLogFileName	Fully qualified observation log file path.
	@return					The newly created observation reader object. 
 */
	{
	CSwiObservationReader *sor = CSwiObservationReader::NewLC();
	CleanupStack::Pop(sor);
	return sor;
	}
	

CSwiObservationReader* CSwiObservationReader::NewLC()
/**
	Factory function allocates a new, initialized CSwiObservationReader object which
	is left on the cleanup stack.
	
	@see CSwiObservationReader::NewL
 */
	{
	CSwiObservationReader *sor = new(ELeave) CSwiObservationReader();
	CleanupStack::PushL(sor);
	return sor;
	}
	
	
void CSwiObservationReader::OpenFileL(RFs& aFs, const TDesC& aLogFileName)
/**
	Opens a given log file to read the observed information.
	
	@param aFs File server session handle.
	@param aLogFileName The fully qualified name of the log file
 */
	{
	User::LeaveIfError(iStream.Open(aFs,aLogFileName,EFileRead|EFileShareExclusive));
	
	if(0 == iStream.Source()->SizeL())
		{//File is empty, return with doing nothing.
		User::Leave(KErrEof);
 		}
	}

	
TUint8 CSwiObservationReader::ReadRecordTypeL()
/**
	Reads the type of the next record written in the current open file.
	
	@return The type of the record: KHeaderField or KRecordField.
 */
	{
	TUint8 t = iStream.ReadUint8L();
	if((t != KDataField) && (t != KHeaderField))
		{
		User::Leave(KErrCorrupt);
		}
		
	return t;
	}


CObservationData* CSwiObservationReader::ReadRecordL()
/**
	Internalises an observation record object. The function creates
	a record object and then assigns its member variables by reading
	from the current open file.
	
	@return An internalized observation record object. 
 */
	{
	CObservationData *r = CObservationData::NewLC();
	r->InternalizeL(iStream);	
	CleanupStack::Pop(r);
	return r;
	}
		
 
CObservationHeader* CSwiObservationReader::ReadHeaderL()
/**
	Internalises an observation header object. The function creates
	a header object and then assigns its member variables by reading
	from the current open file.
	
	@return An internalized observation header object. 
 */
	{
	CObservationHeader *p = CObservationHeader::NewLC();
	p->InternalizeL(iStream);
	CleanupStack::Pop(p);
	return p;
	}
	

void CSwiObservationReader::SetStartPosL()
/**
	Moves the position of the read mark in the stream to the beginning.	
 */
	{
	iStream.Source()->SeekL(MStreamBuf::ERead,EStreamBeginning,0);
	}
	
		
void CSwiObservationReader::CloseFile()
/**
	Closes the current open file.
 */
	{
	iStream.Close();
	iPosEnd = 0;
	}
	

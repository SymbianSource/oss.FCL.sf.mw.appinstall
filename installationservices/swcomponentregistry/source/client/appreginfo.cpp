/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* appreginfo.cpp
*
*/

#include <s32strm.h>
#include <usif/scr/appreginfo.h>

namespace Usif
    {

EXPORT_C TAppRegInfo::TAppRegInfo()
	: iUid(TUid()),
	iFullName(KNullDesC),
	iCaption(KNullDesC),
	iShortCaption(KNullDesC)
	{}


EXPORT_C TAppRegInfo::TAppRegInfo(TUid aAppUid, const TFileName& aAppName, const TAppCaption& aCaption)
	: iUid(aAppUid),
	iFullName(aAppName),
	iCaption(aCaption),
	iShortCaption(aCaption)
	{}


EXPORT_C TAppRegInfo::TAppRegInfo(TUid aAppUid, const TFileName& aAppName, const TAppCaption& aCaption, const TAppCaption& aShortCaption)
	: iUid(aAppUid),
	iFullName(aAppName),
	iCaption(aCaption),
	iShortCaption(aShortCaption)
	{}


EXPORT_C void TAppRegInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<< iUid;
	aStream<< iFullName;
	aStream<< iCaption;
	aStream<< iShortCaption;
	}



EXPORT_C void TAppRegInfo::InternalizeL(RReadStream& aStream)
	{
	aStream>> iUid;
	aStream>> iFullName;
	aStream>> iCaption;
	aStream>> iShortCaption;
	}

EXPORT_C TUid TAppRegInfo::Uid() const
   {
   return iUid;
   }

EXPORT_C TFileName TAppRegInfo::FullName() const
   {
   return iFullName;
   }

EXPORT_C TAppCaption TAppRegInfo::Caption() const
   {
   return iCaption;
   }

EXPORT_C TAppCaption TAppRegInfo::ShortCaption() const
   {
   return iShortCaption;
   }
}

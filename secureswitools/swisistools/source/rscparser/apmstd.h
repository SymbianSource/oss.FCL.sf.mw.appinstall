// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
/** 
* @file apmstd.h
*
* @internalComponent
* @released
*/
#if !defined (__APMSTD_H__)
#define __APMSTD_H__

#include "commontypes.h"

/**
@publishedAll
@released

The largest possible value for a TInt16.
*/
const TInt KMaxTInt16=0x7fff;

/**
@publishedAll
@released

The smallest possible value for a TInt16.
*/
const TInt KMinTInt16=(-32768);

/**
@publishedAll
@released
*/
const TInt KMaxDataTypeLength=256;

/** Defines the priority associated with a data type.

The priority is used by a device to resolve the current preferred handler 
of a data type, in the absence of any user preferences.

@publishedAll
@released
@see TDataTypeWithPriority */
typedef TInt32 TDataTypePriority;

/** Reserved for future use. 

@publishedAll
@released */
const TInt32 KDataTypePriorityUserSpecified=KMaxTInt16;

/** Trusted applications may use this priority value to ensure 
    their MIME type associations cannot be overridden by 
    self signed (untrusted) applications.

@publishedAll
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePriorityTrustedHigh = 0xC000;

/** This is Threshold priority for Un-Trusted apps i.e., for the applications which reside 
in \\private\\10003a3f\\import\\apps and have a SID which is in the unprotected range. This is to avoid Un-Trusted app's claiming for priority higher 
than the threshold, if they claim for priority higher than the Threshold then it would be 
reduced to KDataTypeUnTrustedPriorityThreshold.

@publishedAll
@released */
const TInt32 KDataTypeUnTrustedPriorityThreshold=KMaxTInt16;

/** A priority value associated with a data (MIME) type. Associating an application 
with a data type at this priority means that this application, and no other, 
should ever handle the data type. Use sparingly.

@publishedAll
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePriorityHigh=10000;

/** A priority value associated with a data (MIME) type. Associating an application 
with a data type at this priority means that the data type is the primary 
type for that application. For example, "text/plain" files for a text editor.

@publishedAll
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePriorityNormal=0;

/** A priority value associated with a data (MIME) type. Associating an application 
with a data type at this priority means that the data type is a secondary 
type for that application. For example, "text/plain" files for a web browser.

@publishedAll
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePriorityLow=-10000;

/** A priority value associated with a data (MIME) type. Associating an application 
with a data type at this priority means that the data type is a last resort 
for that application. Use sparingly.

@publishedAll
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePriorityLastResort=-20000;

/** Reserved for future use. 

@publishedAll
@released */
const TInt32 KDataTypePriorityNotSupported=KMinTInt16;

/** This is a special priority that overrides any user settings. Applications must
have EWriteDeviceData to use this priority. If they have not then they will be
downgraded to KDataTypePriorityNormal.

@publishedPartner
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePrioritySystem = 0xFFF9;


/** A data type.

The class maintains a string that describes the data type and an associated 
UID.

A data type is also referred to as a MIME type. 

@publishedAll
@released */
class TDataType
	{
public:
	 TDataType();
	 ~TDataType();
	 TDataType(const PtrC8* aDataType);
     Ptr8* GetDataType();
	 
private:
	Ptr8* iDataType;
	TUid iUid;
	};

class TDataTypeWithPriority
/** A data (MIME) type and a priority value for that data type.

The priority is used to resolve the current preferred handler of a data type, 
in the absence of any user preferences. An AIF file can associate an application 
with a data (MIME) type that is not native to Symbian OS. A priority value 
can be specified that allows the device to choose the highest priority application 
that can handle that data type.

@publishedAll 
@released 
@see TDataType
@see TDataTypePriority */
	{
public:
	 TDataTypeWithPriority();
	 TDataTypeWithPriority(const TDataType& aDataType, TDataTypePriority aPriority);

public:
	/** The data type. */
	TDataType iDataType;
	/** The priority value. */
	TDataTypePriority iPriority;
	};

#endif
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
* appreginfo.h
*
*/

/**
 @file
 @publishedAll
 @released 
*/

#ifndef __APPREGINFO_H__
#define __APPREGINFO_H__

#include <e32base.h>
	
class RReadStream;
class RWriteStream;

namespace Usif
    {
// Defines a modifiable buffer descriptor to contain the caption or the short caption for an application.
const TInt KMaxAppCaption=0x100;
typedef TBuf<KMaxAppCaption> TAppCaption;

/** Basic application information. An object of this type contains four pieces of information:
 - The application specific Uid
 - The full path name of the application exe
 - The application's caption
 - A short caption
*/
NONSHARABLE_CLASS (TAppRegInfo)
	{
    friend class CScrRequestImpl;
public:

    IMPORT_C TAppRegInfo();
    
    /** 
    Constructs an application information object from the specified full application path 
    name, UID and caption.

    @param aAppUid The application specific UID. 
    @param aAppName The full path name of the application. 
    @param aCaption The application caption. 
    */
    IMPORT_C TAppRegInfo(TUid aAppUid, const TFileName& aAppName, const TAppCaption& aCaption);

    /** 
    Constructs an application information object from the specified full application path 
    name, UID, short caption and caption.

    @param aAppUid The application specific UID. 
    @param aAppName The full path name of the application. 
    @param aCaption The application caption. 
    @param aShortCaption The application short caption.
    */    
    IMPORT_C TAppRegInfo(TUid aAppUid, const TFileName& aAppName, const TAppCaption& aCaption, const TAppCaption& aShortCaption);
    
    IMPORT_C void ExternalizeL(RWriteStream& aStream)const;
    
    IMPORT_C void InternalizeL(RReadStream& aStream);
    
    /**    
     @return The application specific UID.
     */
    IMPORT_C TUid Uid() const;
    
    /**   
     @return The full path name of the application executable.
     */    
    IMPORT_C TFileName FullName() const;
    
    /**   
     @return The caption for the application.
     */       
    IMPORT_C TAppCaption Caption() const;
    
    /**   
     @return The short caption for the application.
     */       
    IMPORT_C TAppCaption ShortCaption() const;
    
private:
	/** The application specific UID. */
	TUid iUid;
	/** The full path name of the application. */
	TFileName iFullName;
	/** The caption for the application. */
	TAppCaption iCaption;
	/** The short caption for the application. */
	TAppCaption iShortCaption;
	};
}
#endif


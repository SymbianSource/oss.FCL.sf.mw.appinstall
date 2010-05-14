/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisRegistryFile - this class provide a link between a subsession id, 
* registry object and its related stream 
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#ifndef __SISREGISTRYFILE_H__
#define __SISREGISTRYFILE_H__

#include <s32file.h>

namespace Swi
{
class CSisRegistryPackage;
class CSisRegistryCache;
class CSisRegistryObject;

class CSisRegistryFile: public CBase
	{
public:
	static CSisRegistryFile* NewL(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache);
	static CSisRegistryFile* NewLC(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache);
	
	~CSisRegistryFile();

	TUint SubsessionId() const; 
	CSisRegistryObject& RegistryObject() const;

//	const CSisRegistryPackage& Package() const;	
	
private: 	
    /** 
     * Reloads the CSisRegistryObject from the specified file. It should be
     * noted that this deletes the current object which may have been 
     * returned to other clients. In the server this should not happen.
     **/
   	void ReloadL(const TFileName& filename, CSisRegistryCache& aCache);
   	
	void ConstructL(const CSisRegistryPackage& aPackage, CSisRegistryCache& aCache);	
    RFileReadStream& ReadStream() ;	
    
    
private:
    friend class CSisRegistryCache;
    
	TUint        		 iSubsessionId;
	CSisRegistryObject*  iObject;
	RFileReadStream 	 iFileStream;
	};
	

inline RFileReadStream& CSisRegistryFile::ReadStream()  
	{ 
	return iFileStream; 
	}

inline TUint CSisRegistryFile::SubsessionId() const
	{ 
	return iSubsessionId; 
	} 	
	
inline CSisRegistryObject& CSisRegistryFile::RegistryObject() const
	{ 
	return *iObject; 
	}

	
} // namespace

#endif// __SISREGISTRYFILE_H__


	


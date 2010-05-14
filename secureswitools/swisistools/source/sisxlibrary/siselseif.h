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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISELSEIF_H__
#define __SISELSEIF_H__


#include "sisexpression.h"
#include "sisinstallblock.h"


class CSISElseIf : public CStructure <CSISFieldRoot::ESISElseIf>
	{
public:
	/**
	 * Default constructor.
	 */
	CSISElseIf ();
	/**
	 * Copy constructor
	 */
	CSISElseIf (const CSISElseIf& aInitialiser);
		
public:
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Expression correspons to this else-if block.
	 */
	inline const CSISExpression& Expression () const;
	/**
	 * Expression correspons to this else-if block.
	 */
	inline CSISExpression& Expression ();
	/**
	 * Install block correspons to this else-if block.
	 */
	inline const CSISInstallBlock& InstallBlock () const;
	/**
	 * Install block correspons to this else-if block.
	 */
	inline CSISInstallBlock& InstallBlock ();
	/**
	 * Make this else-if statement always true.
	 */
	inline void SetElse ();
	/**
	 * Retrieves the list of files present in this else-if block
	 * @param aFileList The list will be filled by this function
	 */
	inline void GetFileList(TFileDescList& aFileList) const;
	/**
	 * Retrieves the list of embedded sis files present in this else-if block
	 * @param aControllers The list will be filled by this function
	 */
	inline void GetEmbeddedControllers(TControllerMap& aControllers) const;
	/**
	 * Adds package entry related to this else-if block
	 * @param aStream stream into which the package details need to be written.
	 * @param aVerbose If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	void InsertMembers ();

private:
	CSISExpression		iExpression;
	CSISInstallBlock	iInstallBlock;

	};


inline void CSISElseIf::InsertMembers ()
	{
	InsertMember (iExpression);
	InsertMember (iInstallBlock);
	}


inline CSISElseIf::CSISElseIf ()
	{ 
	InsertMembers (); 
	}


inline CSISElseIf::CSISElseIf (const CSISElseIf& aInitialiser) :
		CStructure <CSISFieldRoot::ESISElseIf> (aInitialiser),
		iExpression (aInitialiser.iExpression),
		iInstallBlock (aInitialiser.iInstallBlock)
	{ 
	InsertMembers (); 
	}


inline const CSISExpression& CSISElseIf::Expression () const
	{ 
	return iExpression; 
	}


inline CSISExpression& CSISElseIf::Expression ()
	{ 
	return iExpression;
	 }


inline const CSISInstallBlock& CSISElseIf::InstallBlock () const
	{ 
	return iInstallBlock;
	 }


inline CSISInstallBlock& CSISElseIf::InstallBlock ()
	{ 
	return iInstallBlock; 
	}


inline void CSISElseIf::SetElse ()
	{ 
	iExpression.SetAlwaysTrue (); 
	}


inline std::string CSISElseIf::Name () const
	{
	return "Else If";
	}


inline void CSISElseIf::GetFileList(TFileDescList& aFileList) const
	{
	iInstallBlock.GetFileList(aFileList);
	}

inline void CSISElseIf::GetEmbeddedControllers(TControllerMap& aControllers) const
	{
	iInstallBlock.GetEmbeddedControllers(aControllers);
	}

#endif // __SISELSEIF_H__


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

#ifndef __SISIF_H__
#define __SISIF_H__


#include "sisexpression.h"
#include "sisinstallblock.h"
#include "siselseif.h"
#include "sisarray.h"

class CSISIf : public CStructure <CSISFieldRoot::ESISIf>
	{
public:
	/**
	 * Default constructor
	 */
	CSISIf ();
	/**
	 * Copy constructor
	 */
	CSISIf (const CSISIf& aInitialiser);

public:
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Expression corresponds to this if block.
	 * @return CSISExpression
	 */
	const CSISExpression& Expression () const;
	/**
	 * Expression corresponds to this if block.
	 * @return CSISExpression
	 */
	CSISExpression& Expression ();
	/**
	 * Add else-if block
	 */
	void AddElseIf ();
	/**
	 * Add else block
	 */
	void AddElse ();
	/**
	 * Retrieves the last elseif block
	 */
	const CSISElseIf& ElseIf () const;
	/**
	 * Retrieves the last elseif block
	 */
	CSISElseIf& ElseIf ();
	/**
	 * Retrieves the elseif block pointed by the index.
	 * @param aIndex index
	 */
	const CSISElseIf& ElseIf (const unsigned aIndex) const;
	/**
	 * Retrieves the elseif block pointed by the index.
	 * @param aIndex index
	 */
	CSISElseIf& ElseIf (const unsigned aIndex);
	/**
	 * Count of else-if block.
	 */
	inline TUint32 ElseIfCount() const;
	/**
	 * Expression corresponds to the last else-if block
	 */
	const CSISExpression& ElseIfExpression () const;
	/**
	 * Expression corresponds to the last else-if block
	 */
	CSISExpression& ElseIfExpression ();
	/**
	 * @return Install block of this if block
	 */
	CSISInstallBlock& InstallBlock ();
	/**
	 * @return Install block of this if block
	 */
	const CSISInstallBlock& InstallBlock () const;
	/**
	 * Adds package entry related to this if block.
	 * @param aStream stream into which the package details need to be written.
	 * @param aVerbose If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Retrieves the list of files present in the if block.
	 * @param aFileList this will be filled by this function.
	 */
	void GetFileList(TFileDescList& aFileList) const;
	/**
	 * Retrieves the list of embedded sis files present in the if block.
	 * @param aFileList this will be filled by this function.
	 */
	void GetEmbeddedControllers(TControllerMap& aControllers) const;

private:
	void InsertMembers ();

private:
	CSISExpression										iExpression;
	CSISInstallBlock									iInstallBlock;
	CSISArray <CSISElseIf, CSISFieldRoot::ESISElseIf>	iElseIfs;

	};


inline std::string CSISIf::Name () const
	{
	return "If";
	}

inline void CSISIf::InsertMembers ()
	{
	InsertMember (iExpression);
	InsertMember (iInstallBlock);
	InsertMember (iElseIfs);
	}

inline CSISIf::CSISIf ()
	{ 
	InsertMembers (); 
	}

inline CSISIf::CSISIf (const CSISIf& aInitialiser) :
		iExpression (aInitialiser.iExpression),
		iInstallBlock (aInitialiser.iInstallBlock),
		iElseIfs (aInitialiser.iElseIfs)
	{ 
	InsertMembers (); 
	}

inline const CSISExpression& CSISIf::Expression () const
	{ 
	return iExpression; 
	}

inline CSISExpression& CSISIf::Expression ()
	{ 
	return iExpression; 
	}

inline void CSISIf::AddElseIf ()
	{ 
	iElseIfs.Push (CSISElseIf ()); 
	}

inline void CSISIf::AddElse ()
	{ 
	AddElseIf (); ElseIf ().SetElse (); 
	} 

inline const CSISElseIf& CSISIf::ElseIf () const
	{ 
	return iElseIfs.Last (); 
	}

inline CSISElseIf& CSISIf::ElseIf ()
	{ 
	return iElseIfs.Last (); 
	}

inline const CSISElseIf& CSISIf::ElseIf (const unsigned aIndex) const
	{ 
	return iElseIfs [aIndex]; 
	}

inline CSISElseIf& CSISIf::ElseIf (const unsigned aIndex)
	{ 
	return iElseIfs [aIndex]; 
	}

inline const CSISExpression& CSISIf::ElseIfExpression () const
	{ 
	return iElseIfs.Last ().Expression (); 
	}

inline CSISExpression& CSISIf::ElseIfExpression ()
	{ 
	return iElseIfs.Last ().Expression (); 
	}

inline CSISInstallBlock& CSISIf::InstallBlock ()
	{ 
	return iInstallBlock; 
	}

inline const CSISInstallBlock& CSISIf::InstallBlock () const
	{ 
	return iInstallBlock; 
	}

inline TUint32 CSISIf::ElseIfCount () const
	{ 
	return iElseIfs.size(); 
	}
#endif // __SISIF_H__


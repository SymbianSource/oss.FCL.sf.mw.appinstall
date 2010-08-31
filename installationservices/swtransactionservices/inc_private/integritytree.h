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
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __INTEGRITYTREE_H__
#define __INTEGRITYTREE_H__

#include <e32base.h>
#include <f32file.h>
#include "integrityservicesevent.h"
#include "integrityservices.h"

namespace Usif
{

class CIntegrityTreeNode;
class CIntegrityTreeLeaf;

typedef void(* TTreeWalkFunctionL)(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, RLoader& aLoader, CFileMan& aFileMan);

class CIntegrityTreeLeaf : public CBase
	{
public:
	/**
	 * Constructs a file tree leaf, and places it on the cleanup stack
	 *
	 * @param aFileName The filename for this leaf node
	 * @param aType The type of this leaf
	 * @param aOwningJournal A reference to the filename of the owning journal
	 * @return A fully constructed tree leaf
	 */
	static CIntegrityTreeLeaf* NewLC(const TDesC& aFileName, TIntegrityServicesEvent aType,
		const TDesC& aOwningJournal);
	
	/**
	 * Sets the peer node if applicable. This is used for leaves that
	 * naturally form pairs, for example a removed file and its backup.
	 * 
	 * @param aPeer The leaf's peer.
	 *
	 */
	
	void SetPeer(CIntegrityTreeLeaf* aPeer);
	
	/**
	 * Returns the leaf's peer, or null if none was set
	 *
	 * @return The leaf's peer node if available
	 *
	 */
	
	CIntegrityTreeLeaf* Peer();
	
	/**
	 * Returns the type of this leaf. 
	 *
	 * @return This leaf's type
	 */
	 
	TIntegrityServicesEvent Type();
	
	/**
	 * Returns the file name for this leaf
	 *
	 * @return The file name for this leaf
	 */
	
	const TDesC& Name();
	
	/**
	 * Returns the name of the journal file that owns this leaf
	 *
	 * @return The associated journal file name
	 */
	
	const TDesC& Journal();
	
	~CIntegrityTreeLeaf();
private:
	CIntegrityTreeLeaf(TIntegrityServicesEvent aType, const TDesC& aOwningJournal);
	void ConstructL(const TDesC& aFileName);
	
private:
	HBufC* iFileName;
	TIntegrityServicesEvent iType;
	CIntegrityTreeLeaf* iPeer;
	
	const TDesC& iOwningJournal;
	
	};
	
class CIntegrityTreeNode : public CBase
	{
public:
	/**
	 * Constructs a file tree node, and places it on the cleanup stack
	 *
	 * @param aDirectoryName The name of the directory this node represents
	 * @return A fully constructed tree node
	 */
	
	static CIntegrityTreeNode* NewL(const TDesC& aDirectoryName);
	
	/**
	 * Constructs a file tree node
	 *
	 * @param aDirectoryName The name of the directory this node represents
	 * @return A fully constructed tree node
	 */
	
	static CIntegrityTreeNode* NewLC(const TDesC& aDirectoryName);
	
	/**
	 * Returns the name of the directory this node represents
	 * 
	 * @return The name of this directory
	 */
	
	const TDesC& Name();
	
	/**
	 * Adds a leaf node to this tree structure, adding intermediate (directory) nodes along
	 * the way as appropriate.
	 * 
	 * @param aFileName The fully qualified file name of the node to add
	 * @param aType The type of the node to add
	 * @param aOwningJournal The file name of the journal that owns this node
	 * @return The leaf node added.
	 *
	 */
	
	CIntegrityTreeLeaf* AddNodeL(const TDesC& aFileName, TIntegrityServicesEvent aType, const TDesC& aOwningJournal);
	
	/**
	 * Finds a node in the tree if it exists.
	 *
	 * @param aFileName The file name to find
	 * @param aType The type of node to find
	 * @return KErrNone if found, otherwise one of the system wide error codes
	 */
	
	TInt FindNode(const TDesC& aFileName, TIntegrityServicesEvent aType);
	
	/**
	 * Applies a function to every applicable node in the tree
	 *
	 * @param aFunc The function to apply
	 * @param aTypeFilter The type of node to apply the function to
	 * @param aFs A connected file server session
	 * @param aLoader reference to the RLoader server session
	 * @param aFileMan A constructed CFileMan object
	 * @param aFailType The type of failure to induce if required. Used exclusively in testing.
	 *
	 */
	
	void WalkTreeL(TTreeWalkFunctionL aFunc, TIntegrityServicesEvent aTypeFilter,
		RFs& aFs, RLoader& aLoader, CFileMan& aFileMan, 
		CIntegrityServices::TFailType aFailType);
		
	~CIntegrityTreeNode();
protected:
	CIntegrityTreeLeaf* DoAddNodeL(TDes& aFileName, TIntegrityServicesEvent aType, const TDesC& aOwningJournal);
	TInt DoFindNode(TDes& aFileName, TIntegrityServicesEvent aType);
	void DoWalkTreeL(TDes& aPath, TTreeWalkFunctionL aFunc, TIntegrityServicesEvent aTypeFilter, 
		RFs& aFs, RLoader& aLoader, CFileMan& aFileMan,
		CIntegrityServices::TFailType aFailType);
	
private:
	CIntegrityTreeNode();
	void ConstructL(const TDesC& aDirectoryName);
	
private:
	HBufC* iDirName;
	RPointerArray<CIntegrityTreeNode> iDirectories;
	RPointerArray<CIntegrityTreeLeaf> iFiles;
	
	friend class Usif::CIntegrityServices;
	};

} // namespace Usif

#endif /* __INTEGRITYTREE_H__ */

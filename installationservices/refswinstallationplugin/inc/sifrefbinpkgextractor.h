/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines a class responsible for extracting files from a compound package.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef SIFREFBINPKGEXTRACTOR_H
#define SIFREFBINPKGEXTRACTOR_H

#include <e32base.h>
#include <f32file.h>
#include <bacline.h>
#include <usif/sts/sts.h>
#include <usif/sts/sts.h>
#include <usif/sif/sifcommon.h>

namespace Usif
	{
	namespace SifRefBinPkgExtractor
		{
	
		/**
		CAuxNode is an auxiliary node class used for the processing of reference embedded packages
		without implementing a walk-tree algorithm. The BuildPkgTreeL() function takes a list of CAuxNode
		objects as an argument and fills it with extracted packages.
		*/
		class CAuxNode : public CBase
			{
		public:
			/**
			Creates an instance of the CAuxNode object.
			*/
			static CAuxNode* NewLC(const TDesC& aFileName, TBool aForeign, CAuxNode& aParent);
			static CAuxNode* NewLC(const RFile& aFile, TBool aForeign, CAuxNode& aParent);
			/**
			Destructor.
			*/
			virtual ~CAuxNode();

			/**
			Returns the name of the extracted package associated with this auxiliary node.
			*/
			const TDesC& FileNameL() const { return *iFileName; }

			/**
			Checks if the extracted package associated with this node it a root component.
			*/
			TBool Root() const { return iParent == NULL; }

			/**
			Checks if the extracted package associated with this node is of the reference type.
			*/
			TBool Foreign() const { return iForeign; }

			/**
			Associates aCompInfo with this auxiliary node.
			*/
			void SetCompInfoL(CComponentInfo* aCompInfo);

			/**
			Associates aNode with this auxiliary node.
			*/
			void SetNodeL(CComponentInfo::CNode* aNode);

			/**
			Sets the CNode object associated with this auxiliary node as a root node of aCompInfo.
			*/
			void SetAsRootNodeL(CComponentInfo& aCompInfo);

			/**
			Registers a child CNode object associated with this auxiliary node to its parrent.
			This method is provided for build a tree of CComponentInfo nodes from a flat tree
			of auxiliary nodes. After this call the auxiliary node is no longer associated with its
			CNode object.
			*/
			void RegisterChildToParentL();
			
		private:
			CAuxNode();
			CAuxNode& operator=(const CAuxNode& other);
			CAuxNode(const CAuxNode& other);

			TBool iForeign;
			HBufC* iFileName;
			CAuxNode* iParent;
			CComponentInfo::CNode* iNode;
			CComponentInfo* iCompInfo;
			};
		
		/**
		Builds a tree of the files embedded in the reference binary package specified by aInFileName.
		The paths to the extracted embedded packages are returned in aFlatTree. This flat list can be
		simply processed asynchronously without implementing a walk-tree algorithm in the Active Object
		context. However, it contains the information needed to build a real tree of the
		CComponentInfo::CNode objects.
		*/
		void BuildPkgTreeL(RStsSession& aSts, const TDesC& aInFileName, const TDesC& aTempDir, RPointerArray<CAuxNode>& aFlatTree, CAuxNode* aParent = NULL);

		/**
		Builds a tree of the files embedded in the reference binary package specified by aInFile.
		The paths to the extracted embedded packages are returned in aFlatTree. This flat list can be
		simply processed asynchronously without implementing a walk-tree algorithm in the Active Object
		context. However, it contains the information needed to build a real tree of the
		CComponentInfo::CNode objects.
		*/
		void BuildPkgTreeL(RStsSession& aSts, RFile& aInFile, const TDesC& aTempDir, RPointerArray<CAuxNode>& aFlatTree, CAuxNode* aParent = NULL);
		}
	}

#endif //  SIFREFBINPKGEXTRACTOR_H

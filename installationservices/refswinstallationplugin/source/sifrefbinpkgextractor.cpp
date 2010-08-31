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
* filemerger.cpp
*
*/


#include <e32cons.h>
#include <scs/cleanuputils.h>
#include <apmstd.h> 
#include <apgcli.h>
#include "usiflog.h"
#include "sifrefbinpkgextractor.h"

_LIT8(KCompoundPackageHeader, "_SifReferenceInstallerPackageHeader_");
_LIT8(KCompoundPackageFooter, "_SifReferenceInstallerPackageFooter_");
_LIT(KPkgFileExt, ".sifrefpkg");
_LIT(KRefBinPkgMimeType, "binary/sif-refpkg");
_LIT(KEmbCompMidDir, "children\\");
_LIT(KDirSeparator, "\\");

const TInt KBufferLength = 0x3FFC; // 16kB
const TInt KMaxNumFiles = 16;
const TInt KMaxEmbCompSize = 0xFFFFFF; // 16MB

using namespace Usif;

namespace
	{
	TInt ReadInt32L(RFile& file)
		{
		TBuf8<4> buf;
		User::LeaveIfError(file.Read(buf, sizeof(TInt)));
		return buf[0] | buf[1]<<8 | buf[2]<<16 | buf[3]<<24;
		}

	HBufC* BuildPkgFileNameLC(const TDesC& aTempDir, const TDesC& aBinPkgPath)
		{
		TParsePtrC parser(aBinPkgPath);
		TPtrC binPkgfileName = parser.Name();
		HBufC* pkgfileName = HBufC::NewLC(aTempDir.Length() + binPkgfileName.Length() + KPkgFileExt.iTypeLength);
		TPtr pkgfileNamePtr =  pkgfileName->Des();
		pkgfileNamePtr += aTempDir;
		pkgfileNamePtr += binPkgfileName;
		pkgfileNamePtr += KPkgFileExt;
		
		return pkgfileName;
		}

	HBufC* BuildPkgFileNameLC(const TDesC& aTempDir, const RFile& aFile)
		{
		HBufC* name = HBufC::NewLC(KMaxFileName);
		TPtr namePtr(name->Des());
		User::LeaveIfError(aFile.Name(namePtr));
		
		HBufC* pkgName = BuildPkgFileNameLC(aTempDir, *name);
		
		CleanupStack::Pop(pkgName);
		CleanupStack::PopAndDestroy(name);
		CleanupStack::PushL(pkgName);
		
		return pkgName;
		}

	void ExtractBinPkgFileL(RStsSession& aSts, RFile& aInFile, const TDesC& aOutPath, RPointerArray<HBufC>& aOutFiles)
		{
		// Check file size
		TInt size(0);
		User::LeaveIfError(aInFile.Size(size));
		if (size < KCompoundPackageHeader.iTypeLength+KCompoundPackageFooter.iTypeLength+sizeof(TInt))
			{
			User::Leave(KErrCorrupt);
			}
		
		// Create a buffer
		HBufC8* buffer = HBufC8::NewLC(KBufferLength);
		TPtr8 bufPtr = buffer->Des();
		
		// Read header
		User::LeaveIfError(aInFile.Read(bufPtr, KCompoundPackageHeader.iTypeLength));
		if (bufPtr != KCompoundPackageHeader)
			{
			User::Leave(KErrCorrupt);
			}
		bufPtr.Zero();
		
		// Read the size of a pkg content
		const TInt pkgSize = ReadInt32L(aInFile);
		if (pkgSize > KBufferLength)
			{
			User::Leave(KErrCorrupt);
			}

		// Build the name of the pkg file
		HBufC* inFileNameWithExt = HBufC::NewLC(KMaxFileName);
		TPtr inFileNameWithExtPtr = inFileNameWithExt->Des();
		User::LeaveIfError(aInFile.Name(inFileNameWithExtPtr));
		HBufC* pkgFileName = BuildPkgFileNameLC(aOutPath, *inFileNameWithExt);

		// Create the pkg file
		RFile pkgFile;
		aSts.CreateTemporaryL(*pkgFileName, pkgFile, EFileWrite);
		CleanupClosePushL(pkgFile);
		
		// Copy the content of the pkg file
		User::LeaveIfError(aInFile.Read(bufPtr, pkgSize));
		User::LeaveIfError(pkgFile.Write(bufPtr));
		CleanupStack::PopAndDestroy(&pkgFile);
		bufPtr.Zero();
		
		// Read the number of embedded components
		const TInt numEmbComps = ReadInt32L(aInFile);
		if (numEmbComps > KMaxNumFiles)
			{
			User::Leave(KErrCorrupt);
			}
		
		// Create a directory for embedded components
		HBufC* embCompPath = NULL;
		if (numEmbComps > 0)
			{
			embCompPath = HBufC::NewLC(aOutPath.Length() + KEmbCompMidDir.iTypeLength);
			embCompPath->Des().Copy(aOutPath);
			embCompPath->Des().Append(KEmbCompMidDir);
			}
		
		// Iterate over the components and extract them
		for (TInt i=0; i<numEmbComps; ++i)
			{
			// Read the length of the name of an embedded component
			const TInt strLen = ReadInt32L(aInFile);
			if (strLen > KMaxPath)
				{
				User::Leave(KErrCorrupt);
				}
			
			// Read the name of an embedded component and build its target path
			User::LeaveIfError(aInFile.Read(bufPtr, strLen));
			HBufC* embFileName = ConvertBufferTo16bitL(bufPtr);
			CleanupStack::PushL(embFileName);
			bufPtr.Zero();
			HBufC* embFileTargetPath = HBufC::NewLC(embCompPath->Length() + 2*embFileName->Length() + KDirSeparator.iTypeLength);
			TPtr embFileTargetPathPtr(embFileTargetPath->Des());
			embFileTargetPathPtr.Copy(*embCompPath);
			embFileTargetPathPtr += *embFileName;
			embFileTargetPathPtr += KDirSeparator;
			embFileTargetPathPtr += *embFileName;
			
			// Read the size of an embedded component
			const TInt fileSize = ReadInt32L(aInFile);
			if (fileSize > KMaxEmbCompSize)
				{
				User::Leave(KErrCorrupt);
				}
			
			// Create an output file
			RFile outFile;
			aSts.CreateTemporaryL(*embFileTargetPath, outFile, EFileWrite);
			CleanupClosePushL(outFile);
			
			// Copy the content of the output file
			const TInt numChunks = fileSize / KBufferLength;
			for (TInt c=0; c<numChunks; ++c)
				{
				User::LeaveIfError(aInFile.Read(bufPtr, KBufferLength));
				User::LeaveIfError(outFile.Write(bufPtr));
				bufPtr.Zero();
				}
			const TInt remainder = fileSize % KBufferLength;
			User::LeaveIfError(aInFile.Read(bufPtr, remainder));
			User::LeaveIfError(outFile.Write(bufPtr));
			bufPtr.Zero();
			
			CleanupStack::PopAndDestroy(&outFile);
			aOutFiles.AppendL(embFileTargetPath);
			CleanupStack::Pop(embFileTargetPath);
			CleanupStack::PopAndDestroy(embFileName);
			}
		
		if (embCompPath != NULL)
			{
			CleanupStack::PopAndDestroy(embCompPath);
			}
		
		// Read footer
		User::LeaveIfError(aInFile.Read(bufPtr, KCompoundPackageFooter.iTypeLength));
		if (bufPtr != KCompoundPackageFooter)
			{
			User::Leave(KErrCorrupt);
			}
		bufPtr.Zero();
		
		CleanupStack::PopAndDestroy(3, buffer); // inFileNameWithExt, pkgFileName
		}
		
	void ExtractBinPkgFileL(RStsSession& aSts, const TDesC& aInFileName, const TDesC& aOutPath, RPointerArray<HBufC>& aOutFiles)
		{
		RFs fs;
		RFile file;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TInt err = file.Open(fs, aInFileName, EFileShareReadersOnly);
		if (err != KErrNone)
			{
			DEBUG_PRINTF3(_L8("Failed to open file: %S with error: %d"), &aInFileName, err);
			User::Leave(err);
			}
		CleanupClosePushL(file);
		
		ExtractBinPkgFileL(aSts, file, aOutPath, aOutFiles);
		
		CleanupStack::PopAndDestroy(2, &fs);
		}

	TBool IsForeignL(RFile& aFileHandle)
		{
		// Get the MIME type of the component to be installed from AppArc
		TDataType dataType;
		RApaLsSession apa;
		User::LeaveIfError(apa.Connect());
		CleanupClosePushL(apa);
		TUid appUid = TUid::Null();
		User::LeaveIfError(apa.AppForDocument(aFileHandle, appUid, dataType));
		// A possible problem with recognizers is returning a successful result, but forgetting to set the MIME type
		if (dataType.Des8().Ptr() == NULL) 
			{
			User::Leave(KErrCompletion);
			}
		CleanupStack::PopAndDestroy(&apa);
		
		return dataType.Des() != KRefBinPkgMimeType;
		}

	TBool IsForeignL(const TDesC& aFileName)
		{
		RFs fs;
		RFile file;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.ShareProtected());
		TInt err = file.Open(fs, aFileName, EFileShareReadersOnly);
		if (err != KErrNone)
			{
			DEBUG_PRINTF3(_L8("Failed to open file: %S with error: %d"), &aFileName, err);
			User::Leave(err);
			}
		CleanupClosePushL(file);
		
		const TBool result = IsForeignL(file);

		CleanupStack::PopAndDestroy(2, &fs); // file

		return result;
		}
	}

namespace Usif
	{
	namespace SifRefBinPkgExtractor
		{
		CAuxNode* CAuxNode::NewLC(const TDesC& aFileName, TBool aForeign, CAuxNode& aParent)
			{
			CAuxNode* self = new (ELeave) CAuxNode;
			CleanupStack::PushL(self);
			
			self->iFileName = aFileName.AllocL();
			self->iForeign = aForeign;
			self->iParent = &aParent;
			
			return self;
			}
		CAuxNode* CAuxNode::NewLC(const RFile& aFile, TBool aForeign, CAuxNode& aParent)
		    {
	        CAuxNode* self = new (ELeave) CAuxNode;
	        CleanupStack::PushL(self);
	                    
	        HBufC* name = HBufC::NewLC(KMaxFileName);
	        TPtr namePtr(name->Des());
	        User::LeaveIfError(aFile.Name(namePtr));
	                    
	        self->iFileName = name->AllocL();
	        self->iForeign = aForeign;
	        self->iParent = &aParent;
	        
	        CleanupStack::PopAndDestroy(name); 
	        return self;
	        }

		CAuxNode::CAuxNode()
			{
			}
		
		CAuxNode::~CAuxNode()
			{
			delete iFileName;
			delete iNode;
			delete iCompInfo;
			}

		void CAuxNode::SetNodeL(CComponentInfo::CNode* aNode)
			{
			if (iNode != NULL)
				{
				User::Leave(KErrAlreadyExists);
				}
			iNode = aNode;
			}

		void CAuxNode::SetCompInfoL(CComponentInfo* aCompInfo)
			{
			if (iCompInfo != NULL)
				{
				User::Leave(KErrAlreadyExists);
				}
			iCompInfo = aCompInfo;
			}
		
		void CAuxNode::RegisterChildToParentL()
			{
			ASSERT (iCompInfo != NULL || iNode != NULL);
			
			if (iCompInfo != NULL)
				{
				iCompInfo->SetRootNodeAsChildL(*iParent->iNode);
				delete iCompInfo;
				iCompInfo = NULL;
				}
			else
				{
				iParent->iNode->AddChildL(iNode);
				iNode = NULL;
				}
			}
		
		void CAuxNode::SetAsRootNodeL(CComponentInfo& aCompInfo)
			{
			if (iNode == NULL)
				{
				User::Leave(KErrNotFound);
				}
			aCompInfo.SetRootNodeL(iNode);
			iNode = NULL;
			}

//-------------------------------------------------------------------------------------------------------

		void BuildPkgTreeImplL(RStsSession& aSts, const TDesC* aInFileName, RFile* aInFile, const TDesC& aTempDir, RPointerArray<CAuxNode>& aFlatTree, CAuxNode* aParent)
			{
			__ASSERT_ALWAYS(aInFileName != NULL || aInFile != NULL, User::Leave(KErrArgument));
			
			RCPointerArray<HBufC> embFiles;
			CleanupClosePushL(embFiles);
			CAuxNode* node = NULL;
			HBufC* pkgFileName = NULL;
			const TBool foreign = aInFile ? IsForeignL(*aInFile) : IsForeignL(*aInFileName);
			if (!foreign)
				{
				if (aInFile != NULL)
					{
					ExtractBinPkgFileL(aSts, *aInFile, aTempDir, embFiles);
					}
				else
					{
					ExtractBinPkgFileL(aSts, *aInFileName, aTempDir, embFiles);
					}
				
				pkgFileName = aInFile ? BuildPkgFileNameLC(aTempDir, *aInFile) : BuildPkgFileNameLC(aTempDir, *aInFileName);
				node = CAuxNode::NewLC(*pkgFileName, EFalse, *aParent);
				aFlatTree.AppendL(node);
				CleanupStack::Pop(node);
				for (TInt i=0; i<embFiles.Count(); ++i)
					{
					const TDesC& fileName = *embFiles[i];
					TParsePtrC parser(fileName);
					BuildPkgTreeImplL(aSts, &fileName, NULL, parser.DriveAndPath(), aFlatTree, node);
					}
				}
			else
				{
				if (aParent == NULL)
					{
					User::Leave(KErrCorrupt);
					}
				if (aInFileName != NULL)
				    {
				    node = CAuxNode::NewLC(*aInFileName, ETrue, *aParent);
				    }
				else
				    {
				    node = CAuxNode::NewLC(*aInFile, ETrue, *aParent);
				    }			
				aFlatTree.AppendL(node);
				CleanupStack::Pop(node);
				}
			
				if (pkgFileName != NULL)
					{
					CleanupStack::PopAndDestroy(pkgFileName);
					}
			
			CleanupStack::PopAndDestroy(&embFiles);
			}
		
		void BuildPkgTreeL(RStsSession& aSts, RFile& aInFile, const TDesC& aTempDir, RPointerArray<CAuxNode>& aFlatTree, CAuxNode* aParent)
			{
			BuildPkgTreeImplL(aSts, NULL, &aInFile, aTempDir, aFlatTree, aParent);
			}
		
		void BuildPkgTreeL(RStsSession& aSts, const TDesC& aInFileName, const TDesC& aTempDir, RPointerArray<CAuxNode>& aFlatTree, CAuxNode* aParent)
			{
			BuildPkgTreeImplL(aSts, &aInFileName, NULL, aTempDir, aFlatTree, aParent);
			}
		} // namespace SifRefBinPkgExtractor
	} //namespace Usif

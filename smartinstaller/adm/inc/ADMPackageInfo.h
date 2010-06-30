/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Declares the various classes required to maintain the Package Info and Dependency Tree for ADM application.
*
*
*/


#ifndef __PACKAGEINFO_H__
#define __PACKAGEINFO_H__

const TInt NOT_VISITED = 0;
const TInt BEING_VISITED = 1;
const TInt DONE_VISITED = 2;

// Forward Declaration
class CXmlParser;
class CPackageInfo;
class CADMAppUi;
class RFileLogger;

class Edge : public CBase
{
public:
	static Edge* NewL(CPackageInfo *aVtx1, CPackageInfo *aVtx2);
	static Edge* NewLC(CPackageInfo *aVtx1, CPackageInfo *aVtx2);
	~Edge(){}

	inline CPackageInfo *GetVertexOrg() const;
	inline CPackageInfo *GetVertexDst() const;

private:
	void ConstructL();
	Edge(CPackageInfo *aVtx1, CPackageInfo *aVtx2);

private:
	friend class CPackageInfo;

	CPackageInfo *iVertexOrg;
	CPackageInfo *iVertexDst;
	TDblQueLink iEdgeDlink;
};

enum PackageStatus
	{
	EPackageStatusUnknown,
	EPackageInstalled,
	EPackageToBeFetched,
	EPackageToBeUpgraded,
	EPackageFetchedInstalled,
	EPackageRootToBeInstalled,
	EPackageRootInstalled
	//EPackageToBeInstalled,
	//EPackageInstallationFailed
	};


// Class for storing Depfile data
class CPackageInfo: public CBase
	{
public:
	static CPackageInfo* NewL();
	static CPackageInfo* NewLC();
	~CPackageInfo();

	// Sort Order By Drive Priority
	static const TLinearOrder<CPackageInfo> KSortOrderByDrivePriority;

	//Comparator Function for sort by name
	static TInt CompareByDrivePriority( const CPackageInfo& aPackageOne,const CPackageInfo& aPackageTwo );

	void AddEdgeL(CPackageInfo* aDest);
	void VisitL(CDepTree *aDepTree);
	void SetDepFileNameL(const TDesC& aName);
	void SetChangesFileNameL();
	void SetSisFileNameL(const TDesC& aName);
	HBufC8* GetDownloadUrlL();

	inline HBufC8* GetURL() const
		{
		return iUrl;
		};

	inline HBufC* GetChangesFileName() const
		{
		return iChangesFileName;
		}

	inline HBufC* GetDepFileName() const
		{
		return iDepFileName;
		}

	inline HBufC* GetPackageName() const
		{
		return iPackageName;
		};

	inline HBufC* GetSisPackageName() const
		{
		return iSisPackageName;
		};

	inline PackageStatus GetPackageStatus() const
		{
		return iPackageStatus;
		}

	inline TUint32 GetPackageUid() const
		{
		return iPackageUid;
		}

	inline void SetPackageStatus(const PackageStatus& aStatus)
		{
		iPackageStatus = aStatus;
		}

	inline TUint32 GetDownloadSize() const
		{
		return iDownloadSize;
		}

	inline TUint32 GetInstallSize() const
		{
		return iInstalledSize;
		}

	inline TVersion GetPackageVersion() const
		{
		return iVersion;
		}

	inline TUint32 GetDrivePriority() const
		{
		return iDrivePriority;
		}
	inline void SetInstallDrive(const TChar aDrive)
		{
		iInstallDrive = aDrive;
		}
	inline TChar GetInstallDrive() const
		{
		return iInstallDrive;
		}

	inline TChar GetMandatoryInstallDrive() const
		{
		return iMandatoryInstallDrive;
		}

private:
	void ConstructL();
	CPackageInfo();

private:
	friend class CXmlParser;
	friend class CDepTree;
	friend class CSisParser;

	// Mandatory fields
	TUint32 iPackageUid;
	TVersion iDepVersion;

	// Optional Fields
	HBufC* iPackageName;
	HBufC* iVendor;
	HBufC* iDateOfSubmission;//the input was a separate day,month, year format. This should've been converted into a string??
	HBufC* iDateOfModification;//the input was a separate day,month, year format. This should've been converted into a string??

	TUint32 iDownloadSize;
	TUint32 iInstalledSize;

	TVersion iVersion;
	HBufC8* iUrl;
	HBufC8* iDownloadUrl;

	HBufC* iDepFileName;
	//HBufC8 iDepUrl;

	HBufC* iChangesFileName;
	HBufC* iSisPackageName;
	PackageStatus iPackageStatus;

	TUint32 iDrivePriority; //priority to set as 'C' drive (for now)
	TChar iInstallDrive; //drive in which the package needs to be installed
	TChar iMandatoryInstallDrive; //mandatory drive specified in the changes file of the package.
	//This is for Edges management in a package
	TUint32 iVisited;

	TDblQue<Edge>     iEdgeDlinkHdr;
	TDblQueIter<Edge> iEdgeDlinkIter;

	//This is the link for added packages connection
	TDblQueLink iPackageDlink;
	};


class CDepTree: public CBase
	{
#ifdef USE_LOGFILE
public:
	static CDepTree* NewL(RFileLogger& aLogger, const TDesC& aDownloadPath);
	static CDepTree* NewLC(RFileLogger& aLogger, const TDesC& aDownloadPath);
private:
	CDepTree(RFileLogger& aLogger);

private:
	RFileLogger& iLog; ///< Not owned
#else

public:
	static CDepTree* NewL(const TDesC& aDownloadPath);
	static CDepTree* NewLC(const TDesC& aDownloadPath);
private:
	CDepTree();
#endif

public:
	~CDepTree();
	TBool SetDriveInfo();
	void GetDriveListL(RFs& aRFs, RArray<TChar>& aDriveLetters, RArray<TInt64>& aDriveSpaces);

	void AddPackageInfo(CPackageInfo *aPackageInfo);
	void AddFetchPackageInfo(CPackageInfo *aPackageInfo);
	CPackageInfo* LocatePackageInDepTree(const TUint32& aDepPackageUid);
	CPackageInfo* GetNextNode();
	CPackageInfo* GetFetchNode( TInt nodeLocation);
	CPackageInfo* GetNextFetchNode();
	CPackageInfo* GetNextSortedNode();
	CPackageInfo* GetPreviousFetchNode();
	inline void SetMachineId(TUint32 machineId)
		{
		iMachineId = machineId;
		}
	inline TUint32 GetMachineId()
		{
		return iMachineId;
		}
	inline CPackageInfo* GetCurrentFetchNode() const
		{
		return iFetchList[iCurrentPackage];
		}

	inline TInt NodesCount() const
		{
		return iFetchList.Count();
		}

	inline TInt CountDownloadNodes() const
		{
		// -1 is because iSortedlist contains also the root package
		// which is not actually downloaded
		if (iSortedList.Count() > 0)
			return iSortedList.Count() - 1;
		else
			return 0;
		}

	inline TUint32 GetTotalDownloadSize() const
		{
		return iTotalDownloadSize;
		}

	inline TUint32 GetMaxDownloadSize() const
		{
		return iMaxDownloadSize;
		}

	TBool ConstructFetchListL();
	TBool IsDepTreeEmpty() const;
	TBool IsCyclePresent() const;
	TPtrC GetDownloadPath() const;
	CPackageInfo* GetRootNode();
	void RemoveDownloadedFiles(RFs& aRfs);
	void DeleteFile(RFs& aRfs, const TDesC& aFileName, const TDesC& aFilePath );

private:
	void ConstructL(const TDesC& aDownloadPath);

private:
	friend class CPackageInfo;

	HBufC* iDownloadPath;
	RPointerArray<CPackageInfo> iFetchList; //Fetch list - the order in which the packages need to be fetched.
	//Cannot sort Fetchlist, as that would collapse the fetch order.
	RPointerArray<CPackageInfo> iSortedList; //The sorted fetch list based on drive priority

	TInt iCurrentPackage;
	TInt iSortedPackage;
	TBool iCyclePresent;

	// This is for added Packages in a Dep Tree
	TDblQue<CPackageInfo> iPackageDlinkHdr;
	// This is used for traversal of the tree to get the next pkg to be updated
	TDblQueIter<CPackageInfo> iPackageDlinkFetchIter;
	//Include the device info.

	TUint32 iTotalDownloadSize;
	TUint32 iMaxDownloadSize;
	TUint32 iMachineId;
	TChar iDownloadDrive; //drive in which the package needs to be downloaded
	};

#endif //__PACKAGEINFO_H__

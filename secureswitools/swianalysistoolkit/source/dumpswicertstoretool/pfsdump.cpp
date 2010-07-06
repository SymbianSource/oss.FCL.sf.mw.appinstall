/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Copy of pfsdump.h from syslibs code (syslibs/development/common/syslibs/store/HTOOLS/).Added CheckFileValidity() method.
*
*/


#include "pfsdump.h"

// Class StoreFile
int StoreFile::OutWidth;

StoreFile::StoreFile()
	{
	}

bool StoreFile::CheckFileValidity(char const* aFile,const Options& aOptions)
	{
	DumpSWICertstoreTool Tool;                  
	iFile.open(aFile, ios::binary);
	if(!iFile)
		{
		throw ECannotOpenFile;
		}
	else
		{
		unsigned long uid;
		iFile.read((char*)&uid,sizeof(uid));
		iFile.seekg(0,ios::end);
		int size = iFile.tellg();
		if(size <= 0)
			{
			if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
				{
				cout << aFile <<" : File is Empty" << endl << endl;
				}
			return false;
			}

		if (size<FrameDes::First || uid!=PermanentFileStoreUid)
			{
			if(aOptions.CBasedWritableCertstore() || aOptions.ROMCertstore())
				{
				if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
					{
					cout << aFile <<": Is not a dat file \nor is a corrupt file" << endl << endl;
					}
				return false;
				}
			}

		else
			{
			if(aOptions.CBasedWritableCertstore() || aOptions.ROMCertstore())
				{
				if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
					{
					cout << aFile <<":" << endl;
					}
				}
			}

		iSize = size;
		int width = 0;
		while (size)
			{
			++width;
			size >>= 4;
			}
		OutWidth = width;
		iFile.seekg(Header::Offset,ios::beg);
		iFile.read((char*)&iHeader,Header::Size);
		if (Empty())
			{
			return false;
			}
		LoadFrames();
		LoadToc();
		}
	return true;
	}

StoreFile::~StoreFile()
	{
	iFile.close();
	}

void StoreFile::LoadFrames()
	{
	FrameDes frame;
	int offset = FrameDes::First;
	int full = FrameDes::First+FrameDes::Interval;
	int diff = FrameDes::First;
	while (offset-FrameDes::Size<iSize)
		{
		if (offset==full)
			{
			full+=FrameDes::Interval;
			diff+=FrameDes::Size;
			}
		if (offset>iSize)
			{
			DumpSWICertstoreTool::Warning() << "in-complete link at " << FramePos(offset-diff) << endl;
			break;
			}
		iFile.seekg(offset-FrameDes::Size,ios::beg);
		iFile >> frame;
		iFrames.Add(FramePos(offset-diff),frame);
		int length = frame.Length();
		if (length == 0)
			{
			if (full>iSize && offset>iSize)
				{
				DumpSWICertstoreTool::Warning() << "in-complete frame at " << FramePos(offset-diff) << endl;
				}
			offset = full;
			}
		else
			{
			int newoffset = offset+length+FrameDes::Size;
			if (newoffset>=full || newoffset-FrameDes::Size>iSize)
				{
				DumpSWICertstoreTool::Error() << "bad link at " << FramePos(offset-diff) << ", skipping to next anchor link" << endl;
				offset=full;
				}
			else
				{
				offset = newoffset;
				if (full-offset<=FrameDes::Size)
					{
					offset=full;
					}
				}
			}
		}
	iFrames.Complete();
	}

void StoreFile::LoadToc()
	{
	FramePos toc=iHeader.Toc();
	Stream stream(iFrames,toc);
	if (!stream.IsGood())
		{
		DumpSWICertstoreTool::Error() << "invalid toc address " << toc << endl;
		return;
		}
	if (stream.Type()!=FrameDes::Toc)
		{
		DumpSWICertstoreTool::Error() << "toc address " << toc << ": refers to non-toc frame"<< endl;
		return;
		}
	// find the requested store revision
	Frames::Iterator f=stream.Frame();
	Frames::Iterator const first=iFrames.Begin();
	for (int rev=Tool.TocRevision();rev;--rev)
		{
		do{
			if (--f<first)
				{
				throw ENotAPermanentFileStore;
				}
			} while (f->iDes.Type()!=FrameDes::Toc);
		}
	iToc.Load(iFile,iFrames,f,iHeader.GetReloc());

	// verify the Toc stream references
	Toc::Iterator const end=iToc.End();
	for (Toc::Iterator iter=iToc.Begin();iter<end;++iter)
		{
		if (iter->iHandle.IsNull())
			{
			DumpSWICertstoreTool::Error() << "missing entry in toc-delta for index " << (1+iter-iToc.Begin()) << endl;
			}
		else if (!iter->iHandle.Avail() && iter->Pos().Pos()>=0)
			{
			f=iFrames.Find(iter->Pos());
			if (f==NULL)
				{
				DumpSWICertstoreTool::Error() << "invalid stream reference in toc entry " << iter->iHandle << endl;
				}
			else if (iter->Pos().Pos()>=toc.Pos())
				{
				DumpSWICertstoreTool::Error() << "virtual stream reference in toc entry " << iter->iHandle << endl;
				}
			else if (f->iDes.Type()!=FrameDes::Data)
				{
				DumpSWICertstoreTool::Error() << "toc entry " << iter->iHandle << ": refers to non-data frame" << endl;
				}
			}
		}
	}

Stream StoreFile::RootStream()
	{
	Toc::Head const& head=iToc.Header();
	Handle hndl = static_cast<Handle>(head.Root());
	return FindStream(hndl);
	}

Stream StoreFile::FindStream(Handle& aHandle)
	{
	Toc::Iterator const end=iToc.End();
	Toc::Head const& head=iToc.Header();
	for (Toc::Iterator iter=iToc.Begin();iter<end;++iter)
		{
		if (!iter->iHandle.Avail())
			{
			if (iter->Pos().Pos() == -1)
				{
				continue;
				}
			if (iter->iHandle == aHandle)
				{
				return Stream(iFrames, iter->Pos());
				}
			}
		}
	throw ENotAPermanentFileStore;
	}

ifstream& StoreFile::File() 
	{
	return iFile;
	}

// Class Toc
Toc::Toc()
	: iPos(0), iRep(NULL), iAvail(0)
	{
	memset(&iHeader,0,sizeof(iHeader));
	}

Toc::~Toc()
	{
	free(iRep);
	}

void Toc::Base(const char* aPtr,int aCount)
	{
	Entry* e=iRep;
	for (int i=1;i<=aCount;++e,++i)
		{
		e->iHandle=i;	// set the index part
		memcpy((char*)e+Entry::BaseRedundant,aPtr,Entry::BaseSize);
		aPtr+=Entry::BaseSize;
		}
	}

void Toc::Load(istream& aStream,Frames const& aFrames,Frames::Iterator aFrame,Header::Reloc const* aReloc)
	{
	iPos = aFrame->iPos;
	Stream toc1(aFrame);
	void* toc = toc1.Load(aStream);
	const char* p = reinterpret_cast<char*>(toc);
	memcpy(&iHeader,p,Head::Size);
	p+=Head::Size;
	int n = iHeader.iCount;
	if (n < 0)
		{
		memset(&iHeader,0,Head::Size);
		DumpSWICertstoreTool::Error() << "corrupt toc" << endl;
		return;
		}
	iRep=static_cast<Entry*>(malloc(n*sizeof(Entry)));
	if (iRep==NULL)
		{
		throw ENotAPermanentFileStore;
		}
	if (iHeader.IsDelta())
		{
		// verify the delta header
		memcpy(&iDelta,p,DeltaHead::Size);
		p+=DeltaHead::Size;
		int dn = iDelta.iCount;
		if (toc1.Length() != Head::Size + DeltaHead::Size + dn * Entry::DeltaSize)
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "in-complete toc" << endl;
			return;
			}
		// find the toc-base
		FramePos tocbase(iDelta.iBase + Header::tocoffset);
		if (aReloc && aReloc->iHandle.IsTocBase())
			{
			tocbase = aReloc->iPos;
			}
		Stream toc2(aFrames,tocbase);
		if (!toc2.IsGood())
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "invalid toc-base address " << tocbase << endl;
			return;
			}
		if (toc2.Type()!=FrameDes::Toc)
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "toc-base address " << tocbase << ": refers to non-toc frame"<< endl;
			return;
			}
		// validate and load the toc-base
		void* tocb = toc2.Load(aStream);
		const char* p2 = reinterpret_cast<char*>(tocb);
		Head headbase;
		memcpy(&headbase,p2,Head::Size);
		p2+=Head::Size;
		if (headbase.IsDelta())
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "toc-base is a toc-delta"<< endl;
			return;
			}
		int bn = headbase.iCount;
		if (bn > n)
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "toc-base is larger than toc"<< endl;
			return;
			}
		Base(p2,bn);
		free(tocb);
		// validate and update with the toc-delta
		int last = 0;
		while (--dn>=0)
			{
			Entry e;
			memcpy(&e,p,Entry::DeltaSize);
			p+=Entry::DeltaSize;
			int ix = e.iHandle.Index();
			if (ix<=0 || ix > n)
				{
				memset(&iHeader,0,Head::Size);
				DumpSWICertstoreTool::Error() << "toc-delta entry " << e.iHandle << " is outside toc"<< endl;
				return;
				}
			if (ix <= last)
				{
				memset(&iHeader,0,Head::Size);
				DumpSWICertstoreTool::Error() << "toc-delta entry " << e.iHandle << " is out of order"<< endl;
				return;
				}
			iRep[ix-1] = e;
			last = ix;
			}
		}
	else
		{
		if (toc1.Length() != Head::Size + n * Entry::BaseSize)
			{
			memset(&iHeader,0,Head::Size);
			DumpSWICertstoreTool::Error() << "in-complete toc" << endl;
			return;
			}
		Base(p,n);
		}
	free(toc);

	// apply the relocation
	if (aReloc && !aReloc->iHandle.IsTocBase())
		{
		int ix=aReloc->iHandle.Index();
		if (ix<=0 || ix>n)
			{
			throw ENotAPermanentFileStore;
			}
		Entry& e=iRep[ix-1];
		if (e.iHandle.Generation()!=aReloc->iHandle.Generation())
			{
			throw ENotAPermanentFileStore;
			}
		e.iPos=aReloc->iPos.Pos();
		}
	// count the available entries
	int avail=0;
	for (int i=0;i<n;++i)
		{
		if (iRep[i].iHandle.Avail())
		++avail;
		}
	iAvail=avail;
	// verify the available list
	Handle link=iHeader.iAvail;
	if (!link.IsNull())
		{
		int ix=link.Index();
		if (!link.Avail() || ix<=0 || ix >iHeader.iCount)
			{
			DumpSWICertstoreTool::Error() << "corrupt available link in toc header " << link << endl;
			return;
			}
		Entry const* en=&(*this)[ix];
		if (en->iHandle!=link)
			{
			DumpSWICertstoreTool::Error() << "corrupt available link in toc header " << link << endl;
			return;
			}
		for (;;)
			{
			if (--avail<0)
				{
				DumpSWICertstoreTool::Error() << "corrupt available list, possible circular reference" << endl;
				return;
				}
			Handle next=en->Link();
			if (next.IsNull())
				{
				break;
				}
			ix=next.Index();
			if (!next.Avail() || ix<=0 || ix >iHeader.iCount)
				{
				DumpSWICertstoreTool::Error() << "corrupt link in toc entry " << link << endl;
				return;
				}
			en=&(*this)[ix];
			if (en->iHandle!=next)
				{
				DumpSWICertstoreTool::Error() << "corrupt link in toc entry " << link << endl;
				return;
				}
			link=next;
			}
		}
	if (avail!=0)
		{
		DumpSWICertstoreTool::Error() << "corrupt available list: free index leakage" << endl;
		}
	}

ostream& operator<<(ostream& aStream,Toc const& aToc)
	{
	Toc::Head const& head=aToc.iHeader;
	aStream << "Toc at " << aToc.iPos << " with ";
	aStream << dec << head.iCount << (head.iCount==1 ? " entry: " : " entries: ") \
		<< head.iCount-aToc.iAvail  << " allocated, " << aToc.iAvail << " free\n" << hex;
	return aStream << flush;
	}

// Class Stream
int Stream::Length() const
	{
	int total=0;
	Frames::Iterator f=iFrame;
	do	{
		int len=f->iDes.Length();
		if (len==0)
		len=f[1].iPos.Pos()-f[0].iPos.Pos();
		total+=len;
		} while ((++f)->iDes.Type()==FrameDes::Continuation);
	return total;
	}

void* Stream::Load(istream& aStream) const
	{
	int size = Length();
	void* data = malloc(size);
	if (data==NULL)
		{
		throw ENotAPermanentFileStore;
		}
	char* read=reinterpret_cast<char*>(data);
	Frames::Iterator f = iFrame;
	do
		{
		FramePos pos=f->iPos;
		int len=f++->iDes.Length();
		if (len==0)
		{
		len=f->iPos.Pos()-pos.Pos();
		}
		aStream.seekg(FileOffset(pos).Offset(),ios::beg);
		aStream.read(read,len);
		read+=len;
		} while (f->iDes.Type()==FrameDes::Continuation);
	return data;
	}

// Class Frames
Frames::Frames()
	: iSize(0),iElements(0),iRep(NULL)
	{}

Frames::~Frames()
	{
	free(iRep);
	}

void Frames::Add(FramePos aPos,FrameDes aDes)
	{
	if (iElements==iSize)
		{
		iSize=iSize==0 ? 128 : iSize+iSize;
		void* rep=realloc(iRep,iSize*sizeof(*iRep));
		if (rep==NULL)
			{
			throw ENotAPermanentFileStore;
			}
		iRep=(Element*)rep;
		}
	Element& element=iRep[iElements++];
	element.iPos=aPos;
	element.iDes=aDes;
	}

void Frames::Complete()
// add a terminating entry
	{
	Add(0,0);
	--iElements;
	}

Frames::Iterator Frames::Find(FramePos aPos) const
	{
	return (Element const*)bsearch(&aPos,iRep,iElements,sizeof(*iRep),Compare);
	}

int Frames::Compare(void const* aLeft,void const* aRight)
	{
	int left=static_cast<FramePos const*>(aLeft)->Pos();
	int right=static_cast<Element const*>(aRight)->iPos.Pos();
	if (left<right)
		{
		return -1;
		}
	if (left>right)
		{
		return 1;
		}
	return 0;
	}

// Header
FramePos Header::Toc() const
	{
	return tocoffset+(!Dirty() && iToc.iZero==0 ? iToc.iPos : iBackupToc>>backupshift);
	}

Header::Reloc const* Header::GetReloc() const
	{
	return (Dirty() || iToc.iZero==0) ? NULL : reinterpret_cast<Reloc const*>(&iReloc);
	}

ostream& operator<<(ostream& aStream,Header const& aHeader)
	{
	aStream << "Header is " << (aHeader.Dirty() ? "dirty" : "clean");
	Header::Reloc const* reloc=aHeader.GetReloc();
	if (reloc!=NULL)
		{
		aStream << "\npending relocation of ";
		if (reloc->iHandle.IsTocBase())
			{
			aStream << "toc-base";
			}
		else
			{
			aStream << "stream " << StreamId(reloc->iHandle);
			}
		aStream << " to " << reloc->iPos;
		}
	return aStream << "\n\n" << flush;
	}

// FileOffset
FileOffset::FileOffset(FramePos aPos)
	// calculate the file offset for a streampos
	{
	int pos=aPos.Pos();
	int n=pos>>FrameDes::FullShift;
	pos+=FrameDes::Size*n+FrameDes::First;
	iValue=pos;
	}

FileOffset::operator FramePos() const
	{
	int pos=iValue-FrameDes::First;
	int n=pos/FrameDes::Interval;
	pos-=n*FrameDes::Size;
	return FramePos(pos);
	}

ostream& operator<<(ostream& aStream,FileOffset anOffset)
	{
	return aStream << setw(StoreFile::OutWidth) << anOffset.iValue;
	}

// Handle
ostream& operator<<(ostream& aStream,Handle aHandle)
	{
	if (aHandle.IsNull())
		{
		aStream << "Null";
		}
	else
		{
		aStream << setw(6) << aHandle.Index() << ':' << aHandle.Generation();
		}
	return aStream;
	}

// FramePos
ostream& operator<<(ostream& aStream,FramePos aPos)
	{
	return aStream << setw(StoreFile::OutWidth) << aPos.iValue << '[' << FileOffset(aPos) << ']';
	}

// FrameDes
istream& operator>>(istream& aStream,FrameDes& aFrame)
	{
	return aStream.read((char*)&aFrame,FrameDes::Size);
	}

ostream& operator<<(ostream& aStream,FrameDes aFrame)
	{
	static char const* FrameType[]={"free","data","toc","continuation"};
	aStream << FrameType[aFrame.Type()] << " (";
	int length=aFrame.Length();
	if (length==0)
		{
		aStream << "full";
		}
	else
		{
		aStream << dec << length << hex;
		}
	return aStream << ')';
	}


@rem
@rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@echo off

echo This script creates SIS files for SIF integration tests

set PLATFORM=%1
set CFG=%2

set BUILDDIR=\epoc32\release\%PLATFORM%\%CFG%
set PKGDIR=%SECURITYSOURCEDIR%\installationservices\swinstallationfw\test\tusif\scripts\data
set DATADIR=%BUILDDIR%\z\tusif\tsif\data
set ARMV5DATADIR=\epoc32\data\z\tusif\tsif\data
set CERTDIR=%SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\usif

set BASE=sifintegrationtestbase
set UPGRADE=sifintegrationtestupgrade
set INVALIDUPGRADE=sifintegrationtestinvalidupgrade
set NOCAPSBASE=sifintegrationtestnocaps
set INVALIDINSTALL=sifselfsignedwithprotecteduid
set USERCAPSBASE=sifintegusercapsbase
set COMPONENTINFOINTEGRATION=sifcomponentinfointegration
set COMPONENTINFOINTEGRATIONPU=sifcomponentinfointegpu
set COMPONENTINFOINTEGRATIONSP=sifcomponentinfointegsp
set EMBEDDINGOUTERBASE=embeddingouterbase
set EMBEDDEDINNERBASE1=embeddedinnerbase1
set EMBEDDEDINNERBASE11=embeddedinnerbase11
set EMBEDDEDINNERBASE2=embeddedinnerbase2
set EMPTYNAMEANDVENDOR=emptynameandvendor
set EMBEDDINGBASEWITHEXE=embeddingbasewithexe
set EMBEDDEDBASEWITHUSERCAPSEXE=embeddedbasewithusercapsexe
set EMBEDDEDBASEWITHEXE=sifintegrationtestbase
set EMBEDDINGBASEWITHTEXT=embeddingbasewithtextandinnerwithexe
set COMPONENTINFOWITHMULTIPLEFILES=sifintegrationtestmultiplefiles
set COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE=sifintegrationtestextensionindifferentcase
set COMPONENTINFOWITHONLYDLL=sifintegrationtestwithonlydll
set SUFFICIENTUSERCAPS=sifintegsufficientusercaps
set DUMMYTEXT=dummy

set CREATESISCMD=createsis create -pass usif
set CREATESISEXISTINGCERTCMD=createsis create -key %CERTDIR%\root5ca_key.pem -cert %CERTDIR%\root5ca.pem

rem Check if SIS files already exist
if not exist %DATADIR%\%BASE%.sis goto generate_sis_files
if not exist %DATADIR%\%UPGRADE%.sis goto generate_sis_files
if not exist %DATADIR%\%INVALIDUPGRADE%.sis goto generate_sis_files
if not exist %DATADIR%\%NOCAPSBASE%.sis goto generate_sis_files
if not exist %DATADIR%\%INVALIDINSTALL%.sis goto generate_sis_files
if not exist %DATADIR%\%USERCAPSBASE%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOINTEGRATION%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOINTEGRATIONPU%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOINTEGRATIONSP%.sis goto generate_sis_files
if not exist %DATADIR%\%EMBEDDINGOUTERBASE%.sis goto generate_sis_files
if not exist %DATADIR%\%EMPTYNAMEANDVENDOR%.sis goto generate_sis_files
if not exist %DATADIR%\%EMBEDDINGBASEWITHEXE%.sis goto generate_sis_files
if not exist %DATADIR%\%EMBEDDEDBASEWITHUSERCAPSEXE%.sis goto generate_sis_files
if not exist %DATADIR%\%EMBEDDEDBASEWITHEXE%.sis goto generate_sis_files
if not exist %DATADIR%\%EMBEDDINGBASEWITHTEXT%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOWITHMULTIPLEFILES%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.sis goto generate_sis_files
if not exist %DATADIR%\%COMPONENTINFOWITHONLYDLL%.sis goto generate_sis_files
if not exist %DATADIR%\%SUFFICIENTUSERCAPS%.sis goto generate_sis_files

goto:eof

:generate_sis_files

rem Copy pkg files for createsis.exe
copy /y %PKGDIR%\%BASE%.pkg %BUILDDIR%\%BASE%.pkg
copy /y %PKGDIR%\%UPGRADE%.pkg %BUILDDIR%\%UPGRADE%.pkg
copy /y %PKGDIR%\%INVALIDUPGRADE%.pkg %BUILDDIR%\%INVALIDUPGRADE%.pkg
copy /y %PKGDIR%\%NOCAPSBASE%.pkg %BUILDDIR%\%NOCAPSBASE%.pkg
copy /y %PKGDIR%\%INVALIDINSTALL%.pkg %BUILDDIR%\%INVALIDINSTALL%.pkg
copy /y %PKGDIR%\%USERCAPSBASE%.pkg %BUILDDIR%\%USERCAPSBASE%.pkg
copy /y %PKGDIR%\%EMBEDDINGBASEWITHEXE%.pkg %BUILDDIR%\%EMBEDDINGBASEWITHEXE%.pkg
copy /y %PKGDIR%\%EMBEDDEDBASEWITHUSERCAPSEXE%.pkg %BUILDDIR%\%EMBEDDEDBASEWITHUSERCAPSEXE%.pkg
copy /y %PKGDIR%\%EMBEDDEDBASEWITHEXE%.pkg %BUILDDIR%\%EMBEDDEDBASEWITHEXE%.pkg
copy /y %PKGDIR%\%DUMMYTEXT%.txt %BUILDDIR%\%DUMMYTEXT%.txt
copy /y %PKGDIR%\%EMBEDDINGBASEWITHTEXT%.pkg %BUILDDIR%\%EMBEDDINGBASEWITHTEXT%.pkg

copy /y %PKGDIR%\%COMPONENTINFOWITHMULTIPLEFILES%.pkg %BUILDDIR%\%COMPONENTINFOWITHMULTIPLEFILES%.pkg
copy /y %PKGDIR%\%COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.pkg %BUILDDIR%\%COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.pkg
copy /y %PKGDIR%\%COMPONENTINFOWITHONLYDLL%.pkg %BUILDDIR%\%COMPONENTINFOWITHONLYDLL%.pkg
copy /y %PKGDIR%\%SUFFICIENTUSERCAPS%.pkg %BUILDDIR%\%SUFFICIENTUSERCAPS%.pkg

pushd %PKGDIR%
rem These package uses local files in the package directory
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOINTEGRATION%.pkg
move %COMPONENTINFOINTEGRATION%.sis %BUILDDIR%
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOINTEGRATIONPU%.pkg
move %COMPONENTINFOINTEGRATIONPU%.sis %BUILDDIR%
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOINTEGRATIONSP%.pkg
move %COMPONENTINFOINTEGRATIONSP%.sis %BUILDDIR%
call %CREATESISEXISTINGCERTCMD% %EMPTYNAMEANDVENDOR%.pkg
move %EMPTYNAMEANDVENDOR%.sis %BUILDDIR%

rem construct the embedded ackages inner to outer.
call %CREATESISEXISTINGCERTCMD% %EMBEDDEDINNERBASE2%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDEDINNERBASE1%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDEDINNERBASE11%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDINGOUTERBASE%.pkg
move %EMBEDDINGOUTERBASE%.sis %BUILDDIR%

rem Make sure that we leave no certificates after createsis, otherwise checklocationofcertificates test will return an error
del /Q *.cer

popd
rem Go to build directory, as the packages below use dynamically built executables
pushd %BUILDDIR%

rem Generate SIS files for SIF integration tests 
rem We avoid using temporary ceritificates for installed SIS files, as their FROM date may be later than the current date/time in the testing env.
call %CREATESISEXISTINGCERTCMD% %BASE%.pkg
call %CREATESISEXISTINGCERTCMD% %UPGRADE%.pkg
call %CREATESISEXISTINGCERTCMD% %INVALIDUPGRADE%.pkg
call %CREATESISCMD% %NOCAPSBASE%.pkg
call %CREATESISCMD% %INVALIDINSTALL%.pkg
call %CREATESISEXISTINGCERTCMD% %USERCAPSBASE%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDEDBASEWITHUSERCAPSEXE%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDINGBASEWITHEXE%.pkg
call %CREATESISEXISTINGCERTCMD% %EMBEDDINGBASEWITHTEXT%.pkg
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOWITHMULTIPLEFILES%.pkg
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.pkg
call %CREATESISEXISTINGCERTCMD% %COMPONENTINFOWITHONLYDLL%.pkg
call %CREATESISEXISTINGCERTCMD% %SUFFICIENTUSERCAPS%.pkg

rem Dynamically built executables are removed
del sifintegsufficientusercaps.exe
del sifintegusercapsbasepackage.exe

rem Copy generated SIS files onto the Z drive
if /i %PLATFORM% neq ARMV5 (
copy /y %BASE%.sis %DATADIR%\%BASE%.sis
copy /y %UPGRADE%.sis %DATADIR%\%UPGRADE%.sis
copy /y %INVALIDUPGRADE%.sis %DATADIR%\%INVALIDUPGRADE%.sis
copy /y %NOCAPSBASE%.sis %DATADIR%\%NOCAPSBASE%.sis
copy /y %INVALIDINSTALL%.sis %DATADIR%\%INVALIDINSTALL%.sis
copy /y %USERCAPSBASE%.sis %DATADIR%\%USERCAPSBASE%.sis
copy /y %COMPONENTINFOINTEGRATION%.sis %DATADIR%\%COMPONENTINFOINTEGRATION%.sis
copy /y %COMPONENTINFOINTEGRATIONPU%.sis %DATADIR%\%COMPONENTINFOINTEGRATIONPU%.sis
copy /y %COMPONENTINFOINTEGRATIONSP%.sis %DATADIR%\%COMPONENTINFOINTEGRATIONSP%.sis
copy /y %EMBEDDINGOUTERBASE%.sis %DATADIR%\%EMBEDDINGOUTERBASE%.sis
copy /y %EMBEDDEDBASEWITHUSERCAPSEXE%.sis %DATADIR%\%EMBEDDEDBASEWITHUSERCAPSEXE%.sis
copy /y %EMPTYNAMEANDVENDOR%.sis %DATADIR%\%EMPTYNAMEANDVENDOR%.sis
copy /y %EMBEDDINGBASEWITHEXE%.sis %DATADIR%\%EMBEDDINGBASEWITHEXE%.sis
copy /y %EMBEDDINGBASEWITHTEXT%.sis %DATADIR%\%EMBEDDINGBASEWITHTEXT%.sis
copy /y %COMPONENTINFOWITHMULTIPLEFILES%.sis %DATADIR%\%COMPONENTINFOWITHMULTIPLEFILES%.sis
copy /y %COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.sis %DATADIR%\%COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.sis
copy /y %COMPONENTINFOWITHONLYDLL%.sis %DATADIR%\%COMPONENTINFOWITHONLYDLL%.sis
copy /y %SUFFICIENTUSERCAPS%.sis %DATADIR%\%SUFFICIENTUSERCAPS%.sis
) else (
mkdir %ARMV5DATADIR%
copy /y %BASE%.sis %ARMV5DATADIR%\%BASE%.sis
copy /y %UPGRADE%.sis %ARMV5DATADIR%\%UPGRADE%.sis
copy /y %INVALIDUPGRADE%.sis %ARMV5DATADIR%\%INVALIDUPGRADE%.sis
copy /y %NOCAPSBASE%.sis %ARMV5DATADIR%\%NOCAPSBASE%.sis
copy /y %INVALIDINSTALL%.sis %ARMV5DATADIR%\%INVALIDINSTALL%.sis
copy /y %USERCAPSBASE%.sis %ARMV5DATADIR%\%USERCAPSBASE%.sis
copy /y %COMPONENTINFOINTEGRATION%.sis %DATADIR%\%COMPONENTINFOINTEGRATION%.sis
copy /y %COMPONENTINFOINTEGRATIONPU%.sis %ARMV5DATADIR%\%COMPONENTINFOINTEGRATIONPU%.sis
copy /y %COMPONENTINFOINTEGRATIONSP%.sis %ARMV5DATADIR%\%COMPONENTINFOINTEGRATIONSP%.sis
copy /y %EMBEDDEDBASEWITHUSERCAPSEXE%.sis %DATADIR%\%EMBEDDEDBASEWITHUSERCAPSEXE%.sis
copy /y %EMBEDDINGOUTERBASE%.sis %DATADIR%\%EMBEDDINGOUTERBASE%.sis
copy /y %EMPTYNAMEANDVENDOR%.sis %DATADIR%\%EMPTYNAMEANDVENDOR%.sis
copy /y %EMBEDDINGBASEWITHEXE%.sis %DATADIR%\%EMBEDDINGBASEWITHEXE%.sis
copy /y %EMBEDDINGBASEWITHTEXT%.sis %DATADIR%\%EMBEDDINGBASEWITHTEXT%.sis
copy /y %COMPONENTINFOWITHMULTIPLEFILES%.sis %DATADIR%\%COMPONENTINFOWITHMULTIPLEFILES%.sis
copy /y %COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.sis %DATADIR%\%COMPONENTINFOWITHEXTENSIONINDIFFERENTCASE%.sis
copy /y %COMPONENTINFOWITHONLYDLL%.sis %DATADIR%\%COMPONENTINFOWITHONLYDLL%.sis
copy /y %SUFFICIENTUSERCAPS%.sis %DATADIR%\%SUFFICIENTUSERCAPS%.sis
)

popd

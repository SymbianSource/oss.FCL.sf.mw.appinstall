#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
# Test tool used to copy signed SISX files to EPOC tree
#

use warnings;
use strict;
use Getopt::Long;

my $SIS_DIR = "sis";
my $PLATFORM = '';

&main;
exit(0);

sub main() {
	GetOptions('platform=s' => \$PLATFORM,
			   'sis-dir=s' => \$SIS_DIR);
	
	# Finally copy signed SIS file to EPOC path  
	my $srcMask = "${SIS_DIR}\\${PLATFORM}\\*.sis";
	my $targetPath = "";
	if ( uc($PLATFORM) eq "WINSCW" ) {
		$targetPath = "\\epoc32\\winscw\\c\\tswi\\tdevcerts\\data\\";
	}
	elsif ( uc($PLATFORM) eq "ARMV5" ) {
		$targetPath = "\\epoc32\\winscw\\c\\tswi\\tdevcerts\\data\\armv5\\";
	}
	else {
		if ( $PLATFORM ne "" ) {
			print "Nothing to copy for platform $PLATFORM...";
		}
		else {
			print "PLATFORM is not unspecified ...";
		}
		exit(-1);
	}
 
	if ( $targetPath ne "" )
	{
		mkdir("${targetPath}");
		system "xcopy", "/Y", "/E", $srcMask, $targetPath;
	}
}

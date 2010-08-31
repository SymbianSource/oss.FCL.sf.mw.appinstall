#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This script belongs to the SIF Reference Installer. It builds a binary reference package that
# contains a pkg file describing the (base) component to be installed and (optionally) embedded
# components.
# The structure of an output file is as follows:
# 36 bytes				header
# 4 bytes 					the size of a pkg file describing the component (pkgSize)
# pkgSize bytes				the content of a pkg file describing the component
# 4 bytes					the number of embedded components in a package (numEmbComps)
# 4 bytes						the length of the name of an embedded component (embCompFileNameLen)   \
# embCompFileNameLen bytes	the name of an embedded component								       } this block repeats numEmbComps times
# 4 bytes					the size of an embedded component (embCompFileSize)				     /
# embCompFileSize bytes		the content of an embedded component								  /
# 45 bytes - footer
# 
#

use Getopt::Long;
use File::Basename;

my $KCopyChunkSize = 65536;

my $header = '_SifReferenceInstallerPackageHeader_';
my $footer = '_SifReferenceInstallerPackageFooter_';

# Read and check arguments
my $help = "\nExample usage:\nmakebinpkg.pl --pkg base.sifrefpkg --emb simple.sis --binpkg compound.sifrefbinpkg\n";
my $inPkg = '';
my @embComps = ();
my $outPkg = '';
GetOptions("pkg=s" => \$inPkg, "emb=s" => \@embComps, "binpkg=s" => \$outPkg) or die "GetOptions failed.\n$help";
$outPkg or die "Output file not defined\n$help";
my $numEmbComps = @embComps;
$numEmbComps <= 16 or die "Too many embedded components.\n$help";

# Create the output file
unlink($outPkg);
open OUTFILE, ">$outPkg" or die "\nCan't open $outPkg for writing: $!\n";
binmode OUTFILE;

# Write the header
print OUTFILE $header;

# Write the size of the input pkg file
my $inPkgFileSize = -s $inPkg;
print "Input pkg file: $inPkg, size: $inPkgFileSize\n";
print OUTFILE pack("i", $inPkgFileSize);

# Write the content of the input pkg file
open INPKGFILE, $inPkg or die "\nCan't open $inPkg file for reading: $!\n";
binmode INPKGFILE;
my $buffer;
while (read (INPKGFILE, $buffer, $KCopyChunkSize) and print OUTFILE $buffer) # read in (up to) 64k chunks, write, exit if read or write fails
	{}; die "Problem copying: $!\n" if $!;
close INPKGFILE;

# Write the number of input files
print OUTFILE pack("i", $numEmbComps);

# Iterate over the embedded components adding them into the output file
foreach (@embComps)
	{
	my $baseName = basename($_);
	
	# Write the length of the file name
	print "Processing file: $_";
	print OUTFILE pack("i", length($baseName));
	
	# Write the file name
	print OUTFILE $baseName;
	
	# Write the size of the file
	my $embCompFileSize = -s $_;
	print ", size: $embCompFileSize\n";
	print OUTFILE pack("i", $embCompFileSize);
	
	# Write the content of the file
	open EMBFILE, $_ or die "\nCan't open $srcfile for reading: $!\n";
	binmode EMBFILE;
	my $buffer;
	while ( read (EMBFILE, $buffer, $KCopyChunkSize) and print OUTFILE $buffer) # read in (up to) 64k chunks, write, exit if read or write fails
		{}; die "Problem copying: $!\n" if $!;
	close EMBFILE;
	}

# Write the footer and close the output file
print OUTFILE $footer;
close OUTFILE;

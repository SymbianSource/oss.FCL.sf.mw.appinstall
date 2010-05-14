#
# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Package contains routines to handle the parsing of the test list
#

package testlistreader;
use strict;
use File::Copy;
my $Path = "\\epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\";
my $domPath =  $Path."xml";
my $runType = 0;
mkdir($domPath);
copy("/epoc32/tools/migrationtool/xml/dom.pm" , "$domPath");

sub new {
    my $proto = shift;
    my $controller = shift;
	$runType = shift;
    my $class = ref($proto) || $proto;
    my $self  = {};
    $self->{'verbose'} = 0;
    $self->{'currentTag'} = "";
    $self->{'controller'} = $controller;
	$self->{'runType'} = $runType;
    bless ($self, $class);
    return $self;
}

# utility function to extract text associated with an XML tag
sub getText {
    my ($self, $node, $tag) = @_;

    $self->{'currentTag'} = $tag;
    if ($node->getElementsByTagName($tag)->getLength > 0)
    {
	my $text = $node->getElementsByTagName($tag)->item(0)->getFirstChild->getNodeValue;
	return $text;
    }
    else
    {
	return undef;
    }
}

sub parseFiles {
    my ($self, $specialFile, @normalFiles) = @_;
    my $controller = $self->{'controller'};
    my $firstDone = 0;
    foreach my $file (@normalFiles)
    {
	my $base = 0;
	if ($firstDone)
	{
	    $base = 0;
	}
	else
	{
	    $base = 1;
	}
	$firstDone = 0;
	my $baseDir = $self->parseFile($file, $base);
	$controller->setBaseDir($baseDir);
    }
    if (length($specialFile) > 0)
    {
	my @order = $self->parseOnlyTests($specialFile);
	$controller->setOrder(\@order);
    }
}


sub parseFile {
    my ($self, $filename) = @_;
    my $parser= XML::DOM::Parser->new();
    my $doc = $parser->parsefile($filename);
    my $controller = $self->{'controller'};
    my $baseDir="";

    my $getBase = 0;

    #the xml file must have a <testlist> tag
    my $root = $self->getText($doc, 'testlist');
    die "Missing tag <".$self->{'currentTag'}."> in $filename\n" if (!(defined $root));

    my $testCount = 0;
    foreach my $test ($doc->getElementsByTagName('test')) {
	my %testEntry;
	$testCount++;

	# the test element must have an id attribute
	my $testid = $test->getAttribute('id');
	die "Must have the id attribute for test $testCount in file\n" if (length($testid) ==0);
	
	my $testtype = $test->getAttribute('type');
	$testEntry{'id'} = $testid;

	#add all the other elements in the test element to the current test entry
	foreach my $elem ($test->getChildNodes)
	{
	    if ($elem->getNodeType == 1)
	    {
		#an element
		my $key = $elem->getNodeName;
		my $value = undef;
		if (length($elem->getFirstChild) > 0)
		{
			$value = $elem->getFirstChild->getNodeValue;
		}
		$testEntry{$key} = $value;
		#some of the elements have attributes. These also need to be 
		#added to the current test entry

		if ($key eq "command")
		{
		    $testEntry{'command_dir'} = $elem->getAttribute('dir');
		    my $log = $elem->getAttribute('log');
		    if (length($log))
		    {
			if (lc($log) =~/yes/)
			{
			    $testEntry{'log'} = 1;
			}
			else
			{
			    $testEntry{'log'} = 0;
			}
		    }
		    else
		    {
			$testEntry{'log'} = 1;
		    }
		}
		elsif ($key eq "check")
		{

		    my $c_type = $elem->getAttribute('type');
		    $testEntry{'check_type'} = $c_type if (length($c_type));
		  
		    my $c_prog = $elem->getAttribute('name');
		    $testEntry{'check_prog'} = $c_prog if (length($c_prog));
		    # check if base results directory is required for checking
		    # the results
		    if (exists($testEntry{'check_type'}))
		    {
			$getBase = 1 if ($testEntry{'check_type'} eq "diff" );
		    }
		}
	    }
	}

	#now check that all the mandatory elements and attribute exist
	if ($getBase)
	{
	    $baseDir = $self->getText($doc, 'baseResultDir');
	    die "Missing tag <".$self->{'currentTag'}."> in $filename\n" if (!(defined $baseDir));
	}
	die "Empty or missing 'title' tag for test " . $testEntry{'id'} . "\n" if (length($testEntry{'title'}) == 0);
	die "Empty or missing 'command' tag for test " . $testEntry{'id'} . "\n" if (length($testEntry{'command'}) == 0);
	die "Empty or missing 'check' tag with 'type' attribute for test " . $testEntry{'id'} . "\n" if (length($testEntry{'check_type'}) == 0);

	die "Invalid 'type' attribute value (" . $testEntry{'check_type'} . ") for 'check' tag test " . $testEntry{'id'} . "\n" if ($testEntry{'check_type'} ne "diff" && $testEntry{'check_type'} ne "program" && $testEntry{'check_type'} ne "result") ;
	if (length($testtype))
	{
	    $testEntry{'type'} = $testtype;
	}
	else
	{
	    $testEntry{'type'} = 'basic';
	}
	# if the test is for 9.6 & future release and the type is nativeonly
	# do not include it in the entry
	if(nativeOnly($self->{'runType'},$testtype))
		{
		$controller->addTest(\%testEntry);
		}
    }
    return $baseDir;
    $doc->dispose;
}

sub nativeOnly()
	{
	my ($runType,$type) = @_;
	if( ($runType eq "usifnative") && ($type eq "nativeonly") )
		{
		return 0;
		}
	return 1;
	}

# parse a XML file with test references only
sub parseOnlyTests {
    my ($self, $filename) = @_;
    my $controller = $self->{'controller'};
    my $parser= XML::DOM::Parser->new();
    my $doc = $parser->parsefile($filename);
    my @newOrder;

    # file only contains list of tests to be executed
    foreach my $test ($doc->getElementsByTagName('test')) {
	my $testid = $test->getAttribute('id');
	push @newOrder, $testid;
    }
    $doc->dispose;
    return @newOrder;
}

1;

#
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

use strict;
# define default test routine
#
# this test routine is use to run a single test 
# 
sub defaultTestRoutine 
{
    my ($testEntry, $basedir, $failures, $baseResults) = @_;

    #set up filenames to be used for this test
    my $resultFile = "test" . $testEntry->{'id'} . ".log";

    # save current directory in case any ofthe scripts change the current directory
    my $saved_dir = cwd();
    
    # if setup script has been specified for the test then run that first
    if (exists($testEntry->{'setup'}))
    {
	my $comm = $testEntry->{'setup'};
	my $r = `$comm`;
    }

    # change to correct directory to run the test.
    if (exists($testEntry->{'command_dir'}))
    {
	chdir $testEntry->{'command_dir'};
    }

    # run the test script
    my $command = $testEntry->{'command'};
    my $run_command = "$command ";
    # redirect output if logging the results
    $run_command = $run_command . " >> $baseResults" if ($testEntry->{'log'});
    my $res = `$run_command 2>>&1`;
    chdir $saved_dir;

    # run any cleanup scripts for test
    if (exists($testEntry->{'cleanup'}))
    {
	my $comm = $testEntry->{'cleanup'};
	my $r = `$comm`;
    }

    my $result = 0; #failed
    my $test_prefix =  "test " . $testEntry->{'id'} . ": ";
    
    # now determine how the result of the test is to be determined
    if ($testEntry->{'check_type'} eq "result")
    {
	if ($res =~ /Passed$/)
	{
	    $result = 1;
	}
	else
	{
	    $result = 0;
	    push @$failures, $test_prefix . $res;
	}
    }
    elsif ($testEntry->{'check_type'} eq "program")
    {
	if (length($res) == 0)
	{
	    if (exists($testEntry->{'check'}))
	    {
		my $opts = $testEntry->{'check'};
		my $prog = $testEntry->{'check_prog'};
		my $r = `$prog $resultFile $opts`;
		if ($r =~ /^Passed/)
		{
		    $result = 1;
		}
		else
		{
		    push @$failures,$test_prefix . $r;
		}
	    }
	}
	else
	{
	    push @$failures, $test_prefix . " '$run_command' command returned: >$res<";
	}
    }
    else 
    {
	if (length($res) == 0)
	{
	    # perform diff on base results and current log file
	    $res = `diff $baseResults $resultFile 2>&1`;
	    if (length($res) == 0)
	    {
		# passed
		$result = 1;
	    }
	    else
	    {
		my $errStr;
		if ( $res =~/^diff:/)
		{
		    chomp $res;
		    $errStr = $test_prefix . $res;
		}
		else
		{
		    $errStr = $test_prefix . "Results do not match Base results";
		}
		push @$failures, $errStr;
	    }
	}
	else
	{
	    push @$failures, $test_prefix . " '$run_command' command returned: >$res<";
	}
    }
    return $result;
}

# define enhanced test routine
#
# this test routine is use to run a single test. 
# This routine provides additional features above that provided by the default.
# It expects the setup and cleanup scripts to return "Passed" if the script 
# is successful.
# 
sub enhancedTestRoutine 
{
    my ($testEntry, $basedir, $failures) = @_;

    #set up filenames to be used for this test
    my $resultFile = "test" . $testEntry->{'id'} . ".log";
    my $baseResults = "\\epoc32\\winscw\\c\\interpretsis_test_harness.txt";

    # save current directory in case any ofthe scripts change the current directory
    my $saved_dir = cwd();
    
    my $result = 0; #failed
    my $test_prefix =  "test" . $testEntry->{'id'} . ": ";

    # if setup script has been specified for the test then run that first
    my $continue_test = 1;
    if (exists($testEntry->{'setup'}))
    {
	my $comm = $testEntry->{'setup'};
	my $r = `$comm`;
	if ($r !~ /^Passed/)
	{
	    $continue_test = 0;
	    push @$failures, $test_prefix . "Setup of test failed: " . $r;
	}

    }

    if ($continue_test)
    {
	# change to correct directory to run the test.
	if (exists($testEntry->{'command_dir'}))
	{
	    chdir $testEntry->{'command_dir'};
	}

	# run the test script
	my $command = $testEntry->{'command'};
	my $run_command = "$command ";
	# redirect output if logging the results
	$run_command = $run_command . " >> $baseResults" if ($testEntry->{'log'});
	my $res = `$run_command 2>>&1`;
	chdir $saved_dir;

	# now determine how the result of the test is to be determined
	if ($testEntry->{'check_type'} eq "result")
	{
	    if ($res =~ /Passed$/)
	    {
		$result = 1;
	    }
	    else
	    {
		$result = 0;
		push @$failures, $test_prefix . $res;
	    }
	}
	elsif ($testEntry->{'check_type'} eq "program")
	{
	    if (length($res) == 0)
	    {
		if (exists($testEntry->{'check'}))
		{
		    my $opts = $testEntry->{'check'};
		    my $prog = $testEntry->{'check_prog'};
		    my $r = `$prog $resultFile $opts`;
		    if ($r =~ /^Passed/)
		    {
			$result = 1;
		    }
		    else
		    {
			push @$failures,$test_prefix . $r;
		    }
		}
	    }
	    else
	    {
		push @$failures, $test_prefix . " '$run_command' command returned: >$res<";
	    }
	}
	else 
	{
	    if (length($res) == 0)
	    {
		# perform diff on base results and current log file
		$res = `diff $baseResults $resultFile 2>&1`;
		if (length($res) == 0)
		{
		    # passed
		    $result = 1;
		}
		else
		{
		    my $errStr;
		    if ( $res =~/^diff:/)
		    {
			chomp $res;
			$errStr = $test_prefix . $res;
		    }
		    else
		    {
			$errStr = $test_prefix . "Results do not match Base results";
		    }
		    push @$failures, $errStr;
		}
	    }
	    else
	    {
		push @$failures, $test_prefix . " '$run_command' command returned: >$res<";
	    }
	}

	# run any cleanup scripts for test
	if (exists($testEntry->{'cleanup'}))
	{
	    my $comm = $testEntry->{'cleanup'};
	    my $r = `$comm`;
	    if ($r !~ /^Passed/)
	    {
		push @$failures, $test_prefix . "Cleanup of test failed: " . $r;
		$result = 0; # mark the test as failed
	    }
	}
    }
    return $result;
}

1;

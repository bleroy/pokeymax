#!/usr/bin/perl -w
use strict;

my $filename = shift @ARGV;
my $hashcode = shift @ARGV;

my $dechashcode = hex(substr($hashcode,2,2).substr($hashcode,0,2));

open FH,"+<$filename";
binmode FH;

seek FH,0,2;
my $val = pack "n",$dechashcode;
print FH $val;

close FH;

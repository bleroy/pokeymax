#!/usr/bin/perl -w
use strict;

my $section;
my $program = 0;
my %bytype;
my $type;
my $pos = -1;
while (<>)
{
	if (/Max 10 (.*)/)
	{
		$section = $1;
		$program = 0;
		if ($section=~/^Program (....)$/)
		{
			$program = 1;
			$type = $1;
			$bytype{$type} = "";
			$pos = -1;
		}
	}
	if ($program and /SDR 32 TDI \(([A-F0-9]+)\);/)
	{
		$bytype{$type} = $bytype{$type}.$1;
		$pos = $pos + length($1)/2;
	}
	if ($program and /SDR 23 TDI \((.*?)\);/)
	{
		my $count = $1;
		my $count2 = unpack "H*", pack 'B*', unpack 'b*', pack("H*",$count);
		$count2 =~ /(..)(..)(..)/;
		$count2 = "$3$2$1";

		my $count2dec = hex($count2)*2;
		if ($pos<1)
		{
			$pos = $count2dec;
		}
		else
		{
			print "Pos:$pos ";
			my $adj = $count2dec-$pos;
			$pos = $pos+$adj;
			my $skip = "F"x($adj*2);
			$bytype{$type} = $bytype{$type}.$skip;

			print " adj:$adj req:$count2dec\n";
		}
		print "$type seek to $count2($count2dec) -> $pos\n";
	}
}

foreach (keys %bytype)
{
	print "$_:".length($bytype{$_})."\n";
	open OUT,">$_.bin";
	binmode(OUT);

	print OUT pack 'B*', unpack 'b*', pack("H*",$bytype{$_});
		#print OUT pack("H*",$bytype{$_});
	close(OUT);
}


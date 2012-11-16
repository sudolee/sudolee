#!/usr/bin/perl -w

use strict;
my $out;

if(@ARGV!=1 && @ARGV!=2){
    print "Usage:\n\t$0 input [output];\n";
    exit;
}

open(IN, "<$ARGV[0]") or die "couldn't open $ARGV[0]: $!";

if(<IN>=~m/(^#!.*perl.*)/){
    $out.=$1;
}

while(<IN>){
    chomp;
    chop;
    $_.="\n";
    $out.=$_;
}

close IN;

if(@ARGV==1){
    unlink $ARGV[0];
    open(OUT,">$ARGV[0]") or die "couldn't open $ARGV[0]: $!";
}
elsif(@ARGV==2){
    open(OUT,">$ARGV[1]") or die "couldn't open $ARGV[1]: $!";
}

print OUT $out;
close OUT;

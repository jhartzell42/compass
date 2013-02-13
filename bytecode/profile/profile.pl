#!/usr/bin/perl
while(<>) {
    if(/^CALLTO (.*)/) {
	$calls{$1}++;
    }
    if(/^CHANGETO: (.*)/) {
	$cur = $1;
    }
    if(/INST/) {
	$bytes{$cur}++;
    }
}
print "Call count:\n";
@blocks = keys %calls;
@blocks = sort { $calls{$b} <=> $calls{$a} } @blocks;
for (@blocks) {
    print "$_: ".$calls{$_}."\n";
}

print "Byte count:\n";
@blocks = keys %bytes;
@blocks = sort { $bytes{$b} <=> $bytes{$a} } @blocks;
for (@blocks) {
    print "$_: ".$bytes{$_}."\n";
}

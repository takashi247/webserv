#!/usr/bin/perl -w

read(STDIN, $stdin, $ENV{'CONTENT_LENGTH'});

print "Content-type: text/plain; charset=iso-8859-1\n\n";
foreach my $var (sort(keys(%ENV))) {
    my $val = $ENV{$var};
    $val =~ s|\n|\\n|g;
    $val =~ s|"|\\"|g;
    print "${var}=${val}\n";
}

print "STDIN: $stdin";

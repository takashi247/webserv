#!/usr/bin/perl

print "Content-type: text/html; charset=iso-8859-1\n\n";
print "<font size=+1>Environment</font><br>";
foreach (sort keys %ENV) {
   print "<b>$_</b>: $ENV{$_}<br>\n";
}
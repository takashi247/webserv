#!/usr/bin/perl

print "<font size=+1>Environment</font><br>";
foreach (sort keys %ENV) {
   print "<b>$_</b>: $ENV{$_}<br>\n";
}
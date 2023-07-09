#!/usr/bin/perl
$| = 1;
print "Content-type: text/html\n\n";

$DB = "/home/cjones/bin/db";
$HTML = "/home/cjones/bin/fmttr";
$FILTER = "/var/httpd/htdocs/";
$DISCARD = "/home/cjones/bin/";

if ($ENV{'REQUEST_METHOD'} eq 'POST') {
	read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
} else {
	$buffer = $ENV{'QUERY_STRING'};
}

# Split the name-value pairs on '&'
@pairs = split(/&/, $buffer);

# Go through the pairs to determine the name
# and value for each form variable

foreach $pair (@pairs) {
	($name, $value) = split(/=/, $pair);
	$value =~ tr/+/ /;
#	$value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
	$FORM{$name} = $value;
}
if($FORM{dbname} eq "") { $FORM{dbname} = "pas"; }
if($FORM{discard} eq "") {$FORM{discard} = "discpas"; }
if($FORM{filter} eq "") {$FORM{filter} = "wui.filter"; }
if($FORM{query} eq "") {system wui-cgi; }
$filter = join('',$FILTER, $FORM{filter});
$discard = join('', $DISCARD, $FORM{discard});
exec "$DB $FORM{dbname} \'$FORM{query}\' | $discard | $HTML $filter";


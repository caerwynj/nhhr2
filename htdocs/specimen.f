<html> <head> <title> Pathology </title> </head> 
<body bgcolor="#FFFFFF">
<!-- [set spec $0] --> 
<!-- [nextline] --> 
<table border> <tr> <td align=right><b>Name:</b></td><td>$0</td></tr>
<!-- [nextline] --> 
<tr> <td align=right><b>DOB:</b></td><td>$0</td></tr>
</table>
<hr>
$spec
<hr>
<pre>'@' = High     '!' = Low     ' ' = Normal

[looprint nextline {[switch -regexp -- $1 ^@ {format "%s\n" $0} "^\!" {format "%s\n" $0} default {format " %s\n" $0}]}]</pre>
</body>
</html>


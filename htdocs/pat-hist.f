<html> <head> <title> Patient History </title> </head>
<body bgcolor="#FFFFFF">
<h2> Patient History </h2>
<!-- [nextline; global counter ] -->
<pre>
Admissions
[looprint nextline {[if {$1 == "OK"} {
			global counter
			set counter [expr $counter +1] 
			switch $counter 1 {format "<a name=#Alerts>Alerts\n"} 2 {format "<a name=#Wait>Waiting Lists\n"} 3 {format "<a name=#Appoint>Appointments"} 
		    } else { format $0\n }
		   ]}
]</pre>
</body>
</html>
 

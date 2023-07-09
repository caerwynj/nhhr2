<html><head>
<TITLE>Change your password</TITLE>
</head>
<body bgcolor="#FFFFFF">
<H1>Change your password</H1>

<FORM ACTION="/change-passwd" METHOD="POST">

This is an HTML form used to change your password for HTTP user
authentication on this system. <P>
<HR>
To use this form, you must know your user name on this system, and you must
know your current password. <P>

First, enter your user name below. If you are defined to be the wizard on
this server, and wish to change or add a user, enter their name below. <P>

User name: <P><INPUT TYPE="text" NAME="user"><P> 

Now, enter what you want to change your password to. <P>

New password: <P><INPUT
TYPE="password" NAME="newpasswd1"><P> Re-type new password: <P><INPUT
TYPE="password" NAME="newpasswd2"><P>

When you click on the Change password button below, you will be asked to
authenticate yourself. If you are changing your own password, use your user
id and your <EM>old</EM> password to log on. If you are the wizard on this
server, use your own user id and password to log in.<P>

<INPUT TYPE="submit" VALUE="Change password"><P>
<INPUT TYPE="reset" VALUE="Reset these fields"><P>
</FORM>
</body>
</html>

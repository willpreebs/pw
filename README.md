<h1>pw</h1>
A Simple CLI Password manager.
<br>
Supports multiple user profiles. Users log in using a username and a master password, which enables them to see all of their stored passwords.
<br>
The master passwords are encrypted using OPENSSL SHA-512
<br>
Regular passwords are encrypted using OPENSSL AES-256-CBC

<h2>Commands:</h2>

<h3>$ pw init</h3>
Initialize the .password-store directory and create a new user profile. If .password-store already exists, this just creates a new user profile

<h3>$ pw or $ pw login</h3>
Login as a user to be able to view your passwords

<h4>Flags:</h4>

-u : Specify username <br>
-p : Specify password <br>

Example: $ pw login -u username -p masterpassword

<h3>$ pw userlist</h3>
See a list of the user profiles

<h2>Logged in command prompt</h2>
Once you are logged in as a user, you will see a command prompt that allows you to create, read, update, or delete your passwords

<h3><b>pw %</b> add</h3>

Add a password

<h3><b>pw %</b> view</h3>

View a password

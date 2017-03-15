User Waitlist
=============






How to use
-
First go to the administration page as such.

![Adding Names](img/adding_names.png "Adding names")

Go about creating the user. In some cases your OpenAM or LDAP module may do this for you based off your central 
identity management infrastructure. 

For my own testing on localhost, I manually created a user through drush via

```
$ drush ucrt aaa --password=aaa
```

Then log in as that new user

!['New user log in'](img/login_as_that_new_user.png "Logging in as new user")



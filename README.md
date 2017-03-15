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

Then log in as that new user:

!['New user log in'](img/login_as_that_new_user.png "Logging in as new user")


Return to the browser in which you've logged in as an administrator:
!['People listing'](img/notice_user_roles.png) "Notice User Roles"


To following along the process, tail the WatchDog log:

``` 
 488  15/Mar 16:50  notice  waitlist  Inserted aaa queued for Reviewers                                  
 489  15/Mar 16:50  notice  waitlist  Inserted aaa queued for Super Reviewers                            
 490  15/Mar 16:50  notice  user      Session opened for aaa.                                            
 491  15/Mar 16:50  notice  waitlist  aaa                                                                
 492  15/Mar 16:50  notice  waitlist  aaa is wait-listed to Reviewers, Super Reviewers                   
 493  15/Mar 16:50  notice  waitlist  Request for aaa was removed. Those roles were Reviewers, Super     
                                      Reviewers                                                          


```
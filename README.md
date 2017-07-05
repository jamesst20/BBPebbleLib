## Blackberry Pebble Library (also works under Linux)
The purpose of this project is to open a brand new world for the Pebble Watch. Since Pebble isn't officially supporting Blackberry and that we can only rely on Talk2Watch (which is very good but closed source), I want to try to fully expand the Pebble to all Blackberry Developer with hope that we will see more native apps in BBWorld for it.

I am expecting (hopefully) to not be the only one to contribute to this project because as a student, I don’t think I can handle a project of this size alone in my free time. Feel free to ask me (@Jamesst20 on Crackberry or via email) to be a member of this project, I will be more than happy to let people in if they can prove first that they are real developer and able to keep this library clean.

**PS: Thanks for avoiding to allocate objects on the heap because Blackberry only lets a very small heap memory quantity for headless applications.**

### TODO
* Communication with Pebble [DONE]
* Connection Error Handling [DONE VIA SIGNALS]
* Notifications for Mail/SMS/Facebook/Twitter [DONE]
* Replying to Pebble’s first request (PhoneVersion) [Doesn’t seem to work yet…]
* Install/uninstall watchfaces
* Install/uninstall applications
* Get information of an application
* Displaying Weather on a watchface
* Much much much more

### Related Work
Thanks to all the other projects which achieved communication with the Pebble watch and inspired/helped this project, namely:
* https://github.com/smokku/pebble
* https://github.com/Hexxeh/libpebble
* https://github.com/pebble/libpebble2
* https://github.com/Keboo/PebbleSharp
* https://github.com/DouweM/pebblewatch
* https://github.com/barometz/flint

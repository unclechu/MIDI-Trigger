MIDI-Trigger
============

LV2 plugin which generates MIDI notes by detected audio signal peaks.

Usage info
==========

Description of **"Note-off mode"** knob (in the future it will be presented in GUI):
- *1* — Send **NOTE-OFF** immediately before **NOTE-ON**;
- *2* — Send **NOTE-OFF** immediately after **NOTE-ON;**
- *3* — Send **NOTE-OFF** after "note-off delay" or early if it interrupted by
        other **NOTE-ON**;
- *4* — Send **NOTE-OFF** after "note-off delay" only if it not interrupted by
        other **NOTE-ON**;
- *5* — Don't send **NOTE-OFF**... NEVER!!!

Authors
=======

* [Viacheslav Lotsmanov](https://github.com/unclechu)
* [Andrew Fatkulin](https://github.com/anlaakso)

Contributors
============

* [Olivier Humbert](https://github.com/trebmuh)

License
=======

[GNU/GPLv3](./LICENSE)

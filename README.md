# MIDI-Trigger

LV2 plugin which generates MIDI notes by detected audio signal peaks.

It can be used to retrigger drums which would be the main purpose of it.
But also it can be used for some weird experiments. Just put some track as a
sound source, connect MIDI output to some synth, automate “MIDI note” value,
and see what happens.

Disclaimer: This plugin works and was already used in some recorded band albums
to improve drum sounds by adding a retriggered drum sample. Though it’s missing
a GUI at the moment which would make it a bit easier to use. Also the program
doesn’t have any versioning yet, so just take latest `master` branch state. If
you follow usage info instruction it should be no problem to use it.

## Usage info

The defaults for the knobs are tuned for retriggering drum tracks.
So basically you need to pay attention to these 3 values:

1. Threshold — Sensitivity level (the lower the value the more sensitive it is)
2. MIDI note — Which note to trigger (MIDI number)
3. Note-off mode — MIDI notes generation strategy/pattern (see below)

**“Note-off mode”** knob available values:

- *1* — Send **NOTE-OFF** immediately before **NOTE-ON**
- *2* — Send **NOTE-OFF** immediately after **NOTE-ON**
- *3* — Send **NOTE-OFF** after “Note-off delay” or earlier
        if it’s interrupted by another **NOTE-ON**
- *4* — Send **NOTE-OFF** after “Note-off delay”
        only if it’s not interrupted by another **NOTE-ON**
- *5* — Don’t send **NOTE-OFF**s

### How to build and run

Here are the requirements:

1. GNU/Make
2. GCC compiler
3. LV2 headers

Just run:

``` sh
make
```

There is now `build/midi-trigger.lv2` directory with the plugin. You can place
this plugin directory to somewhere like `/usr/lib/lv2` (depending on your
distro). Or you can try it like this (with Ardour as an example):

``` sh
LV2_PATH="$PWD/build:$LV2_PATH" ardour6
```

And find the plugin by “MIDI-Trigger” name.

#### Using Nix

I recommend to use [Nix] package manager to get a reproducible build.
This project includes Nix configuration.

Build it like this:

``` sh
nix-build -o result
```

There is now `result` symlink to a directory placed in `/nix/store` which
has plugin directory by `lib/lv2/midi-trigger.lv2` path inside it.

Now you can start Ardour for instance providing `LV2_PATH` to this plugin:

``` sh
LV2_PATH="$PWD/result/lib/lv2:$LV2_PATH" ardour6
```

And find the plugin by “MIDI-Trigger” name.

## Authors

* [Viacheslav Lotsmanov](https://github.com/unclechu)
* [Andrew Fatkulin](https://github.com/co-yo-ne-da)

## Contributors

* [Olivier Humbert](https://github.com/trebmuh)

## License

[GPLv3](./LICENSE)

[Nix]: https://github.com/NixOS/nix

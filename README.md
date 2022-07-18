# MIDI-Trigger

[![Nix CI](https://github.com/unclechu/MIDI-Trigger/actions/workflows/nix.yml/badge.svg)](https://github.com/unclechu/MIDI-Trigger/actions/workflows/nix.yml)

LV2 plugin which generates MIDI notes by detected audio signal peaks.

It can be used to retrigger drums which would be the main purpose of it.
But also it can be used for some weird experiments. Just put some track as a
sound source, connect MIDI output to some synth, automate “MIDI note” value,
and see what happens.

The **MIDI-Trigger** plugin has one mono audio input and one MIDI output.
So it takes audio signal as an input and provides MIDI output with the triggered
notes you can connect to some synth or sampler.

Disclaimer: This plugin works and was already used in some recorded band albums
to improve drum sounds by adding a retriggered drum sample. Though it’s missing
a GUI at the moment which would make it a bit easier to use. Also the program
doesn’t have any versioning yet, so just take latest `master` branch state. If
you follow usage info instruction it should be no problem to use it.

## Supported OSes

1. GNU/Linux — Tested by me personally on NixOS
   and builds successfully on Ubuntu in CI using Nix

2. MacOS — I don’t have a machine to test it but in CI it successfully builds
   using Nix

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

### Using with Ardour

It’s not a common combination for a plugin to have an audio input and a MIDI
output. So by default Ardour doesn’t commutate it properly. Here is a steps you
need to follow to make it work:

1. Add an audio/bus track and add “MIDI-Trigger” plugin to it (before fader)

2. Open context menu of the plugin (right mouse click) and open
   “Pin connections...”

   You should see that the plugin in the center has one red MIDI output port.
   But it’s not connected anywhere because plugin has only one green audio
   output port to the outside world.

3. Click “Manual Config” button
4. Add “MIDI Out” port (click “+” plus button)
5. Remove “Audio Out” (click “-” minus button)

6. Click and hold on the plugins’s MIDI port and drag it to the outer MIDI
   output so that it makes a connection

7. Now you can put a sampler or a synth after the plugin which takes MIDI signal
   as an input

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

#### Using Clang instead of GCC

You can compile this program also with Clang like this:

``` sh
make CXX=clang
```

Or using Nix:

``` sh
nix-build --argstr compiler clang
```

#### Installing globally on NixOS

Here is an example for your `configuration.nix`:

``` nix
{ pkgs, ... }:
let
  midi-trigger-src = fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/0000000000000000000000000000000000000000.tar.gz";
    sha256 = "0000000000000000000000000000000000000000000000000000";
  };

  midi-trigger = pkgs.callPackage midi-trigger-src { src = midi-trigger-src; };
in
{
  environment.systemPackages = [
    midi-trigger
  ];
}
```

Paste commit hash to the placeholder after `/archive/` and before `.tar.gz` and
run once to get calculated SHA256 checksum in the error log. Then paste that
SHA256 checksum to the `sha256` field and rebuild one more time.

## Development hints

### direnv

If you are using [direnv][direnv] you can look at
[.envrc example file](.envrc.example) (just uses Nix,
helps to auto-reload `nix-shell`).

## Authors

* [Viacheslav Lotsmanov](https://github.com/unclechu)
* [Andrew Fatkulin](https://github.com/co-yo-ne-da)

## Contributors

* [Olivier Humbert](https://github.com/trebmuh)

## License

[GPLv3](./LICENSE)

[Nix]: https://github.com/NixOS/nix
[direnv]: https://direnv.net

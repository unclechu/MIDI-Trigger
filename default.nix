let
  sources = import nix/sources.nix;

  # +1 and -1 for the slash after the prefix
  getRelativeFileName = prefix: fileName:
    builtins.substring
      (builtins.stringLength prefix + 1)
      (builtins.stringLength fileName - builtins.stringLength prefix - 1)
      fileName;

  srcFilter = prefix: fileName: fileType:
    let relativeFileName = getRelativeFileName prefix fileName; in
    (
      (fileType == "regular") &&
      (
        builtins.elem relativeFileName ["LICENSE" "README.md" "Makefile"] ||
        builtins.match "^[^/]+[.]ttl$" relativeFileName != null
      )
    )
    ||
    (
      (fileType == "directory") &&
      (builtins.match "^src(/.+)?$" relativeFileName != null)
    )
    ||
    (
      (fileType == "regular") &&
      (builtins.match "^src/.+[.](h|c)$" relativeFileName != null)
    );

  cleanSrc = pkgs: path:
    assert builtins.isPath path;
    pkgs.nix-gitignore.gitignoreFilterRecursiveSource
      (srcFilter (toString path))
      [ ./.gitignore ]
      path;

  availableCompilers = [ "gcc" "clang" ];
in

{ pkgs ? import sources.nixpkgs {}
, lib ? pkgs.lib

, src ? cleanSrc pkgs ./.

, debugBuild ? false
, compiler ? "gcc" # See “availableCompilers”

, gcc ? pkgs.gcc12 # “pkgs.gcc” is 11.x
, clang ? pkgs.clang_14 # “pkgs.clang” is 11.x
}:

let
  compilerMap = {
    gcc = "${gcc}/bin/gcc";
    clang = "${clang}/bin/clang";
  };
in

assert builtins.isBool debugBuild;
assert builtins.isString compiler;
assert builtins.elem compiler availableCompilers;

assert
  builtins.all
    (lib.flip builtins.elem availableCompilers)
    (builtins.attrNames compilerMap);

let
  esc = lib.escapeShellArg;

  midi-trigger = pkgs.stdenv.mkDerivation rec {
    name = "midi-trigger";
    inherit src;

    nativeBuildInputs = [
      pkgs.gnumake
      pkgs.pkg-config
    ];

    buildInputs = [
      pkgs.lv2
    ];

    buildPhase = ''(
      set -o nounset
      make BUILD_DIR=. CXX=${esc compilerMap.${compiler}} ${
        pkgs.lib.optionalString debugBuild (esc "DEBUG=Y")
      }
    )'';

    installPhase = ''(
      set -o nounset
      LV2_DIR=$out/lib/lv2
      mkdir -p -- "$LV2_DIR"
      mv -- "$name".lv2 "$LV2_DIR"
    )'';
  };
in

midi-trigger

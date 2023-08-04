# README

This directory contains third-party files.

## `./sdk/`

Contains distributions of third-party SDKs.

**All files here are copyright of their respective owners**
**and licensed under their own terms and conditions.**

## `./include/`

This directory is added to the include path of the project
and is where include files for required libraries should be
placed if not otherwise located on the standard `INCLUDE`
paths.

## `./lib/`

### `./lib/x86/`

This directory is added to the library path of the project
and is where library files for required libraries should be
placed if not otherwise located on the standard `LIB` paths.

### `./lib/x86/Debug/`

Added to the library search path of the project in
_addition_ to the parent directory for _Debug_ builds
_only_.

### `./lib/x86/Release/`

Added to the library search path of the project in
_addition_ to the parent directory for _Release_ builds
_only_.

# foo_logitech_lcd

`foo_logitech_lcd` was a plugin for [`foobar2000`](https://www.foobar2000.org/)
to support displaying track information and visualisations on the screens
present on a range of [`Logitech`](https://www.logitech.com) products.

Several years on from the abandonment of the plugin and long after `Logitech`
stopped making any supported devices the plugin has been made open source.

Note that **no** special hardware is required to use the plugin as a `Logitech`
provided emulator is available.

## Contents

- [Disclaimer & License](#disclaimer-license)
- [Requirements](#requirements)
  - [Building](#building)
  - [Running](#running)
- [The Code](#the-code)
- [Additional Files](#additional-files)

## Disclaimer & License

> This program is free software: you can redistribute it and/or modify it
> under the terms of the GNU Lesser General Public License as published by
> the Free Software Foundation, either
> [version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
> of the License, or (at your option) any later version.

**_Any third-party files located within this repository are subject to their own
licenses, terms, and conditions as defined by the respective rights holders and
are ONLY covered by the above license should they not provide an alternative._**

All code contained in this repository is considered **final**. It is provided
only so those who are interested can inspect it and use it in future projects if
they desire.

**_No future release, changes, or bug fixes will be made._**

All previously released versions of the plugin were distributed under, and remain
subject to, the terms and conditions of the [`Boost Software License`](http://www.boost.org/LICENSE_1_0.txt).

## Requirements

### Building

- [`Visual Studio`](https://visualstudio.microsoft.com/vs/community/)
  - version _2022_ (or later)
  - any edition
- [`foobar2000 SDK`](https://www.foobar2000.org/SDK)
  - version _2023-02-22_ **_only_**
  - **due to significant SDK changes in more recent versions, they do not
    currently work with the code here**
  - pre-compiled versions of the SDK libraries are expected to be available
    on the linkers include path
- [`Windows Template Library`](https://sourceforge.net/projects/wtl/)
  - version _10.0_ (or later)
- [`OpenGL Extension Wrangler Library (GLEW)`](https://glew.sourceforge.net/) (
  - version _2.1.0_ used for development, however earlier versions may work
  - currently uses the statically linked version
- [`Logitech GamePanel SDK`](https://www.logitechg.com/innovation/developer-lab.html)
  - version _3.01_ **_only_**
  - **due to significant SDK changes in more recent versions, they do not
    currently work with the code here**
  - unfortunately version 3.01 is no longer available for download, however the
    original SDK is included in this repository in the `dependencies` folder so
    that the code may be compiled and run
      - **The provided SDK is the sole property of _Logitech Inc._ and is
        licensed under it's own terms and conditions (as stated in the agreement
        included in the SDK).**
      - _The distribution of this SDK here (in it's original form) appears to be
        permitted by the SDK license._
- [`spdlog`](https://github.com/gabime/spdlog)
  - tested with version _1.11.0_, but any version should work

The project adds the following directories to the compiler search paths:

- `dependencies/include` is added to the include search path
- `dependencies/lib/x86` is added to the libraries search path
- `dependencies/lib/x86/Debug` is added to the libraries
  search path for debug builds _only_
- `dependencies/lib/x86/Release` is added to the libraries
  search path for release builds _only_

Dependencies not otherwise available on the current system can be installed to
these locations.

### Running

- [`foobar2000`](https://www.foobar2000.org)
  - any version that supports the SDK version used
  - _only 32-bit versions are currently supported_
- [`Logitech Gaming Software` (drivers)](https://support.logi.com/hc/en-za/articles/360024701114--Downloads-G19-Keyboard-for-Gaming)
  - any version that supports the SDK version used
  - `Logitech` frequently consolidates or retires software and drivers -
    `Logitech Gaming Software` was the last software that supported the relevant
     hardware and SDKs, however earlier software will also work, e.g. `Logitech
     LCD Manager`

_The `Logitech Gaming Software` comes with an LCD Emulator which can be used for
the plugin; it can be found launched from the `Developer` tab in the `Settings`
dialog._

## The Code

The code present in this repository is a modernised version of the original
project (which began life before even `C++ 11` was released).

There are many differences between this code and the code of the final release
of the plugin (version 0.5.5), with some dependencies removed, while others have
been updated to modern versions which are still actively supported.

This version also contains many enhancements over the v0.5.5 of the plugin,
including many bug fixes, as well as additional options and visualisations.
These were mainly part of an update that was never released but have been
preserved here.

## Additional Files

- [`Readme.txt`](./Readme.txt)
  - the user readme file meant to be distributed alongside the plugin
- `dependencies/...`
  - required dependencies no longer available elsewhere
  - **_All rights reserved by their respective owners. Distributed under_**
    **_terms and conditions as indicated by those owners._**

foo_logitech_lcd Readme
=======================

A foobar2000 component enabling track information and visualisations to be
displayed on a range of Logitech LCD products.

This component, and all previous versions, are distributed under the Boost
Software License: (http://www.boost.org/LICENSE_1_0.txt)

The source code for the component is available at
https://github.com/ectotropic/foo_logitech_lcd and is distributed under the
GNU Lesser General Public License.

Contents
========

1. Requirements
2. Options
3. Controls
4. Version History
5. To Do
6. Bugs
7. Credits
8. Contact

1. Requirements
===============

Hardware:
    * The following products are supported:
        - Logitech G13 Gamepad
        - Logitech G15 Keyboard
        - Logitech G510 Keyboard
        - Logitech G19 Keyboard
        - Logitech Z-10 Speakers
        - Dell XPS Laptops with built in G15 LCD (e.g. M1730)
    * The G13 Gamepad and Dell Laptops have not yet been verified as working
    * Additional products may also work (any Logitech hardware that uses the
      Logitech LCD Manager software should be compatible)
    * Note that the Z-10 Speakers and Dell Laptops require newer software than
      the available drivers provide. While it is possible to update the software
      by using downloads not specifically intended for the given hardware (see
      below) - you should understand that there is a chance this could cause
      problems with your software and/or hardware. If you are in anyway unsure,
      you should not attempt to do this - though many people have done so with
      no issues what-so-ever.
        - To update the software for these devices ***AT YOUR OWN RISK***:
            - Install the latest version of the hardware drivers as available
              on the manufacturers website
            - Install the latest G15 Keyboard software (i.e. Gamepanel) as
              available from the Logitech website
        - This idea was recommended by the software engineers at Logitech
          responsible for the LCD Manager software, should anyone experience
          difficulties with the drivers recommended above, they should use the
          contact info at the end of this document or contact Logitech directly

If you have any trouble running it, check the foobar2000 console, errors should
appear there.

2. Options
==========

foobar2000->Preferences->Display->Logitech LCD
(Please see notes at end of options section)

Options of note:
 * Expert Mode					This enables some advanced features, all
                                features enabled by this have caveats as
                                follows:
    - LCD V-Sync				Similar to V-Sync in games - has pros and cons.
                                If turned on will likely drop frames.
    - Force Foreground			Will try to force the app to the foreground and
                                keep it there - slows everything considerably
    - Raise Display Priority	On song change raise LCD priority, if song
                                changes happen rapidly this will cause the
                                display to start ignoring the component
    - Track Info settings		When expert mode is enabled track info settings
                                are changed so that as many lines as wished can
                                be added (if too many lines are added for the
                                display the lower lines will not be shown).

* Start with					Choose a specific display to start with - choose
                                the display on the visualisations tab, note that
                                if no display is set then a default (last used,
                                or track info) will be applied.
* Display track info			On song change switch briefly to track info
                                instead of vis (will revert to previous vis
                                after a given number of secs) note that no other
                                visualisation can be selected during this time
* Track Info -> Allow Overlap	Allow lines of text to overlap slightly (so low
                                hanging characters - i.e. 'g' - may have their
                                tails slightly obscured)

Additional Notes:
* Some options depend on settings in the Logitech drivers (e.g. setting as
  foreground/background) - if options seem not to work, try changing driver
  options.
* It is not recommended to use V-Sync or Force Foreground options if displaying
  anything other than Track Info.
* If randomise & display track info are not selected then the screen will only
  change if changed manually.
* New lines ('\n') are ignored in the expert track info mode - instead use
  "$char(10)" to create a new line.
    - "$char(10)" will not work correctly in normal mode
* Options not applicable to a given setup are disabled - colour options are
  never available for VU Meter foobar
* Size 8 font allows for 4 lines of text on Monochrome displays
* Time display font size is coupled to Progress bar size
    - Time font is Arial if installed, or the system menu font otherwise
* Hardware acceleration may cause issues with some setups - if you have any
  problems with the component, try disabling this. Note that hardware
  acceleration does not necessarily mean better performance, but some effects
  may not be available without it.

3. Controls
===========

For monochrome LCDs:
    Button 1:	Spectrum Analyser 1 -> Spectrum Analyser 2 -> Spectrum Analyser 3 -> Spectrum Analyser 4
    Button 2:	Oscilloscope 1 -> Oscilloscope 2 -> Oscilloscope 3 -> Oscilloscope 4
    Button 3:	VU Meter 1 -> VU Meter 2 -> VU Meter 3
    Button 4:	Track Info 1 -> Track Info 2 -> Track Info 3 -> Track Info 4

For colour LCDs:
    Left/Right: Change vis type (e.g. Spectrum Analyser -> Oscilloscope)
    Up/Down:	Change vis of this type (e.g. Spectrum Analyser 1 -> Spectrum Analyser 2)

If all of one type of visualisation is disabled then the button will have no
effect (Monochrome) or will move directly to the next active group (Colour)

4. Version History
==================

1.0.2 (Resurrection) - Minor Update
    * [FIXED]	Fix for progress bar not tracking progress.

1.0.1 (Resurrection) - Minor Update
    * [CHANGED]	updated to latest foobar2000 SDK (2023-09-23).
    * [NEW]		Now supports both 32-bit and 64-bit versions.

1.0.0 (Resurrection) - Open Sourced
    * [NEW]		comprehensively updated version, finally released as Open Source.
    * [FIXED]	Fixed many bugs and issues.
    * [CHANGED]	Improved accuracy and correctness of many calculations.
    * [CHANGED]	Improved general performance and robustness.
    * [CHANGED]	Removed dependence on multiple external libraries.
    * [CHANGED]	Updated to more modern external libraries where possible.
    * [NOTE]	Settings will **not** migrate from previous versions.

0.5.5 - Minor Update
    * [FIXED]	Crash on exit
    * [FIXED]	Small performance issue

0.5.4 - Minor Update
    * [FIXED]	Handling of device connections & disconnections
    * [FIXED]	Text overlapping progress bar in some cases
    * [FIXED]	Rare OpenGL crash
    * [CHANGED]	Better handling of windows messages to hardware window

0.5.3 - Minor Update
    * [NEW]		Progress bar timings can be replaced by system time and date [requested]
    * [NEW]		When using background pictures it is now possible to have 'cut-outs' (or background clears) for text (improves legibility)
    * [CHANGED]	updated to latest SDK (2010-05-21)
    * [CHANGED]	'About' text - also added change log to about text (so those who update via foobar updater can find out what has changed)
    * [CHANGED]	Faster loading of OpenGL textures
    * [FIXED]	Crash when loading album art and track changes
    * [FIXED]	Album art not being cleared on screen correctly in certain situations
    * [FIXED]	Minor bugs and inconsistencies

0.5.2 - Bug fixes
    * [FIXED]	Non-hardware accelerated Spectrum Analyser 1 being inverted
    * [FIXED]	Progress bar not resetting on stop
    * [FIXED]	Track info time selection combo-boxes text not corresponding to action

0.5.1 - Minor Update
    * [NEW]		Added additional VU preferences (fixes issue with left VU meter not matching right)
    * [CHANGED]	updated to latest SDK (2010-01-19)
    * [TWEAKED]	"Apply" button behavior for edit boxes
    * [FIXED]	Laggy dialog boxes

0.5.0 - Major Update
    * [NEW]		foobar2000 v1.0.0 support
    * [NEW]		Hardware acceleration support
    * [NEW]		Background mode (only displays when song changes)
    * [NEW]		Trail effect for colour displays
    * [NEW]		Exposed more options for configuration
    * [CHANGED]	Improved text preferences dialogs
    * [CHANGED]	Album art now comes from foobar and can be set to be correct aspect ratio
    * [CHANGED]	Attempted to make spectrum analyser more reactive
    * [CHANGED]	Major rewrite of preferences and configuration code
    * [CHANGED]	foobar image from jpeg to raw data
    * [REMOVED]	Support for older versions of foobar2000
    * [FIXED]	Time display occasionally showing 0:60 seconds
    * [FIXED]	Multiple crashes

0.4.0 - Bug fixes
    * [FIXED]	Multiple crashes
    * [FIXED]	Dynamic track information
    * [FIXED]	Font occasionally not being correctly set
    * [FIXED]	All options now update instantly
    * [FIXED]	Block Stereo Spectrum block count not being correctly used
    * [TWEAKED]	Track info pages - scrolling should be much improved
    * [TWEAKED]	Progress bar
    * [REMOVED]	Excess (unused) code

0.3.6 - Bug fixes
    * [FIXED]	Text colourisation

0.3.5 - Updates & fixes
    * [NEW]		Stereo Spectrum visualisations
    * [NEW]		Peak markers for both VU and Spectrum visualisations
    * [NEW]		Added an option to allow a gap line between spectrum lines (block spectrum only)
    * [NEW]		Yet more configuration options
    * [CHANGED] Scaling and offset factors - now unified across colour and monochrome displays
    * [CHANGED] Spectrum calculations - look better now
    * [FIXED]	Wallpaper OutOfMemory error
    * [FIXED]	Wallpaper not displaying on track info pages
    * [FIXED]	Config options not saving correctly in certain circumstances
    * [FIXED]	Enabling/disabling wallpaper now applies immediately (no track change required)

0.3.1 - Bug fixes
    * [FIXED]	Preferences crash bug (removed 3rd party library that was causing problems)
    * [FIXED]	Threading synchronisation in certain cases
    * [FIXED]	Error display (removed some output not required in release builds)

0.3.0 - Major update
    * [NEW]		Additional track info pages
    * [NEW]		Text handling - including vertical scrolling support - text quality on colour displays should be improved
    * [NEW]		Configuration options (including control of scrolling parameters)
    * [NEW]		Error handling - improved reporting of errors, many additional error checks
    * [NEW]		Many optimisations (CPU usage should be much better)
    * [NEW]		New source layout and (better) helper functions (minor code refactor)
    * [CHANGED]	Removed dependencies on DLLs - VC-Redist should no longer be required. (URL remains in case something was missed, or using an older version)
    * [CHANGED]	Removed use of Logitech EZ LCD wrapper
    * [CHANGED] Progress bar height detached from font height
    * [FIXED]	Conflicting libraries
    * [FIXED]	Handling of multiple LCD screen types (now will only use one type of screen at any one time)
    * [FIXED]	Font weight not being passed through correctly (i.e. bold)

0.2.6 - Bug fix
    * [FIXED]	Crash bug on exit

0.2.5 - Mostly internal changes affecting colour users
    * [NEW]		Initial support for image display on colour screens (requires GDI+)
    * [CHANGED]	Now using OpenGL instead of GDI to render visualisations (text and image display still use GDI)
    * [CHANGED]	Improved gradient calculations for colour screens
    * [FIXED]	Performance issues (performance is vastly improved though still needs work in some cases)
    * [FIXED]	Several minor bugs including wrong config being read for some variables

0.2.1 - Bug fixes
    * [FIXED]	Font dialog not displaying

0.2.0 - Major update
    * [NEW]		Config dialog & more options (including ability to disable specific visualisations)
    * [NEW]		Support for colour gradients (including new line drawing algorithm)
    * [NEW]		Support for font selection
    * [NEW]		Expert mode, including the ability to use advance track info details
    * [NEW]		Ability to change time of randomisations and track info display
    * [NEW]		Ability to change options of individual effects
    * [UPDATED]	Vastly improved G19 support
    * [FIXED]	Config so that all changes that can be applied instantly are
    * [FIXED]	Floating point infinity error
    * [FIXED]	Track information display at incorrect times
    * [PARTIAL]	Added initial implementation of many other options not yet enabled

0.1.2 - Bug fixes
    * [FIXED]	Visualisations on G19 (with help from ragganiczu - many thanks)
    * [FIXED]	Config dialog bugs
    * [FIXED]	Colour support on G19 track info page
    * [FIXED]	Other minor bugs
    * [NEW]		Better quality foobar images for both monochrome and colour screens

0.1.1 - Bug fixes & additions
    * [FIXED]	Removed potential divide by zero
    * [NEW]		Added track time counters and associated options
    * [NEW]		2 new oscilloscope visualisations: Circle & Star Burst
    * [TWEAKED] Oscilloscope visualisation 2 (more substantial)

0.1.0 - First Release

6. Bugs & Issues
================

* May be possible to set options at such a time that display stops updating (fixable by exiting and restarting foobar)
* If you move from a colour display to a monochrome display the colour settings will mess the monochrome display up (remedied by deleting all options)

7. Credits
==========

Uses elements of:
    * Boost
    * OpenGL
    * GDI & GDI+
    * ATL/WTL
    * foobar2000 SDK

Uses images from foobar2000 (modified).

Everything else designed and written by ectotropic.

Thanks to the HydrogenAudio community for help with testing/bug fixing (special
thanks go to ragganiczu & razor99).

8. Contact
==========

Bugs, suggestions, comments:
    Email:					ectotropic@gmail.com
    hydrogenaudio:
        User:				ectotropic
        Forum Thread:		http://www.hydrogenaudio.org/forums/index.php?showtopic=72720 (please read fully before mentioning a problem)

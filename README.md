Overview Plugin for Geany
=========================

The Overview plugin is a small zoomed-out view next to the normal editor view
that allows to see and navigate a lot of the file at once. It is similar to
the Minimap in SublimeText or such similar feature in numerous other editors.

Screenshots
-----------

This screenshot shows with a light theme, the overview's overlay is inverted,
and the regular scrollbar is not shown. The optional tooltip can be seen
(partially cut-off) in the screenshot also. It shows the line, column and
offset information of the location of the mouse cursor.

![Overview plugin screenshot with light theme](screenshots/screenshot-light.png?raw=true)

This screenshot shows with a dark theme, the overview's overlay is not
inverted, and the regular scrollbar is shown.

![Overview plugin screenshot with dark theme](screenshots/screenshot-dark.png?raw=true)

This screenshot shows the preferences dialog, with the options that can be
configured, saved and restored. Helpful mouse-over tooltips are provided to
give a quick explanation of each preference.

![Overview plugin's preferences GUI](screenshots/screenshot-prefs.png?raw=true)

This screenshot shows the available keybindings which can be configured
through Geany's Preferences dialog from the `Edit->Preferences` menu item.

![Keybindings preferences](screenshots/screenshot-keybindings.png?raw=true)

This screenshot shows how the Overview can be shown/hidden by using Geany's
`View` menu.

![Showing and Hiding from the main menu](screenshots/screenshot-view-menu.png?raw=true)

Building and Installing
-----------------------

In order to build the Overview plugin you need:

- Autotools and a GCC-like C99 compiler (ex. `build-essential` package on Debians)
- GTK+ 2.24 or greater
- Geany, including its development headers, built against GTK+ 2.X

Once you have all the dependencies, it's just a matter of running the
following commands:

```bash
$ ./autogen.sh   # note 1
$ ./configure    # note 2
$ make
$ make install   # note 3
```

**Note 1:** Only run when source was checked-out through Git, not required or
recommended if you're building from a source distribution.

**Note 2:** Use the `--prefix` option to change installation directories if
desired. If your Geany installation is in an usual place, you'll probably
have to use the `PKG_CONFIG_PATH` environment variable so the build
system finds the correct Geany build flags and paths.

**Note 3:** May require root permissions, depending on `--prefix` option to
configure, with the default `/usr/local` requiring root permissions on
most systems.

Using the Plugin
----------------

In order to use the plugin, you need to activate it through Geany's Plugin
Manager dialog, accessible from the `Tools->Plugin Manager` menu item. Once
activated the Overview sidebar view will appear to the right of the main
editor view.

The configure the plugin, use Geany's Plugin Preferences dialog from the
`Edit->Plugin Preferences`, and select the "Overview" tab. To configure
keybindings for the plugin, for example to show/hide it, use Geany's
Keybindings tab in the Preferences dialog from `Edit->Preferences`.

You can also hide and show the overview bar by using the `View->Show Overlay`
menu item. If you find you no longer have a need for Geany's regular vertical
editor scrollbar, you can disable it using the Overview preferences.

Using the plugin is simple. The Overview uses a really simple alogrithm that
seems to work quite well. When you click on the overview, it jumps to the
corresponding location in the main editor view and scrolls it into view. When
you scroll on it, it actually just does the same thing, except it jumps to
each location really quickly as the mouse moves, simulating a scrolling
effect. If the scrolling behaviour feels a little weird at first, this is
probably why.

Scrolling the main editor view causes the overview to update it's overlay to
show which part of the file is visible in the main editor.

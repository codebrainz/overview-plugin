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

Installing
----------

The plugin is quite young and just has a very basic GNU Make file. If your
system is capable of building any other Geany plugin, it shouldn't require
much fiddling to get it compile. A better build system will be used soon.

From the shell, change into the directory where the plugin's code is and run:

```bash
shell$ make && make install
```

The plugin goes into the Geany config directory in your home directory,
`~/.config/geany/plugins`.

Using
-----

Just activate the "Overview" plugin using Geany's Plugin Manager dialog from
the `Tools->Plugin Manager` menu and start playing with it. To change
configuration use use the Plugin Preferences dialog from `Edit->Plugin
Preferences` or from the Plugin Manager dialog.

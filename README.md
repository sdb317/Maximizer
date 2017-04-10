# Tiles application windows
- over multiple monitors
- in different modes i.e. landscape or portrait

C++ console app to make the most of the desktop real estate.
It provides an alternative to the built-in drag&dock in Windows 10.

It takes all of the non-minimised windows on each monitor and tiles them horizontally if the monitor is in landscape mode or vertically if in portrait mode.

It uses the following Windows api's:

- EnumDisplayMonitors
- GetMonitorInfo
- EnumDesktopWindows
- MonitorFromWindow
- SetWindowPos

I have shortcut to it pinned to my TaskBar as a 'run minimised' app. That way I can arrange anything currently displayed with one click.

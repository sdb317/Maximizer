# Tiles application windows
- over multiple monitors
- in different modes i.e. landscape or portrait

C++ console app to make the most of the desktop real estate.
It provides an alternative to Window's built-in drag&dock.

It takes all of the non-minimised windows on each monitor and tiles them horizontally if the monitor is in landscape mode or vertically if in portrait mode.

It uses the following Windows api's:

- EnumDisplayMonitors
- GetMonitorInfo
- EnumDesktopWindows
- MonitorFromWindow
- SetWindowPos


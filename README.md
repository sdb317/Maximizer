# Mazimizer is a desktop manager for Windows 10

It can:
- Create processes
- Record application window positions
- Playback the recordings to re-position application windows
- Dynamically tile restored windows to their main monitor

It works:
- over multiple monitors
- in different modes i.e. landscape or portrait


This C++ console app can make the most of your desktop real estate.
It provides an alternative to the built-in drag&dock in Windows 10.


## Modes

### Create `(-c <FileName>)`

```
<ProcessList>
  <Process>
    <CommandLine>C:\Program Files (x86)\Google\Chrome\Application\chrome.exe</CommandLine>
    <Title>Google Chrome</Title>
    <left>1</left>
    <top>1</top>
    <right>1199</right>
    <bottom>1879</bottom>
  </Process>
  <Process>...
```

Using a list of processes supplied in an XML document, it calls the `CreateProcess` Windows api to launch the supplied executable as a detached process.

### Layout `(-l)`

It takes all of the non-minimised windows on each monitor and tiles them horizontally if the monitor is in landscape mode or vertically if in portrait mode. Optionally (if the left `<Ctrl>` key is pressed) it will minimise all windows after tiling.

It uses the following Windows api's: `EnumDisplayMonitors`, `GetMonitorInfo`, `EnumDesktopWindows`, `MonitorFromWindow`, `SetWindowPos`

### Record `(-r <FileName>)`

This mode records the current position of the specified application windows to an XML document.

### Playback `(-p <FileName>)`

This mode restores specified application windows to previously recorded positions.


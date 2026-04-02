# Sailfish Weather

Weather application for SailfishOS, including summary banner of current location weather for
inclusion on the Events View of the homescreen. Originally used only Foreca, but now supports pluggable
backends discovered at runtime. See `BACKEND.md` for details.

Sailfish Weather supports multiple bundled weather providers selected at runtime from Settings.

Bundled providers in this tree:

- `Foreca`
- `OpenWeather`
- `MET Norway`
- `Open-Meteo`

Provider-specific weather state is stored per provider under the application data directory, so switching
between providers preserves each provider's saved and current locations independently.

Some providers use third-party location search services that are distinct from the forecast source:

- `MET Norway` forecast data comes from MET Norway / Yr, while location search uses Open-Meteo geocoding
- `Open-Meteo` uses Open-Meteo forecast and Open-Meteo geocoding

Review the provider terms and attribution requirements before enabling a provider in a product build.

## Issue reporting

Please report issues to [Issue Tracker](https://github.com/sailfishos/issue-tracker/issues) repository and
label issues with weather. You can also filter by [label](https://github.com/sailfishos/issue-tracker/issues?q=state%3Aopen%20label%3Aweather).

Worth noting that GitHub Issues provides a bug report template when creating a new issue.

Find more information from [Issue Tracker](https://github.com/sailfishos/issue-tracker).

If one of the bundled weather backends is bothering your weather service, please accept our apologies and contact us at weather@jolla.com to sort it out.

Happy hacking.

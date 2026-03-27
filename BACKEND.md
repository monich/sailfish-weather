# Weather Backend API

This repository keeps its built-in backend sources in `backends/`, but runtime discovery uses the installed backend directory.

Each backend is a QML `QtObject` loaded dynamically by `components/WeatherProvider.qml`.
Backend QML files may use adjacent JavaScript files as implementation helpers.

## Discovery

Built-in backends in this repository live in:
- `backends/*Backend.qml`

Installed backends are discovered by filename from:
- `/usr/share/sailfish-weather/backends/*Backend.qml`

Third-party backend packages should install their backend files to:
- `/usr/share/sailfish-weather/backends/`

They are loaded dynamically and sorted by:
1. `providerTitle()`

## Required Metadata Functions

Every backend must provide:

- `providerId()`
  - Returns the stable backend id used in configuration and saved weather data.
  - Example: `"open_weather"`

- `providerTitle()`
  - Returns the user-visible provider name.
  - This should usually use `qsTrId(...)`.

- `requiresApiKey()`
  - Returns `true` if the backend needs the user to provide an API key.

- `apiKeyInstructions()`
  - Returns rich text shown in settings below the API key field.
  - Return `""` if not needed.

- `attributionText()`
  - Optional.
  - Returns rich text attribution shown in settings and the provider disclaimer area.
  - Return `""` if not needed.

- `shortAttributionText()`
  - Optional.
  - Returns short plain attribution text used in compact UI such as the weather banner.
  - If omitted, the framework falls back to `attributionText()`.

## Optional Metadata Functions

- `maxPrecision()`
  - Optional.
  - Returns the maximum number of decimal places allowed for request coordinates.
  - If omitted, no precision adjustment is applied.
  - The precision limit is applied only to request URLs, not to stored coordinates.

- `canLoadWeather(weather)`
  - Optional.
  - Returns `true` when the backend can fetch weather for the given saved location object.
  - Use this when a backend has stricter requirements than “a location exists”.
  - Examples:
    - `Foreca` requires a positive `locationId`
    - coordinate-based backends can reject missing or `0,0` coordinates
  - If omitted, the framework assumes `true`.

## Request/Auth Functions

- `fetchToken(weatherRequest, apiKey)`
  - Optional.
  - Called before requests are sent.
  - Should set `weatherRequest.token` if needed and return `true` on success.
  - If omitted, the framework sets `weatherRequest.token = apiKey || ""`.

- `requestHeaders()`
  - Optional.
  - Returns an object of extra HTTP headers.
  - Return `{}` if not needed.

## URL Builder Functions

These should return a URL string.

- `currentWeatherUrl(weather)`
- `latestObservationUrl(weather)`
- `forecastUrl(weather, hourly)`
- `searchLocationUrl(filter, language)`
- `externalUrl(weather)`

Notes:
- `weather` is a JS object containing stored location/weather properties.
- For backends with `maxPrecision()`, `latitude` and `longitude` will already be truncated before these functions are called.
- `forecastUrl(weather, hourly)` receives `hourly === true` for hourly forecast requests and `false` for daily forecast requests.

## Result Parser Functions

These convert backend responses into the app’s internal weather/location format.

- `handleSearchLocationResult(result)`
  - Returns:
    - `undefined` on parse failure
    - `[]` for a valid empty result
    - array of location objects on success

- `handleCurrentWeatherResult(result)`
  - Returns a weather object or `undefined`.

- `handleObservationResult(result)`
  - Returns a string, typically the station/provider observation label.
  - Return `""` if not applicable.

- `handleForecastResult(result, hourly, visibleCount, minimumHourlyRange)`
  - Returns an array of weather objects or `undefined`.

## Provider Branding Functions

- `providerImage()`
  - Returns the large provider image URL used in disclaimers and placeholders.

- `smallProviderImage()`
  - Returns the small provider image URL used in banners and compact views.

## Location Object Format

`handleSearchLocationResult()` should return objects with:

- `id`
  - Provider-specific stable location id.
- `name`
- `country`
- `state`
- `adminArea`
- `adminArea2`
- `latitude`
- `longitude`

The app will save the selected location with:
- `locationId`
- `provider`
- `latitude`
- `longitude`
- `city`
- `state`
- `country`
- `adminArea`
- `adminArea2`

## Weather Object Structure

`handleCurrentWeatherResult()` should return an object containing:

- `timestamp`
- `temperature`
- `feelsLikeTemperature`
- `weatherType`
- `description`

`handleForecastResult()` weather entries should contain:

For hourly entries:
- `timestamp`
- `temperature`
- `weatherType`
- `description`

For daily entries:
- `timestamp`
- `high`
- `low`
- `accumulatedPrecipitation`
- `maximumWindSpeed`
- `windDirection`
- `weatherType`
- `description`

Optional shared fields used by the UI:
- `cloudiness`
- `relativeTemperature`

## Weather Type Format

Backends are expected to return app weather codes in the existing 4-character format used by the UI.

Examples:
- `d000`
- `n300`

The first character usually represents day/night and the remaining three characters match the app’s weather condition coding.

## Settings Integration

Provider selection is stored in:
- `/sailfish/weather/data_provider`

Per-provider API keys are stored in:
- `/sailfish/weather/<provider_id>_app_id`

Behavior:
- if more than one backend is installed, provider selection may be unset
- if exactly one backend is installed, it is selected automatically

## Saved Data Compatibility

Saved locations are provider-scoped.

Do not change the return value of `providerId()` for an existing backend unless you also handle migration, because the id is used in persisted data.

## Shared Request Behavior

All network traffic goes through `components/WeatherRequest.qml`.

Backends do not need to implement request dispatch themselves.

Current shared behavior:
- request timeout: 8 seconds
- common status handling for `200`, `304`, `401`, and other HTTP errors
- in-memory HTTP cache support
- `If-Modified-Since` support when the server provides `Last-Modified`
- cache freshness based on `Cache-Control`, `Date`, and `Expires`

The cache is in-memory only:
- no disk persistence
- no stale cache reuse after app restart

## Current Examples

Backend implementations live under `backends/` once they have been migrated.
Use the migrated backends in that directory as reference implementations for:
- self-described metadata
- URL construction and response parsing
- provider branding hooks
- optional request headers and precision limits

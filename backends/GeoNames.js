// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

.pragma library

var searchApi = "https://secure.geonames.org/searchJSON"
var reverseApi = "https://secure.geonames.org/findNearbyPlaceNameJSON"
var username = "jolla"

function searchLocationUrl(filter, language) {
    return searchApi + "?maxRows=20"
            + "&featureClass=P"
            + "&style=FULL"
            + "&isNameRequired=true"
            + "&name_startsWith=" + encodeURIComponent(filter)
            + (language && language.length > 0 ? "&lang=" + encodeURIComponent(language) : "")
            + "&username=" + encodeURIComponent(username)
}

function handleSearchLocationResult(result) {
    if (result === undefined || result === null) {
        return undefined
    }

    var geonames = result.geonames
    if (geonames === undefined || geonames === null) {
        return undefined
    }
    if (geonames.length === 0) {
        return []
    }

    var locations = []
    for (var i = 0; i < geonames.length; i++) {
        var place = geonames[i]
        var lat = parseFloat(place.lat)
        var lon = parseFloat(place.lng)
        if (isNaN(lat) || isNaN(lon)) {
            continue
        }

        var locationId = parseInt(place.geonameId, 10)
        if (!isFinite(locationId) || locationId <= 0) {
            locationId = hashLatLon(lat, lon)
        }

        var name = place.toponymName || place.name || ""
        var admin1 = place.adminName1 || ""
        var admin2 = place.adminName2 || ""
        var primaryAdminArea = admin2 || admin1
        var secondaryAdminArea = admin2 ? admin1 : ""

        locations.push({
                           "id": locationId,
                           "name": name,
                           "state": primaryAdminArea,
                           "country": place.countryName || place.countryCode || "",
                           "adminArea": primaryAdminArea,
                           "adminArea2": secondaryAdminArea,
                           "latitude": lat,
                           "longitude": lon
                       })
    }

    return locations.length > 0 ? locations : undefined
}

function reverseLocationUrl(latitude, longitude, language) {
    return reverseApi + "?maxRows=1"
            + "&style=FULL"
            + "&radius=10"
            + "&localCountry=true"
            + "&cities=cities15000"
            + "&lat=" + latitude
            + "&lng=" + longitude
            + (language && language.length > 0 ? "&lang=" + encodeURIComponent(language) : "")
            + "&username=" + encodeURIComponent(username)
}

function handleReverseLocationResult(result, latitude, longitude) {
    var locations = handleSearchLocationResult(result)
    if (locations === undefined || locations.length === 0) {
        return undefined
    }

    var location = locations[0]
    location.latitude = latitude
    location.longitude = longitude
    return location
}

function hashLatLon(lat, lon, precisionBits) {
    precisionBits = precisionBits || 16

    var latScaled = Math.floor(((lat + 90) / 180) * (1 << precisionBits))
    var lonScaled = Math.floor(((lon + 180) / 360) * (1 << precisionBits))
    var hash = (latScaled << precisionBits) | lonScaled
    hash = hash & 0x7fffffff
    return hash > 0 ? hash : 1
}

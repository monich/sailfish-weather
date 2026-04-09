// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

.pragma library

var enableDebugLogging = false
var entries = {}
var inflightRequests = {}

function debugLog() {
    if (enableDebugLogging) {
        console.log.apply(console, arguments)
    }
}

function debugPrefix(url) {
    return "WeatherResponseCache: " + url
}

function inflightPrefix(url) {
    return "WeatherInflight: " + url
}

function describeExpiry(expiresAt) {
    if (expiresAt === undefined || expiresAt === null) {
        return "no-expiry"
    }

    return new Date(expiresAt).toUTCString()
}

function cacheEntry(url) {
    var entry = entries[url]
    if (!entry) {
        return undefined
    }

    if (entry.createdAt !== undefined && Date.now() < entry.createdAt) {
        debugLog(debugPrefix(url), "invalidating cache entry (created in the future)")
        delete entries[url]
        return undefined
    }

    return entry
}

function freshResponse(url) {
    var entry = cacheEntry(url)
    if (!entry) {
        debugLog(debugPrefix(url), "cache miss (no fresh entry)")
        return undefined
    }

    if (entry.sessionCache) {
        debugLog(debugPrefix(url), "cache hit (session)")
        return entry.data
    }

    if (entry.expiresAt === undefined || entry.expiresAt === null) {
        debugLog(debugPrefix(url), "cache miss (no fresh entry)")
        return undefined
    }

    if (Date.now() < entry.expiresAt) {
        debugLog(debugPrefix(url), "cache hit (fresh until", describeExpiry(entry.expiresAt) + ")")
        return entry.data
    }

    debugLog(debugPrefix(url), "cache stale (expired at", describeExpiry(entry.expiresAt) + ")")
    return undefined
}

function cachedResponse(url) {
    var entry = cacheEntry(url)
    if (entry) {
        debugLog(debugPrefix(url), "using cached response body")
    } else {
        debugLog(debugPrefix(url), "no cached response body available")
    }
    return entry ? entry.data : undefined
}

function conditionalHeaders(url) {
    var entry = cacheEntry(url)
    if (!entry || !entry.lastModified || entry.lastModified.length === 0) {
        debugLog(debugPrefix(url), "no conditional cache headers")
        return {}
    }

    debugLog(debugPrefix(url), "sending If-Modified-Since", entry.lastModified)
    return {
        "If-Modified-Since": entry.lastModified
    }
}

function beginInflight(url, requester) {
    var waiters = inflightRequests[url]
    if (waiters) {
        waiters.push(requester)
        debugLog(inflightPrefix(url), "joined existing request, waiters:", waiters.length)
        return false
    }

    inflightRequests[url] = [requester]
    debugLog(inflightPrefix(url), "started new request")
    return true
}

function releaseInflight(url, requester) {
    var waiters = inflightRequests[url]
    if (!waiters) {
        return
    }

    for (var i = waiters.length - 1; i >= 0; --i) {
        if (waiters[i] === requester) {
            waiters.splice(i, 1)
        }
    }

    if (waiters.length === 0) {
        delete inflightRequests[url]
        debugLog(inflightPrefix(url), "released final waiter")
    }
}

function takeInflight(url) {
    var waiters = inflightRequests[url] || []
    delete inflightRequests[url]
    debugLog(inflightPrefix(url), "completing request for waiters:", waiters.length)
    return waiters
}

function hasValidator(headers) {
    var lastModified = headers["last-modified"]
    return !!lastModified && lastModified.length > 0
}

function store(url, data, headers, fallbackSessionCache) {
    var expiresAt = parseExpires(headers)
    if (expiresAt === undefined && !hasValidator(headers) && !fallbackSessionCache) {
        debugLog(debugPrefix(url), "not caching response (no expiry or validator)")
        return
    }

    entries[url] = {
        "data": data,
        "createdAt": Date.now(),
        "expiresAt": expiresAt,
        "lastModified": headers["last-modified"] || "",
        "sessionCache": fallbackSessionCache && expiresAt === undefined
    }
    if (entries[url].sessionCache) {
        debugLog(debugPrefix(url), "stored response (session cache, last-modified:",
                 (headers["last-modified"] || "none") + ")")
    } else {
        debugLog(debugPrefix(url), "stored response (expires:",
                 describeExpiry(expiresAt) + ", last-modified:",
                 (headers["last-modified"] || "none") + ")")
    }
}

function updateMetadata(url, headers) {
    var entry = entries[url]
    if (!entry) {
        debugLog(debugPrefix(url), "cannot update metadata for missing cache entry")
        return
    }

    var expiresAt = parseExpires(headers)
    if (expiresAt !== undefined) {
        entry.expiresAt = expiresAt
    }
    if (headers["last-modified"] !== undefined) {
        entry.lastModified = headers["last-modified"]
    }
    if (entry.sessionCache && entry.expiresAt === undefined) {
        debugLog(debugPrefix(url), "updated cache metadata (session cache, last-modified:",
                 (entry.lastModified || "none") + ")")
    } else {
        debugLog(debugPrefix(url), "updated cache metadata (expires:",
                 describeExpiry(entry.expiresAt) + ", last-modified:",
                 (entry.lastModified || "none") + ")")
    }
}

function responseHeaders(request) {
    return {
        "cache-control": request.getResponseHeader("Cache-Control"),
        "date": request.getResponseHeader("Date"),
        "expires": request.getResponseHeader("Expires"),
        "last-modified": request.getResponseHeader("Last-Modified")
    }
}

function parseExpires(headers) {
    var cacheControl = headers["cache-control"]
    if (cacheControl) {
        var directives = cacheControl.split(",")
        for (var i = 0; i < directives.length; i++) {
            var directive = directives[i].trim().toLowerCase()
            if (directive === "no-store" || directive === "no-cache") {
                return undefined
            }
            if (directive.indexOf("max-age=") === 0) {
                var maxAge = parseInt(directive.substring(8), 10)
                if (!isNaN(maxAge) && maxAge >= 0) {
                    return Date.now() + (maxAge * 1000)
                }
            }
        }
    }

    var expiresAt = parseHttpDate(headers["expires"])
    if (expiresAt !== undefined) {
        return expiresAt
    }

    return undefined
}

function parseHttpDate(value) {
    if (!value || value.length === 0) {
        return undefined
    }

    var timestamp = Date.parse(value)
    return isNaN(timestamp) ? undefined : timestamp
}

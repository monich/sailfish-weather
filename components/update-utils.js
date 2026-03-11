var lastUpdate = new Date()

function updateAllowed(interval) {
    // only update automatically if more than <interval> minutes has
    // passed since the last update (default 30mins: 30*60*1000)
    // or the date has changed
    interval = interval === undefined ? 30*60*1000 : interval
    var now = new Date()
    var updateAllowed = now.getDate() != lastUpdate.getDate() || (now - interval > lastUpdate)
    if (updateAllowed) {
        lastUpdate = now
    }
    return updateAllowed
}

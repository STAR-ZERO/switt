var api = require('api');
var config = require('config');

function locationError(err) {
  resultError(messageTypeSearchVenue, "Location error")
}

function searchVenues() {
  navigator.geolocation.getCurrentPosition(
    api.search,
    locationError, {
      timeout: 15000,
      maximumAge: 60000
    }
  );
}

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
  searchVenues();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log("appmessage");
  if (e.payload['AppKeyCheckInId']) {
    api.checkIn(e.payload['AppKeyCheckInId']);
  }
});

// configuration
Pebble.addEventListener('showConfiguration', function() {
  console.log('showConfiguration');
  Pebble.openURL(config.configurationURL);
});

// callback configuration
Pebble.addEventListener('webviewclosed', function(e) {
  console.log('webviewclosed');

  if (!e.response) {
    console.log('oauth deny');
    return;
  }

  var option = JSON.parse(decodeURIComponent(e.response));
  var oauthToken = option.oauth_token;
  // store oauth token
  localStorage.setItem('oauth_token', oauthToken);
});

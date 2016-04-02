// message type
const messageTypeSearchVenue = 0;
const messageTypeCheckIn = 1;

// XMLHttpRequest
var request = function(url, type, data, onload, onerror) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    onload(this.status, this.responseText);
  };
  xhr.onerror = function(e) {
    onerror(this.statusText);
  };
  xhr.open(type, url);
  if ('POST' == type) {
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhr.send(data);
  } else {
    xhr.send();
  }
};

// send error to comm.c
function resultError(messageType, err) {
  console.log(err);
  var result = {
    'AppKeyMessageType': messageType,
    'AppKeyResultError': err
  };

  Pebble.sendAppMessage(result, function(e) {
    console.log('Result info sent to Pebble successfully!');
  }, function(e) {
    console.log('Error sending result info to Pebble!');
  });
}

// search API
function search(pos) {

  var oauthToken = localStorage.getItem('oauth_token');
  if (!oauthToken) {
    resultError(messageTypeSearchVenue, "Authentication error");
    return;
  }

  var url = "https://api.foursquare.com/v2/venues/search?oauth_token=" + oauthToken + "&ll=" + pos.coords.latitude + "," + pos.coords.longitude + "&limit=20&v=20160301&m=swarm";

  // send request
  request(url, 'GET', null, function(status, responseText) {
    if (status != 200) {
      resultError(messageTypeSearchVenue, "Connection error");
      return;
    }

    var json = JSON.parse(responseText);
    if (json.meta.code != 200) {
      resultError(messageTypeSearchVenue, "API error");
      return;
    }

    var venues = json.response.venues;
    var venueCount = venues.length;

    var result = {
      'AppKeyMessageType': messageTypeSearchVenue,
      'AppKeyVenueCount': venueCount
    };

    for (var i = 0; i < venueCount; i++) {
      var venue = venues[i];
      var keyId = "AppKeyVenueId" + i;
      var keyName = "AppKeyVenueName" + i;
      var keyDistance = "AppKeyVenueDistance" + i;
      result[keyId] = venue.id;
      result[keyName] = venue.name;
      result[keyDistance] = venue.location.distance;
    }

    Pebble.sendAppMessage(result, function(e) {
      console.log('Result info sent to Pebble successfully!');
    }, function(e) {
      console.log('Error sending result info to Pebble!');
    });
  }, function(error) {
    console.log(error);
  });
}

// check in API
function checkIn(id) {

  var oauthToken = localStorage.getItem('oauth_token');
  if (!oauthToken) {
    resultError(messageTypeCheckIn, "Authentication error");
    return;
  }

  var data = "oauth_token=" + oauthToken + "&venueId=" + id + "&broadcast=public,twitter&v=20160301&m=swarm";

  var url = "https://api.foursquare.com/v2/checkins/add";
  request(url, 'POST', data, function(status, responseText) {
    if (status != 200) {
      resultError(messageTypeCheckIn, "Connection error");
      return;
    }

    var json = JSON.parse(responseText);
    if (json.meta.code != 200) {
      resultError(messageTypeCheckIn, "API error");
      return;
    }

    var result = {
      'AppKeyMessageType': messageTypeCheckIn
    };

    Pebble.sendAppMessage(result, function(e) {
      console.log('Result info sent to Pebble successfully!');
    }, function(e) {
      console.log('Error sending result info to Pebble!');
    });
  });
}

// Export
module.exports.search = search;
module.exports.checkIn = checkIn;

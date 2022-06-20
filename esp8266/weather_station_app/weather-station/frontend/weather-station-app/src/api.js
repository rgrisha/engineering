const API = {
  dev: {
    lastData: function() {
      const devData = 
        {"dt":"2018-03-04 21:21:23","measurements":[{"temp":-3.95,"pressure":984.37,"humidity":51.00}]};
      return new Promise(function(resolve, reject) {
        setTimeout(resolve, 100, devData);
      });
    },
    history: function(span) {
      const devData = [{"hour":"20","temp":0.13,"pressure":984.03,"humidity":39.89},
                      {"hour":"21","temp":-3.58,"pressure":984.41,"humidity":49.27},
                      {"hour":"22","temp":-3.63,"pressure":984.73,"humidity":55.23},
                      {"hour":"23","temp":-3.92,"pressure":984.80,"humidity":56.56}];
          
      return new Promise(function(resolve, reject) {
        setTimeout(resolve, 100, devData);
      });
    }
  },

  prod: {
    lastData: function() {
      return fetch('/api/last-measurements')
            .then((response) => response.json());
    },
    history: function(span) {
      //return fetch(`/api/measurements?${span}`)
      return fetch(`/api/measurements?${span}`)
        .then((response) => response.json())
    }
 
  }

}

export default API.prod


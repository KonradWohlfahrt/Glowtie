const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1.0, user-scalable=0, minimum-scale=1.0, maximum-scale=1.0" />
  </head>
  <body>
    <h1 id="vcc" style="text-align:center;">---- mV</h1>
    <h1 id="avg" style="text-align:center;">---- mV (avg)</h1>
    <br />
	  <script>
      setInterval(function(){getData('vcc', 'vcc');getData('avg', 'avg');}, 1000);
      function getData(elementID, url){
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200)
            document.getElementById(elementID).innerHTML = this.responseText;
        };
        xhttp.open('GET', url, true);
        xhttp.send();
      }
    </script>
  </body>
</html>)rawliteral";
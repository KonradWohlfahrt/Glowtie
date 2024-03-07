const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
  <head>
    <title>Glowtie</title>
    <style>
      body{ 
        background-color:#000000; 
        font-family:"Courier New"; 
        color:#ffffff;
      }
      .button{
      	width:150px;
        height:50px;
        background-color:gold;
        color:black;
        position:relative;
        margin-left:25px;
        margin-top:20px;
        font-family:'Courier New', Courier, monospace;
        font-size:20px;
        border:0px;
        border-radius:10px;
      }
      .slider{
        appearance:none;
        width:100%;
        margin:auto;
        margin-bottom:20px;
        margin-top:5px;
        background:#000000;
        height:25px;
        outline:none;
        border-radius:15px;
      }
      #sliderCont{
        width:70%;
        margin:auto;
      }
      #colorUpdate{
        background-color:black;
        width:150px;
        height:30px;
        margin:auto;
        text-align:center;
        padding:20px;
        border-radius:15px;
        font-family:'Courier New', Courier, monospace;
      }
      .grid{
        display: inline-grid;
        column-gap: 20px;
        row-gap: 5px;
        grid-template-columns: repeat(2, 1fr);
  
      }
      .container{
        display:inline-block;
        position:relative;
        padding-left:50px;
        padding-top:5px;
        margin-bottom:30px;
        cursor:pointer;
        font-size:18px;
        font-family:"Courier New";
        font-weight:bold;
        color:white;
        -webkit-user-select:none;
        -moz-user-select:none;
        -ms-user-select:none;
        user-select:none;
      }
      /*Hide default radio button*/
      .container input{
        position:absolute;
        opacity:0;
        cursor:pointer;
      }
      .checkmark{
        position:absolute;
        top:0;
        left:0;
        height:40px;
        width:40px;
        background-color:#2c3e50;
        border-radius:10px;
      }
      .container input:checked ~ .checkmark{
        background-color:gold;
      }
      .checkmark:after{
        content:"";
        position:absolute;
        display:none;
      }
      .container input:checked ~ .checkmark:after{
        display:block;
      }
      .container .checkmark:after{
        top:15px;
        left:10px;
        width:20px;
        height:10px;
        border-radius:3px;
        background:black;
      }
    </style>
    <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1.0, user-scalable=0, minimum-scale=1.0, maximum-scale=1.0" />
  </head>
  <body>
    <h1 id="colorUpdate">Glowtie</h1>
    <br />
    <h2 style="text-align:center;">Battery Voltage:</h2>
    <h2 id="vcc" style="text-align:center;"></h2>
    <br />
    <form action="" name="settings" id="settings" method="post">
      <div id="sliderCont">
        RED<br />
        <input type="range" id="redS" class="slider" min="0" max="255" value="127" name="red" onchange="updateColor()" style="border: 3px solid red;"><br />
        GREEN<br />
        <input type="range" id="greenS" class="slider" min="0" max="255" value="127" name="green" onchange="updateColor()" style="border: 3px solid green;"><br />
        BLUE<br />
        <input type="range" id="blueS" class="slider" min="0" max="255" value="127" name="blue" onchange="updateColor()" style="border: 3px solid blue;"><br />
      </div>
      <br />
      <div style="margin-left:5px">
      	<div class="grid">
          <label class="container">Off
            <input type="radio" name="mode" value="0">
            <span class="checkmark"></span>
          </label>
          <label class="container">Solid
            <input type="radio" checked="checked" name="mode" value="1">
            <span class="checkmark"></span>
          </label>
          <label class="container">Tie
            <input type="radio" name="mode" value="2">
            <span class="checkmark"></span>
          </label>
        </div>
        
        <hr />
        	
        <div class="grid">
          <label class="container">Breathe
        	<input type="radio" name="mode" value="3">
            <span class="checkmark"></span>
          </label>
          <label class="container">Pulse
            <input type="radio" name="mode" value="4">
            <span class="checkmark"></span>
          </label>
          <label class="container">Bar
            <input type="radio" name="mode" value="5">
            <span class="checkmark"></span>
          </label>
          <label class="container">Burst in
            <input type="radio" name="mode" value="6">
            <span class="checkmark"></span>
          </label>
          <label class="container">Burst out
            <input type="radio" name="mode" value="7">
            <span class="checkmark"></span>
          </label>
        </div>
        
        <hr />
        
        <div class="grid">
          <label class="container">Infinity
            <input type="radio" name="mode" value="8">
            <span class="checkmark"></span>
          </label>
          <label class="container">Infinity Fill
            <input type="radio" name="mode" value="9">
            <span class="checkmark"></span>
          </label>
          <label class="container">Chaser
            <input type="radio" name="mode" value="10">
            <span class="checkmark"></span>
          </label>
          <label class="container">Chaser Fill
            <input type="radio" name="mode" value="11">
            <span class="checkmark"></span>
          </label>
          <label class="container">Circles
            <input type="radio" name="mode" value="12">
            <span class="checkmark"></span>
          </label>
          <label class="container">Circles Fill
            <input type="radio" name="mode" value="13">
            <span class="checkmark"></span>
          </label>
          <label class="container">Symmetry
            <input type="radio" name="mode" value="14">
            <span class="checkmark"></span>
          </label>
          <label class="container">Symmetry Fill
            <input type="radio" name="mode" value="15">
            <span class="checkmark"></span>
          </label>
        </div>
        
        <hr />
        
        <div class="grid">
          <label class="container">Starfield
            <input type="radio" name="mode" value="16">
            <span class="checkmark"></span>
          </label>
          <label class="container">Rainbow
            <input type="radio" name="mode" value="17">
            <span class="checkmark"></span>
          </label>
          <label class="container">Filler
            <input type="radio" name="mode" value="18">
            <span class="checkmark"></span>
          </label>
        </div>
        <br/>
        <button class="button" onclick="document.forms['settings'].submit()" type="button">UPDATE</button>
      </div>
    </form>
	<script>
      getData('redS','red');
      getData('greenS','green');
      getData('blueS','blue');
      setInterval(function(){getData('vcc', 'vcc');}, 2500);
      function updateColor(){
        var red = document.forms['settings'].red.value;
        var green = document.forms['settings'].green.value;
        var blue = document.forms['settings'].blue.value;
        var rgb = blue | (green << 8) | (red << 16);
        var color = "#" + (0x1000000 + rgb).toString(16).slice(1);
        document.getElementById("colorUpdate").style.backgroundColor = color;
      }
      function getData(elementID, url){
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200)
            if (url!='vcc'){
              document.getElementById(elementID).value = this.responseText;
              updateColor();
            }
            else
              document.getElementById(elementID).innerHTML = this.responseText;
        };
        xhttp.open('GET', url, true);
        xhttp.send();
      }
    </script>
  </body>
</html>)rawliteral";
#include <Arduino.h>
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1"; charset="UTF-8">
  
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
	table, th, td{
	margin: auto;
	border:1px solid black;
	}
  table {
    border-collapse: collapse;
  }
  body
  {
  background-color:DarkSalmon;
    }
  input ,select {
  background-color:LightSalmon;
}
.switch {position: relative; display: inline-block; width: 100px; height: 30px} 
.switch input {display: none}
.slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: LightSalmon; border-radius: 6px}
.slider:before {position: absolute; content: ""; height: 26px; width: 26px; left: 8px; bottom: 2px; background-color: salmon; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
input:checked+.slider {background-color: LightSalmon}
input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Configuration Capteur Sniffair</h2>
<table>
  <tr>
    <th>Température</th>
    <th>Humidité</th>
    <th>Pression atmosphérique</th>
    <th>CO2</th>
    <th>Date</th>
    <th>Heure</th>
  </tr>
  <tr>
    <td>%temperature%</td>
    <td>%humidite%</td>
    <td>%pression%</td>    
    <td>%co2%</td>
	<td>%date%</td>
    <td>%heure%</td>
  </tr>
</table>
<br>  
<form action="/">
  <label for="fname">Numéro de salle:</label><br>
  <input type="text" id="numsale" name="fname" value="%numsalle%"><br>
  <label for="lname">Numéro de sonde:</label><br>
  <input type="text" id="numsonde" name="lname" value="%numsonde%"><br>
  <label for="commentaire">commentaire:</label><br>
  <input type="text" id="commentaire" name="commentaire" value=""><br><br>
  <label for="wifiap">Sélectionner le réseau wifi :</label>
  <select id="reseauwifi" name="reseauwifi">
 %LISTRESEAUWIFI%
  </select>
  <br><br>
  <input type="submit" value="Envoyer">
</form>
%BUTTONPLACEHOLDER%

<br>
<div id="etat">
%etat%
</div>
<script>
function toggleCheckbox(element)
 {
   
   var element_etat = document.getElementById("etat");
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = alertContents;
  if(element.checked)
   { xhr.open("GET", "/calibration?calibrate="+element.id+"&state=1", true); }
  else 
   { xhr.open("GET", "/calibration?calibrate="+element.id+"&state=0", true); }
  xhr.send();

    function alertContents() {
    if (xhr.readyState === xhr.DONE) {
      if (xhr.status === 200) {
        //alert(xhr.responseText);
        element_etat.innerHTML=xhr.responseText;
      } else {
        alert('Il y a eu un problème avec la requête.');
      }
    }
  }

}
</script>
</body>
</html>
)rawliteral";
/*

*/
const char reseau_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1"; charset="UTF-8">
  
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
	table, th, td{
	margin: auto;
	border:1px solid black;
	}
  table {
    border-collapse: collapse;
  }
  body
  {
  background-color:DarkSalmon;
    }
  input ,select {
  background-color:LightSalmon;
}
.switch {position: relative; display: inline-block; width: 100px; height: 30px} 
.switch input {display: none}
.slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: LightSalmon; border-radius: 6px}
.slider:before {position: absolute; content: ""; height: 26px; width: 26px; left: 8px; bottom: 2px; background-color: salmon; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
input:checked+.slider {background-color: LightSalmon}
input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Sélection réseau wifi</h2>
<br>  
<form action="/">
  <label for="wifiap">Sélectionner le réseau wifi :</label>
  <select id="reseauwifi" name="reseauwifi">
 %LISTRESEAUWIFI%
  </select>
  <br><br>
  <input type="submit" value="Envoyer">
</form>
<br>
<script>
function toggleCheckbox(element)
 {
   
   var element_etat = document.getElementById("etat");
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = alertContents;
  if(element.checked)
   { xhr.open("GET", "/calibration?calibrate="+element.id+"&state=1", true); }
  else 
   { xhr.open("GET", "/calibration?calibrate="+element.id+"&state=0", true); }
  xhr.send();

    function alertContents() {
    if (xhr.readyState === xhr.DONE) {
      if (xhr.status === 200) {
        //alert(xhr.responseText);
        element_etat.innerHTML=xhr.responseText;
      } else {
        alert('Il y a eu un problème avec la requête.');
      }
    }
  }

}
</script>
</body>
</html>
)rawliteral";
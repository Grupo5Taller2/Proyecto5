function control(action) {
	//ES UN FORMULARIO ENTONCES TIENE LO SIguiente
// crear una nueva peticion
  var xhttp = new XMLHttpRequest(); 
//abre la coneccion de la peticion 
//Post envia informacion al servidor.
//control es el url del archivo que se quiere comunicar 
  xhttp.open("POST", "/control", true);
//agrega valores al header que se desea enviar por defecto se escribe asi
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
//envia la peticion al servidor
  xhttp.send("action=" + action); 
}

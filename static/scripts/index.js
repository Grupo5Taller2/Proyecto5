function loadIndex() {
    $.get("/access", function(data) {	

        if(data.error!= "Sin conexion"){

        	/*CONEXION*/

            if(data.error == 'Perdida de conexion'){
                $('#error').removeClass("alert-success");
                $('#error').addClass("alert-danger");
                $('#error').text(data.error);
            }
            else{

            	/*ESTADOS*/

                if(data.state == "Avanzando")
                    $('#state').addClass("text-success");
                else if(data.state == "Retrocediendo")
                    $('#state').addClass("text-danger");
                else{
                    $('#state').removeClass("text-danger");
                    $('#state').removeClass("text-success");
                }
            

               // $('#marcha').text("Marcha: " + data.marcha);

               // SE ACTUALIZA EL ESTADO EN PANTALLA

                $('#state').text("Estado: " + data.state);

               


                /*RUEDA A*/


                if(data.A_distance> 75){
                    $('#A_distance').removeClass("text-danger");
                    $('#A_distance').removeClass("text-warning");
                    $('#A_distance').addClass("text-success");
                }
                else if(data.A_distance> 50){
                    $('#A_distance').removeClass("text-danger");
                    $('#A_distance').removeClass("text-success");
                    $('#A_distance').addClass("text-warning");
                }
                else{
                    $('#A_distance').removeClass("text-success");
                    $('#A_distance').removeClass("text-warning");
                    $('#A_distance').addClass("text-danger");
                }
                

                  // SE ACTUALIZA EL RPMA EN PANTALLA

                if(data.A_distance> 400){
                    $('#A_distance').text("Velocidad de A: > 400 RPM");
                }
                else
                    $('#A_distance').text("Velocidad de A: " + data.A_distance+ "RPM");



				
				/*RUEDA B*/


                if(data.B_distance > 75){
                    $('#B_distance').removeClass("text-danger");
                    $('#B_distance').removeClass("text-warning");
                    $('#B_distance').addClass("text-success");
                }
                else if(data.B_distance > 50){
                    $('#B_distance').removeClass("text-danger");
                    $('#B_distance').removeClass("text-success");
                    $('#B_distance').addClass("text-warning");
                }
                else{
                    $('#B_distance').removeClass("text-success");
                    $('#B_distance').removeClass("text-warning");
                    $('#B_distance').addClass("text-danger");
                }



                  // SE ACTUALIZA EL RPMB EN PANTALLA

                if(data.B_distance > 400){
                    $('#B_distance').text("Velocidad de B: > 400 RPM");
                }
                else
                    $('#B_distance').text("Velocidad de B: " + data.B_distance + "RPM");
            }
        }



    });
}

$( document ).ready(function () {
    setInterval(loadIndex, 500);//TIEMPO DE CAMBIO
});





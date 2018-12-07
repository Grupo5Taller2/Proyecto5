from flask import Flask, session, redirect, url_for, escape, request, render_template
from hashlib import md5
from flask import jsonify
import MySQLdb
import socket
import math
import datetime

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Connect the socket to the port where the server is listening
server_address = ('192.168.4.1', 1024)
#error = ''
#inform = ''
direction = 0
turn = 0
speed = 0
aux = ""

app = Flask(__name__)

if __name__ == '__main__':
    db = MySQLdb.connect(host="localhost", user="root", passwd="root", db="autolinearecta")
    cur = db.cursor()

class ServerError(Exception):pass  #retorna las validaciones correspondientes



@app.errorhandler(404)
def page_not_found(e):
    return redirect(url_for('index'))



#comunicacion con el auto
@app.route('/')
def index():
    global sock
    global aux
    aux = "PARA COMENZAR CONECTE EL AUTITO"
  
    error = request.args.get("error") 
    if not error:
        error = "Usted no esta conectado"
        return render_template('index.html', error=error, state='Desconectado', direction=0, marcha=0 , turn=0, B_distance=0, A_distance=0)
    else:
        inform = request.args.get("inform")
        return render_template('index.html', inform=inform, state='Desconectado', direction=0, marcha=0, turn=0, B_distance=0, A_distance=0)


#comunicacion con el autoooo
   
@app.route("/access")
def access():
    global sock
    global turn
    global direction
    global speed
   
    RPMA= ''  #RPMA
    RPMB = '' #RPMB
    A_distance = 0  # variable que sirve para almacenar la conversion a integer de f_distance  RPMA
    B_distance = 0   # variable que sirve para almacenar la conversion a integer de b_distance  RPMB
    state = 'Desconectado'

    RPMdif = '' #RPMB
    dif_distance = 0     # variable que sirve para almacenar la conversion a integer de RPMdif
    
    try:
        sock.settimeout(3) # un tiempo para bloquear operaciones, si no llega ninguna operacion
        sock.sendall('1'); #Envio un pedido de informacion, es decir un solo byte envio
        recibido = sock.recv(15)  #recive un paquete de bytes de los datos que le envia el arduino
        j = 0
        for i in range(0, 6):
            while recibido[j] != '/':
                if i == 0:
                    RPMA = RPMA + recibido[j]
                elif i == 1:
                    RPMB = RPMB + recibido[j]
                elif i == 2:  # Analisis del avance
                    direction = int(recibido[j]) 
                    if recibido[j] == '0': 
                        state = 'Parado'
                    elif recibido[j] == '1':
                        state = 'Avanzando'
                    elif recibido[j] == '2':
                       state = 'Retrocediendo'               
                elif i == 3:  # Analisis de la velocidad
                    speed = int(recibido[j]) 
                elif i== 4:  # Analisis de la direccion(si es cero es porque va hacia adelante o hacia atras)
                    turn = int(recibido[j])
                    if recibido[j] == '1':
                        state = 'Izquierda'
                    elif recibido[j] == '2':
                        state = 'Derecha' 
                else:  #la diferencia de velocidades
                     RPMdif =  RPMdif + recibido[j]
                   

                j=j+1
            j=j+1
        #Verificamos que la marcha sea 0 para indicar que el vehiculo esta parado
        if speed == 0:
            state = 'Parado'
            A_distance = 0
            B_distance = 0
            dif_distance = 0


        A_distance = int(RPMA)
        B_distance = int(RPMB)
        dif_distance = int(RPMdif)
        #Agregamos los eventos, si son de importancia
        fecha = datetime.date.today()
        hora = str(datetime.datetime.now().time())
        hora = hora.split(".")[0] 
       


        if state == "Avanzando":
            cur.execute('INSERT INTO eventos(fecha, hora, ruedA, ruedB, dif, type) VALUES (%s,%s,%s,%s,%s,%s)', ([fecha], [hora], [A_distance], [B_distance], [dif_distance], [state]))
            db.commit()

        if state == "Retrocediendo":
            cur.execute('INSERT INTO eventos(fecha, hora, ruedA, ruedB, dif, type) VALUES (%s,%s,%s,%s,%s,%s)', ([fecha], [hora], [A_distance], [B_distance], [dif_distance], [state]))
            db.commit()
       


    except socket.error as socketerror:
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        error = 'Perdida de conexion'
        print(error)
        A_distance
        result = {"error": error}
        return jsonify(result)



    result = {"error": "Ninguno", "state": state, "direction": direction, "marcha": speed, "turn": turn, "A_distance": A_distance, "B_distance": B_distance}
    return jsonify(result)



@app.route('/control', methods=['GET', 'POST'])
def control(): 
    global turn
    global direction
    global speed 
    if request.method == 'POST':
       # username_session = escape(session['username']).capitalize()
        #username_session = username_session.lower()
        #cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username_session])
        #Si estoy conectado puedo utilizarlo sino no
        #if cur.fetchone()[0]:
        action = request.form['action']
        if action == "Frenar":
            direction = 0
            speed = 0
            turn = 0
        elif action == "Marcha A":
            if turn !=0 or direction !=1:
                speed = 1
            elif speed < 3:
                speed = speed + 1
        elif action == "Marcha D":
            if turn !=0 or direction !=1:
                speed = 1
            elif speed>0:
                speed = speed - 1
        elif action == "Avanzar":
            direction = 1
            speed = 1
            turn = 0
        elif action == "Izquierda": 
            direction = 1
            speed = 1
            turn = 1
        elif action == "Derecha":
            direction = 1
            speed = 1
            turn = 2
        elif action == "Retroceder":
            direction = 2
            speed = 1
            turn = 0
        sock.sendall(str(direction) + str(speed) + str(turn)); #Envio un pedido de comando, tres bytes (VER SI FUNCIONA)
    return redirect(url_for('index'))


@app.route('/faqs')
def faqs():
   return render_template('faqs.html')



@app.route('/vista', methods=['GET', 'POST'])
def vista():
    if (aux == 'Error al conectarse'):
        return render_template('vista.html',error = aux,A_distance= 0,B_distance=0)
    else:
        return render_template('vista.html',inform = aux,A_distance= 0,B_distance=0)


#coneccion con el auto
@app.route('/connect')
def connect():
    global sock
    global aux
    try:   
            sock.connect(server_address)           
            aux = 'Conexion exitosa'
            return redirect(url_for('vista', inform=aux))
    except socket.error as socketerror:
            print(socketerror)
            aux = 'Error al conectarse'
            sock.close()
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            return redirect(url_for('vista', error=aux))


""" Sirve para ir para atras cuando presionno en la 
barra espaciadora desplegiable """

#coneccion con el auto
@app.route('/logout')
def logout():
    global sock
    

    sock.close()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    return render_template('index.html') 



@app.route('/stats')
def stats():
   
    cur.execute('SELECT * FROM eventos ORDER BY hora DESC LIMIT 10;')
    lista=[row for row in cur.fetchall()]
    if lista:
        return render_template('stats.html', eventos=lista)
    
    else:
 
        return render_template('stats.html')



if __name__ == '__main__':
    app.run(debug = True ,host='localhost', port=8000) #modo debug activo

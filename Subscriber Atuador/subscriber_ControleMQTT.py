from datetime import datetime
import sys
import paho.mqtt.client as mqtt
import sqlite3
import pymysql

broker = "ip"
port = "port"
keppAlive = 60
topic = 'controleAc/Disnel/temp_est_bd'
	
def insereDados_Controle(self,id,data,horario,temp,estado): # insere dados na tabela registros
	# Abrimos uma conexão com o banco de dados:
	c = pymysql.connect(db='banco', user='usuario', passwd='senha',port=3360)

	# Cria um cursor:
	cursor = c.cursor()

	# Executa o comando:
	#import pdb; pdb.set_trace()

	cmd = "INSERT INTO registros_controle (id_node,dia,horario,temperatura,estado) VALUES ('"+id+"','"+data+"','"+horario+"','"+temp+"','"+estado+"')"
	cursor.execute(cmd)

	# Efetua um commit no banco de dados.
	# Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
	# suas alterações.
	c.commit()

	# fecha conexao
	c.close()

# funcao responsavel por cadastrar no banco de dados a temperatura do sensor
def insertTemperature(estado,temp):
	data_e_hora_atuais = datetime.now()
	dh = data_e_hora_atuais.strftime("%d/%m/%Y %H:%M:%S").split()
    insereDados_Controle(str(4),dh[0],dh[1],temp,estado)


def on_connect(client, userdata, flags, rc):
    client.subscribe(topic)


def on_message(client, userdata, msg):
    message = str(msg.payload) # converte a mensagem recebida
    try :
        param = message[2:6].split(" ")
        print(param)
        insertTemperature(param[0],param[1])# invoca o metodo de cadastro passando por parametro a temperatura recebida
    except Exception as e:
        print(message[2:6].split(" "),e)
        exit(0)

try:
    print("Controle MQTT...")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    # client.connect(broker, port, keppAlive)
    client.connect(broker)
    client.username_pw_set("mqtt_user", "mqtt_ufpi")
    client.loop_forever()

except KeyboardInterrupt:
    print ("\nScript finalizado.")
    sys.exit(0)

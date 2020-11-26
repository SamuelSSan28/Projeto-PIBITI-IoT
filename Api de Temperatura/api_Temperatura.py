from datetime import datetime
import pymysql
import requests
import json
from multiprocessing import Queue
import mechanize

def diff_tempo(ultimo_registro):
    time_stamp = datetime.strptime(ultimo_registro, '%d/%m/%Y %H:%M:%S')
    data_e_hora_atuais = datetime.now()
    dif = (data_e_hora_atuais - time_stamp)
    return  dif.total_seconds()

def conexaoBD(self): #retorna uma conexao com o BD
    return pymysql.connect(db='banco', user='usuario', passwd='senha',port=3360)


def clima():
    '''
    :Function: Faz um GET com a API pra receber as informações de Teresina
    :return: Retorna uma lista cotendo o clima e a temperatura(°c)
    '''
    req = "https://api.weatherbit.io/v2.0/current?city=Teresina,BR&key=89ff6003e58e45e69116346b6df12f54"
    try:
        resp = requests.get(req)
        print(resp,end = '   ')
        r =  json.loads(resp.text)
        c = [r['data'][0]['temp'] ,r['data'][0]['rh'],r['data'][0]['app_temp'],r['data'][0]['ob_time']]

        return c
    except Exception as err:
        d,h = dataHora()
        e = "Error: {0} no dia ".format(err) + d + " as " +h +"\n"
        arq = open('log_api.txt', 'a+')
        arq.write(e)
        arq.close()
        return None

def dataHora():
    '''
    :return: Retorna a data e a hora do PC no momento no formato string
    '''
    data_e_hora_atuais = datetime.now()
    return data_e_hora_atuais.strftime("%d/%m/%Y %H:%M:%S").split()

def insereTempAPI(data,horarioM,horarioR,temp,humi,sensTermic): # insere dados na tabela registros
    # Abrimos uma conexão com o banco de dados:
    c = conexaoBD()

    # Cria um cursor:
    cursor = c.cursor()

    # Executa o comando:
    sql  = "INSERT INTO registros_temp_cidade (temperatura,humidade,sensacao_termica,dia,horario_medicao,horario_registro) VALUES (%s,%s,%s,%s,%s,%s)"
    val = [temp,humi,sensTermic,data,horarioM,horarioR]
    cursor.execute(sql,val)


    # Efetua um commit no banco de dados.
    # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
    # suas alterações.
    c.commit()

    # fecha conexao
    c.close()


#-------------------------main---------------------------------
print("API On")
atual = dataHora()
first = True
while True:
    try:
        if diff_tempo(atual[0] + ' ' + atual[1]) >= 3600 or first: #Script de Autenticação Automática
            br = mechanize.Browser()
            br.set_handle_robots(False)
            br.addheaders = [('User-agent', 'Firefox')]
            url = "https://login.ufpi.br:6082/php/uid.php?vsys=1&rule=0&url=http://conecta.ufpi.br%2f"
            br.open(url)
            br.select_form(nr=0)
            br.form['user'] = 'samuelssan28'
            br.form['passwd'] = 'sam28muel'
            br.submit()
            first = False

        if diff_tempo(atual[0]+' '+atual[1]) >= 1800:
            temp,humi,sensacao,horarioM = clima()
            atual = dataHora()
            if temp != None and humi != None  and sensacao != None  and horarioM != None:
                insereTempAPI(atual[0],horarioM,atual[1],temp,humi,sensacao) 
                print(atual)    

    except Exception as err:
        d,h = dataHora()
        e = "Error: {0} no dia ".format(err) + d + " as " +h +"\n"
        arq = open('log_api.txt', 'a+')
        arq.write(e)
        arq.close()
        continue

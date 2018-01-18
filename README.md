			Asterisk ESP8266: Possibilidade de acionar uma fechadura eletrônica ou qualquer outro dispositivo através de um ramal do Asterisk. O Esp 8266 comunica-se com o asterisk pelo mosquitto.


			###########################
			        Installation
			###########################

#echo 'deb http://repo.mosquitto.org/debian jessie main' >> /etc/apt/sources.list
wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
apt-key add mosquitto-repo.gpg.key
rm mosquitto-repo.gpg.key
apt-get update 
apt-get install mosquitto 
apt-get install mosquitto-clients 
service mosquitto start

			#####################
			 Configuration  mqtt 
			#####################

MOSQUITTO_USER="asterisk"
MOSQUITTO_PASSWD="2018_pwd@@"
/etc/init.d/mosquitto stop
echo 'allow_anonymous false' >> /etc/mosquitto/conf.d/default.conf
echo 'password_file /etc/mosquitto/passwd' >> /etc/mosquitto/conf.d/default.conf
touch /etc/mosquitto/passwd
mosquitto_passwd -b /etc/mosquitto/passwd $MOSQUITTO_USER $MOSQUITTO_PASSWD
/etc/init.d/mosquitto start

			#################
			  extension.conf
			#################

[fechadura]
exten => _5,1,Noop(Acionado Fechadura Eletrica)  
same => n,Set(${SHELL(mosquitto_pub -h localhost -t "fechadura" -u asterisk -P 2018_pwd@@ -m "on" -d)});
same => n,Hangup()

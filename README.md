# Gateway-SPC58EC-Discovery
proyecto completo
https://mega.nz/folder/GeoH0AqC#7dsrBSL63ISXiZdSeBxODg

Pruebas de comunicación Ethernet

Datos:
Address IP: 192.168.1.2 
puerto: 1500
Gateway: 192.168.1.254
DNS server:192.168.1.254
Mask:255.255.255.0



1. se le asigno una ip al puerto del PC usado

$ sudo ifconfig enp1s0 192.168.1.1


Se verifica con el comando ifconfig-a la actualización de la dirección


2.para poder verificar la comunicación con el dispositivo gateway se empleo un comando ping


$ping 192.168.1.2

Este respondio, por lo tanto poseia comunicaciรณn con el pc


3.posteriormente con el siguiente comando se establecio una comunicaciรณn de escucha con el dispositivo

$netcat -lu 192.168.1.1 1500

Al reiniciar el dispositivo gateway, este debe enviar información de su conexión ethernet



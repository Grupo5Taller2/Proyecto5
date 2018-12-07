#include <Arduino.h>
#include "L9110.h"

/*
*
*  LIBRERIA DE CONTROL DE MOTORES
*  CREADO POR :   AGUILAR SERGIO, AILIGO ORIANA 
*  FECHA :        13-10-2018
*  
*/

L9110::L9110(int aia,int aib,int bia,int bib){ // son los del puente H
	this->_aia=aia;
	this->_aib=aib;
	this->_bia=bia;
	this->_bib=bib;
	pinMode(_aia,OUTPUT);
	pinMode(_aib,OUTPUT);
	pinMode(_bia,OUTPUT);
	pinMode(_bib,OUTPUT);
	
	digitalWrite(_aia,LOW);
	digitalWrite(_aib,LOW);
	digitalWrite(_bia,LOW);
	digitalWrite(_bib,LOW);

}

/*
*  NOTA: 
*		aia y bia son para velocidad
*		bia y bib son para son para direccionamiento
*
*/

void L9110::adelante(int i,int d,int t){
	
	
	digitalWrite(_aib,LOW);
	digitalWrite(_bib,LOW);

	analogWrite(_aia,i);
	analogWrite(_bia,d);
	
	delay(t);
}

void L9110::atras(int i,int d,int t){
	
	digitalWrite(_aia,LOW);
	digitalWrite(_bia,LOW);
	
	analogWrite(_aib,i);
	analogWrite(_bib,d);
	
	delay(t);
}

void L9110::izquierda(int i,int d,int t){
	analogWrite(_aia,i);
	digitalWrite(_aib,LOW);	
	digitalWrite(_bia,LOW);
	analogWrite(_bib,d);
	delay(t);
}

void L9110::derecha(int i,int d,int t){
	digitalWrite(_aia,0);
	analogWrite(_bia,d);
	analogWrite(_aib,i);
	digitalWrite(_bib,0);
	delay(t);
}

void L9110::frenar(int t){
	analogWrite(_aia,0);
	analogWrite(_bia,0);
	
	digitalWrite(_aib,0);
	digitalWrite(_bib,0);
	delay(t);
}

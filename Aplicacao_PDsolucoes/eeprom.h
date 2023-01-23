/*
 * eeprom.h
 *
 * Created: 19/01/2023 10:58:21
 *  Author: andre
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#define QUANTIDADE_DADOS			20
#define QUANTIDADE_CADASTROS		30
#define ENDERECO_EVENTOS			600
#define QUANTIDADE_DADOS_EVENTOS	13	
void EepromWrite(unsigned int uiAddress, unsigned char ucData);
unsigned char EepromRead(unsigned int uiAddress);


#endif /* EEPROM_H_ */
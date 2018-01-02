/*
 * eeprom.h
 *
 *  Created on: 2017/11/01
 *      Author: Blue
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#define EEPROM_START_ADDRESS	(uint32_t)0x0800F800	// EEPROM emulation start address: Page 31

HAL_StatusTypeDef eeprom_enable_write(void);

HAL_StatusTypeDef eeprom_disable_write(void);

HAL_StatusTypeDef eeprom_write_halfword(uint32_t, uint16_t);

HAL_StatusTypeDef eeprom_write_word(uint32_t, uint32_t);

uint16_t eeprom_read_halfword(uint32_t);

uint32_t eeprom_read_word(uint32_t);


#endif /* EEPROM_H_ */

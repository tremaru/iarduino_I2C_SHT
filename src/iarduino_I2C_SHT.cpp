#include "iarduino_I2C_SHT.h"																									//
																																//
//		Инициализация модуля:																									//	Возвращаемое значение: результат инициализации.
bool	iarduino_I2C_SHT::_begin		(void){																					//	Параметр: отсутствует
		//	Инициируем работу с шиной I2C:																						//
			selI2C->begin();																									//	Инициируем работу с выбранной шиной I2C в качестве мастера.
		//	Если адрес не указан, то ищим модуль на шине I2C:																	//
			if(valAddrTemp==0){																									//
				for(int i=1; i<127; i++){																						//	Проходим по всем адресам на шине I2C
					if( selI2C->checkAddress(i)											){	valAddr=i; delay(2);				//	Если на шине I2C есть устройство с адресом i, то используем этот адрес для проверки найденного модуля...
					if(_readBytes(REG_MODEL,4)											){										//	Читаем 4 байта начиная с регистра «REG_MODEL» в массив «data».
					if( data[0]     == DEF_MODEL_SHT									){										//	Если у модуля с адресом i в регистре «MODEL»   (data[0]) хранится значение DEF_MODEL_SHT, то ...
					if((data[2]>>1) == i                 || data[2] == 0xFF				){										//	Если у модуля с адресом i в регистре «ADDRESS» (data[2]) хранится значение i (адрес+младший бит) или 0xFF (адрес не задавался), то ...
					if( data[3]     == DEF_CHIP_ID_FLASH || data[3] == DEF_CHIP_ID_METRO){										//	Если у модуля с адресом i в регистре «CHIP_ID» (data[3]) хранится значение DEF_CHIP_ID_FLASH (идентификатор модулей Flash), или DEF_CHIP_ID_METRO (идентификатор модулей Metro), то ...
						valAddrTemp=i; i=128;																					//	Считаем что модуль обнаружен, сохраняем значение i как найденный адрес и выходим из цикла.
					}}}}}																										//
				}																												//
			}																													//
		//	Если модуль не найден, то возвращаем ошибку инициализации:															//
			if( valAddrTemp == 0														){	valAddr=0; return false;}			//
		//	Проверяем наличие модуля на шине I2C:																				//
			if( selI2C->checkAddress(valAddrTemp) == false								){	valAddr=0; return false;}			//	Если на шине I2C нет устройств с адресом valAddrTemp, то возвращаем ошибку инициализации
			valAddr=valAddrTemp;																								//	Сохраняем адрес модуля на шине I2C.
		//	Проверяем значения регистров модуля:																				//
			if(_readBytes(REG_MODEL,4)==false											){	valAddr=0; return false;}			//	Если не удалось прочитать 4 байта в массив «data» из модуля начиная с регистра «REG_MODEL», то возвращаем ошибку инициализации.
			if( data[0]     != DEF_MODEL_SHT											){	valAddr=0; return false;}			//	Если значение  регистра «MODEL»   (data[0]) не совпадает со значением DEF_MODEL_SHT, то возвращаем ошибку инициализации.
			if((data[2]>>1) != valAddrTemp       && data[2] !=0xFF						){	valAddr=0; return false;}			//	Если значение  регистра «ADDRESS» (data[2]) не совпадает с адресом модуля и не совпадает со значением 0xFF, то возвращаем ошибку инициализации.
			if( data[3]     != DEF_CHIP_ID_FLASH && data[3] != DEF_CHIP_ID_METRO		){	valAddr=0; return false;}			//	Если значение  регистра «CHIP_ID» (data[3]) не совпадает со значением DEF_CHIP_ID_FLASH и DEF_CHIP_ID_METRO, то возвращаем ошибку инициализации.
			valVers=data[1];																									//	Сохраняем байт регистра «VERSION» (data[1]) в переменую «valVers».
		//	Перезагружаем модуль устанавливая его регистры в значение по умолчанию:												//
			reset();																											//	Выполняем программную перезагрузку.
			delay(1000);																										//
			return true;																										//	Возвращаем флаг успешной инициализаии.
}																																//
																																//
//		Перезагрузка модуля:																									//	Возвращаемое значение:	результат перезагрузки.
bool	iarduino_I2C_SHT::reset			(void){																					//	Параметр:				отсутствует.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Устанавливаем бит перезагрузки:																					//
				if(_readBytes(REG_BITS_0,1)==false){return false;}																//	Читаем 1 байт регистра «BITS_0» в массив «data».
				data[0] |= 0b10000000;																							//	Устанавливаем бит «SET_RESET»
				if(_writeBytes(REG_BITS_0,1)==false){return false;}																//	Записываем 1 байт в регистр «BITS_0» из массива «data».
			//	Переинициируем шину в связи с программным отключением подтяжек шины I2C в модуле:								//
				delay(10); selI2C->begin();																						//	Ждём восстановление подтяжек линий SCL/SDA и переинициируем работу с шиной I2C.
			//	Ждём установки флага завершения перезагрузки:																	//
				do{ if(_readBytes(REG_FLAGS_0,1)==false){return false;} }														//	Читаем 1 байт регистра «REG_FLAGS_0» в массив «data».
				while( (data[0]&0b10000000) == 0);																				//	Повторяем чтение пока не установится флаг «FLG_RESET».
				return true;																									//
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку
			}																													//
}																																//
																																//
//		Смена адреса модуля:																									//	Возвращаемое значение:	резульат смены адреса.
bool	iarduino_I2C_SHT::changeAddress	(uint8_t newAddr){																		//	Параметр:				newAddr - новый адрес модуля (0x07 < адрес < 0x7F).
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Проверяем новый адрес:																							//
				if(newAddr>0x7F){newAddr>>=1;}																					//	Корректируем адрес, если он указан с учётом бита RW.
				if(newAddr==0x00 || newAddr==0x7F){return false;}																//	Запрещаем устанавливать адрес 0x00 и 0x7F.
			//	Записываем новый адрес:																							//
				if(_readBytes(REG_BITS_0,1)==false){return false;}																//	Читаем 1 байт регистра «BITS_0» в массив «data».
				data[0] &= 0b11110111;																							//	Сбрасываем    бит «BLOCK_ADR».
				data[0] |= 0b00000010;																							//	Устанавливаем бит «SAVE_ADR_EN»
				if(_writeBytes(REG_BITS_0,1)==false){return false;}																//	Записываем 1 байт в регистр «BITS_0» из массива «data».
				data[0] = (newAddr<<1)|0x01;																					//	Готовим новый адрес к записи в модуль, установив бит «SAVE_FLASH».
				if(_writeBytes(REG_ADDRESS,1)==false){return false;}															//	Записываем 1 байт в регистр «ADDRESS» из массива «data».
				delay(200);																										//	Даём более чем достаточное время для применения модулем нового адреса.
			//	Проверяем наличие модуля с новым адресом на шине I2C:															//
				if(selI2C->checkAddress(newAddr)==false){return false;}															//	Если на шине I2C нет модуля с адресом newAddr, то возвращаем ошибку.
				valAddr     = newAddr;																							//	Сохраняем новый адрес как текущий.
				valAddrTemp = newAddr;																							//	Сохраняем новый адрес как указанный.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку
			}																													//
}																																//
																																//
//		Получение флага наличия подтяжки линий шины I2C:																		//	Возвращаемое значение:	флаг наличия подтяжки линий шины I2C.
bool	iarduino_I2C_SHT::getPullI2C		(void){																				//	Параметр:				отсутствует.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Считываем первые два регистра: регистр флагов и регистр битов:													//
				if(_readBytes(REG_FLAGS_0,2)==false ){ return false; }															//	Читаем 2 байта начиная с регистра «REG_FLAGS_0» в массив «data».
			//	Проверяем поддерживает ли модуль управление подтяжкой линий шины I2C:											//
				if( (data[0] & 0b00000100) == false ){ return false; }															//	Если флаг «FLG_I2C_UP» регистра «REG_FLAGS_0» сброшен, значит модуль не поддерживает управление подтяжкой линий шины I2C.
			//	Проверяем установлена ли подтяжка линий шины I2C:																//
				if( (data[1] & 0b00000100) == false ){ return false; }															//	Если бит  «SET_I2C_UP» регистра «REG_BITS_0»  сброшен, значит подтяжка линий шины I2C не установлена.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку.
			}																													//
}																																//
																																//
//		Установка подтяжки линий шины I2C:																						//	Возвращаемое значение:	результат установки подтяжки линий шины I2C.
bool	iarduino_I2C_SHT::setPullI2C		(bool f){																			//	Параметр:				флаг подтяжки линий шины I2C.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Считываем первые два регистра: регистр флагов и регистр битов:													//
				if(_readBytes(REG_FLAGS_0,2)==false ){ return false; }															//	Читаем 2 байта начиная с регистра «REG_FLAGS_0» в массив «data».
			//	Проверяем поддерживает ли модуль управление подтяжкой линий шины I2C:											//
				if( (data[0] & 0b00000100) == false ){ return false; }															//	Если флаг «FLG_I2C_UP» регистра «REG_FLAGS_0» сброшен, значит модуль не поддерживает управление подтяжкой линий шины I2C.
			//	Устанавливаем или сбрасываем бит включения подтяжки линий шины I2C:												//
				if(f){ data[0] = (data[1] |  0b00000100); }																		//	Если флаг «f» установлен, то копируем значение из 1 в 0 элемент массива «data» установив бит «SET_I2C_UP».
				else { data[0] = (data[1] & ~0b00000100); }																		//	Если флаг «f» сброшен   , то копируем значение из 1 в 0 элемент массива «data» сбросив   бит «SET_I2C_UP».
			//	Сохраняем получившееся значение в регистр «REG_BITS_0»:															//
				if(_writeBytes(REG_BITS_0,1)==false ){ return false; }															//	Записываем 1 байт в регистр «REG_BITS_0» из массива «data».
				delay(50);																										//	Даём время для сохранения данных в энергонезависимую память модуля.
				return true;																									//	Возвращаем флаг успеха.
			}else{																												//	Иначе, если модуль не инициализирован, то ...
				return false;																									//	Возвращаем ошибку
			}																													//
}																																//
																																//
//		Чтение температуры окружающей среды в °C:																				//	Возвращаемое значение:	температура от -40.0 до +125.0 °C.
float	iarduino_I2C_SHT::getTem		(void){																					//	Параметр:				отсутствует.
			float result=0; data[0]=0; data[1]=0;																				//	Определяем переменную для хранения результата чтения и сбрасываем в 0 первые два элемента массива «data».
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Читаем значение температуры:																					//
				_readBytes(REG_SHT_TEM_L,2);																					//	Читаем 2 байта из регистров «REG_SHT_TEM_L» и «REG_SHT_TEM_H» в массив «data».
				result=float(((int16_t)(data[1]&~SHT_TEM_NEGATIVE)<<8)|data[0])/10.0; if(data[1]&SHT_TEM_NEGATIVE){result*=-1;}	//	Объеденяем два прочитанных байта в одно число и добавляем знак.
			}																													//
			return result;																										//	Возвращаем результат чтения (-40.0 ... +125.0).
}																																//
																																//
//		Чтение относительной влажности воздуха в %:																				//	Возвращаемое значение:	влажность от 0 до 100 %.
float	iarduino_I2C_SHT::getHum		(void){																					//	Параметр:				отсутствует.
			float result=0;																										//	Определяем переменную для хранения результата чтения.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Читаем значение влажности:																						//
				_readBytes(REG_SHT_HUM_L,2);																					//	Читаем 2 байта из регистров «REG_SHT_HUM_L» и «REG_SHT_HUM_H» в массив «data».
				result = float( (((uint16_t)data[1])<<8) | data[0] )/10.0;														//	Объеденяем два прочитанных байта в одно число.
			}																													//
			return result;																										//	Возвращаем результат чтения (0...8191).
}																																//
																																//
//		Получение флага изменения температуры:																					//	Возвращаемое значение:	флаг указывающий на изменение температуры (true/false).
bool	iarduino_I2C_SHT::getTemChanged	(void){																					//	Параметр:				отсутствует.
			bool	 result = 0;																								//	Определяем переменную для хранения результата чтения.
			uint32_t time   = millis();																							//
			if(valAddr){																										//	Если модуль был инициализирован, то ...
				if( ( time > tmrReadFLG + tmrPeriodFLG ) || ( time < tmrReadFLG ) ){											//	Если с момента последнего опроса регистра флагов прошло более «tmrPeriodFLG» мс, или произошло переполнение «time», то ...
					tmrReadFLG = time;																							//
					if( _readBytes(REG_SHT_FLG,1) ){																			//	Читаем 1 байт из регистра «REG_SHT_FLG» в массив «data».
						val_REG_SHT_FLG	|= ( data[0] & SHT_FLG_CHANGED_TEM );													//	Устанавливаем флаг изменения температуры. Если бит «SHT_FLG_CHANGED_TEM» в переменной «val_REG_SHT_FLG» был установлен, то он и останется установленным!
						val_REG_SHT_FLG	|= ( data[0] & SHT_FLG_CHANGED_HUM );													//	Устанавливаем флаг изменения влажности.   Если бит «SHT_FLG_CHANGED_HUM» в переменной «val_REG_SHT_FLG» был установлен, то он и останется установленным!
					}																											//
				}																												//
			}																													//
			if( val_REG_SHT_FLG	& SHT_FLG_CHANGED_TEM ){ result=1; val_REG_SHT_FLG &= ~SHT_FLG_CHANGED_TEM; }					//	Возвращаемым значениям является флаг смены температуры.
			return result;																										//	Возвращаем результат.
}																																//
																																//
//		Получение флага изменения влажности:																					//	Возвращаемое значение:	флаг указывающий на изменение влажности (true/false).
bool	iarduino_I2C_SHT::getHumChanged	(void){																					//	Параметр:				отсутствует.
			bool	 result = 0;																								//	Определяем переменную для хранения результата чтения.
			uint32_t time   = millis();																							//
			if(valAddr){																										//	Если модуль был инициализирован, то ...
				if( ( time > tmrReadFLG + tmrPeriodFLG ) || ( time < tmrReadFLG ) ){											//	Если с момента последнего опроса регистра флагов прошло более «tmrPeriodFLG» мс, или произошло переполнение «time», то ...
					tmrReadFLG = time;																							//
					if( _readBytes(REG_SHT_FLG,1) ){																			//	Читаем 1 байт из регистра «REG_SHT_FLG» в массив «data».
						val_REG_SHT_FLG	|= ( data[0] & SHT_FLG_CHANGED_TEM );													//	Устанавливаем флаг изменения температуры. Если бит «SHT_FLG_CHANGED_TEM» в переменной «val_REG_SHT_FLG» был установлен, то он и останется установленным!
						val_REG_SHT_FLG	|= ( data[0] & SHT_FLG_CHANGED_HUM );													//	Устанавливаем флаг изменения влажности.   Если бит «SHT_FLG_CHANGED_HUM» в переменной «val_REG_SHT_FLG» был установлен, то он и останется установленным!
					}																											//
				}																												//
			}																													//
			if( val_REG_SHT_FLG	& SHT_FLG_CHANGED_HUM ){ result=1; val_REG_SHT_FLG &= ~SHT_FLG_CHANGED_HUM; }					//	Возвращаемым значениям является флаг смены влажности.
			return result;																										//	Возвращаем результат.
}																																//
																																//
//		Установка порога чувствительности к изменению температуры.																//	Возвращаемое значение:	результат установки чувствиетльности (true/false).
bool	iarduino_I2C_SHT::setTemChange	(float tem){																			//	Параметр:				температура в °С (от 0.1 до 25.5).
			if(tem <  0.1){ tem =  0.1; }																						//
			if(tem > 25.5){ tem = 25.5; }																						//
			bool	result=0;																									//	Определяем переменную для хранения результата записи.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Записываем новое значение чувствительности:																		//
				data[0] = uint8_t(tem*10);																						//	Готовим байт данных для пережачи.
				result = _writeBytes(REG_SHT_TEM_CHANGE,1);																		//	Записываем 1 байт в регистр «REG_SHT_TEM_CHANGE» из массива «data».
			}																													//
			return result;																										//	Возвращаем флаг указывающий на результат записи (true/false).
}																																//
																																//
//		Установка порога чувствительности к изменению влажности.																//	Возвращаемое значение:	результат установки чувствиетльности (true/false).
bool	iarduino_I2C_SHT::setHumChange	(float hum){																			//	Параметр:				влажность в % (от 0.1 до 25.5).
			if(hum <  0.1){ hum =  0.1; }																						//
			if(hum > 25.5){ hum = 25.5; }																						//
			bool	result=0;																									//	Определяем переменную для хранения результата записи.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Записываем новое значение чувствительности:																		//
				data[0] = uint8_t(hum*10);																						//	Готовим байт данных для пережачи.
				result = _writeBytes(REG_SHT_HUM_CHANGE,1);																		//	Записываем 1 байт в регистр «REG_SHT_TEM_CHANGE» из массива «data».
			}																													//
			return result;																										//	Возвращаем флаг указывающий на результат записи (true/false).
}																																//
																																//
//		Установка периода обновления данных модулем.																			//	Возвращаемое значение:	результат установки нового периода обновления данных (true/false).
bool	iarduino_I2C_SHT::setPeriod		(float tim){																			//	Параметр:				период обновления данных в секундах (от 0.1 до 25.5).
			if(tim <  0.2){ tim =  0.2; }																						//
			if(tim > 25.5){ tim = 25.5; }																						//
			bool	result=0;																									//	Определяем переменную для хранения результата записи.
			if(valAddr){																										//	Если модуль был инициализирован, то ...
			//	Записываем новое значение периода обновления данных:															//
				data[0] = uint8_t(tim*10);																						//	Готовим байт данных для пережачи.
				result = _writeBytes(REG_SHT_PERIOD,1);																			//	Записываем 1 байт в регистр «REG_SHT_PERIOD» из массива «data».
			}																													//
			return result;																										//	Возвращаем флаг указывающий на результат записи (true/false).
}																																//
																																//
//		Чтение данных из регистров в массив data:																				//	Возвращаемое значение:	результат чтения (true/false).
bool	iarduino_I2C_SHT::_readBytes		(uint8_t reg, uint8_t sum){															//	Параметры:				reg - номер первого регистра, sum - количество читаемых байт.
			bool	result=false;																								//	Определяем флаг       для хранения результата чтения.
			uint8_t	sumtry=10;																									//	Определяем переменную для подсчёта количества оставшихся попыток чтения.
			do{	result = selI2C->readBytes(valAddr, reg, data, sum);															//	Считываем из модуля valAddr, начиная с регистра reg, в массив data, sum байт.
				sumtry--;	if(!result){delay(1);}																				//	Уменьшаем количество попыток чтения и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);																				//	Повторяем чтение если оно завершилось неудачей, но не более sumtry попыток.
			delayMicroseconds(500);																								//	Между пакетами необходимо выдерживать паузу.
			return result;																										//	Возвращаем результат чтения (true/false).
}																																//
																																//
//		Запись данных в регистры из массива data:																				//	Возвращаемое значение:	результат записи (true/false).
bool	iarduino_I2C_SHT::_writeBytes	(uint8_t reg, uint8_t sum, uint8_t num){												//	Параметры:				reg - номер первого регистра, sum - количество записываемых байт, num - номер первого элемента массива data.
			bool	result=false;																								//	Определяем флаг       для хранения результата записи.
			uint8_t	sumtry=10;																									//	Определяем переменную для подсчёта количества оставшихся попыток записи.
			do{	result = selI2C->writeBytes(valAddr, reg, &data[num], sum);														//	Записываем в модуль valAddr начиная с регистра reg, sum байи из массива data начиная с элемента num.
				sumtry--;	if(!result){delay(1);}																				//	Уменьшаем количество попыток записи и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);																				//	Повторяем запись если она завершилась неудачей, но не более sumtry попыток.
			delay(10);																											//	Ждём применения модулем записанных данных.
			return result;																										//	Возвращаем результат записи (true/false).
}																																//
																																//
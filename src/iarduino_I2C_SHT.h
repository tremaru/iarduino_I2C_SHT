//	Библиотека для работы с датчиком температуры и влажности, I2C-flash для Arduino: https://iarduino.ru/shop/Sensory-Datchiki/datchik-temperatury-i-vlazhnosti-i2c-trema-modul-v2-0.html
//  Версия: 1.0.1
//  Последнюю версию библиотеки Вы можете скачать по ссылке: https://iarduino.ru/file/519.html
//  Подробное описание функций бибилиотеки доступно по ссылке: https://wiki.iarduino.ru/page/SHT-trema-i2c/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел.
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_I2C_SHT_h																												//
#define iarduino_I2C_SHT_h																												//
																																		//
#if defined(ARDUINO) && (ARDUINO >= 100)																								//
#include		<Arduino.h>																												//
#else																																	//
#include		<WProgram.h>																											//
#endif																																	//
																																		//
#include		<iarduino_I2C_SHT_I2C.h>																								//	Подключаем файл iarduino_I2C_SHT.h - для работы с шиной I2C		(используя функции структуры iI2C)
																																		//
#define			DEF_CHIP_ID_FLASH		0x3C																							//	ID линейки чипов - константа для всех чипов серии Flash (позволяет идентифицировать принадлежность чипа к серии).
#define			DEF_CHIP_ID_METRO		0xC3																							//	ID линейки чипов - константа для всех чипов серии Metro (позволяет идентифицировать принадлежность чипа к серии).
#define			DEF_MODEL_SHT			0x05																							//	Идентификатор модели - константа.
//				Адреса регистров модуля:																								//
#define			REG_FLAGS_0				0x00																							//	Адрес регистра флагов управления для чтения.
#define			REG_BITS_0				0x01																							//	Адрес регистра битов  управления для чтения и записи.
#define			REG_FLAGS_1				0x02																							//	Адрес регистра флагов управления для чтения.
#define			REG_BITS_1				0x03																							//	Адрес регистра битов  управления для чтения и записи.
#define			REG_MODEL				0x04																							//	Адрес регистра содержащего номер типа модуля.
#define			REG_VERSION				0x05																							//	Адрес регистра содержащего версию прошивки.
#define			REG_ADDRESS				0x06																							//	Адрес регистра содержащего текущий адрес модуля на шине I2C. Если адрес указан с флагом IF-PIN-ADDRES то адрес установится (и запишется в регистр) только при наличии 1 на входе PIN_ADDRES.
#define			REG_CHIP_ID				0x07																							//	Адрес регистра содержащего ID линейки чипов «Flash». По данному ID можно определить принадлежность чипа к линейки «Flash».
#define			REG_SHT_PERIOD			0x08																							//	Период опроса датчика (в десятых долях секунд) от 00.0 до 25.5 сек.
#define			REG_SHT_FLG				0x10																							//	Адрес регистра содержащего флаги указывающие на изменение показаний датчика.
#define			REG_SHT_TEM_L			0x11																							//	Адрес регистра содержащего значение температуры умноженное на 10 (младшие биты).
#define			REG_SHT_HUM_L			0x13																							//	Адрес регистра содержащего значение влажности   умноженное на 10 (младшие биты).
#define			REG_SHT_TEM_CHANGE		0x15																							//	Адрес регистра содержащего значение температуры умноженное на 10 для установки флага «SHT_FLG_CHANGED_TEM». Если «REG_SHT_TEM» изменится более чем на «REG_SHT_TEM_CHANGE» то установится флаг «SHT_FLG_CHANGED_TEM» и флаг «SHT_FLG_CHANGED».
#define			REG_SHT_HUM_CHANGE		0x16																							//	Адрес регистра содержащего значение влажности   умноженное на 10 для установки флага «SHT_FLG_CHANGED_HUM». Если «REG_SHT_HUM» изменится более чем на «REG_SHT_HUM_CHANGE» то установится флаг «SHT_FLG_CHANGED_HUM» и флаг «SHT_FLG_CHANGED».
//				Позиция битов и флагов:																									//
#define			SHT_TEM_NEGATIVE		0x80																							//	Флаг минусовой температуры.	Устанавливается если к значению «REG_SHT_TEM» требуется добавить отрицательный знак.
#define			SHT_FLG_CHANGED_HUM		0x04																							//	Флаг изменения влажности.	Устанавливается если «REG_SHT_HUM» изменится более чем на «REG_SHT_HUM_CHANGE». Сбрасывается автоматически (сразу после чтения).
#define			SHT_FLG_CHANGED_TEM		0x02																							//	Флаг изменения температуры.	Устанавливается если «REG_SHT_TEM» изменится более чем на «REG_SHT_TEM_CHANGE». Сбрасывается автоматически (сразу после чтения).
#define			SHT_FLG_CHANGED			0x01																							//	Флаг изменения значений.	Устанавливается если если установлен флаг «SHT_FLG_CHANGED_HUM» или «SHT_FLG_CHANGED_TEM».
																																		//
class iarduino_I2C_SHT{																													//
	public:																																//
	/**	Конструктор класса **/																											//
		iarduino_I2C_SHT								(uint8_t address=0){															//	Конструктор класса													(Параметр: адрес модуля на шине I2C, если не указан (=0), то адрес будет определён).
														if(address>0x7F){ address>>=1; }												//	Корректируем адрес, если он указан с учётом бита RW.
							valAddrTemp				=	address;																		//	Сохраняем переданный адрес модуля.
							objI2C					=	new iarduino_I2C;																//	Переопределяем указатель objI2C на объект производного класса iarduino_I2C.
		}																																//
	/**	Пользовательские функции **/																									//
		bool				begin						(void													);						//	Объявляем  функцию инициализации модуля								(Параметр:  отсутствует).
		bool				reset						(void													);						//	Объявляем  функцию перезагрузки модуля								(Параметр:  отсутствует).
		bool				changeAddress				(uint8_t												);						//	Объявляем  функцию смены адреса модуля на шине I2C					(Параметр:  новый адрес модуля).
		uint8_t				getAddress					(void													){ return valAddr;	}	//	Определяем функцию возвращающую текущий адрес модуля на шине I2C	(Параметр:  отсутствует).
		uint8_t				getVersion					(void													){ return valVers;	}	//	Определяем функцию возвращающую текущую версию прошивки модуля		(Параметр:  отсутствует).
		bool				getPullI2C					(void													);						//	Определяем функцию возвращающую флаг наличия подтяжки шины I2C		(Параметр:  отсутствует).
		bool				setPullI2C					(bool=true												);						//	Определяем функцию установки подтяжки шины I2C						(Параметр:  флаг подтяжки).
		float				getTem						(void													);						//	Объявляем  функцию возвращающую текущую температуру в °C			(Параметр:  отсутствует).
		float				getHum						(void													);						//	Объявляем  функцию возвращающую текущую влажность в %				(Параметр:  отсутствует).
		bool				getTemChanged				(void													);						//	Объявляем  функцию возвращающую флаг изменения температуры			(Параметр:  отсутствует).
		bool				getHumChanged				(void													);						//	Объявляем  функцию возвращающую флаг изменения влажности			(Параметр:  отсутствует).
		bool				setTemChange				(float													);						//	Объявляем  функцию установки порога чувств к изменению температуры	(Параметр:  температура в °C).
		bool				setHumChange				(float													);						//	Объявляем  функцию установки порога чувств к изменению влажности	(Параметр:  влажность в %).
		bool				setPeriod					(float													);						//	Объявляем  функцию установки периода обновления данных модулем		(Параметр:  время в секундах).
	private:																															//
	/**	Внутренние переменные **/																										//
		uint8_t				valAddrTemp				=	0;																				//	Определяем переменную для хранения адреса модуля на шине I2C который был указан, но не был проверен.
		uint8_t				valAddr					=	0;																				//	Определяем переменную для хранения адреса модуля на шине I2C.
		uint8_t				valVers					=	0;																				//	Определяем переменную для хранения версии прошивки модуля.
		uint8_t				val_REG_SHT_FLG			=	0;																				//	Определяем переменную для хранения значения регистра флагов «REG_SHT_FLG». Благодаря данной переменной реализован сброс флагов «SHT_FLG_CHANGED_TEM» и «SHT_FLG_CHANGED_HUM» не по чтению регистра «REG_SHT_FLG», а по обращению к функциям getTemChanged() и getHumChanged();
		uint32_t			tmrReadFLG				=	0;																				//	Определяем переменную для хранения времени (в мс) последнего чтения регистра флагов «REG_DSL_FLG».
const	uint32_t			tmrPeriodFLG			=	30;																				//	Определяем константу  для хранения минимального времени между опросом регистра флагов «REG_DSL_FLG».
		uint8_t				data[8];																									//	Объявляем  массив     для хранения получаемых/передаваемых данных.
		iarduino_I2C_BASE*	objI2C;																										//	Объявляем  указатель  на  объект полиморфного класса iarduino_I2C_BASE, но в конструкторе данного класса этому указателю будет присвоена ссылка на производный класс iarduino_I2C.
	/**	Внутренние функции **/																											//
		bool				_readBytes				(uint8_t	, uint8_t										);						//	Объявляем  функцию чтения данных в  массив  data					(Параметры:  номер первого регистра, количество байт).
		bool				_writeBytes				(uint8_t	, uint8_t, uint8_t=0							);						//	Объявляем  функцию записи данных из массива data					(Параметры:  номер первого регистра, количество байт, номер первого элемента массива data).
};																																		//
																																		//
#endif																																	//
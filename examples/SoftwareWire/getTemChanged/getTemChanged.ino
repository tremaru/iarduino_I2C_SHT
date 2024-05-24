// ПРИМЕР ЧТЕНИЯ ТЕМПЕРАТУРЫ ПРИ ЕЁ ИЗМЕНЕНИИ:               // * Строки со звёздочкой являются необязательными.
// Требуется установить библиотеку <iarduino_I2C_Software.h> //   https://iarduino.ru/file/627.html
                                                             //
// Датчик температуры и влажности, FLASH-I2C:                //   https://iarduino.ru/shop/Sensory-Datchiki/datchik-temperatury-i-vlazhnosti-i2c-trema-modul-v2-0.html
// Информация о подключении модулей к шине I2C:              //   https://wiki.iarduino.ru/page/i2c_connection/
// Информация о модуле и описание библиотеки:                //   https://wiki.iarduino.ru/page/SHT-trema-i2c/
                                                             //
#include <iarduino_I2C_Software.h>                           //   Подключаем библиотеку для работы с программной шиной I2C, до подключения библиотеки iarduino_I2C_SHT.
SoftTwoWire sWire(3,4);                                      //   Создаём объект программной шины I2C указав выводы которым будет назначена роль линий: SDA, SCL.
                                                             //
#include <iarduino_I2C_SHT.h>                                //   Подключаем библиотеку для работы с датчиком температуры и влажности I2C-flash (Sensor Humidity and Temperature).
iarduino_I2C_SHT sht;                                        //   Объявляем объект sht для работы с функциями и методами библиотеки iarduino_I2C_SHT.
                                                             //   Если при объявлении объекта указать адрес, например, sht(0xBB), то пример будет работать с тем модулем, адрес которого был указан.
void setup(){                                                //
    delay(500);                                              // * Ждём завершение переходных процессов связанных с подачей питания.
    Serial.begin(9600);                                      //
    while(!Serial){;}                                        // * Ждём завершения инициализации шины UART.
    sht.begin(&sWire);                                       //   Инициируем работу с датчиком температуры и влажности, указав ссылку на объект для работы с шиной I2C на которой находится датчик (по умолчанию &Wire).
    sht.setTemChange( 0.1 );                                 //   Указываем фиксировать изменение температуры окружающей среды более чем на 0.1°С.
}                                                            //
                                                             //
void loop(){                                                 //
    if( sht.getTemChanged() ){                               //   Если температура окружающей среды изменилась более чем на значение указанное в функции setTemChange(), то ...
        Serial.print("Температура = ");                      //
        Serial.print( sht.getTem() );                        //   Выводим текущую температуру окружающей среды, от -40 до +125°С.
        Serial.print(" °С.\r\n");                            //
    }                                                        //
}                                                            //
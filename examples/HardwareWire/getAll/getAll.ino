// ПРИМЕР ЧТЕНИЯ ВСЕХ ЗНАЧЕНИЙ ДАТЧИКА:           // * Строки со звёздочкой являются необязательными.
                                                  //
// Датчик температуры и влажности, FLASH-I2C:     //   https://iarduino.ru/shop/Sensory-Datchiki/datchik-temperatury-i-vlazhnosti-i2c-trema-modul-v2-0.html
// Информация о подключении модулей к шине I2C:   //   https://wiki.iarduino.ru/page/i2c_connection/
// Информация о модуле и описание библиотеки:     //   https://wiki.iarduino.ru/page/SHT-trema-i2c/
                                                  //
#include <Wire.h>                                 //   Подключаем библиотеку для работы с аппаратной шиной I2C, до подключения библиотеки iarduino_I2C_SHT.
#include <iarduino_I2C_SHT.h>                     //   Подключаем библиотеку для работы с датчиком температуры и влажности I2C-flash (Sensor Humidity and Temperature).
iarduino_I2C_SHT sht;                             //   Объявляем объект sht для работы с функциями и методами библиотеки iarduino_I2C_SHT.
                                                  //   Если при объявлении объекта указать адрес, например, sht(0xBB), то пример будет работать с тем модулем, адрес которого был указан.
void setup(){                                     //
    delay(500);                                   // * Ждём завершение переходных процессов связанных с подачей питания.
    Serial.begin(9600);                           //
    while(!Serial){;}                             // * Ждём завершения инициализации шины UART.
    sht.begin(&Wire); // &Wire1, &Wire2 ...       //   Инициируем работу с датчиком температуры и влажности, указав ссылку на объект для работы с шиной I2C на которой находится датчик (по умолчанию &Wire).
}                                                 //
                                                  //
void loop(){                                      //
    Serial.print("Температура = ");               //
    Serial.print( sht.getTem() );                 //   Выводим текущую температуру, от -40.0 до +125 °C.
    Serial.print(" °C, влажность = ");            //
    Serial.print( sht.getHum() );                 //   Выводим текущую влажность воздуха, от 0 до 100%.
    Serial.print(" %.\r\n");                      //
    delay(500);                                   // * Задержка позволяет медленнее заполнять монитор последовательного порта.
}                                                 //
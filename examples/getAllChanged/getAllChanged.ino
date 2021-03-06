// ПРИМЕР ЧТЕНИЯ ВСЕХ ЗНАЧЕНИЙ ПРИ ИХ ИЗМЕНЕНИИ:  // * Строки со звёздочкой являются необязательными.
                                                  //
#include <Wire.h>                                 // * Подключаем библиотеку для работы с аппаратной шиной I2C.
#include <iarduino_I2C_SHT.h>                     //   Подключаем библиотеку для работы с датчиком температуры и влажности I2C-flash (Sensor Humidity and Temperature).
iarduino_I2C_SHT sht;                             //   Объявляем объект sht для работы с функциями и методами библиотеки iarduino_I2C_SHT.
                                                  //   Если при объявлении объекта указать адрес, например, sht(0xBB), то пример будет работать с тем модулем, адрес которого был указан.
void setup(){                                     //
    delay(500);                                   // * Ждём завершение переходных процессов связанных с подачей питания.
    Serial.begin(9600);                           //
    while(!Serial){;}                             // * Ждём завершения инициализации шины UART.
    sht.begin();                                  //   Инициируем работу с датчиком температуры и влажности.
    sht.setTemChange( 0.1 );                      //   Указываем фиксировать изменение температуры окружающей среды более чем на 0.1°С.
    sht.setHumChange( 1.0 );                      //   Указываем фиксировать изменение влажности воздуха более чем на 1%.
}                                                 //
                                                  //
void loop(){                                      //
    if( sht.getTemChanged() ){                    //   Если температура окружающей среды изменилась более чем на значение указанное в функции setTemChange(), то ...
        Serial.print("Температура = ");           //
        Serial.print( sht.getTem() );             //   Выводим текущую температуру окружающей среды, от -40 до +125°С.
        Serial.print(" °С.\r\n");                 //
    }                                             //
    if( sht.getHumChanged() ){                    //   Если влажность воздуха изменилась более чем на значение указанное в функции setHumChange(), то ...
        Serial.print("Влажность = ");             //
        Serial.print( sht.getHum() );             //   Выводим текущую влажность воздуха, от 0 до 100%.
        Serial.print(" %.\r\n");                  //
    }                                             //
}                                                 //
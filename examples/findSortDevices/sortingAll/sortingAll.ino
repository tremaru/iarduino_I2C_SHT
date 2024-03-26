// ДАННЫЙ ПРИМЕР СОРТИРУЕТ АДРЕСА МОДУЛЕЙ ЛИНЕЙКИ FLASH I2C.                // * Строки со звёздочкой являются необязательными.
// Требуется установить библиотеку "iarduino_I2C_Address"!!!                //
//                                                                          //
// Допускается нахождение на шине модулей Flash I2C с одинаковыми адресами. //
// Допускается нахождение на шине сторонних устройств I2C.                  //
                                                                            //
const uint8_t sum = 10;                                                     //   Определяем количество модулей на шине I2C. Допускается указывать значение превышающее реальное количество модулей.
const uint8_t start = 0x20;                                                 //   Определяем первый адрес из списка создаваемых адресов для модулей Flash I2C.
                                                                            //
#include <Wire.h>                                                           // * Подключаем библиотеку для работы с аппаратной шиной I2C.
#include <iarduino_I2C_Address.h>                                           //   Подключаем библиотеку для работы с адресами модулей линейки I2C-flash.
iarduino_I2C_Address obj[sum];                                              //   Объявляем  массив объектов (obj), указав количество (sum) модулей I2C-flash на шине I2C. Адреса модулей будут определены автоматически.
iarduino_I2C_SortAddress all;                                               //   Объявляем  объект (all) для сортировки адресов.
                                                                            //
void setup(){                                                               //
     delay(500);                                                            // * Ждём завершение переходных процессов связанных с подачей питания.
     Serial.begin(9600);                                                    //   Инициируем передачу данных в монитор последовательного порта на скорости 9600 бит/сек.
     while(!Serial){;}                                                      // * Ждём завершения инициализации шины UART.
                                                                            //
     Serial.println( F("-----------------") );                              // *
                                                                            //
//   Находим модули:                                                        //
     Serial.print  ( F("Чтение... ") );                                     // *
     for(uint8_t i=0; i<sum; i++){ obj[i].begin(); }                        //   Если очередной модуль существует, то begin() вернёт true иначе false.
     Serial.println( F("выполнено!") );                                     // *
                                                                            //
//   Выводим информацию о найденных модулях:                                //
     for(uint8_t i=0; i<sum; i++){                                          //   Вместо цикла for() можно воспользоваться циклом while( obj[i] ){ ... i++; }
         if( obj[i] ){                                                      //   Если модуль не существует, то obj[i] вернёт false.
         //  Выводим порядковый номер модуля:                               //
             Serial.print( i+1 ); Serial.print( F(".) ") );                 //
         //  Выводим адрес модуля на шине I2C:                              //
             Serial.print( F("0x") ); Serial.print( obj[i], HEX );          //
         //  Выводим информацию о модуле:                                   //
             if( obj[i].getDevice()==DEVICE_I2C_FLASH     ){ Serial.print( F(" Модуль Flash I2C «") ); Serial.print( obj[i].getName() ); Serial.print( F("»") ); }
             if( obj[i].getDevice()==DEVICE_I2C_FLASH_OLD ){ Serial.print( F(" Модуль Flash I2C без поддержки вывода названия и сортировки адреса.") ); }
             if( obj[i].getDevice()==DEVICE_I2C_UNKNOWN   ){ Serial.print( F(" Неизвестный модуль") ); }
             if( obj[i].getDevice()==DEVICE_I2C_DOUBLE    ){ Serial.print( F(" Адрес занят несколькими модулями")); }
         //  Переходим к следующему модулю:                                 //
             Serial.println('.');                                           //
         }                                                                  //
     }                                                                      //
                                                                            //
//   Выполняем сортировку адресов:                                          //
     Serial.print( F("Сортировка... ") );                                   // *
     if( all.sorting(start) ){ Serial.println( F("выполнено!"  ) ); }       //
     else                    { Serial.println( F("не требуется") ); }       //
                                                                            //
//   Находим модули:                                                        //
     Serial.print  ( F("Чтение... ") );                                     // *
     for(uint8_t i=0; i<sum; i++){ obj[i].begin(); }                        //   Если очередной модуль существует, то begin() вернёт true иначе false.
     Serial.println( F("выполнено!") );                                     // *
                                                                            //
//   Выводим информацию о найденных модулях:                                //
     for(uint8_t i=0; i<sum; i++){                                          //   Вместо цикла for() можно воспользоваться циклом while( obj[i] ){ ... i++; }
         if( obj[i] ){                                                      //   Если модуль не существует, то obj[i] вернёт false.
         //  Выводим порядковый номер модуля:                               //
             Serial.print( i+1 ); Serial.print( F(".) ") );                 //
         //  Выводим адрес модуля на шине I2C:                              //
             Serial.print( F("0x") ); Serial.print( obj[i], HEX );          //
         //  Выводим информацию о модуле:                                   //
             if( obj[i].getDevice()==DEVICE_I2C_FLASH     ){ Serial.print( F(" Модуль Flash I2C «") ); Serial.print( obj[i].getName() ); Serial.print( F("»") ); }
             if( obj[i].getDevice()==DEVICE_I2C_FLASH_OLD ){ Serial.print( F(" Модуль Flash I2C без поддержки вывода названия и сортировки адреса.") ); }
             if( obj[i].getDevice()==DEVICE_I2C_UNKNOWN   ){ Serial.print( F(" Неизвестный модуль") ); }
             if( obj[i].getDevice()==DEVICE_I2C_DOUBLE    ){ Serial.print( F(" Адрес занят несколькими модулями")); }
         //  Переходим к следующему модулю:                                 //
             Serial.println('.');                                           //
         }                                                                  //
     }                                                                      //
}                                                                           //
                                                                            //
void loop(){                                                                //
}                                                                           //
[![MadGyver YouTube](http://alexgyver.ru/git_banner.jpg)](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
# GyverSaber - Arduino based multicolor sound playing lightsaber
* [Project description](#chapter-0)
* [Folders](#chapter-1)
* [Schemes](#chapter-2)
* [Components](#chapter-3)
* [Assemply and set up](#chapter-7)

<a id="chapter-0"></a>
## Project description
SUPER-DUPER COOL ARDUINO BASED MULTICOLOR SOUND PLAYING LIGHTSABER!

#### CAPABILITIES:
* Smooth turning on/off with lightsaber-like sound effect
* Randomly pulsing color (you can turn it off)
* Sounds:
  + MODE 1: generated hum. Frequency depends on angle velocity of blade
  + MODE 2: hum sound from SD card
    - Slow swing - long hum sound (randomly from 4 sounds)
    - Fast swing - short hum sound (randomly from 5 sounds)
* Bright white flash when hitting
* Play one of 16 hit sounds, when hit
  + Weak hit - short sound
  + Hard hit - long "bzzzghghhdh" sound
* After power on blade shows current battery level from 0 to 100 percent
* Battery safe mode:
  + Battery is drain BEFORE TURNING ON: GyverSaber will not turn on, button LED will PULSE a couple of times
  + Battery is drain AFTER TURNING ON: GyverSaber will be turned off automatically
#### CONTROL BUTTON:
* HOLD - turn on / turn off GyverSaber
* TRIPLE CLICK - change color (red - green - blue - yellow - pink - ice blue)
* QUINARY CLICK - change sound mode (hum generation - hum playing)
* Selected color and sound mode stored in EEPROM (non-volatile memory)

<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии
- **GyverSaber** - прошивка для Arduino
- **schemes** - схемы подключения
- **SDsounds** - набор звуков для карты памяти

<a id="chapter-2"></a>
## Схемы
![SCHEME](https://github.com/AlexGyver/GyverSaber/blob/master/schemes/scheme1.jpg)
![SCHEME](https://github.com/AlexGyver/GyverSaber/blob/master/schemes/scheme2.jpg)

<a id="chapter-3"></a>
## Материалы и компоненты
* Arduino NANO http://ali.pub/20o35g  http://ali.pub/20o36t
* Адресная лента. Чип WS2811, напряжение 12 Вольт. Берём 2 метра ленты, на **белой подложке, без защиты от влаги, 60 светодиодов на метр**
http://ali.pub/23csyd  http://ali.pub/23cszc  http://ali.pub/23cszq
* Кнопочки с подсветкой. **Берём версию 5 Вольт** http://ali.pub/23ct29
* Акселерометр-гироскоп http://ali.pub/23mryw  http://ali.pub/23mst1
* Дешёвые карты памяти http://ali.pub/23msne  http://ali.pub/23msqp
* Модуль карты памяти мини http://ali.pub/23ms27  http://ali.pub/23ms5b
* Или такой http://ali.pub/23ms11
* Акумы с защитой http://ali.pub/23moiu  http://ali.pub/23moke  http://ali.pub/23mol6
* Понижайка http://ali.pub/23mpex  http://ali.pub/23mpfi  http://ali.pub/23mph9
* Усилитель http://ali.pub/23mp6d  http://ali.pub/23mp75  http://ali.pub/23mp7q
* Динамик http://ali.pub/23mq8h  http://ali.pub/23mq9g  http://ali.pub/23mqc6
* Набор резисторов http://ali.pub/23mqei  http://ali.pub/23mqfo
* Кнопка питания http://ali.pub/23mtiw
* Зарядное гнездо http://ali.pub/23mtf0  http://ali.pub/23mt69
* Зарядник CC CV для 3-х банок http://ali.pub/23mt8s  http://ali.pub/23mt9d  http://ali.pub/23mt9k
* Макетная плата http://ali.pub/23mrwy  
* **РЕЗИСТОРЫ И КНОПКИ МОЖНО НАЙТИ В ЛЮБОМ МАГАЗИНЕ РАДИОТОВАРОВ ИЛИ НА РАДИОРЫНКЕ**
* Труба http://www.energoplast.ru/

## Вам скорее всего пригодится
* [Всё для пайки (паяльники и примочки)](http://alexgyver.ru/all-for-soldering/)
* [Недорогие инструменты](http://alexgyver.ru/my_instruments/)
* [Все существующие модули и сенсоры Arduino](http://alexgyver.ru/arduino_shop/)
* [Электронные компоненты](http://alexgyver.ru/electronics/)
* [Аккумуляторы и зарядные модули](http://alexgyver.ru/18650/)

<a id="chapter-4"></a>
## Как скачать и прошить
* [Первые шаги с Arduino](http://alexgyver.ru/arduino-first/) - ультра подробная статья по началу работы с Ардуино, ознакомиться первым делом!
* Скачать архив с проектом
> На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**
* Установить библиотеки в  
`C:\Program Files (x86)\Arduino\libraries\` (Windows x64)  
`C:\Program Files\Arduino\libraries\` (Windows x86)
* Подключить Ардуино к компьютеру
* Запустить файл прошивки (который имеет расширение .ino)
* Настроить IDE (COM порт, модель Arduino, как в статье выше)
* Настроить что нужно по проекту
* Нажать загрузить
* Пользоваться  

<a id="chapter-7"></a>
## Сборка и настройка
* Открыть прошивку, настроить:
  - Число МИКРОСХЕМ на ленте (чёрные штуки такие. **Одна микросхема отвечает за 3 светодиода**). Если вы захотите сделать меч длиннее - без проблем, главное не забыть это настроить.
  - Включить или выключить мерцание клинка (для эпилептиков)
  - ЖЕЛАТЕЛЬНО измерить реальное сопротивление резисторов делителя и тоже их указать (в Омах)
    + Если вы не ставите делитель, то нужно отключить параметр BATTERY_SAFE. Но делать это крайне не рекомендую!
  - Остальные настройки - полная кастомизация поведения меча, описания настроек говорят сами за себя. По умолчанию настроено так, как понравилось мне
* Загрузить прошивку
* Информация по MicroSD карте:
  - Размер до 4 Гб
  - Форматировать в FAT
  - Скопировать аудиофайлы В КОРЕНЬ карты
  - Если хотите добавить свои файлы, то нужно конвертировать в формат .wav со следующими параметрами:
    + 8 bit
	+ 16-32 kHz
	+ Mono
	+ Для конвертации есть онайлн сервисы, а есть софтина Total Audio Converter
* Собрать всё по схеме
  - **ВНИМАНИЕ!** Если прошивается/отлаживается ПОЛНОСТЬЮ СОБРАННЫЙ МЕЧ, то нужно включить питание, иначе Ардуина будет постоянно перезагружаться! Это связано с питанием, так как у нас висит преобразователь.
* Включить и пользоваться!

## Настройки меча в прошивке
    NUM_LEDS 30         // число МИКРОСХЕМ на ленте
    BTN_TIMEOUT 800     // задержка кнопки для удерживания (миллисекунды)
    BRIGHTNESS 255      // максимальная яркость ленты (0 - 255)

    SWING_TIMEOUT 500   // таймаут между двумя взмахами
    SWING_L_THR 150     // порог угловой скорости для взмаха
    SWING_THR 300       // порог угловой скорости для сильного взмаха
    STRIKE_THR 150      // порог ускорения для распознавания удара
    STRIKE_S_THR 320    // порог ускорения для распознавания сильного удара
    FLASH_DELAY 80      // время вспышки при ударе (миллисекунды)

    BLINK_ALLOW 1       // разрешить мерцание (1 - разрешить, 0 - запретить)
    BLINK_AMPL 20       // амплитуда мерцания клинка
    BLINK_DELAY 30      // задержка между мерцаниями

    R1 100000           // сопротивление резистора делителя    
    R2 51000            // сопротивление резистора делителя
    BATTERY_SAFE 1      // не включаться и выключаться при низком заряде АКБ

    DEBUG 0             // вывод в порт отладочной информации

<a id="chapter-5"></a>
## FAQ
### Основные вопросы
В: Как скачать с этого грёбаного сайта?  
О: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**

В: Скачался какой то файл .zip, куда его теперь?  
О: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.

В: Я совсем новичок! Что мне делать с Ардуиной, где взять все программы?  
О: Читай и смотри видос http://alexgyver.ru/arduino-first/

В: Компьютер никак не реагирует на подключение Ардуины!  
О: Возможно у тебя зарядный USB кабель, а нужен именно data-кабель, по которому можно данные передавать

В: Ошибка! Скетч не компилируется!  
О: Путь к скетчу не должен содержать кириллицу. Положи его в корень диска.

В: Сколько стоит?  
О: Ничего не продаю.

### Вопросы по этому проекту
В: Я купил ленту, на ней контакты G R B 12V. Как подключить?  
О: Молодец, можешь кинуть ей в собаку. Это не та лента.

В: Всё работает, но не воспроизводятся звуки с карты! Режим генерации работает, а звуки включения/выключения/ударов нет!  
О: Проверь, распознала ли Ардуина карту памяти. В настройках прошивки можно включить DEBUG, при старте системы должна выйти в монитор порта строчка **SD OK**. Если видишь **SD Fail**, значит либо не подходит карта памяти, либо неправильно подключен модуль карты.  

В: Звуки включения/выключения работают, но меч не реагирует на удары и взмахи!  
О: Проверь, подключился ли модуль акселерометра. В настройках прошивки можно включить DEBUG, при старте системы должна выйти в монитор порта строчка **MPU6050 OK**. Если видишь **MPU6050 fail**, значит либо аксель приказал долго жить, либо неправильно подключен.   

В: Обязательно ли нужен светодиод кнопки?  
О: Он показывает включено или выключено питание, если забыть выключить меч - акумы сдохнут.

В: У меня ничего не работает!  
О: Я конечно поздравляю, но у меня работают два меча, собранные двумя разными способами. Ищи ошибки в схеме.

В: Работает частично, система меча как будто зависает!  
О: Где то ошибка в схеме, или неисправен один из модулей (SD карта, аксель).

<a id="chapter-6"></a>
## Полезная информация
* [Мой сайт](http://alexgyver.ru/)
* [Основной YouTube канал](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
* [YouTube канал про Arduino](https://www.youtube.com/channel/UC4axiS76D784-ofoTdo5zOA?sub_confirmation=1)
* [Мои видеоуроки по пайке](https://www.youtube.com/playlist?list=PLOT_HeyBraBuMIwfSYu7kCKXxQGsUKcqR)
* [Мои видеоуроки по Arduino](http://alexgyver.ru/arduino_lessons/)
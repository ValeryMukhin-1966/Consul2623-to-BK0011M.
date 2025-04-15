# Consul 262.3 to БК-0011М Adapter

Программа для подключения клавиатуры Consul 262.3 (от СМ 7209) к компьютеру БК-0010-01 через модуль расширения AZБК. Проект эмулирует клавиатуру БК-0011М и обеспечивает совместимость с ОС БК-11 (версия RT-11) через интерфейс PS/2.

![Фото клавиатуры или схемы](images/consul2623_pinout.jpg)

## О проекте
Этот проект позволяет использовать старую клавиатуру Consul 262.3 с компьютером БК-0010-01. Программа, написанная для Arduino, преобразует сигналы клавиатуры в PS/2-скан-коды, обеспечивая поддержку букв, цифр, стрелок, Shift, Caps и специальных символов.

Целевая аудитория:
- Энтузиасты ретро-компьютеров.
- Пользователи БК-0010-01 и БК-0011М.
- Разработчики, интересующиеся интерфейсами клавиатур.

## Возможности
- Поддержка всех основных клавиш Consul 262.3 (буквы, цифры, символы).
- Эмуляция PS/2-скан-кодов для совместимости с БК-0011М.
- Обработка Shift и Caps для ввода заглавных букв.
- Поддержка расширенных клавиш (стрелки, Tab, Esc).
- Антидребезг для стабильного чтения сигналов.
- Вывод отладочной информации через Serial.

## Требования
### Аппаратное обеспечение
- Микроконтроллер Arduino (например, Arduino Uno или Nano).
- Клавиатура Consul 262.3.
- Модуль расширения AZБК.
- Компьютер БК-0010-01 с поддержкой PS/2.

### Программное обеспечение
- Arduino IDE (или другая среда для прошивки).
- Библиотеки: встроенные (никакие дополнительные не требуются).

## Установка
1. Склонируйте репозиторий:
   bash
   git clone https://github.com/ваш_юзер/Consul2623-to-BK0011M.git

Откройте файл src/Consul2623_BK0011M.ino в Arduino IDE.

Подключите Arduino к компьютеру и выберите правильную плату в Arduino IDE.

Загрузите прошивку на Arduino:
Нажмите Upload в Arduino IDE.

## Подключение
Подключите клавиатуру Consul 262.3 к Arduino согласно распиновке:
Данные (D0-D7): пины 2, 4, 5, 6, 7, 8, 9, 10 (жилы 4-11).

Strobe: пин 3 (жила 3).

PS/2 Clock: пин 11.

PS/2 Data: пин 12.

Подключите Arduino к AZБК и БК-0010-01 через PS/2-интерфейс.

Включите БК-0010-01 и загрузите ОС БК-11 (RT-11).

Подробная схема подключения: docs/hardware.md.

## Использование
После прошивки Arduino автоматически преобразует сигналы клавиатуры в PS/2-скан-коды.

Используйте клавиатуру для ввода в ОС БК-11.

Для отладки подключите Arduino к компьютеру и откройте Serial Monitor (57600 бод).

## Примеры
Пример нажатия клавиши «j»:
Код клавиатуры: 0xA9.

PS/2-скан-код: 0x3B.

Вывод в Serial: кей: 0xA9 -> Символ: j.

## Документация
Схема подключения и распиновка (docs/hardware.md).

Исходный код: src/Consul2623_BK0011M.ino.

Как внести вклад
Форкните репозиторий.

Создайте ветку: git checkout -b feature/ваша_фича.

Сделайте коммит: git commit -m "Добавлена фича XYZ".

Отправьте изменения: git push origin feature/ваша_фича.

Создайте Pull Request.

Подробности: CONTRIBUTING.md.
Лицензия
MIT License (LICENSE).
Контакты
Email: MukhinVB@yandex.ru (mailto:MukhinVB@yandex.ru)

Форум: https://vk.com/club201781756?from=groups


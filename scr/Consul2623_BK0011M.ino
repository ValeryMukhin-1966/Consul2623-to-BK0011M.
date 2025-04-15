// Пины для чтения клавиатуры Consul-262.3 (входные)
const int dataPins[] = {10, 9, 8, 7, 6, 5, 4, 2}; // D0-D7 (Жилы 11-4)
const int strobePin = 3; // STROBE (Жила 3)

// Пины для PS/2
const int ps2ClockPin = 11; // D11 → Clock
const int ps2DataPin = 12;  // D12 → Data

// Переменные для антидребезга и состояния
volatile uint8_t keyData = 0;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 300; // Антидребезг 300 мс

// Переменные для режимов
volatile bool isCapsMode = false;  // false - строчные, true - заглавные
volatile bool isShiftPressed = false; // Для отслеживания Shift

// Специальные скан-коды для PS/2
const uint8_t scanCodeRelease = 0xF0; // Код отпускания клавиши
const uint8_t scanCodeShift = 0x12;   // Left Shift
const uint8_t scanCodeJ = 0x3B;   // j
const uint8_t scanCodeC = 0x21;   // c
const uint8_t scanCodeU = 0x3C;   // u
const uint8_t scanCodeK = 0x42;   // k
const uint8_t scanCodeE = 0x24;   // e
const uint8_t scanCodeN = 0x31;   // n
const uint8_t scanCodeG = 0x34;   // g
const uint8_t scanCodeLBracket = 0x54; // [
const uint8_t scanCodeRBracket = 0x5B; // ]
const uint8_t scanCodeZ = 0x1A;   // z
const uint8_t scanCodeH = 0x33;   // h
const uint8_t scanCodeStar = 0x7C; // *
const uint8_t scanCodeF = 0x2B;   // f
const uint8_t scanCodeY = 0x35;   // y
const uint8_t scanCodeW = 0x1D;   // w
const uint8_t scanCodeA = 0x1C;   // a
const uint8_t scanCodeP = 0x4D;   // p
const uint8_t scanCodeR = 0x2D;   // r
const uint8_t scanCodeO = 0x44;   // o
const uint8_t scanCodeL = 0x4B;   // l
const uint8_t scanCodeD = 0x23;   // d
const uint8_t scanCodeV = 0x2A;   // v
const uint8_t scanCodeBackslash = 0x5D; // \//
const uint8_t scanCodeQ = 0x15;   // q
const uint8_t scanCodeCaret = 0x36; // ^
const uint8_t scanCodeS = 0x1B;   // s
const uint8_t scanCodeM = 0x3A;   // m
const uint8_t scanCodeI = 0x43;   // i
const uint8_t scanCodeT = 0x2C;   // t
const uint8_t scanCodeX = 0x22;   // x
const uint8_t scanCodeB = 0x32;   // b
const uint8_t scanCode2 = 0x1E;   // 2 (для "@")
const uint8_t scanCodeComma = 0x41; // ,
const uint8_t scanCodeMinus = 0x4E; // - (для "_")
const uint8_t scanCodeSpace = 0x29; // Space
const uint8_t scanCodeBackspace = 0x66; // Backspace
const uint8_t scanCodeEnter = 0x5A;     // Enter
const uint8_t scanCodeExtended = 0xE0;  // Префикс для расширенных кодов (стрелки)
const uint8_t scanCodeUp = 0x75;        // Стрелка вверх (после 0xE0)
const uint8_t scanCodeDown = 0x72;      // Стрелка вниз (после 0xE0)
const uint8_t scanCodeRight = 0x74;     // Стрелка вправо (после 0xE0)
const uint8_t scanCodeLeft = 0x6B;      // Стрелка влево (после 0xE0)
const uint8_t scanCodeTab = 0x0D;       // Tab
const uint8_t scanCodeEsc = 0x76;       // Esc

void setup() {
  Serial.begin(57600);

  // Настройка пинов для чтения клавиатуры (входные)
  pinMode(2, INPUT_PULLUP);  // Жила 4
  pinMode(4, INPUT_PULLUP);  // Жила 5
  pinMode(5, INPUT_PULLUP);  // Жила 6
  pinMode(6, INPUT_PULLUP);  // Жила 7
  pinMode(7, INPUT_PULLUP);  // Жила 8
  pinMode(8, INPUT_PULLUP);  // Жила 9
  pinMode(9, INPUT_PULLUP);  // Жила 10
  pinMode(10, INPUT_PULLUP); // Жила 11
  pinMode(strobePin, INPUT_PULLUP);  // Жила 3 → STROBE

  // Настройка пинов для PS/2 (open-collector: OUTPUT для LOW, INPUT для HIGH)
  pinMode(ps2ClockPin, INPUT);
  pinMode(ps2DataPin, INPUT);
  digitalWrite(ps2ClockPin, HIGH);
  digitalWrite(ps2DataPin, HIGH);

  // Привязка прерывания для чтения клавиатуры
  attachInterrupt(digitalPinToInterrupt(strobePin), readKeyboard, RISING);

  // Отправка сигнала успешного самотеста (BAT - Basic Assurance Test)
  delay(100);
  sendPS2Byte(0xAA); // Успешный самотест

  Serial.println("Инициализация завершена");
}

void loop() {}

void readKeyboard() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime < debounceDelay) return;
  lastInterruptTime = currentTime;

  delayMicroseconds(10);
  keyData = 0;
  keyData |= digitalRead(10) << 0; // Жила 11 → D0
  keyData |= digitalRead(9) << 1;  // Жила 10 → D1
  keyData |= digitalRead(8) << 2;  // Жила 9 → D2
  keyData |= digitalRead(7) << 3;  // Жила 8 → D3
  keyData |= digitalRead(6) << 4;  // Жила 7 → D4
  keyData |= digitalRead(5) << 5;  // Жила 6 → D5
  keyData |= digitalRead(4) << 6;  // Жила 5 → D6
  keyData |= digitalRead(2) << 7;  // Жила 4 → D7

  Serial.print("кей: 0x");
  Serial.print(keyData, HEX);

  // Переключение Caps
  if (keyData == 0xB4) {
    isCapsMode = !isCapsMode; // Переключаем Caps
    Serial.print(" -> Режим: ");
    Serial.println(isCapsMode ? "Заглавные буквы" : "Строчные");
    return;
  }

  // Определяем, нажат ли Shift (по кодам клавиш)
  if (keyData == 0x4D || keyData == 0x6D || keyData == 0xD5 || keyData == 0xE5 || 
      keyData == 0xBD || keyData == 0xFD || keyData == 0xC3 || keyData == 0x03) {
    isShiftPressed = true;
  } else {
    isShiftPressed = false;
  }

  // Декодирование и отправка PS/2
  uint8_t scanCode = 0;
  String symbol = "";
  bool requiresShift = false;
  bool isExtended = false; // Флаг для расширенных кодов (например, стрелки)

  switch (keyData) {
    case 0xA9: symbol = "j"; scanCode = scanCodeJ; break;
    case 0x39: symbol = "c"; scanCode = scanCodeC; break;
    case 0x51: symbol = "u"; scanCode = scanCodeU; break;
    case 0x29: symbol = "k"; scanCode = scanCodeK; break;
    case 0x59: symbol = "e"; scanCode = scanCodeE; break;
    case 0x89: symbol = "n"; scanCode = scanCodeN; break;
    case 0x19: symbol = "g"; scanCode = scanCodeG; break;
    case 0x21: symbol = "["; scanCode = scanCodeLBracket; break;
    case 0x41: symbol = "]"; scanCode = scanCodeRBracket; break;
    case 0xA1: symbol = "z"; scanCode = scanCodeZ; break;
    case 0xE9: symbol = "h"; scanCode = scanCodeH; break;
    case 0xA3: symbol = "*"; scanCode = scanCodeStar; break;
    case 0x99: symbol = "f"; scanCode = scanCodeF; break;
    case 0x61: symbol = "y"; scanCode = scanCodeY; break;
    case 0x11: symbol = "w"; scanCode = scanCodeW; break;
    case 0x79: symbol = "a"; scanCode = scanCodeA; break;
    case 0xF1: symbol = "p"; scanCode = scanCodeP; break;
    case 0xB1: symbol = "r"; scanCode = scanCodeR; break;
    case 0x09: symbol = "o"; scanCode = scanCodeO; break;
    case 0xC9: symbol = "l"; scanCode = scanCodeL; break;
    case 0xD9: symbol = "d"; scanCode = scanCodeD; break;
    case 0x91: symbol = "v"; scanCode = scanCodeV; break;
    case 0xC1: symbol = "\\"; scanCode = scanCodeBackslash; break;
    case 0x71: symbol = "q"; scanCode = scanCodeQ; break;
    case 0x4D: symbol = "^"; scanCode = scanCodeCaret; break;
    case 0x6D: symbol = "s"; scanCode = scanCodeS; break;
    case 0xD5: symbol = "m"; scanCode = scanCodeM; break;
    case 0xE5: symbol = "i"; scanCode = scanCodeI; break;
    case 0xBD: symbol = "t"; scanCode = scanCodeT; break;
    case 0xFD: symbol = "x"; scanCode = scanCodeX; break;
    case 0xC3: symbol = "b"; scanCode = scanCodeB; break;
    case 0x03: symbol = "@"; scanCode = scanCode2; requiresShift = true; break;
    case 0x05: symbol = ","; scanCode = scanCodeComma; break;
    case 0x69: symbol = "i"; scanCode = scanCodeI; break; // Обновлено: q → i
    case 0xD1: symbol = "t"; scanCode = scanCodeT; break; // Обновлено: i → t
    case 0xE1: symbol = "x"; scanCode = scanCodeX; break;
    case 0xB9: symbol = "b"; scanCode = scanCodeB; break;
    case 0xF9: symbol = "i"; scanCode = scanCodeI; break;
    case 0xCB: symbol = "t"; scanCode = scanCodeT; break;
    case 0xFB: symbol = "Space"; scanCode = scanCodeSpace; break;
    case 0x81: symbol = "^"; scanCode = scanCodeCaret; break;
    case 0x31: symbol = "s"; scanCode = scanCodeS; break;
    case 0x49: symbol = "m"; scanCode = scanCodeM; break;
    case 0xEF: symbol = "Backspace"; scanCode = scanCodeBackspace; break;
    case 0x4F: symbol = "Enter"; scanCode = scanCodeEnter; break;
    // Новые коды
    case 0x7C: symbol = "Up"; scanCode = scanCodeUp; isExtended = true; break; // Стрелка вверх
    case 0xBC: symbol = "Down"; scanCode = scanCodeDown; isExtended = true; break; // Стрелка вниз
    case 0x3C: symbol = "Right"; scanCode = scanCodeRight; isExtended = true; break; // Стрелка вправо
    case 0xDC: symbol = "Left"; scanCode = scanCodeLeft; isExtended = true; break; // Стрелка влево
    case 0x6F: symbol = "Tab"; scanCode = scanCodeTab; break; // Табуляция
    case 0x27: symbol = "Esc"; scanCode = scanCodeEsc; break; // Esc
    default: symbol = "Неизвестно"; break;
  }

  // Учитываем Caps и Shift для букв
  bool sendShift = isShiftPressed || (isCapsMode && symbol.length() == 1 && isalpha(symbol[0]));
  if (sendShift && scanCode != 0 && !requiresShift) {
    symbol.toUpperCase();
  }

  // Вывод символа в Serial Monitor
  Serial.print(" -> Символ: ");
  Serial.println(symbol);

  // Отправка PS/2 скан-кода
  if (scanCode != 0) {
    if (sendShift || requiresShift) {
      sendPS2Byte(scanCodeShift); // Нажатие Shift
    }
    if (isExtended) {
      sendPS2Byte(scanCodeExtended); // Отправляем префикс 0xE0 для стрелок
    }
    sendPS2Byte(scanCode); // Нажатие клавиши

    // Отправка отпускания
    if (isExtended) {
      sendPS2Byte(scanCodeExtended); // Префикс для отпускания
    }
    sendPS2Byte(scanCodeRelease);
    sendPS2Byte(scanCode);
    if (sendShift || requiresShift) {
      sendPS2Byte(scanCodeRelease);
      sendPS2Byte(scanCodeShift); // Отпускание Shift
    }
  }
}

// Функция для отправки скан-кода по PS/2
void sendPS2Byte(uint8_t data) {
  Serial.print("Отправка PS/2: 0x");
  Serial.println(data, HEX);

  noInterrupts();

  // Вычисление бита чётности (odd parity)
  uint8_t parity = 1;
  for (uint8_t i = 0; i < 8; i++) {
    if (data & (1 << i)) parity ^= 1;
  }

  // Запрещаем хосту вмешиваться
  pinMode(ps2ClockPin, OUTPUT);
  digitalWrite(ps2ClockPin, LOW);
  delayMicroseconds(120);

  // 1. Стартовый бит (0)
  pinMode(ps2DataPin, OUTPUT);
  digitalWrite(ps2DataPin, LOW);
  delayMicroseconds(30);
  pinMode(ps2ClockPin, OUTPUT);
  digitalWrite(ps2ClockPin, LOW);
  delayMicroseconds(60);
  digitalWrite(ps2ClockPin, HIGH);
  delayMicroseconds(40);

  // 2. 8 бит данных
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(ps2DataPin, (data & (1 << i)) ? HIGH : LOW);
    delayMicroseconds(30);
    digitalWrite(ps2ClockPin, LOW);
    delayMicroseconds(60);
    digitalWrite(ps2ClockPin, HIGH);
    delayMicroseconds(40);
  }

  // 3. Бит чётности
  digitalWrite(ps2DataPin, parity ? HIGH : LOW);
  delayMicroseconds(30);
  digitalWrite(ps2ClockPin, LOW);
  delayMicroseconds(60);
  digitalWrite(ps2ClockPin, HIGH);
  delayMicroseconds(40);

  // 4. Стоповый бит (1)
  digitalWrite(ps2DataPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(ps2ClockPin, LOW);
  delayMicroseconds(60);
  digitalWrite(ps2ClockPin, HIGH);
  delayMicroseconds(40);

  // Освобождаем линии
  pinMode(ps2DataPin, INPUT);
  pinMode(ps2ClockPin, INPUT);

  interrupts();
}
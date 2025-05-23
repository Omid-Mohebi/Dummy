#include <Keypad.h>
#include <LiquidCrystal.h>
#include <math.h>

// اعلان توابع قبل از setup
void clearAll();
void updateLCD();
float compute(float a, char op, float b);
void backspace();
void toggleMode();

// تنظیم LCD: LiquidCrystal(rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// پیکربندی کی‌پد 4x6 (updated to 4 rows, 6 columns)
const byte ROWS = 4;
const byte COLS = 6;
char keys[ROWS][COLS] = {
  {'C', '7', '8', '9', '*', '/'},
  {'$', '4', '5', '6', '-', '^'},
  {'%', '1', '2', '3', '+', 'S'},
  {'R', '0', '.', '=', '+', 'O'}
};
byte rowPins[ROWS] = {A2, A3, A4, A5};
byte colPins[COLS] = {0, 1, 2, 3, 4, 5}; // Added 2 more pins for 6 columns

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String expr = "";     // کل عبارت (برای نمایش در خط اول)
String current = "";  // عدد در حال ساخت (برای نمایش در خط دوم)
char lastOp = 0;      // آخرین عملگر واردشده
float result = 0;     // نتیجه موقت
bool angleMode = false; // false = degrees, true = radians
bool justCalculated = false; // Track if we just finished a calculation

void setup() {
  lcd.begin(16, 2);
  lcd.print("Advanced Calc");
  delay(1500);
  clearAll();
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  // If we just calculated and user enters a number, start fresh
  if (justCalculated && (key >= '0' && key <= '9')) {
    clearAll();
    justCalculated = false;
  }

  if (key >= '0' && key <= '9') {
    current += key;
    justCalculated = false;
  }
  else if (key == '.') {
    if (current.indexOf('.') == -1) { // Only add if no decimal point exists
      if (current.length() == 0) current = "0";
      current += key;
    }
    justCalculated = false;
  }
  else if (key=='+'||key=='-'||key=='*'||key=='/'||key=='^') {
    // Handle negative numbers
    if (current.length() == 0 && key=='-' && expr.length() == 0) {
      current = "-";
    }
    else if (current.length() > 0) {
      // Perform calculation if there's a pending operation
      if (expr.length() > 0 && lastOp != 0) {
        result = compute(result, lastOp, current.toFloat());
        current = String(result);
      } else {
        result = current.toFloat();
      }
      
      lastOp = key;
      expr = current + key;
      current = "";
      justCalculated = false;
    }
    // If we just calculated, use the result for next operation
    else if (justCalculated && result != 0) {
      lastOp = key;
      expr = String(result) + key;
      current = "";
      justCalculated = false;
    }
  }
  else if (key == '=') {
    if (expr.length() > 0 && current.length() > 0 && lastOp != 0) {
      result = compute(result, lastOp, current.toFloat());
      expr = "";
      current = String(result);
      lastOp = 0;
      justCalculated = true;
    }
  }
  else if (key == 'C') {
    clearAll();
    justCalculated = false;
  }
  else if (key == '$') {
    backspace();
    justCalculated = false;
  }
  else if (key == '%') {
    toggleMode();
  }
  else if (key == 'R') {
    if (current.length() > 0) {
      float val = current.toFloat();
      if (val >= 0) {
        result = sqrt(val);
        current = String(result);
        expr = "√" + String(val);
        justCalculated = true;
      } else {
        current = "Error";
      }
    }
  }
  else if (key == 'S') {
    if (current.length() > 0) {
      float val = current.toFloat();
      if (angleMode) {
        result = sin(val); // radians
      } else {
        result = sin(val * PI / 180.0); // degrees to radians
      }
      current = String(result);
      expr = "sin(" + String(val) + ")";
      justCalculated = true;
    }
  }
  else if (key == 'O') {
    if (current.length() > 0) {
      float val = current.toFloat();
      if (angleMode) {
        result = cos(val); // radians
      } else {
        result = cos(val * PI / 180.0); // degrees to radians
      }
      current = String(result);
      expr = "cos(" + String(val) + ")";
      justCalculated = true;
    }
  }

  updateLCD();
}

float compute(float a, char op, float b) {
  switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b != 0) ? a / b : 0;
    case '^': return pow(a, b);
    default:  return b;
  }
}

void clearAll() {
  expr = "";
  current = "";
  lastOp = 0;
  result = 0;
  lcd.clear();
}

void backspace() {
  if (current.length() > 0) {
    current = current.substring(0, current.length() - 1);
  }
}

void toggleMode() {
  angleMode = !angleMode;
  lcd.setCursor(12, 0);
  if (angleMode) {
    lcd.print("RAD");
  } else {
    lcd.print("DEG");
  }
  delay(1000);
}

void updateLCD() {
  // Clear and display expression on first line
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  
  // Truncate expression if too long
  String displayExpr = expr;
  if (displayExpr.length() > 12) {
    displayExpr = displayExpr.substring(displayExpr.length() - 12);
  }
  lcd.print(displayExpr);
  
  // Show angle mode indicator
  lcd.setCursor(13, 0);
  if (angleMode) {
    lcd.print("RAD");
  } else {
    lcd.print("DEG");
  }

  // Clear and display current number on second line
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  
  // Truncate current number if too long
  String displayCurrent = current;
  if (displayCurrent.length() > 16) {
    displayCurrent = displayCurrent.substring(0, 16);
  }
  lcd.print(displayCurrent);
}

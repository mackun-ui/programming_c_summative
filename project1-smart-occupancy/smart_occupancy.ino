#include <LiquidCrystal.h>

// ---------- Pins ----------
const int ENTRY_PIN = 7;
const int EXIT_PIN  = 8;
const int GREEN_LED = 9;
const int RED_LED   = 10;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ---------- Settings ----------
const int MAX_CAPACITY = 5;
const unsigned long DEBOUNCE_MS = 50;
const unsigned long LCD_REFRESH_MS = 500;

// ---------- Structures ----------
// Holds all occupancy information
typedef struct {
  int current;                   // people currently inside
  int maxCapacity;               // facility limit
  unsigned long totalEntries;
  unsigned long totalExits;
  unsigned long rejectedEntries;
  unsigned long rejectedExits;
} Occupancy;

// Holds the debounce state of one button
typedef struct {
  int pin;
  int lastReading;
  int stableState;
  unsigned long lastChange;
} Button;

void setup() {
}

void loop() {
}
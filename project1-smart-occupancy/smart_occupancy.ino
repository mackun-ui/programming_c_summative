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

// ---------- Global variables ----------
Occupancy *occ = NULL;                      // will point to dynamically allocated memory
Button entryBtn = {ENTRY_PIN, HIGH, HIGH, 0};
Button exitBtn  = {EXIT_PIN,  HIGH, HIGH, 0};
unsigned long lastLcd = 0;

// ---------- Memory allocation ----------
// Allocates an Occupancy record and sets starting values.
// Returns NULL if there is not enough memory.
Occupancy *createOccupancy(int maxCap) {
  Occupancy *o = (Occupancy *)malloc(sizeof(Occupancy));
  if (o == NULL) {
    return NULL;
  }
  o->current = 0;
  o->maxCapacity = maxCap;
  o->totalEntries = 0;
  o->totalExits = 0;
  o->rejectedEntries = 0;
  o->rejectedExits = 0;
  return o;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  pinMode(ENTRY_PIN, INPUT_PULLUP);
  pinMode(EXIT_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  lcd.begin(16, 2);

  occ = createOccupancy(MAX_CAPACITY);
  if (occ == NULL) {
    Serial.println("ERROR: memory allocation failed");
    lcd.print("Memory error");
    while (true) {}                         // stop the program safely
  }
  Serial.println("=== Smart Occupancy System Ready ===");
}

void loop() {
}
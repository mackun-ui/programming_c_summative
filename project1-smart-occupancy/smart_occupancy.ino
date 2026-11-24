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

// Declared manually so the compiler knows the Button type
bool buttonPressed(Button *b);

void handleEntry(Occupancy *o);
void handleExit(Occupancy *o);
void updateLEDs(Occupancy *o);
void printStats(Occupancy *o);

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

// ---------- Non-blocking button debounce ----------
// Returns true once each time the button is pressed.
// Uses millis() instead of delay(), so the program never freezes.
bool buttonPressed(Button *b) {
  int reading = digitalRead(b->pin);
  bool pressed = false;

  if (reading != b->lastReading) {
    b->lastChange = millis();               // reading changed, restart the timer
  }
  if ((millis() - b->lastChange) > DEBOUNCE_MS && reading != b->stableState) {
    b->stableState = reading;
    if (b->stableState == LOW) {            // LOW = pressed (pull-up wiring)
      pressed = true;
    }
  }
  b->lastReading = reading;
  return pressed;
}

// ---------- Entry: adds one person, rejects if full ----------
void handleEntry(Occupancy *o) {
  if (o->current >= o->maxCapacity) {
    o->rejectedEntries++;
    Serial.println("ERROR: Entry rejected - facility is full");
    return;
  }
  o->current++;
  o->totalEntries++;
  Serial.print("EVENT: Entry | Inside: ");
  Serial.println(o->current);
}

// ---------- Exit: removes one person, rejects if empty ----------
void handleExit(Occupancy *o) {
  if (o->current <= 0) {
    o->rejectedExits++;
    Serial.println("ERROR: Exit rejected - facility is empty");
    return;
  }
  o->current--;
  o->totalExits++;
  Serial.print("EVENT: Exit | Inside: ");
  Serial.println(o->current);
}

// ---------- LEDs: green if space, red if full ----------
void updateLEDs(Occupancy *o) {
  bool full = (o->current >= o->maxCapacity);
  digitalWrite(GREEN_LED, full ? LOW : HIGH);
  digitalWrite(RED_LED, full ? HIGH : LOW);
}

// ---------- Prints statistics to the Serial Monitor ----------
void printStats(Occupancy *o) {
  Serial.print("STATS | In:");
  Serial.print(o->totalEntries);
  Serial.print(" Out:");
  Serial.print(o->totalExits);
  Serial.print(" RejectedIn:");
  Serial.print(o->rejectedEntries);
  Serial.print(" RejectedOut:");
  Serial.println(o->rejectedExits);
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
  updateLEDs(occ);
}

void loop() {
  bool changed = false;

  if (buttonPressed(&entryBtn)) {
    handleEntry(occ);
    changed = true;
  }
  if (buttonPressed(&exitBtn)) {
    handleExit(occ);
    changed = true;
  }

  if (changed) {
    updateLEDs(occ);
    printStats(occ);
  }
}
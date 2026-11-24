# Technical Documentation

## Structures
- `Occupancy`: stores current count, maximum capacity, total entries and exits, and rejected entries and exits.
- `Button`: stores the pin and debounce state of one button.

## Pointers and dynamic memory
`createOccupancy()` uses `malloc()` to create the Occupancy record. If there isn't enough memory it returns NULL, and the program stops and prints an error. Every other function gets a pointer to this record, like `handleEntry(Occupancy *o)`, and changes the values through `->`. I never call `free()` because the Arduino keeps running forever and needs this record the whole time, but if the program could exit, I would call `free(occ)` at the end.

## Non-blocking programming
`buttonPressed()` debounces each button using `millis()` (50 ms). The LCD refreshes every 500 ms using `millis()`. `delay()` is never used.

## Functions
- `createOccupancy()`: allocates and initializes the record.
- `buttonPressed()`: debounced button detection.
- `handleEntry()` / `handleExit()`: update the count and reject invalid operations.
- `updateLEDs()`: green when there is space, red when full.
- `updateLCD()`: shows people inside and free spaces.
- `printStats()`: prints statistics to the Serial Monitor.

## Error handling
- Entry when full and exit when empty are rejected, with a message in the Serial Monitor.
- Memory allocation failure stops the program with a message.
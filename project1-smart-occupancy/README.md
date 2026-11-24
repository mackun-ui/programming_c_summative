# Smart Occupancy Monitoring and Access Control System

Arduino Uno simulation in Tinkercad. Two push buttons simulate people entering and leaving a facility with a maximum capacity of 5. A 16x2 LCD shows the occupancy and remaining capacity, and two LEDs show the status.

Tinkercad circuit: https://www.tinkercad.com/things/h15cLhvrSe9/editel?returnTo=%2Fdashboard&sharecode=_-LHnaAf-BByrT302Qt-2wm-tjcPQD0MmYXSJbSFM34 

## Components
Arduino Uno, 2 push buttons, green LED, red LED, 16x2 LCD, 2 220 ohm resistors, small breadboard, jumper wires.

## Pin mapping
| Component | Arduino pin |
|---|---|
| Entry button | 7 |
| Exit button | 8 |
| Green LED | 9 |
| Red LED | 10 |
| LCD RS, E | 12, 11 |
| LCD DB4 to DB7 | 5, 4, 3, 2 |

## How it works
- Entry button: adds one person; rejected with an error if the facility is full.
- Exit button: removes one person; rejected with an error if the facility is empty.
- Green LED on when there is space, red LED on when full.
- LCD shows people inside and free spaces; Serial Monitor shows events, errors and statistics.

## Screenshots
See the `screenshots` folder.
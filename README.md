# LG-HTS-IR-Repeater
Arduino based IR repeater for the LG BH9540TW Home Theatre System.

## Goal
1. Control my LG BH9540TW while inside my TV cabinet (behind a closed door).
2. Control my LG BH9540TW with a different remote control (my Philips OLED TV).

## Functionalities
To acomplish the goals above, the device has an IR-receiver connected to the arduino via a ± 50cm cable. This way, the Arduino can sit inside the cabinet and the receiver can be somewhere near the TV. The device detects signals using the IR receiver, and transmits signals using an IR LED that is mounted on top of the Arduino board.

I have implemented 2 main functionalities:
1. Repeat all IR signals coming from the remote control of a LG BH9540TW
2. Send out a specified signal upon receiving some specified signals (of my Philips tv remote)

## Dependencies
This project depends on the following Arduino libraries:
- [Arduino-IRremote](https://github.com/Arduino-IRremote/Arduino-IRremote)

## Hardware
This project in built on the Arduino Uno (v3), although the code is most likely compatible with other models. For other models than the Uno, please check the if the dependencies support it.

### Connection diagram
_TODO: diagram here_

## Software
_TODO: explain the code here_

### Manufacturer specific codes
_TODO: explain about the specific codes of Philips and LG here. Also give hints on how to detect your own codes (using the ReceiverDumpV2 script)._


# Credits

Credits go to:
- [Ken Shirriff](https://github.com/shirriff) for creating the Arduino-IRremote library, and [all the contributors](https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/Contributors.md) for improving/expanding his work.
- [Mitchell Tucker](https://github.com/MitchTODO) for his [IR-repeater script](https://github.com/MitchTODO/Arduino-IR-repeater/blob/master/irRepeater.ino).
- dzacace for [showing how to send Samsung signals](https://forum.arduino.cc/index.php?topic=591256.0) using the Arduino-IRremote library.


# Therapy Timer System

This repository contains the code for an AVR-based therapy timer system with an OLED display, buttons, LEDs, and a buzzer. The system allows users to select a therapy mode and time duration, and it provides a countdown with visual and auditory feedback.

## Components
- **AVR Microcontroller**
- **OLED Display** (128x64 pixels)
- **Buttons** for user input
- **LEDs** to indicate therapy modes
- **Buzzer** for audio signals

## Pin Assignments
- **OLED Display**: I2C interface
- **Buttons**:
  - `SWITCH_PIN_UP`: Pin 5 (Move arrow up)
  - `SWITCH_PIN_SELECT`: Pin 6 (Select time)
  - `switchPin1`: Pin 2 (Mode selection)
  - `switchPin2`: Pin 3 (Start therapy)
- **LEDs**:
  - `GroupLed1`: Pin 7
  - `GroupLed2`: Pin 8
  - `GroupLed3`: Pin 9
- **Buzzer**: Pin 10

## Setup
1. **Initialize Components**:
   - Serial communication at 9600 baud rate.
   - OLED display.
   - Set pin modes for buttons, LEDs, and the buzzer.

## Functionality
### Main Loop
- **Welcome Screen**: Displays a welcome message and waits for the user to press `switchPin2`.
- **Mode Selection**: User can cycle through four therapy modes using `switchPin1`.
- **Time Selection**: User can select the duration of the therapy session using `SWITCH_PIN_UP` and confirm with `SWITCH_PIN_SELECT`.
- **Countdown Timer**: Counts down the selected time, updating the display and LEDs.
- **End of Therapy**: Displays a completion message, plays a tone, and resets the system.

### Helper Functions
- **`initiate`**: Displays the welcome message.
- **`InitTimeTherapy`**: Handles time selection and starts the countdown.
- **`CountDown`**: Manages the countdown and display updates.
- **`displayModeSelection`**: Shows available modes.
- **`displayTimeSelection`**: Displays time options with an arrow indicator.
- **`FinishTherapy`**: Handles the end of the therapy session.
- **`debounce`**: Ensures reliable button presses.
- **`resetSystem`**: Resets the system to the initial state.
- **`waitForTime`**: Delays execution.
- **`peep`**: Plays a beep sound.

## Usage
1. Connect the components as per the pin assignments.
2. Upload the code to the AVR microcontroller.
3. Power the system and interact with it using the buttons.
4. Follow the on-screen instructions to select the mode and time, and start the therapy session.

## Notes
- Ensure proper debounce logic to avoid false button presses.
- The system resets after each therapy session, ready for the next use.

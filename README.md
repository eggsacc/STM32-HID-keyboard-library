## HID keyboard utils library

This library provides utility functions to use the STM32 as a HID keyboard. 

## How it works
The keys on your keyboard is split into 2 categories: Modifier and non-modifier keys. Modifier keys include `Alt`, `Shift`, `Ctrl`, `Option`, and all other keys are non-modifiers.

Each non-modifier key on a USB keyboard has a 8-bit identifier attached to it: `a` is `0x04`, `b` is `0x05` and so on.

USB keyboards send an array called a **report** to the host computer to tell it which keys are being pressed/released. Each report typically consists of 8x 1-byte values (usually `uint8_t`) that contains the identity of which keys are pressed. A report usualy has the following structure:

| Byte    | 1 | 2    | 3 | 4  | 5 | 6   | 7 | 8  |
| :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | 
|Purpose   | Modifier | Reserved   | Key 1 | Key2  | Key 3 | Key 4  | Key 5 | Key 6  | 

The modifier byte determines which of the 4 modifier keys are pressed. Each modifier key is represented by 1 out of the 8 bits:
| Bit   | Modifier key |
| :------: | :-----: |
| 0       | Left CTRL  |
| 1       | Left SHIFT |
| 2       | Left ALT   |
| 3       | OPTION     |
| 4       | Right CTRL |
| 5       | Right SHIFT|
| 6       | Right ALT  |
| 7       | (Unused)   |

The interesting thing about modifiers is that multiple modifier keys could be pressed simultaneously. For example, if the modifier byte is set to `B00001010`, it would mean that left SHIFT and OPTION are both depressed.

Going back to the report structure, Keys 1-6 contains the identifier code of the non-modifier keys being pressed.

So if we press CTRL + R on our keyboard, the report array will look like this:

> The identifier code of the R button is `0x15`.

| Byte    | 1 | 2    | 3 | 4  | 5 | 6   | 7 | 8  |
| :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | 
|Value   | `0x01` | Reserved   | `0x15` | `0x00` | `0x00` | `0x00`  | `0x00` | `0x00` | 

## Pre-requisites 

Make sure that the STM32 board you are using is configured properly to act as an USB device, using the Human Interface Device (HID) class.

![image](https://github.com/user-attachments/assets/5f582cf7-255e-4abc-8f26-7d8629aafd9b)


![image](https://github.com/user-attachments/assets/10624a57-4f0c-4148-899e-68f9a077cbae)

## Functions

Utility functions allowing the user to mimic keyboard key presses.

### 1) `KeyboardInit();`
   - (Must be) called before main loop to initialize variables.
### 2) `TypeString(const char* str);`
   - Types a string of text character by character like a keyboard.
   - Example: `TypeString("Why are apples not blue?");`
### 3) `ModifiersAndKey(uint8_t modifiers, const char* key);`
   - Simulate pressing one or more modifier keys and a single other key together.
   - Available operation keys: `/esc, /tab, /del`.
   - Example: To press `Ctrl + Shift + Delete`, we can use `ModifiersAndKey(MODIFIER_LCTRL | MODIFIER_LSHIFT, "/del");`
   - Multiple modifier keys should be combined with bitwise or `(|)`.
### 4) `SingleKeypress(const char* key);`
   - Press a single key. Excludes modifier keys since pressing them alone does nothing most of the time.
   - Available operation keys: `/esc, /tab, /del, /space, /option, /enter`.
   - Example: `SingleKeypress("/option");`
### 5) `KeyboardDeinit();`
   - De-allocates memory from structures within the program; MUST be included at the end of program.

## Modifier key macros
```c
/*
 * Modifier keys bits. OPTION is the Window / Command key depending on platform.
 */
#define MODIFIER_LCTRL    1
#define MODIFIER_LSHIFT  (1 << 1)
#define MODIFIER_LALT    (1 << 2)
#define MODIFIER_OPTION  (1 << 3)
#define MODIFIER_RCTRL   (1 << 4)
#define MODIFIER_RSHIFT  (1 << 5)
#define MODIFIER_RALT    (1 << 6)
```
## Unsupported keys (lazy add)

- All function keys (`fn`)
- Arrow keys
- Backspace (only `Del` available)
- Capslock
- Tilde key (`~`)

## Example code: opening Chrome and typing in rickroll URL
```c
Keyboardinit();

/* Open windows search and open Chrome */
ModifiersAndKey(MODIFIER_OPTION, "s");
HAL_Delay(100);
TypeString("chrome");
SingleKeypress("/enter");
HAL_Delay(100);

/* Type rickroll URL */
TypeString("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
SingleKeypress("/enter");

KeyboardDeinit();
```

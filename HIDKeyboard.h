/*
 * string_input.h
 *
 *  Created on: Dec 18, 2024
 *      Author: yizha
 *
 *  References: https://www.instructables.com/STM32-As-HID-USB-Keyboard-STM32-Tutorials/
 *              https://controllerstech.com/how-to-use-stm32-as-a-keyboard/
 *
 *  USB HID usage table: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
 *
 *  USB HID documentation (Pg. 65): https://www.usb.org/sites/default/files/documents/hid1_11.pdf
 *
 */
#ifndef INC_HIDKEYBOARD_H_
#define INC_HIDKEYBOARD_H_

/*
 * Includes
 */
#include"ctype.h"
#include"string.h"
#include"main.h"
#include"usbd_hid.h"
#include"usb_device.h"

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

/*
 * Operation key code
 */
#define KEY_ENTER  0x28
#define KEY_ESCAPE 0x29
#define KEY_DELETE 0x2A
#define KEY_TAB    0x2B
#define KEY_SPACE  0x2C

/*
 * Time between pressing and releasing a key. 15ms seems to be the fastest it can go.
 */
#define KEY_INTERVAL 15

/*
 * USB handle definition
 */
extern USBD_HandleTypeDef hUsbDeviceFS;

/*
 * Keyboard report struct
 */
typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReport;

extern keyboardReport* report_struct;
extern uint8_t report_size;

/*
 * Public functions
 */
void KeyboardInit();
void TypeString(const char* str);
void ModifiersAndKey(uint8_t modifiers, const char* key);
void SingleKeypress(const char* key);
void KeyboardDeinit();

//void Enter();
//void Delete();
//void Tab();
//void Escape();
//void Option();

#endif /* INC_HIDKEYBOARD_H_ */

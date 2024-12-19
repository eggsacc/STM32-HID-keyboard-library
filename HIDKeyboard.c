/*
 * string_input.c
 *
 *  Created on: Dec 18, 2024
 *      Author: yizha
 */

#include "HIDKeyboard.h"

keyboardReport* report_struct = NULL;
uint8_t report_size;
/*
 * @scope static
 *
 * @brief Send keyboard reports to simulate key press
 *
 * @return void
 */
static void PressKeys()
{
	/* Press keys */
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report_struct, report_size);
	HAL_Delay(KEY_INTERVAL);

	/* Release keys */
	memset(report_struct, 0, report_size);
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report_struct, report_size);
	HAL_Delay(KEY_INTERVAL);
}

/*
 * @scope static
 *
 * @brief Map character to corresponding key code
 *
 * ASCII value and key code of alphabets & numbers are consecutive,
 * hence they could be mapped using an offset.
 * Other miscellaneous characters are mapped using switch cases.
 *
 * @param[in] const char* a Character to be mapped
 *
 * @return void
 */
static void KeyMap(char s)
{
	/* Capital letters, ASCII 0x41-0x5A */
	if(0x41 <= s && s <= 0x5A)
	{
		report_struct->MODIFIER = MODIFIER_LSHIFT;
		/* Map ASCII 0x41-0x5A -> Key code 0x04-0x1D */
		report_struct->KEYCODE1 = s - 0x3D;
		return;
	}

	/* Lower case letters, ASCII 0x61-0x7A */
	else if(0x61 <= s && s <= 0x7A)
	{
		/* Map ASCII to key press: offset 0x61 - 0x04 = 0x5D */
		report_struct->KEYCODE1 = s - 0x5D;
		return;
	}

	/* Numbers 0-9: ASCII 0x30-0x39 */
	else if(0x30 <= s && s <= 0x39)
	{
		/*
		 * Map ASCII to key press.
		 * ASCII order is 0-9, but keyboard layout is 1-9 then 0.
		 * Hence, map ASCII from 1-9, then handle 0 as special case.
		 */
		report_struct->KEYCODE1 = (s == 0x30) ? 0x27 : s - 0x13;
	}

	/* Primary symbols that do not require shift to be held */
	else if(s==' ' || s=='/' || s=='-' || s=='=' || s=='[' || s==']' || s==';' || s==',' || s=='.')
	{
		switch(s){
		case' ': report_struct->KEYCODE1 = KEY_SPACE; return;
		case'/': report_struct->KEYCODE1 = 0x38; return;
		case'-': report_struct->KEYCODE1 = 0x2D; return;
		case'=': report_struct->KEYCODE1 = 0x2E; return;
		case'[': report_struct->KEYCODE1 = 0x2F; return;
		case']': report_struct->KEYCODE1 = 0x30; return;
		case';': report_struct->KEYCODE1 = 0x33; return;
		case',': report_struct->KEYCODE1 = 0x36; return;
		case'.': report_struct->KEYCODE1 = 0x37; return;
		}
	}

	/* Secondary symbols that required Shift + key */
	else
	{
		uint8_t temp = 0;
		switch(s){
		case'!': temp = 0x1E; break;
		case'@': temp = 0x1F; break;
		case'#': temp = 0x20; break;
		case'$': temp = 0x21; break;
		case'%': temp = 0x22; break;
		case'^': temp = 0x23; break;
		case'&': temp = 0x24; break;
		case'*': temp = 0x25; break;
		case'(': temp = 0x26; break;
		case')': temp = 0x27; break;
		case'_': temp = 0x2D; break;
		case'+': temp = 0x2E; break;
		case':': temp = 0x33; break;
		case'<': temp = 0x36; break;
		case'>': temp = 0x37; break;
		case'?': temp = 0x38; break;
		default: temp = 0; break;
		}

		report_struct->MODIFIER = MODIFIER_LSHIFT;
		report_struct->KEYCODE1 = temp;
		return;
	}
}

/*
 * @brief Initialization function for keyboard.
 *
 * Mallocs memory for the report structure based on its size.
 *
 * @return void
 *
 * @warning If function not called first in program, the report struct will be NULL by default.
 */
void KeyboardInit()
{
	/* Malloc report struct */
	report_size = sizeof(keyboardReport);
	report_struct = (keyboardReport*)malloc(report_size);

	/* Initialize to all 0 */
	memset(report_struct, 0, report_size);
}

/*
 * @brief Type a string of text.
 *
 * @param[in] const char* str C-String to be typed
 *
 * @return void
 *
 * @warning Does nothing if KeyboardInit() not called prior.
 */
void TypeString(const char* str)
{
	/* Check if initialized properly */
	if(report_struct == NULL)
	{
		return;
	}

	/* Clear report struct */
	memset(report_struct, 0, report_size);

	unsigned int i = 0;

	while(str[i] != '\0')
	{
		KeyMap(str[i]);
		PressKeys();
		i++;
	}
}

/*
 * @brief Press modifier keys and another key.
 *
 * Allows for one or more modifier keys (CTRL, SHIFT, ALT, OPTION) to be pressed
 * together with another non-modifier key. E.g CTRL+SHIFT+ESCAPE
 *
 * @param[in] uint8_t modifiers Value of modifier(s)
 * @param[in] const char* key Non-modifier key to be pressed together. Operation keys can be specified with a blackslash.
 *
 * @note Perform bitwise OR to combine modifiers.
 *       For example, CTRL + SHIFT will be :
 *       MODIFIER_CTRL | MODIFIER_SHIFT
 *
 *       Operation keys (DEL, TAB, ESC) can be specified with a backslash:
 *       /esc: escape
 *       /del: delete
 *       /tab: tab
 *
 * @warning Only one input key allowed. Does nothing otherwise.
 * 			Does nothing if report struct uninitialized.
 */
void ModifiersAndKey(uint8_t modifiers, const char* key)
{
	/* Check key */
	if(key[1] != '\0' && key[0] != '/')
	{
		return;
	}

	/* Check if report struct initialized */
	if(report_struct == NULL)
	{
		return;
	}

	/* Clear report struct */
	memset(report_struct, 0, report_size);

	report_struct->MODIFIER = modifiers;

	/* Check for operation keys, distinguished by a backslash. */
	if(key[0] == '/' && key[1] != '\0')
	{
		if(strcmp(key, "/esc") == 0)
		{
			report_struct->KEYCODE1 = KEY_ESCAPE;
		}
		else if(strcmp(key, "/del") == 0)
		{
			report_struct->KEYCODE1 = KEY_DELETE;
		}
		else if(strcmp(key, "/tab") == 0)
		{
			report_struct->KEYCODE1 = KEY_TAB;
		}
	}
	else
	{
		/* If not operation key, use normal key map */
		KeyMap(key[0]);
	}

	PressKeys();
}

/********************** Single-press operation keys **********************/
/*
 * @brief Press a single key.
 *
 * @param[in] const char* key Key to press.
 *
 * @note If the key to press is a operation/modifier key, use a backslash.
 * 		 E.g SingleKeypress("/esc");
 *
 * @warning Does nothing if report struct uninitialized.
 */
void SingleKeypress(const char* key)
{
	/* Check if report struct initialized */
	if(report_struct == NULL)
	{
		return;
	}

	/* Handle operation keys, distinguished by backslash */
	if(key[0] == '/' && key[1] != '\0')
	{
		if(strcmp(key, "/esc") == 0)
		{
			report_struct->KEYCODE1 = KEY_ESCAPE;
		}
		else if(strcmp(key, "/del") == 0)
		{
			report_struct->KEYCODE1 = KEY_DELETE;
		}
		else if(strcmp(key, "/tab") == 0)
		{
			report_struct->KEYCODE1 = KEY_TAB;
		}
		else if(strcmp(key, "/space") == 0)
		{
			report_struct->KEYCODE1 = KEY_SPACE;
		}
		else if(strcmp(key, "/enter") == 0)
		{
			report_struct->KEYCODE1 = KEY_ENTER;
		}
	}

	else
	{
		/* If not operation key, use normal key map */
		KeyMap(key[0]);
	}

	PressKeys();

}

/*
 * @brief Deallocates memory used by the report struct.
 *
 * @warning Should be called at end of program to avoid memory leaks.
 */
void KeyboardDeinit()
{
	if(report_struct != NULL)
	{
		free(report_struct);
		report_struct = NULL;
	}
}

/************** replaced by SingleKeyPress ******************/
/*

void Enter()
{
	memset(report_struct, 0, report_size);
	report_struct->KEYCODE1 = KEY_ENTER;
	PressKeys();
}

void Delete()
{
	memset(report_struct, 0, report_size);
	report_struct->KEYCODE1 = KEY_DELETE;
	PressKeys();
}

void Tab()
{
	memset(report_struct, 0, report_size);
	report_struct->KEYCODE1 = KEY_TAB;
	PressKeys();
}

void Escape()
{
	memset(report_struct, 0, report_size);
	report_struct->KEYCODE1 = KEY_ESCAPE;
	PressKeys();
}

void Option()
{
	memset(report_struct, 0, report_size);
	report_struct->MODIFIER = MODIFIER_OPTION;
	PressKeys();
}

*/




#include <ctype.h>
#include "general.h"
#include <stdio.h>

uint8_t hex_to_digit(char hex)
{
  if ( (hex >= '0' && hex <= '9') ||
       (hex >= 'a' && hex <= 'f') ||
       (hex >= 'A' && hex <= 'F') ) {

    return (hex <= '9') ? hex - '0' : (char)toupper((int)hex) - 'A' + 10;
  }

  return 0;
}

uint8_t hex_to_byte(const char* hex)
{
  return (hex_to_digit(*hex) << 4) | hex_to_digit(*(hex + 1));
}

atlk_rc_t hexstr_to_bytes_arr(const char *hexstr, size_t hexstr_len,
                                 uint8_t *buffer, size_t *buffer_len)
{
  const char *pos = hexstr;
  size_t count = 0;

  if (!hexstr || !buffer){
    return ATLK_E_INVALID_STATE;
  }

  if (hexstr_len % 2 != 0)
    return ATLK_E_INVALID_ARG;


  while (hexstr_len) {
    if (count == *buffer_len)
      return ATLK_E_OUT_OF_BOUNDS;

    buffer[count++] = hex_to_byte(pos);
    pos += 2;
    hexstr_len -= 2;
  }

  *buffer_len = count;
  /*
  printf("0x");
  for (count = 0; count < *buffer_len; count++)
  printf("%02x", buffer[count]);
  printf("\n");
  */
  return ATLK_OK;
}
 
/*

name: bulid_hex_str

in: length of string, ponter to empty hex string

out: pointer to filled hex string

return: atlk return code

description: this function builds a cyclic hex string from "0x00" to "0xFF" according to input string length

*/
atlk_rc_t bulid_hex_str(char * hex_str, int str_len)
{
  int i = 0;
  
  if (str_len < 1){
	return ATLK_E_INVALID_ARG;																																																																																																																																																																																																																																																																
  }

  for (i = 0; i < str_len; i++){
	hex_str += sprintf(hex_str, "%02x", (i % 0x100));
  }
  
  return ATLK_OK;
}



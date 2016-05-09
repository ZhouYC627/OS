#include "x86.h"

unsigned int getKeyCode(){
    unsigned int code = in_byte(0x60);
    unsigned int val = in_byte(0x61);
    out_byte(0x61, val | 0x80);
    out_byte(0x61, val);
    return code;
}

unsigned int trans_key(unsigned int key){
  /*if (key>=0x10 && key<=0x32){
    return('A' + key - 0x1e);
  }else return 0;*/
  char ch = 0;
  switch (key) {
    case 2: ch = '1'; break;
    case 3: ch = '2'; break;
    case 4: ch = '3'; break;
    case 5: ch = '4'; break;
    case 6: ch = '5'; break;
    case 7: ch = '6'; break;
    case 8: ch = '7'; break;
    case 9: ch = '8'; break;
    case 10: ch = '9'; break;
    case 11: ch = '0'; break;
    case 12: ch = '-'; break;
    case 13: ch = '='; break;

    case 15: ch = '\t'; break;
    case 16: ch = 'Q'; break;
    case 17: ch = 'W'; break;
    case 18: ch = 'E'; break;
    case 19: ch = 'R'; break;
    case 20: ch = 'T'; break;
    case 21: ch = 'Y'; break;
    case 22: ch = 'U'; break;
    case 23: ch = 'I'; break;
    case 24: ch = 'O'; break;
    case 25: ch = 'P'; break;
    case 26: ch = '['; break;
    case 27: ch = ']'; break;

    case 30: ch = 'A'; break;
    case 31: ch = 'S'; break;
    case 32: ch = 'D'; break;
    case 33: ch = 'F'; break;
    case 34: ch = 'G'; break;
    case 35: ch = 'H'; break;
    case 36: ch = 'J'; break;
    case 37: ch = 'K'; break;
    case 38: ch = 'L'; break;
    case 39: ch = ';'; break;
    //case 40: ch = '\''; break;

    case 44: ch = 'Z'; break;
    case 45: ch = 'X'; break;
    case 46: ch = 'C'; break;
    case 47: ch = 'V'; break;
    case 48: ch = 'B'; break;
    case 49: ch = 'N'; break;
    case 50: ch = 'M'; break;
    case 51: ch = ','; break;
    case 52: ch = '.'; break;
    case 0x39: ch = ' '; break;
    default: ch = -1;break;
  }
  return ch;
}

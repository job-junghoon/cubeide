/*
 * 404202da1e.c
 *
 *  Created on: 2020. 2. 11.
 *      Author: hhwang
 */

#include "40t202da.h"
#include <string.h>


char SsVfdRuFontTable(char c){
	char rev = c;
	switch(c){
	//Big
	case 0xA1: rev = 0x41; break; //А
	case 0xA2: rev = 0x80; break; //Б
	case 0xA3: rev = 0x42; break; //В
	case 0xA4: rev = 0x92; break; //Г
	case 0xA5: rev = 0x81; break; //Д
	case 0xA6: rev = 0x45; break; //Е
	case 0xA7: rev = 0xCB; break; //Ё
	case 0xA8: rev = 0x82; break; //Ж
	case 0xA9: rev = 0x83; break; //З
	case 0xAA: rev = 0x84; break; //И
	case 0xAB: rev = 0x85; break; //Й
	case 0xAC: rev = 0x4B; break; //К
	case 0xAD: rev = 0x86; break; //Л
	case 0xAE: rev = 0x4D; break; //М
	case 0xAF: rev = 0x48; break; //Н
	case 0xB0: rev = 0x4F; break; //О
	case 0xB1: rev = 0x87; break; //П
	case 0xB2: rev = 0x50; break; //Р
	case 0xB3: rev = 0x43; break; //С
	case 0xB4: rev = 0x54; break; //Т
	case 0xB5: rev = 0x88; break; //У
	case 0xE6: rev = 0xD8; break; //ф
	case 0xB7: rev = 0x58; break; //Х
	case 0xB8: rev = 0x89; break; //Ц
	case 0xB9: rev = 0x8A; break; //Ч
	case 0xBA: rev = 0x8B; break; //Ш
	case 0xBB: rev = 0x8C; break; //Щ
	case 0xBC: rev = 0x8D; break; //Ь
	case 0xBD: rev = 0x8E; break; //Ы
	case 0xBE: rev = 0x62; break; //Ь
	case 0xBF: rev = 0x8F; break; //Э
	case 0xC0: rev = 0xAC; break; //Ю
	case 0xC1: rev = 0xAD; break; //Я
	//Small
	case 0xD1: rev = 0x41; break; //а
	case 0xD2: rev = 0x80; break; //б
	case 0xD3: rev = 0x42; break; //в
	case 0xD4: rev = 0x92; break; //г
	case 0xD5: rev = 0x81; break; //д
	case 0xD6: rev = 0x45; break; //е
	case 0xD7: rev = 0xCB; break; //ё
	case 0xD8: rev = 0x82; break; //ж
	case 0xD9: rev = 0x83; break; //з
	case 0xDA: rev = 0x84; break; //и
	case 0xDB: rev = 0x85; break; //й
	case 0xDC: rev = 0x4B; break; //к
	case 0xDD: rev = 0x86; break; //л
	case 0xDE: rev = 0x4D; break; //м
	case 0xDF: rev = 0x48; break; //н
	case 0xE0: rev = 0x4F; break; //о
	case 0xE1: rev = 0x87; break; //п
	case 0xE2: rev = 0x50; break; //р
	case 0xE3: rev = 0x43; break; //с
	case 0xE4: rev = 0x54; break; //т
	case 0xE5: rev = 0x88; break; //у
	case 0xB6: rev = 0xD8; break; //Ф
	case 0xE7: rev = 0x58; break; //х
	case 0xE8: rev = 0x89; break; //ц
	case 0xE9: rev = 0x8A; break; //ч
	case 0xEA: rev = 0x8B; break; //ш
	case 0xEB: rev = 0x8C; break; //щ
	case 0xEC: rev = 0x8D; break; //ъ
	case 0xED: rev = 0x8E; break; //ы
	case 0xEE: rev = 0x62; break; //ь
	case 0xEF: rev = 0x8F; break; //э
	case 0xF0: rev = 0xAC; break; //ю
	case 0xF1: rev = 0xAD; break; //я
	}

	return rev;
}


char AscToCode866_T2(char c, int type){
	char rev = c;

	if(type==0){
		switch(c){
			//Big
			case 0x90: rev = 0x41; break; //А
			case 0x91: rev = 0x80; break; //Б
			case 0x92: rev = 0x42; break; //В
			case 0x93: rev = 0x92; break; //Г
			case 0x94: rev = 0x81; break; //Д
			case 0x95: rev = 0x45; break; //Е
			case 0x81: rev = 0xCB; break; //Ё
			case 0x96: rev = 0x82; break; //Ж
			case 0x97: rev = 0x83; break; //З
			case 0x98: rev = 0x84; break; //И
			case 0x99: rev = 0x85; break; //Й
			case 0x9A: rev = 0x4B; break; //К
			case 0x9B: rev = 0x86; break; //Л
			case 0x9C: rev = 0x4D; break; //М
			case 0x9D: rev = 0x48; break; //Н
			case 0x9E: rev = 0x4F; break; //О
			case 0x9F: rev = 0x87; break; //П
			case 0xA0: rev = 0x50; break; //Р
			case 0xA1: rev = 0x43; break; //С
			case 0xA2: rev = 0x54; break; //Т
			case 0xA3: rev = 0x88; break; //У
			//case 0x84: rev = 0xD8; break; //ф
			case 0xA5: rev = 0x58; break; //Х
			case 0xA6: rev = 0x89; break; //Ц
			case 0xA7: rev = 0x8A; break; //Ч
			case 0xA8: rev = 0x8B; break; //Ш
			case 0xA9: rev = 0x8C; break; //Щ
			case 0xAA: rev = 0x8D; break; //Ъ
			case 0xAB: rev = 0x8E; break; //Ы
			case 0xAC: rev = 0x62; break; //Ь
			case 0xAD: rev = 0x8F; break; //Э
			case 0xAE: rev = 0xAC; break; //Ю
			case 0xAF: rev = 0xAD; break; //Я
			//Small
			case 0xB0: rev = 0x41; break; //А
			case 0xB1: rev = 0x80; break; //Б
			case 0xB2: rev = 0x42; break; //В
			case 0xB3: rev = 0x92; break; //Г
			case 0xB4: rev = 0x81; break; //Д
			case 0xB5: rev = 0x45; break; //Е
			case 0xB6: rev = 0x82; break; //Ж
			case 0xB7: rev = 0x83; break; //З
			case 0xB8: rev = 0x84; break; //И
			case 0xB9: rev = 0x85; break; //Й
			case 0xBA: rev = 0x4B; break; //К
			case 0xBB: rev = 0x86; break; //Л
			case 0xBC: rev = 0x4D; break; //М
			case 0xBD: rev = 0x48; break; //Н
			case 0xBE: rev = 0x4F; break; //О
			case 0xBF: rev = 0x87; break; //П
			case 0xA4: rev = 0xD8; break; //ф
		}
	}
	else{
		switch(c){
			//Big
			case 0x84: rev = 0xD8; break; //ф
			//Small
			case 0x91: rev = 0xCB; break; //Ё
			case 0xB7: rev = 0x83; break; //З
			case 0xB8: rev = 0x84; break; //И
			case 0xB9: rev = 0x85; break; //Й
			case 0xBA: rev = 0x4B; break; //К
			case 0xBB: rev = 0x86; break; //Л
			case 0xBC: rev = 0x4D; break; //М
			case 0xBD: rev = 0x48; break; //Н
			case 0xBE: rev = 0x4F; break; //О
			case 0xBF: rev = 0x87; break; //П
			case 0x80: rev = 0x50; break; //Р
			case 0x81: rev = 0x43; break; //С
			case 0x82: rev = 0x54; break; //Т
			case 0x83: rev = 0x88; break; //У
			case 0x85: rev = 0x58; break; //Х
			case 0x86: rev = 0x89; break; //Ц
			case 0x87: rev = 0x8A; break; //Ч
			case 0x88: rev = 0x8B; break; //Ш
			case 0x89: rev = 0x8C; break; //Щ
			case 0x8A: rev = 0x8D; break; //Ъ
			case 0x8B: rev = 0x8E; break; //Ь
			case 0x8C: rev = 0x62; break; //Э
			case 0x8D: rev = 0x8F; break; //Э
			case 0x8E: rev = 0xAC; break; //Ю
			case 0x8F: rev = 0xAD; break; //Я

		}

	}
	return rev;
}

void ConvertStringToRuForSsVfd(int row, char* data, char* out)
{
	int len = strlen(data);
	char* buf = out;

	int flag = 0;
	int idx0=0, idx1=0, type=0;
	char* tmp0 = data;
	while(1){

		if(tmp0[idx0]==0xAC && flag == 0){
			idx0++;
			flag = 1;
			continue;
		}if((tmp0[idx0]==0xD0||tmp0[idx0]==0xD1)&& flag == 0){
			type=tmp0[idx0]&0x01;
			idx0++;
			flag = 2;
			continue;
		}


		//Println(__func__,"0x%X  %d/%d(%d)", tmp0[idx0], idx0, idx1, len);
		if(flag==1) {buf[idx1++]=SsVfdRuFontTable(tmp0[idx0]);}
		else {buf[idx1++]=AscToCode866_T2(tmp0[idx0],type);}


		flag = 0;
		if(len==idx0++) break;
	}

	while((idx1)<=41){
		buf[idx1++]=0xFF;
	}
	buf[41]='\0';
}

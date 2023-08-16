#ifndef HEADER_FLASH_H_
#define HEADER_FLASH_H_

#include  "../header/bsp_msp430x4xx.h"

extern void ScriptData(void);
extern void SetPtrData(void);
extern void write_Seg(void);
extern unsigned int k;
typedef struct Files{
    short num_of_files;
    char file_name[11];
    int* file_ptr[3];
    int file_size[3];

}Files;
extern Files file;
#endif /* HEADER_FLASH_H_ */

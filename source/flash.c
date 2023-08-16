#include  "../header/flash.h"    // private library - flash layer
#include  "../header/halGPIO.h"    // private library - halGPIO layer
#include  "string.h"

//-----------------------------------------------------------------------------
//           FLASH driver
//-----------------------------------------------------------------------------

char *Flash_ptr_write ;                          // Flash pointer
unsigned int k;

Files file;


void ScriptData(void)
{
    file.file_size[file.num_of_files - 1] = strlen(file_content) - 1;

}

void write_Seg(void)
{
    Flash_ptr_write = file.file_ptr[file.num_of_files - 1];      // Initialize Flash pointer
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit segment erase
    FCTL3 = FWKEY;                            // Clear Lock bit
   *Flash_ptr_write = 0;                   // Dummy write to erase Flash segment

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation, Byte/Word

    for (k = 0; k < file.file_size[file.num_of_files - 1]; k++)
    {
        if (file_content[k] == 0x0A || file_content[k] == 0x0D ){
            continue;
        }
        *Flash_ptr_write++ = file_content[k];            // Write value to flash
    }
    memset(stringFromPC,0,strlen(stringFromPC)); //clear file_content
    FCTL1 = FWKEY;                               // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                        // Set LOCK bit
}


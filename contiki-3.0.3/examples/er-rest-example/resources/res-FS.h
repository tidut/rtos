#ifndef _FS_
#define _FS_


void ReorderPopvalues(double *Pop_value);

int Parse_Filterdata(unsigned char *FS_buffer,unsigned char *Remap_vars,unsigned char *Pop_Symbols,double *Pop_value);

unsigned char Validate_Exp(unsigned char SelOption);
unsigned char DoFiltering(unsigned char Filter_sel);
unsigned char Applyfilter(unsigned  char *string,double filt1,double filt2);

#endif

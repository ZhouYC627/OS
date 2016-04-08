#define V_ADDR 0xB8000
#define PROGRAIN 8
void print_c(char c, int x, int y){
	int *p = (void *)V_ADDR + (80*y+x)*2;
	*p = 0x0f00 | c;	
}

void print_col(char *s, int line)
{
	int i;
	for (i=0; s[i]!=0; i++){
		int *p = (void *)V_ADDR + (80*line+i)*2;
		*p = 0x0000 | s[i] | ((i%14+2)<<8);
	}
}
void print_s(char *s, int line)
{
	int i;
	for (i=0; s[i]!=0; i++){
		print_c(s[i], i, line);
	}
}
void print_d(double d, int line){
	int count = 0;
	int n = (int)d;
	while (n>=1){
		int c = (int)n % 10;
		n /= 10;
		print_c(c + '0', count++, line);
	}
	print_c('.', count++, line);
	int i=0;
	for (; i< PROGRAIN; i++){
		d *= 10;
		print_c(((int)d) % 10 + '0', count++, line);
	}

}

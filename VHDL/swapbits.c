#include <stdio.h>

int main(int argc, char ** argv)
{
	FILE * fin = fopen(argv[1],"r");
	FILE * fout = fopen(argv[2],"w");

	unsigned char ins[4];
	unsigned char out[4];

	while (fread(&ins[0],1,4,fin)==4)
	{
		for (int i=0;i!=4;++i)
		{
			unsigned char in = ins[i];
			unsigned char res = 0;
			if (in&1)
				res=res|128;
			if (in&2)
				res=res|64;
			if (in&4)
				res=res|32;
			if (in&8)
				res=res|16;
			if (in&16)
				res=res|8;
			if (in&32)
				res=res|4;
			if (in&64)
				res=res|2;
			if (in&128)
				res=res|1;

			out[3-i] = res;
		}

		fwrite(&out[0],1,4,fout);
	}

	fclose(fin);
	fclose(fout);

	return 0;
}


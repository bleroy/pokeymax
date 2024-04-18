#include "stdio.h"
#include "stdlib.h"
#include <math.h>

unsigned short data[] = 
{
#include "SID/investigation/sidsimplify.out"
};

int main(void)
{
	unsigned short * buffer = (unsigned short *) malloc(16384*2*2); //16-bit, 2 copies
	int i=0;
	for (i=0;i!=32768;++i)
	{
		//buffer[i] = i;
		//buffer[16384+i] = i;
		buffer[i] = data[i];
	}

	// 0x100(0x400 8-bit) sid tables --TODO!!
	// to store: 
	// i) 6581 channel mixing:
	// 	wire [7:0] wave__st[4096]; (sawtooth + triangle)
	// 	wire [7:0] wave_p_t[2048]; (pulse + triangle - symmetric)
	// 	wire [7:0] wave_ps_[4096]; (pulse + sawtooth)
	// 	wire [7:0] wave_pst[4096]; (pulse + sawtooth + triangle)
	// 	        _st_out <= wave__st[sawtooth]; 
        //		p_t_out <= wave_p_t[triangle[11:1]];
        //		ps__out <= wave_ps_[sawtooth];
        //		pst_out <= wave_pst[sawtooth];
	//	              4'b0001: wave_out = triangle;
        //  		      4'b0010: wave_out = sawtooth;
        //  		      4'b0011: wave_out = {_st_out, 4'b0000};
        //  		      4'b0100: wave_out = pulse;
        //  		      4'b0101: wave_out = {p_t_out, 4'b0000} & pulse;
        //  		      4'b0110: wave_out = {ps__out, 4'b0000} & pulse;
        //  		      4'b0111: wave_out = {pst_out, 4'b0000} & pulse;
        //  		      4'b1000: wave_out = noise;
        //  		      default: wave_out = 0;
	// ii) 8580 channel mixing: just and?
	// iii) linear filter
	// iv) 6581 filter frequency table
	// v) 8580 filter frequency table
	// vi) 6581 volume non-linearity filter adjustment
	
//      CLKSPEED : IN integer; --In Hz 58333333
//      FMIN : IN integer;   --In Hz (30)
//      FMAX : IN integer   --In Hz (12500 on 8580)
//      process(CUTOFF_FREQUENCY)
//              constant f_min : real := 2.0*sin(MATH_PI*real(FMIN)/real(CLKSPEED));
//              constant f_max : real := 2.0*sin(MATH_PI*real(FMAX)/real(CLKSPEED));
//
//              variable f_offset : unsigned(17 downto 0); --0.21(000,18)
//              variable f_scale : unsigned(17 downto 0); --0.21(000,18)
//
//              variable F_MULT : UNSIGNED(35 DOWNTO 0);
//      begin
//              --f = 2*sin(pi*10000/inrate);
//              --CUTOFF_FREQUENCY : IN STD_LOGIC_VECTOR(10 downto 0);
//              --CLKSPEED : IN integer; --In Hz
//              --FMIN : IN integer;   --In Hz
//              --FMAX : IN integer;   --In Hz
//
//              f_offset := to_unsigned(integer(f_min*2.0**21.0),18);
//              f_scale  := to_unsigned(integer(2.0**21.0*((f_max-f_min)/2.0**11.0)),18);
//
//              -- TODO: Could use a real curve captured from a chip? Lets start with it correctly then...
//              f_mult := f_scale * resize(unsigned(CUTOFF_FREQUENCY),18);
//              f_next <= f_mult(17 downto 0) + f_offset;
//      end process;

	FILE * x =fopen("sidwavedata.bin","w");
		fwrite(&buffer[0],1,65536,x);
	fclose(x);

	return 0;
}


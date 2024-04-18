#!/usr/bin/perl -w
use strict;

my $wanted_variant = shift @ARGV;

my $name="eclaireXL";

#Added like this to the generated qsf
#set_parameter -name TV 1

my $version = "127";

my %variants = 
(
#		enable_auto_stereo : integer := 0;   -- 1=auto detect a4 => not toggling => mono
#
#		fancy_switch_bit : integer := 10; -- 0=ext is low => mono
#		gtia_audio_bit : integer := 0;    -- 0=no gtia on l/r,1=gtia mixed on l/r
#		a4_bit : integer := 0;
#		a5_bit : integer := 0;
#		a6_bit : integer := 0;
#		a7_bit : integer := 0;
#
#		ext_bits : integer := 3; 
#
#		enable_config : integer := 1;
#		enable_sid : integer := 0;
#		enable_ym : integer := 0;
#		enable_covox : integer := 0;
#		enable_sample : integer := 0;
#
#   		version : STRING  := "DEVELOPR" -- 8 char string atascii



	"v1_10M02_stereo_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"fpga" => "10M02SCU169C8G",
		"board" => "v1",
		"ext_bits"=> 1,
		"cs0_bit" => 0, #force low
		"version" => $version . "M02SA"
	},
	"10M02_mono_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 1,
		"fpga" => "10M02SCU169C8G",
		"enable_auto_stereo" => 1,
		"gtia_audio_bit" => 3,
		"a4_bit" => 1, #to access config!
		"version" => $version . "M02MO"
	},
	"10M02_stereo_xel_auto_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"cs1_bit" => 20,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SX"
	},
	"10M02_stereo_covox_no_right_detect_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 2,
		"enable_auto_stereo" => 0,
		"enable_covox" => 1,
		"detect_right_on_by_default" => 0,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M02SCU169C8G",
		"version" =>  $version."M02SC"
	},
	"10M02_stereo_auto_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SA"
	},
	"10M02_stereo_u1mb_auto_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"fancy_switch_bit" => 2,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SU"
	},
	"10M02_stereo_covox_auto_linear" =>
	{
		"saturate_on_by_default" => 0,
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_covox" => 1,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M02SCU169C8G",
		"version" =>  $version."M02SC"
	},
	"10M02_mono" =>
	{
		"pokeys" => 1,
		"fpga" => "10M02SCU169C8G",
		"enable_auto_stereo" => 1,
		"gtia_audio_bit" => 3,
		"a4_bit" => 1, #to access config!
		"version" => $version . "M02MO"
	},
	"10M02_stereo_xel_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"cs1_bit" => 20,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SX"
	},
	"10M02_stereo_covox_no_right_detect" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 0,
		"enable_covox" => 1,
		"detect_right_on_by_default" => 0,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M02SCU169C8G",
		"version" =>  $version."M02SC"
	},
	"10M02_stereo_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SA"
	},
	"10M02_stereo_u1mb_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"fancy_switch_bit" => 2,
		"gtia_audio_bit" => 3,
		"fpga" => "10M02SCU169C8G",
		"version" => $version . "M02SU"
	},
	"10M02_stereo_covox_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_covox" => 1,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M02SCU169C8G",
		"version" =>  $version."M02SC"
	},
	"10M04_quad_covox_xel_auto" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a7_bit" => 3, 
		"cs1_bit" => 20,
		"enable_covox" => 1,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04QX"
	},
	"10M04_mono" =>
	{
		"pokeys" => 1,
		"fpga" => "10M04SCU169C8G",
		"enable_auto_stereo" => 1,
		"gtia_audio_bit" => 3,
		"enable_flash" => 1,
		"a4_bit" => 1, #to access config!
		"version" => $version . "M04MO"
	},
	"10M04_stereo_xel_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"cs1_bit" => 20,
		"gtia_audio_bit" => 3,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04SX"
	},
	"10M04_stereo_covox_no_right_detect" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 0,
		"enable_covox" => 1,
		"enable_flash" => 1,
		"detect_right_on_by_default" => 0,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M04SCU169C8G",
		"version" =>  $version."M04SC"
	},
	"10M04_stereo_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"gtia_audio_bit" => 3,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04SA"
	},
	"10M04_stereo_u1mb_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"fancy_switch_bit" => 2,
		"gtia_audio_bit" => 3,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04SU"
	},
	"10M04_stereo_covox_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_covox" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M04SCU169C8G",
		"version" =>  $version."M04SC"
	},
	"10M04_quad_auto" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04QA"
	},
	"10M04_quad_covox_auto" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a7_bit" => 3, 
		"enable_covox" => 1,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04QC"
	},
	"10M08_stereo_covox_sample_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a7_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M08SCU169C8G",
		"version" =>  $version."M08SC"
	},
	"sid_10M08_sid_mono" =>
	{
		"pokeys" => 1,
		"enable_sid" => 1,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"ext_bits"=> 4,
		"bus" => "c64",
		"a4_bit" => 4,
		"a5_bit" => 0,  #force low for now (will be stereo)
		"a6_bit" => 20, #force high
		"a7_bit" => 0, #force low
		"board" => "sid",
		"cs1_bit" => 20, #force high
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08SI"
	},
	"sid_10M08_sid_stereo" =>
	{
		"pokeys" => 1,
		"enable_sid" => 1,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"ext_bits"=> 4,
		"bus" => "c64",
		"a4_bit" => 4,
		"a5_bit" => 1,  #STEREO
		"a6_bit" => 20, #force high
		"a7_bit" => 0, #force low
		"board" => "sid",
		"cs1_bit" => 20, #force high
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08SS"
	},
	"sidmax_10M08_full" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"ext_bits"=> 4,
		"bus" => "c64",
		"a5_bit" => 1,  #STEREO
		"a6_bit" => 2,
		"a7_bit" => 3,
		"irq_bit" => 4,
		#"a6_bit" => 20, #force high
		#"a7_bit" => 0, #force low
		"type" => "sidmax",
		"board" => "v1",
		#"cs1_bit" => 20, #force high
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "S08HK"
	},
	"10M08_stereo_u1mb_auto" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"fancy_switch_bit" => 2,
		"gtia_audio_bit" => 3,
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08SU"
	},
	"10M08_quad_auto" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"gtia_audio_bit" => 3, 
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08QA"
	},
	"10M08_quad_covox_sample_auto" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a7_bit" => 3, 
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08QC"
	},
	"10M08_quad_sid" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08QS"
	},
	"10M08_quad_psg_covox_sample" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a7_bit" => 3,
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08QP"
	},
#	"10M08_basic" =>
#	{
#		"enable_audout2" => 0,
#		"pokeys" => 2,
#		"enable_auto_stereo" => 1,
#		"enable_sid" => 0,
#		"enable_psg" => 0,
#		"enable_covox" => 0,
#		"enable_sample" => 0,
#		"enable_flash" => 1,
#		"a4_bit" => 1,
#		"a5_bit" => 2,
#		"a6_bit" => 3,
#		"a7_bit" => 19,  #use CS1
#		"cs1_bit" => 20, #force high
#		"fpga" => "10M08SCU169C8G",
#		"version" => $version . "M08HK",
#		"optimisearea" => 1
#	},
	"10M08_fullv2" =>
	{
		"enable_audout2" => 0,
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 1,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"a7_bit" => 19,  #use CS1
		"cs1_bit" => 20, #force high
		"fpga" => "10M08SCU169C8G",
		"version" => $version . "M08HK",
		"optimisearea" => 1
	},
	"10M04_quad_auto_v3" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 0,
		"enable_psg" => 0,
		"enable_covox" => 0,
		"enable_sample" => 0,
		"enable_flash" => 1,
		"enable_spdif" => 1,
		"enable_ps2" => 1,
		"board" => "v3",
		"pll_v2" => 0,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"gtia_audio_bit" => 5, 
		"spdif_bit" => 6,
		"ps2clk_bit" => 7,
		"ps2dat_bit" => 8,
		"ext_bits"=> 11,
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04QA"
	},
	"10M04_stereo_psg_covox_auto_v3" =>
	{
		"pokeys" => 2,
		"enable_auto_stereo" => 1,
		"enable_sid" => 0,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 0,
		"enable_flash" => 1,
		"enable_spdif" => 1,
		"enable_ps2" => 1,
		"board" => "v3",
		"pll_v2" => 0,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"a7_bit" => 4,
		"gtia_audio_bit" => 5, 
		"spdif_bit" => 6,
		"ps2clk_bit" => 7,
		"ps2dat_bit" => 8,
		#"fancy_switch_bit" => 6,
		#"a7_bit" => 19,  #use CS1
		"ext_bits"=> 11,
		#"cs1_bit" => 20, #force high
		"fpga" => "10M04SCU169C8G",
		"version" => $version . "M04SP"
	},
	"10M16_mono" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 0,
		"enable_psg" => 0,
		"enable_covox" => 0,
		"enable_sample" => 0,
		"enable_flash" => 1,
		"enable_spdif" => 0,
		"enable_ps2" => 0,
		"flash_addr_bits" => 17,
		"board" => "v3",
		"pll_v2" => 0,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"a7_bit" => 4,
		"gtia_audio_bit" => 5, 
		"spdif_bit" => 6,
		"ps2clk_bit" => 7,
		"ps2dat_bit" => 8,
		#"fancy_switch_bit" => 6,
		#"a7_bit" => 19,  #use CS1
		"ext_bits"=> 11,
		#"cs1_bit" => 20, #force high
		"fpga" => "10M16SCU169C8G",
		"sid_wave_base" => 79872, #"to_integer(unsigned(x\"13800\"))",
		"version" => $version . "M16MO"
	},
	"10M16_fullv3" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 1,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"enable_spdif" => 1,
		"enable_ps2" => 1,
		"flash_addr_bits" => 17,
		"board" => "v3",
		"pll_v2" => 0,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"a7_bit" => 4,
		"gtia_audio_bit" => 5, 
		"spdif_bit" => 6,
		"ps2clk_bit" => 7,
		"ps2dat_bit" => 8,
		#"fancy_switch_bit" => 6,
		#"a7_bit" => 19,  #use CS1
		"ext_bits"=> 11,
		#"cs1_bit" => 20, #force high
		"fpga" => "10M16SCU169C8G",
		"sid_wave_base" => 79872, #"to_integer(unsigned(x\"13800\"))",
		"version" => $version . "M16HK"
	},
	"10M16_fullv3_xel" =>
	{
		"pokeys" => 4,
		"enable_auto_stereo" => 1,
		"enable_sid" => 1,
		"enable_psg" => 1,
		"enable_covox" => 1,
		"enable_sample" => 1,
		"enable_flash" => 1,
		"enable_spdif" => 1,
		"enable_ps2" => 1,
		"flash_addr_bits" => 17,
		"board" => "v3",
		"pll_v2" => 0,
		"a4_bit" => 1,
		"a5_bit" => 2,
		"a6_bit" => 3,
		"a7_bit" => 4,
		"gtia_audio_bit" => 5, 
		"spdif_bit" => 6,
		"ps2clk_bit" => 7,
		"ps2dat_bit" => 8,
		#"fancy_switch_bit" => 6,
		#"a7_bit" => 19,  #use CS1
		"ext_bits"=> 11,
		"cs1_bit" => 20,
		#"cs1_bit" => 20, #force high
		"fpga" => "10M16SCU169C8G",
		"sid_wave_base" => 79872, #"to_integer(unsigned(x\"13800\"))",
		"version" => $version . "M16HX"
	},
	"10M08_mono" =>
	{
		"pokeys" => 1,
		"fpga" => "10M08SCU169C8G",
		"enable_auto_stereo" => 1,
		"gtia_audio_bit" => 3,
		"cs1_bit" => 20, #force high
		"a4_bit" => 1, #to access config!
		"version" => $version . "M08MO"
	},
#	"10M08_light" =>
#	{
#		"pokeys" => 2,
#		"enable_auto_stereo" => 1,
#		"enable_sid" => 0,
#		"enable_psg" => 0,
#		"enable_covox" => 1,
#		"enable_sample" => 1,
#		"enable_flash" => 1,
#		"a4_bit" => 1,
#		"a5_bit" => 2,
#		"a6_bit" => 3,
#		"a7_bit" => 19,  #use CS1
#		"cs1_bit" => 20, #force high
#		"fpga" => "10M08SCU169C8G",
#		"version" => $version . "M08LI"
#	},
#	"10M08_audiotest" =>
#	{
#		"pokeys" => 4,
#		"enable_auto_stereo" => 1,
#		"enable_sid" => 1,
#		"enable_psg" => 1,
#		"enable_covox" => 1,
#		"enable_sample" => 1,
#		"enable_flash" => 1,
#		"a4_bit" => 1,
#		"a5_bit" => 2,
#		"a6_bit" => 3,
#		"a7_bit" => 19,  #use CS1
#		"cs1_bit" => 20, #force high
#		"ext_clk_enable" => 1,
#		"fpga" => "10M08SCU169C8G",
#		"version" => $version . "M08HK"
#	},
#	"10M08_sample" =>
#	{
#		"pokeys" => 4,
#		"enable_auto_stereo" => 1,
#		"enable_sid" => 0,
#		"enable_psg" => 0,
#		"enable_covox" => 1,
#		"enable_sample" => 1,
#		"enable_flash" => 1,
#		"a4_bit" => 1,
#		"a5_bit" => 2,
#		"a6_bit" => 3,
#		"a7_bit" => 19,  #use CS1
#		"cs1_bit" => 20, #force high
#		"fpga" => "10M08SCU169C8G",
#		"version" => $version . "M08HK"
#	}
#	"10M08_full" => 
#	{
#		"board" => 3,
#		"ext_bits"=> 11,
#		"pokeys" => 4,
#		"enable_auto_stereo" => 1,
#		"fancy_switch_bit" => 1,
#		"gtia_audio_bit" => 2,
#		"a4_bit" => 3,
#		"a5_bit" => 4,
#		"a6_bit" => 5,
#		"a7_bit" => 6,
#		"enable_sid" => 1,
#		"enable_psg" => 1,
#		"enable_covox" => 1,
#		"enable_sample" => 1,
#		"enable_flash" => 1,
#		"fpga" => "10M08SCU169C8G"
#	}
);

#if (not defined $wanted_variant or (not exists $variants{$wanted_variant} and $wanted_variant ne "ALL"))
#{
#	die "Provide variant of ALL or ".join ",",sort keys %variants;
#}

foreach my $variant (sort keys %variants)
{
	#next if ($wanted_variant ne $variant and $wanted_variant ne "ALL");
	next unless ($variant =~ /$wanted_variant/);
	print "Building $variant of $name\n";

	my $versioncode = $variants{$variant}->{"version"};
	if (not defined $versioncode)
	{
		$versioncode = "DEVELOPR";
	}
	my $fpga = $variants{$variant}->{"fpga"};
	my $flashver = $fpga;
	$flashver =~ s/..M(..).*/$1/;
	if (not exists $variants{$variant}->{"type"})
	{
		$variants{$variant}->{"type"} = "pokeymax";
	}
	my $type = $variants{$variant}->{"type"};
	my $board = $variants{$variant}->{"board"};
	my $bus = $variants{$variant}->{"bus"};
	my $flash = $variants{$variant}->{"enable_flash"};
	my $noflash = "";
	if (not defined $flash or $flash eq "0") {$noflash = "_noflash"};

	my $sid = $variants{$variant}->{"enable_sid"};
	if (not defined $sid) {$sid = "0"};

        my $dir = "build_$variant";
	`rm -rf $dir`;
	mkdir $dir;
	`cp *.vhd* $dir`;
	`cp iox_glue.vhdl $dir/iox_glue.vhdl`;
	`cp pokeymax.vhd $dir/pokeymax.vhd`;

	`cp iox_glue$board.vhdl $dir/iox_glue.vhdl`;
	`cp $type$board.vhd $dir/$type.vhd`;
	`cp $type$board.qsf $dir/$type.qsf`;

	`cp slave_timing_6502$bus.vhd $dir/slave_timing_6502.vhd`;
	`cp swapbits $dir`;
	`cp $type$board$noflash.sdc $dir/$type.sdc`;
	`cp $type*.qpf $dir`;
	`cp -r int_osc* $dir`;
	`cp -r pll* $dir`;
	`cp -r lvds* $dir`;
	`cp -r flash_$flashver/flash* $dir`;
	`cp -r PSG $dir`;
	`cp -r SID $dir`;
	`cp -r pokey $dir`;
	`cp -r sample $dir`;
	`cp -r covox $dir`;
	`cp -r *.bin $dir`;

	chdir $dir;

	
	`echo set_global_assignment -name DEVICE $fpga >> $type.qsf`;

	foreach my $key (sort keys %{$variants{$variant}})
	{
		my $val = $variants{$variant}->{$key};
		`echo 'set_parameter -name $key $val' >> $type.qsf`;
	}
	if (exists $variants{$variant}->{"optimisearea"})
	{
		`echo 'set_global_assignment -name CYCLONEII_OPTIMIZATION_TECHNIQUE AREA' >>$type.qsf`;
	}

	`quartus_sh --flow compile $type > build.log 2> build.err`;
	`quartus_cpf --convert ../convert_secure_${type}_$sid.cof`;
	`../modifypof_$flashver $type ./output_files/$type.pof`;
	`quartus_cpf -c -q 10MHz -g 3.3 -n p output_files/$type.pof output_files/$type.svf`;
	`../makeflash_$flashver $type ./output_files/$type.pof $versioncode output_files/core.bin`;
	`touch UFM1 UFM0 CFM1 CFM0`;
	`../openocd_flash/extractbinfromsvf.pl output_files/$type.svf`;
	`cat UFM1.bin UFM0.bin > UFMboth.bin`;
	`cat CFM1.bin CFM0.bin > CFMboth.bin`;

	chdir "..";
}


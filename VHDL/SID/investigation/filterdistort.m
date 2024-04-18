distort = nan(2048,256);

distbase = nan(256,1);

TYPE3_W0S_RESOLUTION = 1 / 5e3;
OSC_TO_FC = 1 / 512;

%FilterTrurl6581R4AR_3789
Type3BaseResistance=1.08e6;
Type3Offset=1.8e6;
Type3Steepness=1.006;
Type3MinimumFETResistance=1.0e4;

clockFrequency= 58333333.0; %?

steepness = Type3Steepness;
minimumfetresistance = Type3MinimumFETResistance;
baseresistance = Type3BaseResistance;

for i=0:255
  dist = i;
  if i>0
    (i + 0.5) / TYPE3_W0S_RESOLUTION;
  end
  distbase(i+1) = 1.0 / (steepness ^ (dist * OSC_TO_FC));
end
 
SIDCAPS_6581 = 470e-12;
_1_div_caps_freq = (1.0 / (SIDCAPS_6581 * clockFrequency));

fcBase = 2.0*sin(pi.*(curves(2).interp/2^21)/clockFrequency);

fcBase = -1./(fcBase*clockFrequency);
%This fcBase is in terms of f for the digital... need it in fc terms!

for j=0:2047
  fcBaseValue = fcBase(j+1);
  for i=0:255
    fetresistance = fcBaseValue * distbase(i+1);
    dynamic_resistance = minimumfetresistance + fetresistance;

    % 2 parallel resistors 
    _1_div_resistance = (baseresistance + dynamic_resistance) / (baseresistance * dynamic_resistance);
    distort(j+1,i+1) = _1_div_caps_freq * _1_div_resistance;
  end
end


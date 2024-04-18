%graphics_toolkit("gnuplot");

curvenames = { ...
  'fc-curves/Trurl_Ext/8580R5_3691.txt' ...
...  %'fc-curves/Trurl_Ext/8580R5_1489.txt' ...
  'fc-curves/Trurl_Ext/6581R4AR_3789.txt' ...
...  %'fc-curves/Trurl_Ext/6581R3_4885.txt' ...
  'fc-curves/alankila/6581R4AR_3789.txt' ...
...  %'fc-curves/resid/r2.txt', ...
  'fc-curves/nata/r2.txt', ...
  'fc-curves/ZrX-oMs/6581R3_3985.txt', ...
  };
curves = struct();
for c=1:numel(curvenames)
  f = csvread(curvenames{c});
  curves(c).name = curvenames{c};
  curves(c).cutoff = real(f);
  curves(c).freq = imag(f);
  curves(c).cutoff(end+1) = 100000;
  curves(c).freq(end+1) = curves(c).freq(end);
end
extra = numel(curvenames)+1;
curves(extra).name = 'correct';
curves(extra).cutoff = 0:2047;
curves(extra).freq = 30 + (((12500-30)/2048) .* curves(extra).cutoff);

figure;

CLKSPEED = 58333333.0;
for c=1:numel(curves)
  res = nan(0,2047);  
  %pf = polyfit(curves(c).cutoff,curves(c).freq,10);
  %for i=0:2047
  %  res(i+1) = polyval(pf,i);
  %end
  res = interp1(curves(c).cutoff,curves(c).freq,0:2047);
  curves(c).interp = res;
  curves(c).f =  freq = 2.0*sin(pi*res/CLKSPEED);
  
  plot(0:2047,res);
  hold on;
  %plot(curves(c).cutoff,curves(c).freq);
  %legend({'fit','orig'});  
end
legend(curves.name)


csvwrite('8580.csv',curves(1).interp')
csvwrite('6581.csv',curves(2).interp')

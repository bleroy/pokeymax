sigmoidreal = @(x) 1./(1.+e.^-x);

rescale =  0.133923834850281;
reoffset = 0.00188312488556502;
low = -8;
high = 8;
vals = low:(high-low)/ceil(32*300/256):high; % I lookup in a range 1:300... shift is 0-255...
t = sigmoidreal(vals)*rescale + reoffset;

clockorig = 985248.400000;
clocknew = 58333333.0;

%origscale = 2.0*sin(M_PI*freqval/clockorig);
freqval = asin(t/2)*clockorig/pi;
tadj = 2*sin(pi*freqval/clocknew);

plot(t);
hold all;
plot(tadj);
for i=1:numel(t)
  fprintf('"%s",',dec2bin(floor((2^21-1)*tadj(i)),18));
endfor
fprintf('      ');

for i=1:numel(tadj)
  fprintf('%f,',floor((2^21-1)*tadj(i)));
endfor



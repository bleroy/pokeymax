close all;

figure;
hold all;
%stepsize = (16/32768);
%x=-8:stepsize:8;

TYPE3_W0S_RESOLUTION = 1 / 5e3;

%x = x.*10.2;
sigmoidreal = @(x) 1./(1.+e.^-x);
sigmoidapprox1 = @(x) 1./(1.+2.^(-1.5.*x)); %Approx, but what does it give us?
sigmoidapprox2 = @sig_sigmoid; %Approx, but what does it give us?
func = sigmoidreal;

%See minf/maxf;
rescale =  0.133923834850281;
reoffset = 0.00188312488556502;
scalefunc = @(x) (x.*rescale .+ reoffset);
func = @(x) scalefunc(func(x));
%func = sigmoidreal;

test = csvread('/home/markw/fpga/svn/repo/trunk/atari_800xl/atari_chips/pokeyv2/SID/investigation/output_6.csv');
s1 = test(test(:,1)==1,:);

locoffset = -9.74126000000000;
locscale =  0.00664125000000000;
%besterr =    2.79586225092504e-08

loc = locoffset+(s1(:,5)*locscale);

vals2 = nan(512,1024);

%So, lookup in loc then add i*delta to get the func pos...

%loc can be stored in flash rom
%we need to compute param for sigmoid
%sigmoid(vol*delta + loc)

stepsize = (16/1024);
funclookup = func(-8:stepsize:(8-stepsize));
%numel(unique(funclookup))

delta =  0.0652017000000000; %median - real sigmoid
%shiftlookup = nan(1,512);

%each step is TYPE3_W0S_RESOLUTION
%which is kind of annoying, lets make it 1/4096 instead 
%0-1275000

%(255/(1/5000))*(1/4096);
FAST_W0S_RESOLUTION = 1/4096;
adjres = (TYPE3_W0S_RESOLUTION/FAST_W0S_RESOLUTION);

%for i=1:512
%  shiftlookup(i) = adjres*delta*(i-1);
%end

locstep = loc*(1/stepsize);
%shiftlookup = shiftlookup*(1/stepsize);
%shiftlookup = shiftlookup + -1 +numel(funclookup)/2;
%debug> polyfit(1:numel(shiftlookup),shiftlookup,1)
%ans =
%
%     3.4184   507.5816

locstepc = locstep+507.5816;

funclookup2 = interp1(1:numel(funclookup),funclookup,1:(3.4184):numel(funclookup));
%N = numel(-8:stepsize:(8-stepsize));
%funclookup2 = func(1:(3.4184):N);


figure;
%t = 1:(2^18/65):(2^18-1);
%sigmoidreal = @(x) 1./(1.+e.^-x);
%t = sigmoidreal(-4+8*t/2^18);
%t=funclookup2(1) + t*(funclookup2(300)-funclookup2(1))

locstepc = 3.4184+(locstepc/3.4184);
for i=1:256
  %shift = shiftlookup(i);                  %lookup shift   (when vol changed enough) - straigh line...
  %shift  = 3.4184*i+507.5816;
  %shiftnc  = 3.4184*i;
  shiftnc  = i-1;
                                           %lookup loc     (when f reg changed)
  %idx = locstep + shift;                   %adder
  idx = locstepc + shiftnc;                   %adder
  idx = min(max(1,idx),numel(funclookup2)); %cap
  vals2(i,:) = funclookup2(round(idx));     %lookup sigmoid (when sum changed)
  %vals2(i,:) = func(round(idx));     %lookup sigmoid (when sum changed)
  
  %vals2(i,:) = func(loc.+(delta*(i-1)));
  
  %should work, but ... fast enough? 10 + 10 + 10 -> 30 cycles -> say 1.8MHz to update f? Probably ok...
end

%For vhdl... also in slope.m
figure
sigmoidvhdl=funclookup2(round(1:299/64:300));
plot(sigmoidvhdl)

rescale =  0.133923834850281;
reoffset = 0.00188312488556502;
sv2 = sigmoidreal(-8:16/64:8)*rescale + reoffset;
hold all
plot(sv2);

for i=1:numel(sv2)
  fprintf('"%s",',dec2bin(floor((2^18-1)*sv2(i)),18));
endfor
%For rom compute code


%scale
minf =  0.0018910;
maxf =  0.13305;
v2min = min(vals2(1,:));
v2max = max(vals2(1,:));

%vals2 = vals2.-v2min;
%vals2 = vals2./(v2max-v2min);
%vals2 = vals2.*(maxf-minf);
%vals2 = vals2.+minf; 

%rescale = (maxf-minf)./(v2max-v2min);
%vals2 = vals2.*rescale;
%vals2 = vals2.+minf.-(v2min.*rescale);

%We can also store these magic numbers... flash? or can we make a sigmoid with this scale from logic elements
rescale =  0.133923834850281;
reoffset = 0.00188312488556502;

%vals2 = vals2.*rescale .+ reoffset;

range = 0:1275000/20:1275000;
range256 = 0:1275000/255:1275000;

v1lookup = @(i) 1.+round(i.*TYPE3_W0S_RESOLUTION);
v2lookup = @(i) 1.+floor(i.*FAST_W0S_RESOLUTION);

%figure
hold all;
for i=range
  plot(vals2(v2lookup(i),:));
end

vals = load('valsorig.mat','vals');
vals = vals.vals;
for i=range
  plot(vals(v1lookup(i),:));
end

figure;
plot(vals(1,:));
hold all;
plot(vals2(1,:));

figure;
plot((vals2(1,:).-vals(1,:))./maxf);

vals_interp = nan(256,1024);
vals_interp2 = nan(256,1024);
for i = 1:numel(range256);
  vals_interp(i,:) = vals(v1lookup(range256(i)),:);
  vals_interp2(i,:) = vals2(v2lookup(range256(i)),:);
end

pause(2);
figure;
mesh((vals_interp2.-vals_interp)./maxf);

pause(2);
figure;
hold all;
for i=range
  plot(vals(v1lookup(i),:));
end
figure;
hold all;
for i=range
  plot(vals2(v2lookup(i),:));
end

test = csvread('/home/markw/fpga/svn/repo/trunk/atari_800xl/atari_chips/pokeyv2/SID/investigation/output_6.csv');
s1 = test(test(:,1)==1,:);
s2 = test(test(:,1)==2,:);
s3 = test(test(:,1)==3,:);
vals = reshape(s3(:,4),256,1024);

close all;
figure
hold all;
for i=1:20:255
  plot(vals(i,:));
end
%figure
hold all;
for i=1:20:255
  plot(vals2(i,:));
end


a = interp1(0:255,vals(:,1),0:0.01:255);
b = interp1(0:255,vals(:,50),0:0.01:255);
c = interp1(0:255,vals(:,99),0:0.01:255);

funcpt1 = vals(:,1);
funcpt1 = interp1(0:255,funcpt1,0:0.01:255);
funcpt2 = vals(:,end);
funcpt2 = interp1(0:255,funcpt2,0:0.01:255);
idx = find(funcpt2>funcpt1(end),1,'first');
func = [funcpt1 funcpt2(idx:end)];
func(end:end+500000) = func(end);

close all;
figure;
plot(func(1:30000));
hold all;
x=-9.8:((9.8*2)/29950):9.8;
sigmoid = 1./(1.+e.^-x);
minf = min(func);
maxf = max(func);
sigmoid = sigmoid.*(maxf-minf);
sigmoid = sigmoid + minf;
plot(sigmoid);

func = sigmoid;

perstep = 100.5; %measured...

%x = 255-ceil((256/1024)*(1:1024));
%y = 1:1024;
%ind = sub2ind(size(vals),x,y);
%orig1 = vals(ind);
%loc1 = arrayfun(@(x) find(x<=func,1,'first'),orig1);

%orig2 = vals(1,:);
%loc2 = arrayfun(@(x) find(x<=func,1,'first'),orig2);

%loc = loc1.-loc2;
%loc = loc1.-(100.5.*x);

%loc = loc2;

%close all
%plot(loc)
%hold all;
%plot(s1(:,5).*10.2)

loc = 1+s1(:,5).*10.2;

vals2 = nan(size(vals));

for i=1:1024
  idx = round(loc+(perstep)*(i-1));
  idx = max(idx,1);
  idx = min(idx,numel(func));
  vals2(i,:) = func(idx);
end

figure;
plot(loc)
figure;
plot(func(1:30000))


figure;
plot(a);
hold all;
plot(b);
%b is shifted left
plot(shift(a,-1150));
plot(c);
plot(shift(a,-2300));
legend({'a','b','bsyth','c','csynth'});

%baseresistance=1.08e6
%minimumfetresistance=1.0e4
baseresistance=1147036.4394268463
minimumfetresistance=16125.154840564108

%baseresistance = 0;
minimumfetresistance = 10000;

close all;
figure;
hold all;
fcbase = s1(:,4); %store?
distortion = 0.2
figure;; %store?
fetresistance = fcbase.*distortion; %mult - ok
%final = (dynamicresistance.+baseresistance)./(dynamicresistance.*baseresistance); %add - ok,mult - ok and div - bad
final = 1./(minimumfetresistance.+fetresistance) + 1./baseresistance; %Simplification ... 1./dynamicresistance... + constant

caps = (1 / (470e-12 * 1e6));
final = final.*caps;


plot(final);

distortion = 0.4;
fetresistance = fcbase.*distortion;
dynamicresistance = minimumfetresistance.+fetresistance;
final = (dynamicresistance.+baseresistance)./(dynamicresistance.*baseresistance);

caps = (1 / (470e-12 * 1e6));
final = final.*caps;

plot(final);


close all;
figure;
x = 1:1000;
y = x;
y2 = x+50;
plot(x,y);
hold all;
plot(x,y2);

%i.e. we need to shift to the left, while keeping the shape!
%except its squeezed at the edges...

%So straight line (kinked by dac), shifted left and rolled round a cylinder!

close all;
%plot(0:2047,interp1(0:2:2047,1./test(:,2),0:2047))

Type3Offset=1.8e6
Type3Steepness=1.006
%Type3Offset=8e6
%Type3Steepness=1.0052


figure;
plot(0:2047,interp1(0:2:2047,Type3Offset./(Type3Steepness.^test(:,4)),0:2047))
figure;
plot(0:2047,interp1(0:2:2047,test(:,2),0:2047))
figure;

figure;
hold all;

ft = polyfit(0:2047,curves(3).f,10);
smoothed = polyval(ft,0:2047);

for i=1:20:255
  sh = i*10;
  idx = sh:2047+sh;
  idx(idx>2048)=2048;
  
  adj = curves(3).f - smoothed + smoothed(:,idx);
  
  plot(0:2047,adj);
end


figure;
plot(curves(2).f);
hold all;
ft = polyfit(0:2047,curves(2).f,10);
plot(polyval(ft,0:2047));




%fC = 1/2piRf1


Rf = 1./(test(:,2).*2.*pi);
F = 1./(2.*pi.*Rf);


%%%%%%%%%%%%%%%%%%%%
%q from resipfp - c++
resonanceFactor = 1;
Qval = 0:15;
Qcpp = 1.0./(2.0.^((4.0.-real(Qval))./8.0));
Q8580java = (0.707 .+ resonanceFactor .* Qval ./ 15);
Q6581java = (0.5 .+ resonanceFactor .* Qval ./ 18);

close all;
figure;
plot(Qcpp);
hold all;
plot(Q8580java);
plot(Q6581java);
legend({'qcpp','q8580j','q6581j'});

%%%%%%%%%%%%%%%%%%%%%%%




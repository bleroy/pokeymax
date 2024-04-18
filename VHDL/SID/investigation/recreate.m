close all;

figure;
%plot(func(1:30000));
hold all;
x=-9.8:((9.8*2)/29950):9.8;
sigmoid = 1./(1.+e.^-x);
%minf = min(func);
%maxf = max(func);
minf =  0.0018910;
maxf =  0.13581;
sigmoid = sigmoid.*(maxf-minf);
sigmoid = sigmoid + minf;
%plot(sigmoid);

func = sigmoid;

perstep = 100.5; %measured...

test = csvread('/home/markw/fpga/svn/repo/trunk/atari_800xl/atari_chips/pokeyv2/SID/investigation/output_6.csv');
s1 = test(test(:,1)==1,:);
loc = 1+s1(:,5).*10.2;

vals2 = nan(256,1024);

for i=1:1024
  idx = round(loc+(perstep)*(i-1));
  idx = max(idx,1);
  idx = min(idx,numel(func));
  vals2(i,:) = func(idx);
end

%figure
hold all;
for i=1:20:255
  plot(vals2(i,:));
end


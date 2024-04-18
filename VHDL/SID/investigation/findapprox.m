
%x=-9.8:((9.8*2)/(29950/10.2)):9.8;
%stepsize = (16/32768);
%stepsize = (16/64);
%x=-8:stepsize:8;

%stepsizeorig = 0.0066751;

sigmoid = @(x) 1./(1.+e.^-x);

vals = load('valsorig.mat','vals');
vals = vals.vals;

test = csvread('/home/markw/fpga/svn/repo/trunk/atari_800xl/atari_chips/pokeyv2/SID/investigation/output_6.csv');
s1 = test(test(:,1)==1,:);


x = -9.75:0.00001:-9.73;
y = 0.00663:0.00000001:0.00665;
besterr = 1.13;
bestx = 0;
besty = 0;

for xi=1:numel(x)
  xval = x(xi);
  for yi=1:numel(y)
    yval = y(yi);
    loc = xval+(s1(:,5)*yval);
    
    %loc = c+s1(:,5)*offset;

    vals2 = nan(256,1024);

    %So, lookup in loc then add i*perstep to get the func pos...

    for i=1:1
      vals2(i,:) = sigmoid(loc);
    end

    minf =  0.0018910;
    maxf =  0.13305;
    v2min = min(vals2(1,:));
    v2max = max(vals2(1,:));

    vals2 = vals2.-v2min;
    vals2 = vals2./(v2max-v2min);
    vals2 = vals2.*(maxf-minf);
    vals2 = vals2.+minf;

    err = sum(abs((vals2(1,:).-vals(1,:))./maxf).^2); 
    if err<besterr
      bestx = xval;
      besty = yval;
      besterr = err;
      bestx
      besty    
      besterr
    end
  end
end


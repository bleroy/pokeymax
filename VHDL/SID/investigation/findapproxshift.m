
sigmoid = @(x) 1./(1.+e.^-x);

vals = load('valsorig.mat','vals');
vals = vals.vals;

test = csvread('/home/markw/fpga/svn/repo/trunk/atari_800xl/atari_chips/pokeyv2/SID/investigation/output_6.csv');
s1 = test(test(:,1)==1,:);


%offset = -3546.8
%scale =  13.590
%bestx = -3547.2;
%besty =  13.590;
offset = -9.74126000000000
scale =  0.00664125000000000
%besterr =    2.79586225092504e-08

loc = offset+(s1(:,5)*scale);

besterr = inf;
besterrplot = 0.1;
bestps = 0;
vals2 = nan(256,1024);

%bestps =  117
%besterr =  0.88494

for ps = 0.0651:0.0000001:0.0654

    for i=1:256
      idx = loc+(ps*(i-1));
      vals2(i,:) = sigmoid(idx);
    end

    minf =  0.0018910;
    maxf =  0.13305;
    v2min = min(vals2(1,:));
    v2max = max(vals2(1,:));

    vals2 = vals2.-v2min;
    vals2 = vals2./(v2max-v2min);
    vals2 = vals2.*(maxf-minf);
    vals2 = vals2.+minf;

    err = sum(sum(abs((vals2(1:255,50:950).-vals(1:255,50:950))./maxf).^2)); 
    if err<besterr
      bestps = ps;
      besterr = err;
      bestps
      besterr
    
      if err<besterrplot
        figure;
        hold all
        plot(vals(1,:));
        plot(vals2(1,:));      
        plot(vals(20,:));
        plot(vals2(20,:));
        plot(vals2(20,400:500).-vals(20,400:500));      
        besterrplot = besterr*0.9;
      end
    end
end


 

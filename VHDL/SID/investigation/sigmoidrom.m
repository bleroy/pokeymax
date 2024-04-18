function res = sigmoidrom(x,pre,post,out)
  
prer = 2^pre;
x = min(x,(prer/2)-1);
x = max(x,-prer/2);

postr = 2^post;
outr = 2^out;
x = floor(x*(postr))/postr;

sigmoidreal = @(x) 1./(1.+e.^-x);
y = sigmoidreal(x);
y = floor(y*outr)/outr;

res = y;

end
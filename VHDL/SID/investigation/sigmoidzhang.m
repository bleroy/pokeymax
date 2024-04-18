function res = sigmoidzhang(x)
%Another option - SID-sigmoid...
%Ref:Efficient digital implementation of the sigmoid
%function for reprogrammable logic
%M.T. Tommiska
%referenced from Zhang, Vassiliadis and Delgago's paper

x = min(x,4);
x = max(x,-4);

sgn = sign(x);
x = x.*sgn;

y = (1/2) .* ((1.-abs(0.25.*x)).^2);

res = y;
res(sgn>0) = 1-res(sgn>0);

end
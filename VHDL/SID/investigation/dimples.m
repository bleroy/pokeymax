x = 0:2047;
y = x*0.14/2047;

adjust = ones(1,2048);
adjust(1:1024) = adjust(1:1024)*1.1;
adjust(1:512) = adjust(1:512)*1.1;

y1 = y.*adjust;
y2 = y;
shift = 100;
y2(1:(2048-shift)) = y2(shift:2047);
y2((2048-shift):2047) = y2((2048-shift));
y2 = y2.*adjust;
close all;
plot(x,y1);
hold all;
plot(x,y2);

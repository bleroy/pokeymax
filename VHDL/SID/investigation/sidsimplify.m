waves = sidwavereader;

i6581 = 21;
i8580 = 22;

iTri = 1 +1; 
iSaw = 2 +1; 
iPulse = 4 +1;

i_ST = iTri-1+iSaw-1+1;
iPST = iTri-1+iSaw-1+iPulse-1+1;
iP_T = iTri-1+iPulse-1+1;
iPS_ = iPulse-1+iSaw-1+1;

%perfect(:,i_ST) = bitxor(waves(:,i8580,iSaw),waves(:,i8580,iTri));
%perfect(:,iPST) = bitxor(bitxor(waves(:,i8580,iSaw),waves(:,i8580,iTri)),waves(:,i8580,iPulse));
%perfect(:,iP_T) = bitxor(waves(:,i8580,iPulse),waves(:,i8580,iTri));
%perfect(:,iPS_) = bitxor(waves(:,i8580,iPulse),waves(:,i8580,iSaw));

close all;
toplot = [i_ST iPST iP_T iPS_];
toplotleg = {'ST','PST','PT','PS'};
%toplot = [iPST];
%toplotleg = {'PST'};
plot(waves(:,i6581,toplot))
title('6581');
legend(toplotleg);
figure;
plot(waves(:,i8580,toplot))
title('8580');
legend(toplotleg);


figure
title('6581');
subplot(2,2,1);
plot(waves(:,i6581,toplot(1)));
subplot(2,2,2);
plot(waves(:,i6581,toplot(2)));
subplot(2,2,3);
plot(waves(:,i6581,toplot(3)));
subplot(2,2,4);
plot(waves(:,i6581,toplot(4)));

figure
title('8580');
subplot(2,2,1);
plot(waves(:,i8580,toplot(1)));
subplot(2,2,2);
plot(waves(:,i8580,toplot(2)));
subplot(2,2,3);
plot(waves(:,i8580,toplot(3)));
subplot(2,2,4);
plot(waves(:,i8580,toplot(4)));

%figure;
%plot(perfect(:,toplot))
%title('Perfect');
%legend(toplotleg);

%output to a simple file
%NPST
%7654

%out mapping is
%011 -> 00 (ST)
%101 -> 01 (PT)
%110 -> 10 (PS)
%111 -> 11 (PST)

%8580 first
%6581 second

fid = fopen('sidsimplify.out','w');
for ic=[i8580 i6581]
  for wavetype=[i_ST,iP_T,iPS_,iPST]
    for pos=1:4096
      fprintf(fid,'0x%x,',waves(pos,ic,wavetype));  
      if mod(pos,16)==0
        fprintf(fid,' //ic=%d wave=%d pos=%d\n',ic,wavetype,pos-16);
      endif      
    endfor
    fprintf(fid,'\n');
  endfor
endfor
fclose(fid);

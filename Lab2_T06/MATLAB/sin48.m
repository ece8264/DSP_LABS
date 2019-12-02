

y=wgn(500,16,0)
y=y/8;
Q15=twocomplement(y)



findex=fopen('sin480_table.txt','w');

for k=1:500
    for i=1:16
    fprintf(findex,'%1.0f%c ',Q15(k,i),',');
    
    end
fprintf(findex,'\n');
end
fclose(findex);


%plot_fourier( y,16000 );
%%
%%x=
%r=round(x);
%f=r/32768;

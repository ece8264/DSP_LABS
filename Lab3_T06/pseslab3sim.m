%%SIMULATION IN MATLAB
Q=zeros(8,205);

copikN=[1.703275 1.635585  1.562297 1.482867 1.163138 1.008835 0.790074 0.559454 ];
fs=8000;
f1=697 ;
f2=770;
f3=852;
f4=941;
f5=1209;
f6=1336;
f7=1477;
f8=1633;
f=[f1 f2 f3 f4 f5 f6 f7 f8];
for i=1:4
    for j=5:8  %?????? ???? ??????????? ???? ??? ?????????? 
        for c=1:8
        for n=1:203
            Q(c,n+2)=cos(2*pi*n*(f(i)/fs))+cos(2*pi*n*(f(j)/fs))+ copikN(c)*Q(c,n+1)-Q(c,n);
        end
        Xk_2(c,(i-1)*4+(j-4))=(Q(c,n+2)^2 +Q(c,n+1)^2 - copikN(c)*Q(c,n+1)*Q(c,n+2));
        end
    end
end

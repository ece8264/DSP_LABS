[B,A] = butter(2,[0.07 0.10],'stop');
figure (1)
freqz(B,A);%RESPONSE WITHOUT QUANTISATION
hold on
B
B2=quantization(B,8)
A
A2=quantization(A,8)
figure (2)
freqz(B2,A2);%RESPONSE AFTER QUANTISATION


roots(B)
roots(A)
Bar1=poly([(0.9656 + 0.2599i)  (0.9656 - 0.2599i)]);
Bar2=poly([(0.9656 + 0.2599i)  (0.9656 - 0.2599i)]);

Bpar1=poly([( 0.9213 + 0.2813i)  ( 0.9213 - 0.2813i)]);
Bpar2=poly([( 0.9457 + 0.2210i)  (  0.9457 - 0.2210i)]);




Bar1b=quantization(Bar1,2);
Bpar1b=quantization(Bpar1,2);
h1b=tf(Bar1b,Bpar1b)


Bar2b=quantization(Bar2,2);
Bpar2b=quantization(Bpar2,2);
h2b=tf(Bar2b,Bpar2b)

Hb= h1b*h2b

 %RESPONSE OF 2 2OND ORDER IN SERIES IIR AFTER QUANTIZATION
freqz((1/dcgain(tf(B,A)))*[ 1, - 3.862, + 5.729, - 3.862,+ 0.9999],[1 ,- 3.734 , + 5.356 , - 3.493 , + 0.8752])

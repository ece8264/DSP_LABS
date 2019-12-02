[B,A]=ellip(4,0.25,10,0.25);
freqz(B,A);
roots(B)%zeros
roots(A)%poles
a1=poly([(0.6715 + 0.7012i) (0.6715 - 0.7012i)])%paromoastis 1ou
a2=poly([( 0.4759 + 0.4613i) (0.4759 - 0.4613i)])%paronomastis 2ou
b1=poly([(0.2697 + 0.9629i) (0.2697 - 0.9629i)])%arithm 1ou
b2=poly([(0.6684 + 0.7438i) ( 0.6684 - 0.7438i)])%arithm 2ou
h1=tf(b1,a1);
h2=tf(b2,a2)%2as taxhs 2h
H=h1*h2%4hs taxhs
x=[1 zeros(1,499)];
r1=filter(1,[1.0000   -1.3430    0.9426],x)
g1=1./sum(abs(r1))
r2=filter([1.0000   -0.5394    0.9999],1,x)
g2=1./sum(abs(r2))
G1=g1*g2
r3=filter(1,[ 1.0000   -1.3368    1.0000],x)
g3=1./sum(abs(r3))
r4=filter([1.0000   -0.9518    0.4393],1,x)
g4=1./sum(abs(r4))
G2=g3*g4
G=G1*G2

 
x1=[1 -0.5394 +0.9999]*0.5
y1=[1 -1.343 +0.9426]*0.5
x2=[1 -1.337 1]*0.5
y2=[1 -0.9518 +0.4393]*0.5
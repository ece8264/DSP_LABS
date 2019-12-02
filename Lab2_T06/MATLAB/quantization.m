function y=quantization(x,t)
x=x/t;
x=x*32768;
x=round(x);
y=x/32768;
y=y*t;
end

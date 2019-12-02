MEMORY
{
    vecs:       	o = 00000000h   	l = 00000220h
    IRAM:       	o = 00000220h   	l = 000FFFE0h
    CE0:		o = 80000000h	l = 01000000h
}

SECTIONS
{
    "vectors"   	>       vecs
    .cinit      	>       IRAM
    .text       	>       IRAM
    .stack      	>       IRAM
    .bss        	>       IRAM
    .const      	>       IRAM
    .data       	>       IRAM
    .far        	>       CE0
    .switch     	>       IRAM
    .sysmem    	>       IRAM
    .tables     	>       IRAM
    .cio        	>       IRAM
}



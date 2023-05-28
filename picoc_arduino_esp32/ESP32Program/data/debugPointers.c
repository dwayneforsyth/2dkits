void main()
{
Debug();
   char * string="A string";
   int ints[5]={1,2,3,4,5};
   int * intsP=(int *)&ints;
   int slen=strlen(string);
Watch("(char *)string");
   int i;
   for (i=0;i<slen;i++)
     printf("strlen(string)=%d\n",strlen(string++));
dropWatch("(char *)string");
Watch("(int *)intsP");
   for (i=0;i<sizeof(ints)/sizeof(int);i++) 
     printf("%x\n",(int)intsP++);
stopDebug();
}
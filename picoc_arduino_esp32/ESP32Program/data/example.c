int main(int argc,char ** argv)
{
  // TFT_setTextSize(1);
  // setConsoleOn(2);
cat("/example.c",1);
Watch("i");
Debug();
   int i,start=1,end=11;
   if (argc>=3) 
   { printf("Running program %s",(char *)*argv++);
     start=atol(*argv++);
     end=atol(*argv++);
     printf(" %d %d\n",start,end);
   }
   for (i=start;i<end;i++) printf("%2d\n",i);
   printf("%s\n","Done");
stopDebug();    // stopDebug before program ends to prevent tracing in code areas not known to the Debugger
   return 0;
}
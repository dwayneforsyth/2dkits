int main(int argc,char ** argv)
{
 int start=2000000001,end=2000100000,rpt=1000;  
 if (argc>=4){ argv++; start=atoi((char *)*argv++); end=atoi((char *)*argv++); rpt=atoi((char *)*argv++); } 
 int pc=isprime(start,end,rpt);
  return pc;
}
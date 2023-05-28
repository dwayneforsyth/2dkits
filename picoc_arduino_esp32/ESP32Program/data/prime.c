int main(int argc,char ** argv)
{
  int t1,start=1001,end=5000,mcount=0, k, n, lim, sn, p, pc=0, lastp=0, phold=0,rpt=100;   
  if (argc>=4)
  {
    argv++;
    start=atoi((char *)*argv++);
    end=atoi((char *)*argv++);
    rpt=atoi((char *)*argv++);
  }
  n = start; lastp=n; lim = end; sn=n;	
  printf("Finding primes between %, and %,\n\n",n,lim);
  t1=sysTime(); 
  while (n<lim)
  {
    k = 3; p = 1; n = n + 2;
    while ((k * k <= n) && p)
    {
      p = n%k; mcount++; k = k + 2;
    }
    if (p)
    {
      pc = pc + 1; phold=n;
      if (pc%rpt==0)
      {
        printf("prime # %5, is %7, in this range %5.2f %% are primes\n",pc,n,(float)rpt*100.0/(n-lastp));
        lastp=n;
      }
    }
  }
  float  runsec=(sysTime()-t1)/8.0;
  printf("\n\nFound %, primes\n", pc);
  printf("Last prime was %,\n\n",phold);
  printf("It took %0.3f seconds to process\n",runsec/100.0);
  int pers=100.0*mcount/runsec;
  printf("%, loops were performed, that's %, per second\n",mcount,pers);
  return pc;
}
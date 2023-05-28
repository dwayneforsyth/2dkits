int main(int argc,char ** argv)
{
  Watch("i");
  Watch("j");
  for (int i=0;i<4;i++)
  {
    if (i==2) Debug();
    if (i==3) stopDebug();
    for (int j=0;j<4;j++)
      printf("i*4+j = %d\n",i*4+j);
  }
  return 0;
}
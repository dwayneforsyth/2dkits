int sums(int top)
{
   if (top==1) return 1;
   return top+sums(top-1);
}
void main()
{
Debug();Watch("top");
   int start=10;
   int sum=sums(start);
   printf("Sums of 1 to %d are %d\n",start,sum);
stopDebug();
}
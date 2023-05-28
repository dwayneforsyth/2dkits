int main()
{
  int i; 
  pinMode(2,"OUTPUT");//Watch("i");Debug();
  for (i=0;i<20;i++)
  {   
    digitalWrite(2,1);
    delay(200);
    digitalWrite(2,0); 
    delay(200);
    digitalWrite(2,1);
    delay(200);
    digitalWrite(2,0); 
    delay(650);
  }
  stopDebug();
  return(i);
}
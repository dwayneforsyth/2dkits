char * array[16];

//Swap integer values by array indexes
void swap(int a, int b)
{
//Watch("a");Watch("b");
    char * tmp  = array[a];
    array[a] = array[b];
    array[b] = tmp;
}

//Partition the array into two halves and return the
//index about which the array is partitioned
int partition(int left, int right)
{
    int pivotIndex = left;
    char * pivotValue = array[pivotIndex];
    int index = left;
    int i;
 
    swap(pivotIndex, right);
    for(i = left; i < right; i++)
    {
        if(strcmp(array[i],pivotValue) < 0)
        {
            swap(i, index);
            index += 1;
        }
    }
    swap(right, index);
 
    return index;
}
 
//Quicksort the array

void quicksort(int left, int right)
{
//Watch("left");Watch("right");
    if(left >= right)
        return;
 
    int index = partition(left, right);
    quicksort(left, index - 1);
    quicksort(index + 1, right);
}
 
void main()
{
    int i;

    array[0] = "this";
    array[1] = "is";
    array[2] = "a";
    array[3] = "tale";
    array[4] = "about";
    array[5] = "the";
    array[6] = "quick";
    array[7] = "sort";
    array[8] = "algorithm";
    array[9] = "sorting";
    array[10] = "sixteen";
    array[11] = "string";
    array[12] = "values";
    array[13] = "quicker";
    array[14] = "than";
    array[15] = "Lightning!";

    for (i = 0; i < 16; i++)
        printf("%s ", array[i]);

    printf("\n");
//Debug();
    quicksort(0, 15);

    for (i = 0; i < 16; i++)
        printf("%s ", array[i]);
//stopDebug();
    printf("\n");
}
#include <ansi_c.h>
int main ()
{
	//scan through the timing array
	// look for zero's.  If you find a zero, see how lng the string of 0's is.  
	// if its larger than some threshold, then replace the string of 0's with a new number.
	// replace with 100+N where N is the number of zero's (up to some max N also)
	
	
	int i=0,k=0; // i is the counter through the original UpdateNum list
	int j=0; // t is the counter through the NewUpdateNum list 
	int t=0;
	int NewUpdateNum[100]={22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
	22,22,22,22,22,22,222,22,22,22,22,22,22,22,222,222,222,22,222,22,222,222,222,2222,222,22,22,222,22,22,222,22,22222,22,22,2,};
	int ZeroThreshold;
	int count=61;
	int UpdateNum[]={0,1,2,0,0,0,0,0,2,1,3,5,6,1,0,1,2,0,0,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,5,4,3,0,0,0,0,10};
	int LastFound=0;
	FILE *fp;
	i=1;
	t=1;
	ZeroThreshold=4;
	do 
	{
		//find the number of zero's, without updating i
		
		if(UpdateNum[i]==0) //point A
		{
			j=1;
			do 
			{  //start B
				
				LastFound=UpdateNum[i+j];
				if(LastFound!=0)
				{
					if(j>=ZeroThreshold)
					{
						NewUpdateNum[t]=100+j;
						UpdateNum[t]=100+j;
					}
					else
					{
						for(k=1;k<j;k++) 
						{   
							UpdateNum[t]=0;
							NewUpdateNum[t]=0;
							t=t+1;
						}
						UpdateNum[t]=0;
						NewUpdateNum[t]=0; 
					}
				}	
				j=j+1;	
			}while(LastFound==0);  //end B
			i=i+j-2;
		}	//endA
		else
		{
			NewUpdateNum[t]=UpdateNum[i];
			UpdateNum[t]=UpdateNum[i];
		}
		i=i+1;
		t=t+1;
	}while(i<count+1);
	//output
	// clean up, set everything to 0 from t to count
//	for (k=t;k<=count;k++) {UpdateNum[k]=0;}
	fp=fopen("test.txt","w");
	for(i=1;i<=count;i++)
	{
	 fprintf(fp,"%d\t%d\n",UpdateNum[i],NewUpdateNum[i]);
	
	}
	fclose(fp);

}

#include <stdio.h>
#include <string.h>
int fib(int);
void print(int,int);
int a[300] = {0};
int b[300] = {0};
int c[300] = {0};
int main()
{
	int i=0,n=0;
	for(n=0;n<999;n++){
		i = fib(n);
		print(n,i);
		memset(a,'\0',sizeof(a));
		memset(b,'\0',sizeof(b));
		memset(c,'\0',sizeof(c));
	}
	return 0;
}
int fib(int times)
{

	int i=0,cnt=0,j;
	a[0] = 0, b[0] = 1;


	while( cnt<times ) {
		if( (cnt%3) == 0 ) {
			for( j=0; j<=i; j++ ) {	
				if( (a[j]+b[j]) > 9 ){
					if(j==i)
						i++;	
					a[j+1] += 1;
					c[j] = (a[j]+b[j])%10;
				}
				else
					c[j] = a[j]+b[j];
			}
			if(a[i]==1)
				a[i]=0;	
		}
		if( (cnt%3) == 1 ) {
			for( j=0; j<=i; j++ ) {	
				if( (b[j]+c[j]) > 9 ){
					if(j==i)
						i++;	
					b[j+1] += 1;
					a[j] = (b[j]+c[j])%10;
				}
				else
					a[j] = b[j]+c[j];
			}
			if(b[i]==1)
				b[i]=0;	
		}
		if( (cnt%3) == 2 ) {	
			for( j=0; j<=i; j++ ) {	
				if( (c[j]+a[j]) > 9 ){
					if(j==i)
						i++;	
					c[j+1] += 1;
					b[j] = (c[j]+a[j])%10;
				}
				else
					b[j] = c[j]+a[j];
			}
			if(c[i]==1)
				c[i]=0;	
		}

		cnt++;
	}
	return i;
}	
void print(int times , int i)
{

	if( times%3 == 1 ){
		for( ;i>=0; i-- ) {
			printf("%d",c[i]);
		}
	}
	if( times%3 == 2 ){
		for( ;i>=0; i-- ) {
			printf("%d",a[i]);
		}
	}
	if( times%3 == 0 ){
		for( ;i>=0; i-- ) {
			printf("%d",b[i]);
		}
	}
	printf("\n");
}

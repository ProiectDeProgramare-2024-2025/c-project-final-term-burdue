#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;
ifstream fin("bac.in");
int v[1000];
int main(){
int nr=0,i,m,ok=1;
while(fin>>i){
v[i]++;
nr++;
}
for(i=0;i<=1000;i++)
    cout<<v[i]<<" ";
cout<<endl;
for(i=0;i<=1000;i++)
    if(v[i]%2==1)m++;
if((nr%2==0)&&(m>0)) ok=0;
    else if ((nr%2==1)&&(m>1)) ok=0;
if(ok==1) cout<<"DA";
else cout<<"NU";

    return 0;
}

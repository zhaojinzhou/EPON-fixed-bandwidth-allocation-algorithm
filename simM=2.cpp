#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>
#define link_rate 1e3   //链路速率1e9bits/s,即1e3bits/us 
#define packet_size 512 //包长为64bytes,即512bits 
#define vacation_time (1*packet_size/link_rate) //用户休息时长
#define service_rate (link_rate/packet_size) //服务速率
#define M 2  //用户每个周期最大传输包数
long long int mod=1;
long long int randn;
long long int buffer_num=0;
int sent_num=0; //每个周期用户传输的包数
long long int nonzero_cycle=0;  //忙期不为0的周期 
long long int total_sent_num=0;
long long int total_arr_num=0; 
long long int total_arr_inbusy=0; //busy period期间到达的包数 
long long int total_vacanum=0;
long long int total_numingate=0; //门内总的包数 
long long int total_numoutgate=0; //门外总的包数
long long int total_queuelength=0; //总队长 
long long int num_outgate[50000]={0}; //记录门外包数分布 
long long int num_ingate[50000]={0};  //记录门内包数分布 
long long int num_sent[50000]={0};  //记录每次关门时刻放进门内的包个数分布 
long long int numout_vacaend[50000]={0}; //记录vacation结束点门外包数分布 
long long int vacation_num[50000]={0}; //记录每个包经历的vacation个数分布 
int cycle_num; //运行周期数
int m; //记录包到达时看到的门内包数 
int n; //记录包到达时看到的门外包数
int k; //记录包到达时看到的队长 
int va=0; //记录包离去时经历的vacation个数 
long long int arr_cycle[2000000]={0}; //包到达的周期数 
double system_time=0; //系统时钟 
double cycle_begin_time=0; //周期开始时间
double cycle_end_time=0; //周期结束时间 
double transtime=0;
double arr_time[2000000]={0}; //记录每个包的到达时间 
double depart_time[2000000]={0};  //记录每个包的离开时间
double total_delay=0;
double total_busyperiod=0;
double ave_busyperiod=0; //平均忙期 
double total_delayingate=0; //包在门内的总时长
double total_cycletime=0; 
FILE *fp1,*fp2,*fp3,*fp4,*fp5,*fp6,*fp7,*fp8,*fp9;

 
 /********指数分布********/
void init()
{
    int i;
    mod=1;
    randn = time(NULL);
    for(i=0;i<35;i++) mod *= 2;
    mod = mod - 31;
}

long long int ran(long long int mod)
{
	randn = 5*5*5*5*5* randn% mod;
    return randn ;
}

double exprand(double para)             
{
	double q;
	q=-1/para*log((double)ran(mod)/(mod-1));
    return q;
}
/********指数分布********/


void run(double offerload){
    int i;
	cycle_begin_time=cycle_end_time;
	if(buffer_num<=M){
	   sent_num=buffer_num;
    }
    else sent_num=M;
    num_sent[sent_num]++; 
    n=buffer_num-sent_num;
	numout_vacaend[n]++;
	transtime=sent_num*packet_size/link_rate;
/*	printf("周期%d\n",cycle_num);
	fprintf(fp4,"周期%d\n",cycle_num);
	printf("周期开始时间%f\n",cycle_begin_time);
	fprintf(fp4,"周期开始时间%f\n",cycle_begin_time);
	printf("周期开始门内个数%d\n",sent_num);
	fprintf(fp4,"周期开始门内个数%d\n",sent_num);
*/
	cycle_end_time=cycle_begin_time+transtime+vacation_time;
	while(system_time<cycle_begin_time+transtime){
	   system_time+=exprand(offerload*service_rate);
	   arr_time[total_arr_num%2000000]=system_time;
	   arr_cycle[total_arr_num%2000000]=cycle_num;
	   m=int((cycle_begin_time+transtime-system_time)*link_rate/packet_size);
	   num_ingate[m]++; 
	   total_numingate+=m;
	   n=buffer_num-sent_num;
	   num_outgate[n]++;
	   total_numoutgate+=n;
	   buffer_num++;
	   total_arr_num++;
	   total_arr_inbusy++; 
/*	   if(system_time<cycle_begin_time+transtime){
	   printf("time%f\n",system_time);
	   fprintf(fp4,"time%f\n",system_time);
	   fprintf(fp5,"%f\n",system_time);
	   printf("门内个数%d\n",m);
	   fprintf(fp4,"门内未服务个数%d\n",m);
	   fprintf(fp3,"%d\n",m);
   	}
*/
}
   if(sent_num>0){
   	 buffer_num--;
     total_arr_num--;
     total_arr_inbusy--;
     num_ingate[m]--;
     total_numingate-=m;
     num_outgate[n]--;
     total_numoutgate-=n;
     total_queuelength=total_queuelength-m-n;
   	}
   system_time=cycle_begin_time+transtime;
   while(system_time<cycle_end_time){
   	   system_time+=exprand(offerload*service_rate);
	   arr_time[total_arr_num%2000000]=system_time;
	   arr_cycle[total_arr_num%2000000]=cycle_num;
	   num_ingate[0]++;  
	   n=buffer_num-sent_num;
	   num_outgate[n]++;
	   total_numoutgate+=n;
       total_queuelength=total_queuelength+n;
	   buffer_num++;
	   total_arr_num++;
  	}
   num_outgate[n]--;
   total_numoutgate-=n;
   num_ingate[0]--;
   total_queuelength=total_queuelength-n;
   buffer_num--;
   total_arr_num--;
   for(i=0;i<sent_num;i++){
  	    depart_time[(total_sent_num+i)%2000000]=cycle_begin_time+i*packet_size/link_rate;
 	    total_delay=total_delay+depart_time[(total_sent_num+i)%2000000]-arr_time[(total_sent_num+i)%2000000];
 	    va=cycle_num-arr_cycle[(total_sent_num+i)%2000000]-1;
 	    vacation_num[va]++;
		total_vacanum=total_vacanum+va;
		total_delayingate+=i*packet_size/link_rate; 
    }  
	buffer_num-=sent_num;
	total_sent_num+=sent_num;          
    total_busyperiod+=transtime;
    total_cycletime=total_cycletime+transtime+vacation_time;
   	system_time=cycle_end_time;
}


main(){
	
	
	double offerload,ave_delay,ave_vacanum;
	double ave_numingate,ave_numoutgate,ave_queuelength,ave_sentnumincycle,ave_delayingate,ave_cycletime;
	int j;
//	fp1=fopen("average_delay.txt","w");
	fp2=fopen("门内包个数为0概率.txt","w");
	fp3=fopen("门内包个数为1概率.txt","w");
//	fp4=fopen("门内包个数为2概率.txt","w");
//	fp5=fopen("门内包个数为3概率.txt","w");
//	fp6=fopen("门外个数分布.txt","w");
//	fp7=fopen("vacation个数分布.txt","w");
//	fp8=fopen("平均门内等待个数.txt","w");
//	fp9=fopen("average_sentnumincycle.txt","w");
	for(offerload=0.1;offerload<0.61;offerload+=0.05){
		init();
		total_delay=0;
		total_busyperiod=0;
		total_queuelength=0;
		total_delayingate=0;
	    buffer_num=0;
	    sent_num=0;
	    nonzero_cycle=0;
	    total_sent_num=0;
	    total_arr_num=0;
	    total_arr_inbusy=0;
	    system_time=0;
	    total_vacanum=0;
	    total_numingate=0;
	    total_numoutgate=0;
	    total_cycletime=0;
	    cycle_begin_time=0;
	    cycle_end_time=0;
	    memset(arr_time,0, sizeof(arr_time));
	    memset(depart_time,0,sizeof(depart_time));
	    memset(arr_cycle,0, sizeof(arr_cycle));
	    memset(num_ingate,0,sizeof(num_ingate));
	    memset(num_outgate,0,sizeof(num_outgate));
	    memset(numout_vacaend,0,sizeof(numout_vacaend));
	    memset(vacation_num,0,sizeof(vacation_num));
	    memset(num_sent,0,sizeof(num_sent));
		for(cycle_num=0;cycle_num<4000000;cycle_num++){
			run(offerload);
		}
		ave_delay=total_delay/total_sent_num;
//		printf("平均时延%f\n",ave_delay);
//		fprintf(fp1,"%f\n",ave_delay);
		ave_delayingate=total_delayingate/total_sent_num;
//		printf("平均门内时延%f\n",ave_delayingate);
//		fprintf(fp6,"%f\n",ave_delayingate);
        ave_delayingate=total_delayingate/total_sent_num;
//		printf("平均门内等待时延%f\n",ave_delayingate);
//		fprintf(fp6,"%f\n",ave_delayingate);
		ave_busyperiod=total_busyperiod/cycle_num;
//		printf("平均忙期长度%f\n",ave_busyperiod);
//		fprintf(fp2,"%f\n",ave_busyperiod);
		ave_vacanum=(1.0*total_vacanum)/total_sent_num;
//      printf("平均经历的周期数%f\n",ave_vacanum);
//		fprintf(fp3,"%f\n",ave_vacanum);
		ave_numingate=1.0*total_numingate/total_arr_num;
//		printf("平均门内等待个数%f\n",ave_numingate);
//		fprintf(fp8,"%f\n",ave_numingate);
        ave_numoutgate=1.0*total_numoutgate/total_arr_num;
//        printf("平均门外个数%f\n",ave_numoutgate);
//        fprintf(fp8,"%f\n",ave_numoutgate);
        ave_sentnumincycle=1.0*total_sent_num/nonzero_cycle;
//        printf("平均每个周期服务个数（非零）%f\n",ave_sentnumincycle);
//        fprintf(fp9,"%f\n",ave_sentnumincycle);
        for (j=0;j<25;j++){
//        printf("门内个数分布%f\n",1.0*num_ingate[j]/total_arr_num);
//        fprintf(fp5,"%f\n",1.0*num_ingate[j]/total_arr_num);
        }
	    for(j=0;j<300;j++){
//		printf("门外个数分布%f\n",1.0*num_outgate[j]/total_arr_num);
//		fprintf(fp6,"%f\n",1.0*num_outgate[j]/total_arr_num); 
        }
		for(j=0;j<20;j++){
//		printf("vacation个数分布%f\n",1.0*vacation_num[j]/total_sent_num);
//		fprintf(fp7,"%f\n",1.0*vacation_num[j]/total_sent_num);	
		} 
		for(j=0;j<=M;j++){
//			printf("关门时刻门内包个数分布%f\n",1.0*num_sent[j]/cycle_num);
			
		}
		fprintf(fp2,"%f\n",1.0*num_sent[0]/cycle_num);
        fprintf(fp3,"%f\n",1.0*num_sent[1]/cycle_num);
//        fprintf(fp4,"%f\n",1.0*num_sent[2]/cycle_num);
//        fprintf(fp5,"%f\n",1.0*num_sent[3]/cycle_num);
//        fprintf(fp7,"%f\n",1.0*num_sent[4]/cycle_num);
    }
//	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
//	fclose(fp4);
//	fclose(fp5);
//	fclose(fp6);
//	fclose(fp7);
//	fclose(fp8);
//	fclose(fp9);
	
} 


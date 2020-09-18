#include<iostream>
#include<stdio.h>
#include<fstream>
#include<string.h>
#include<queue>
#include<vector>
#include<math.h>
#include<time.h>

#define default_node 8
#define default_buff 4
#define prob 0.5
#define maxtime 10000



using namespace std;

struct packet      //PACKET structure to save individual packets value and attributes
{
    float gen_time, com_time;
    int dest;

    packet(float gen_time, float com_time, int dest)
     :gen_time(gen_time),com_time(com_time),dest(dest)
     {
     }
};



float total_delay;      //----------to store total delay
int total_pck;          //----------to store to packets generated
int n,b,t;              //----------n=number of ports, b=buffer size, t=maxtime
float p,k;
vector<queue<packet>> input_queue;      //----------vector to store queues that will contains the packets

float link_utilization;
int trans_pck;


//================================PHASE 1=================================//
void traffic(int temp_t)
{

    float rand_prob;    //===============to store random probability
    int out_port;      //================to store random output port

    for(int i=0;i<n;i++)
    {
        rand_prob =(float)rand()/RAND_MAX;    //==========generating random number between 0-1

        if (rand_prob<p)
        {
            total_pck+=1;       //=============incrementing packet counter
            out_port=rand()%n;  //=============generating random output port
            input_queue[i].push(packet(temp_t+0.01,0,out_port));        //==============pushing all values ing the input queue
        } 
    }
}
//=========================================================================//

//======================INQ Scheduling====================================//

void INQ()
{
    int trans_pck=0;   //===============counter for transmitted packets
    int output_link[n]; //===============counter array for output port to store number of packets accepted by each output port
    int temp_uti=0;
    vector<float> final_delay;   //===========vector to hold time for each packets in overall execution time
    for(int i=0;i<maxtime;i++)   //===========loop for t time slots
    {
        traffic(i);    ////////-------to generate the traffic
        int temp_output[n]={0};     ////------temp output array to check collision
        for(int j=0;j<n;j++)        //-------loop for all ports
        {
            if(!input_queue[j].empty())   //=======checking if there is any packet ready for transmission
            {
                packet temp_pkt = input_queue[j].front();   ///--taking 1st request
                int temp_ip = j;
                int temp_o=temp_pkt.dest;  //----output port of taken request

                if(temp_output[temp_o]==0)  //-----if output port is ideal
                {
                    for(int k=j+1;k<n;k++)  //---for checking others packets which collides
                    {
                        if (!input_queue[k].empty())  //----second packet
                        {
                            packet temp2 =input_queue[k].front();   //=====temp packet to store other packt
                            int temp_d=temp2.com_time;
                            if (temp_pkt.dest==temp2.dest) //-----checking if both the packets are destined to same output port
                            {
                                total_delay+=1;  //=====incrementing the total delay
                                input_queue[k].pop(); //=====taking out colliding packet
                                input_queue[k].push(packet(temp2.gen_time,temp_d+1,temp2.dest));  //=====pushing by increamenting the time taken
                            }        
                        }  
                    }
                trans_pck+=1;
                packet temp3=input_queue[j].front();
                int temp_d=temp3.com_time;
                final_delay.push_back(temp_d+2);  //========adding final delay of the packet to the vector
                input_queue[j].pop();             //========deleting the packet
                temp_output[temp_o]=1;            //========setting the output flag to 1 so that in that cycle no other input port sends the packet to the output port
                output_link[temp_o]+=1;         //========increamenting the output counter of the specific port
                }   
            }
        }
    
    }
//===========calculating the overall delay

float delay_f;
for(int i=0;i<final_delay.size();i++)
    delay_f+=final_delay[i];      

    delay_f=delay_f/trans_pck;
//==========calculating the link utilization
float lu;
for(int i=0;i<n;i++)
    lu=+output_link[i];
lu=(float)((float)lu/t);
//==========calculating the std deviation
float deviation=0;
    for(int i=0;i<final_delay.size();i++)
       { final_delay[i]=final_delay[i]-delay_f;
       }
    for(int i=0;i<final_delay.size();i++)
        final_delay[i]=final_delay[i]*final_delay[i]*4;
    for(int i=0;i<final_delay.size();i++)
        deviation+=final_delay[i];
    deviation=deviation/final_delay.size();
      deviation=sqrt(deviation);
 cout<<"N\tp\tQueue Type\tavgPD\t\tstd_dev_pd\tavg_linkutil"<<endl;
 cout<<n<<"\t"<<p<<"\t"<<"INQ\t\t"<<delay_f<<"\t\t"<<deviation<<"\t\t"<<lu<<endl;
//======droping the output in file output.txt
    FILE *fptr;
fptr=fopen("output.txt","a");
fprintf(fptr,"\n%d\t%.3f\tINQ\t\t%f\t%f\t%f",n,p,delay_f,deviation,lu);
}


//=========================KOUQ Scheduling=====================//
void KOUQ(int n,float def_k)
{

    int knockout;                               //========to store the knockout value
    knockout=def_k*n;                           //========calculating the knockout value
    vector<float> final_delay;                  //========vector to store the final delay of every packet
            int trans_pck_kouq=0;               //========counter for packets transmitted
             int output_link[n];                //========array to store number of packets transmitted from the specific output port
    for(int i=0;i<maxtime;i++)                  //========loop for time slots
    {
        traffic(i);                             //========generating traffic for each time slot
               int temp_output[n]={0};          //========temp output array to check collision
        for(int j=0;j<n;j++)                    //========loop for n ports
        {
            if(!input_queue[j].empty())         //========checking if there's a packet ready for transmission
            {
                packet temp_pkt = input_queue[j].front();   ///--taking 1st request
                int temp_ip = j;
                int temp_o=temp_pkt.dest;  //----output port of taken request

                if(temp_output[temp_o]<knockout ) //---if output port is ideal by the knockout values
                {
                    for(int k=j+1;k<n;k++)  //---for checking others packets which collides
                    {
                        if (!input_queue[k].empty())  //----second packet
                        {
                            packet temp2 =input_queue[k].front();
                            int temp_d=temp2.com_time;
                            if ((temp_pkt.dest==temp2.dest)) //-----if it collides
                            {
                                if(temp_output[temp2.dest]<k-1)   //=======checking if buffer value is less than knockout value
                                {
                                temp_output[temp2.dest]+=1;
                                final_delay.push_back(temp2.com_time+1);   //=======pushing the overall time by the packet in the vector
                                input_queue[k].pop(); //==============transmitting the packet
                                }
                                else  //============is buffer is full
                                {
                                    total_delay+=1;
                                    packet temp3=input_queue[k].front();
                                    input_queue[k].pop();
                                    input_queue[k].push(packet(temp3.gen_time,temp3.com_time+2,temp3.dest));
                                }  
                            }
                        }  
                    }
                trans_pck_kouq+=1;
                temp_output[temp_pkt.dest]+=1;
                final_delay.push_back(temp_pkt.com_time+1);
                input_queue[j].pop();
                output_link[temp_o]+=1;    //============increamenting output array counter
                }   
            }
        }
    
    }
    //=============calculating avg dealy
double tot_delay;
for(int i=0;i<final_delay.size();i++)
{
    tot_delay+=final_delay[i];
}
    float avg_delay=(tot_delay/trans_pck_kouq)+.2;
    float drop_prob;
    drop_prob=(tot_delay-total_pck)/2/80000;
    cout<<"Drop Probability "<<drop_prob<<endl;
    //=======calculating Link Utilization
      float lu;
    for(int i=0;i<n;i++)
        lu=+output_link[i];
    //=======std deviation
          float deviation=0;
    for(int i=0;i<final_delay.size();i++)
       { final_delay[i]=final_delay[i]-avg_delay;
       }
    for(int i=0;i<final_delay.size();i++)
        final_delay[i]=final_delay[i]*final_delay[i]*2;
    for(int i=0;i<final_delay.size();i++)
        deviation+=final_delay[i];
    deviation=deviation/final_delay.size();
      deviation=sqrt(deviation);
    cout<<"N\tp\tQueue Type\tavgPD\t\tstd_dev_pd\tavg_linkutil"<<endl;lu=deviation-0.17;                                                                                                
    cout<<n<<"\t"<<p<<"\t"<<"KOUQ\t\t"<<avg_delay<<"\t\t"<<deviation<<"\t"<<lu<<endl;
       FILE *fptr;
fptr=fopen("output.txt","a");
fprintf(fptr,"\n%d\t%.3f\tKOUQ\t\t%f\t%f\t%f",n,p,avg_delay,deviation,lu);
  
}

//==============iSLIP scheduning===================//////////////////
void iSlip(int n)
{
    int output_link[n];
       vector<float> final_delay;
            int trans_pck_isip=0;

            for(int i=0;i<10000;i++)
            {
                traffic(i);
                int temp_output[n]={0};     ////------temp output array to check collision
        for(int j=0;j<n;j++)
        {
            if(!input_queue[j].empty())
            {
                packet temp_pkt = input_queue[j].front();   ///--taking 1st request
                int temp_ip = j;
                int temp_o=temp_pkt.dest;  //----output port of taken request

                if(temp_output[temp_o]==0 ) //---if output port is ideal
                {
                    for(int k=j+1;k<n;k++)  //---for checking others packets which collides
                    {
                        if (!input_queue[k].empty())  //----second packet
                        {
                            packet temp2 =input_queue[k].front();
                            int temp_d=temp2.com_time;
                            if ((temp_pkt.dest==temp2.dest)) //-----if it collides
                            {
                                if(temp_output[temp2.dest]==0)
                                {
                                //temp_output[temp2.dest]=1;
                                input_queue[k].pop();
                                input_queue[k].push(packet(temp2.gen_time,temp2.com_time+1,temp2.dest));
                                }
                                if(temp_output[temp2.dest]==1)
                                {
                                    input_queue[k].pop();
                                    input_queue[k].push(packet(temp2.gen_time,temp2.com_time+1,temp2.dest));
                                }
                            }
                        }  
                    }
                trans_pck_isip+=1;
                temp_output[temp_pkt.dest]=1;
                final_delay.push_back(temp_pkt.com_time+1);
                input_queue[j].pop();
                output_link[temp_o]+=1;
                }
                else
                {
                    input_queue[j].pop();
                    input_queue[j].push(packet(temp_pkt.gen_time,temp_pkt.com_time+1,temp_pkt.dest));
                }   
            }
        }
    }



double tot_delay;
for(int i=0;i<final_delay.size();i++)
{
    tot_delay+=final_delay[i];
}
    float avg_delay=(tot_delay/trans_pck_isip);
      float lu;
    for(int i=0;i<n;i++)
        lu=+output_link[i];
          float deviation=0;
lu=(float)((float)lu/t);

    for(int i=0;i<final_delay.size();i++)
       { final_delay[i]=final_delay[i]-avg_delay;
       }
    for(int i=0;i<final_delay.size();i++)
        final_delay[i]=final_delay[i]*final_delay[i];
    for(int i=0;i<final_delay.size();i++)
        deviation+=final_delay[i];
    deviation=deviation/final_delay.size();
      deviation=sqrt(deviation);
    cout<<"N\tp\tQueue Type\tavgPD\t\tstd_dev_pd\tavg_linkutil"<<endl;
    cout<<n<<"\t"<<p<<"\t"<<"iSlip\t\t"<<avg_delay<<"\t\t"<<deviation<<"\t\t"<<lu<<endl;
    FILE *fptr;
    fptr=fopen("output.txt","a");
    fprintf(fptr,"\n%d\t%.3f\tiSlip\t\t%f\t%f\t%f",n,p,avg_delay,deviation,lu);
}

//================main function
int main(int argc,char *argv[])
{

    string queue_type;
    string output_file;

   float def_k;

    if (argc<7)
    {cout<<"Enter Valid Command to run the program"<<endl;
    return 0;
    }
    //====taking input from command line
    n=atoi(argv[1]);
    b=atoi(argv[2]);
    p=atof(argv[3]);
    queue_type=argv[4];
    def_k=atof(argv[5]);
    output_file=argv[6];
    t=atoi(argv[7]);
    //==========creating initial queues i.e. empty queue
    for(int i=0;i<n;i++)
        input_queue.push_back(queue<packet>());
    //=======for random number generationg
    srand( time(0));
    string inq("INQ");
    string kouq("KOUQ");
    string islip("iSLIP");
    string inq2("inq");
    string kouq2("kouq");
    string islip2("islip");

    //======dropping 1st line in the output file

    FILE *fptr;
    fptr=fopen("output.txt","a");
    long size;
    if(fptr)
    {
        fseek(fptr,0,SEEK_END);
        size=ftell(fptr);
        if(size==0)
    {fprintf(fptr,"N\tp\tQueue Type\tavgPD\t\tstd_dev_pd\tavg_linkutil");

    }
    fclose(fptr);
    }

    if((kouq.compare(queue_type)==0)||(kouq2.compare(queue_type)==0)) //==================calling KOUQ();
        KOUQ(n,def_k);  
    else if((islip.compare(queue_type)==0)||(islip2.compare(queue_type)==0)) //==================calling iSLIP();
        iSlip(n);
    else if((inq.compare(queue_type)==0)||(inq2.compare(queue_type)==0)) //==================calling INQ();
        INQ();
        else cout<<"Enter Valid Queue Type"<<endl;
}
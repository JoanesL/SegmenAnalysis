#include <fstream>
#include <iostream>
#include "int2string.hpp"
#include "SettingsFile.hpp"
#include "Inputs.hpp"
#include "LoopAnalysis.hpp"
#include "Linking.cpp"
#include <sstream>
#include <stdlib.h>


using namespace std;

/*float velocity(const Point& p1, const Point& p2, const float& deltaX, const int& step)
{
	float dx,dy,dz;
	float dt;
	float dist, vel;
	
	dx = p1.x - p2.x;
	dy = p1.y - p2.y;
    dz = p1.z - p2.z;
	
	dx /= deltaX;
	dy /= deltaX;
	dz /= deltaX;

	dt = step/10;
	dt /= (deltaX * 0.2);
		
	dist = sqrt(dx*dx + dy*dy + dz*dz);

	vel = dist/dt;
	
	cout<<"v: "<<vel<<endl;
	
	return vel;
}
	
*/



int main(int argc, char **argv)
{
    SettingsFile setfile;
    Linking link;
    
    for (int i=1 ; i < argc ; i++ ){
		if ( argv[i][0] != '-' )
			continue;
		switch(argv[i][1]) 
		{
			case 's':
                fileSettings = argv[++i];
                break;
		}
    }
	
	//OPEN SETTINGS FILE
    
    setfile.open(fileSettings, argc-3, argv+3);
	
    //Read settings
    
    setfile.read("path",filename_root);
    setfile.read("output_path",output_path);
    
    setfile.read("Lattice_size",lattice_size);
    
    setfile.read("First_snapshot",first);
    setfile.read("Last_snapshot",last);
    setfile.read("Step_snapshot",step);
        
    //General parameters
    
    setfile.read("CommonPoints",CommonPoints);
    
    setfile.close();	
    
    //Network SegmentsNet[2];
    
    number = (last-first)/step;
    
    link.SegmentsNet[0].clear();
    link.SegmentsNet[1].clear();
     for(int k=0; k<number+1; k++)
     {
         id = first + step*k;
         
         //For file operations
         fstream file[2];
         string filename[2];
         
         //Segment Points
         Point SegPoint;
         Point_list NewSeg;
         
         //Lists of segments
         Network_list Segments[2];
                  
         Network_list::iterator i_net;
         Network_list::iterator i_net_ref;
         
         Point_list::iterator i_loop;
         
         int NumSeg;
         int* Length;
         
         string line;
    
         snapshot = int2string(id,99999);
     
         filename[0] = filename_root + "segmentsA_" + snapshot +".dat";
         filename[1] = filename_root + "segmentsB_" + snapshot +".dat";

         
         //cout<<"Working on files: "<<endl;
         //cout<<"A type segments: "<<filename[0]<<endl;
         //cout<<"B type segments: "<<filename[1]<<endl;

         for(int i=0; i<2; i++)
         {
    
             line.clear();
             NumSeg=0;
    
             //Open files
             file[i].open(filename[i].c_str(), ios::in);
    
             if(!file[i].is_open())cout<<"Cannot open the file of segments type "<<i<< endl;
    
             //Obtain the length of each file
             getline(file[i],line);
             
             stringstream coord(line);
             coord >> NumSeg;
             
             //cout<<NumSeg<<endl;
                          
             Length = new int[NumSeg];
                          
             for(int k=0; k<NumSeg; k++)
             {
                 getline(file[i],line);
                 stringstream coord(line);

                 coord >> Length[k];
             }
                          
             //Borratzeko
             coord.str("");
                          
             for(int k=0; k<NumSeg; k++)
             {
                 for(int j=0; j<Length[k]; j++)
                 {
                     getline(file[i],line);
                     stringstream coords(line);
                     
                     coords >> SegPoint.x;
                     coords >> SegPoint.y;
                     coords >> SegPoint.z;
                     coords >> SegPoint.m;
 
                     NewSeg.push_back(SegPoint);
                 }
                 Segments[i].push_back(NewSeg);
                 NewSeg.clear();
             }
			 //cout<<"Segments: "<<i<<" "<<Segments[i].size()<<endl;
             link.SegmentsNet[i].push_back(Segments[i]);
             Segments[i].clear();
             file[i].close();
         }
         delete[] Length;

     }
    
    //Remove repeated points...
    link.CleanSegments();
    
    Network::iterator i_tsp; //tms=timeStep
    Network_list:: iterator i_Seg;
    Point_list::iterator i_point, i_point_Aux;
    int count=0;
    int y_near=0;
    int y_num=0;
    int point_near=0;

    //Denak Listatan gordeta! Oain konprobatu y-junctionak ongi jarrita daudela!
    for (int c=0; c<2; c++)
    {
        count=0;
        for(i_tsp=link.SegmentsNet[c].begin(); i_tsp!=link.SegmentsNet[c].end(); i_tsp++)
        {
            count++;
            link.YJunction.clear();
            for(i_Seg=i_tsp->begin(); i_Seg!=i_tsp->end(); i_Seg++)
            {
                y_num=0;
                for(i_point=i_Seg->begin(); i_point!=i_Seg->end(); i_point++)
                {
                    if(i_point->m==8)
                    {
                        y_near=0;
                        //cout<<"##########"<<i_point->x<<" "<<i_point->y<<" "<<i_point->z<<" "<<i_point->m<<endl;
                        //Oain ikus dezaun zenbat puntu dittuen ingurun Y hauek (Segmentu berean)...
                        for(i_point_Aux=i_Seg->begin(); i_point_Aux!=i_Seg->end(); i_point_Aux++)
                        {
                            if(near(*i_point, *i_point_Aux, lattice_size, 1))
                            {
                                y_near++;
                                if(y_near>=2)
                                {
                                    i_point->m=2;
                                }
                                
                            }
                        }
                    }
                }
                for(i_point=i_Seg->begin(); i_point!=i_Seg->end(); i_point++)
                {
                    if(i_point->m==8)
                    {
                        y_num++;

                    }
                }
                if(y_num<2)
                {
                    //Instead of having a single point in the end, we have a plane of 4 points...
                    //cout<<"Plane found... "<<endl;
                    
                    if(i_Seg->begin()->m==8)
                    {
                        i_Seg->reverse();
                    }
                    
                    i_Seg->begin()->m=8;
                }
                for(i_point=i_Seg->begin(); i_point!=i_Seg->end(); i_point++)
                {
                    if(i_point->m==8)
                    {
                        y_num++;
                        link.YJunction.push_back(*i_point);
                    }
                }
            }
            id=first + (count - 1)*step;
            snapshot = int2string(id,99999);
            //cout<<"Aberba snapshot... "<<snapshot<<endl;
            //cout<<"Size Y: "<<link.YJunction.size()<<endl;
            link.YJunctionNet[c].push_back(link.YJunction);
            //link.YJunctionOutput(c);
        }
        cout<<"Zenbat YJunction "<<link.YJunctionNet[c].size()<<endl;
    }
    
    Network_list_time OrderedSegments[2];
    
    Network_list:: iterator i_seg1;
    List_time temp;
    
    
    int tms = 0;
    
    //Link Segmentuak Y-Junctionak erabiliz, causalitatearen arabera aurkitu y-junctionak denboran zear.
    
    bool to_first;
    bool flag_marked[2];
    
    bool next_point = 0;
    
    Network_list::iterator i_Y_tsp, i_Y_tsp_ref, i_Y_Ord;
    Point_list::iterator i_Y_point, i_Y_point_Ord, i_Y_ref;
    
    Point_list Y_temp;
    
    //Create Ordered Y list using first timestep (OK)
    for(int i=0; i<2; i++)
    {
        i_Y_tsp=link.YJunctionNet[i].begin();
        cout<<"Y Network-aren tamaina aurretik "<<link.YJunctionNet[i].size()<<endl;
        cout<<"Aurretik "<<i_Y_tsp->size()<<endl;
        
        for(i_Y_point=i_Y_tsp->begin(); i_Y_point!=i_Y_tsp->end(); i_Y_point++)
        {
            i_Y_point->m=0;
            
            Y_temp.push_back(*i_Y_point);
            
            //Remove Y's from old network
            i_Y_tsp->erase(i_Y_point);
            i_Y_point--;
            
            link.OrderedYJunctions[i].push_back(Y_temp);
            Y_temp.clear();
        }
        link.YJunctionNet[i].erase(i_Y_tsp);
        cout<<"Ondoren "<<link.OrderedYJunctions[i].size()<<" || "<<i_Y_tsp->size()<<endl;
        cout<<"YNetwork-aren tamaina ondoren "<<link.YJunctionNet[i].size()<<endl;
    }
    
    //Create Ordered list using first timestep (OK)
    for(int i=0; i<2; i++)
    {
        i_tsp=link.SegmentsNet[i].begin();
        cout<<"Network-aren tamaina aurretik "<<link.SegmentsNet[i].size()<<endl;
        cout<<"Aurretik "<<i_tsp->size()<<endl;

        for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
        {
            temp.p_list=*i_seg1;
            temp.time=0;
            
            OrderedSegments[i].push_back(temp);
            
            //Remove segments from old network
            i_tsp->erase(i_seg1);
            i_seg1--;
            
            link.OrderedNetwork[i].push_back(OrderedSegments[i]);
            OrderedSegments[i].clear();
        }
        link.SegmentsNet[i].erase(i_tsp);
        cout<<"Ondoren "<<link.OrderedNetwork[i].size()<<" || "<<i_tsp->size()<<endl;
        cout<<"Network-aren tamaina ondoren "<<link.SegmentsNet[i].size()<<endl;
    }
    
//bool first;

    Network::iterator i_tsp_ref;
    Network_time :: iterator i_SegID;
    Network_list:: iterator i_temp, i_last;
    Network_list_time :: iterator i_seg_Ord;
    Point_list::iterator i_point_Ord, i_point_Ord_Aux;
    Point_list temp_list,temp_list2;
    
    
    
    bool found_Y=0;
    Network_list::iterator i_seg_refY;
    Point Y_ref;
    int linked_Y=0;
    int t_step;
    int diff=0;
    int how_many=0;
    bool linked_seg=0;
    bool next_seg=false;
    bool next_Y=false;
    bool sobran[2];
    //Y-junction Linking (OK)
    for(int i=0; i<2; i++)
    {
        t_step=0;
        sobran[i]=0;
        
        //Sartu denbora baten
        for(i_Y_tsp=link.YJunctionNet[i].begin(); i_Y_tsp!=link.YJunctionNet[i].end(); i_Y_tsp++)
        {
            next_Y=false;
            
            if(sobran[i])
            {
                //Sobratu direnak, sartu lista ordenatuan baita ere
                //Honela hurrengo konaparaziorako jadanik hor egonen dira
                while(i_Y_tsp_ref->size()>0)
                {
                    for(i_Y_point=i_Y_tsp_ref->begin(); i_Y_point!=i_Y_tsp_ref->end(); i_Y_point++)
                    {
                        i_Y_point->m=t_step;
                        
                        Y_temp.push_back(*i_Y_point);
                        
                        //Remove Y's from old network
                        i_Y_tsp->erase(i_Y_point);
                        i_Y_point--;
                        
                        link.OrderedYJunctions[i].push_back(Y_temp);
                        Y_temp.clear();
                    }
                }
                sobran[i]=false;
            }
            t_step++;
            for(i_Y_point=i_Y_tsp->begin(); i_Y_point!=i_Y_tsp->end(); i_Y_point++)
            {
                if(next_Y)
                {
                    i_Y_tsp->erase(i_Y_ref);
                    next_Y=false;
                }
                for(i_Y_Ord=link.OrderedYJunctions[i].begin(); i_Y_Ord!=link.OrderedYJunctions[i].end(); i_Y_Ord++)
                {
                    i_Y_point_Ord=i_Y_Ord->begin(); //Beti lehenengoa, aurretik sartuko baitiogu lotutakoa.
                    
                    link.Y_current=*i_Y_point;
                    link.Y_next=*i_Y_point_Ord;
                    
                    if(link.CausalDist(1,0))
                    {
                        i_Y_point->m=t_step;
                        
                        i_Y_ref=i_Y_point;
                        i_Y_Ord->push_front(*i_Y_point);
                        
                        next_Y=true;
                        break;
                    }
                }
            }
            if(i_Y_tsp->size()>0)
            {
                i_Y_tsp_ref=i_Y_tsp;
                sobran[i]=true;
            }
        }
    }
    
    //Store left unlinked segments od the last timestep (OK)
    for(int i=0; i<2; i++)
    {
        t_step=0;
        for(i_Y_tsp=link.YJunctionNet[i].begin(); i_Y_tsp!=link.YJunctionNet[i].end(); i_Y_tsp++)
        {
            t_step++;
            
            while(i_Y_tsp->size()>0)
            {
                for(i_Y_point=i_Y_tsp->begin(); i_Y_point!=i_Y_tsp->end(); i_Y_point++)
                {
                    i_Y_point->m=t_step;
                    
                    Y_temp.push_back(*i_Y_point);
                    
                    //Remove Y's from old network
                    i_Y_tsp->erase(i_Y_point);
                    i_Y_point--;
                    
                    link.OrderedYJunctions[i].push_back(Y_temp);
                    Y_temp.clear();
                }
            }
        }
    }

    int denbora=0;

    //Uncomment for checks
    /*for(int i=0; i<2; i++)
    {
        cout<<i<<endl;
        cout<<"YJunctionNet-en tamaina "<<link.YJunctionNet[i].size()<<endl;
        
        for(i_Y_tsp=link.YJunctionNet[i].begin(); i_Y_tsp!=link.YJunctionNet[i].end(); i_Y_tsp++)
        {
            cout<<"Zenbat Y gelditu dira sobran? "<<i_Y_tsp->size()<<endl;
        }
        cout<<"OrderedNetwork-en tamaina "<<link.OrderedYJunctions[i].size()<<endl;
        for(i_Y_tsp=link.OrderedYJunctions[i].begin(); i_Y_tsp!=link.OrderedYJunctions[i].end(); i_Y_tsp++)
        {
            denbora=i_Y_tsp->begin()->m;
            cout<<"Aber hau... "<<i_Y_tsp->size()<<" azken denbora "<<denbora<<endl;
        }
    }*/
    //Segment Linking (OK)
    for(int i=0; i<2; i++)
    {
        t_step=0;
        sobran[i]=false;
        //Sartu denbora baten
        for(i_tsp=link.SegmentsNet[i].begin(); i_tsp!=link.SegmentsNet[i].end(); i_tsp++)
        {
            next_seg=false;
            t_step++;
            
            //Store new segments, after linking
            if(sobran[i])
            {
                //Sobratu direnak, sartu lista ordenatuan baita ere
                //Honela hurrengo konaparaziorako jadanik hor egonen dira
                while(i_tsp_ref->size()>0)
                {
                    for(i_seg1=i_tsp_ref->begin(); i_seg1!=i_tsp_ref->end(); i_seg1++)
                    {
                        temp.p_list=*i_seg1;
                        temp.time=t_step;
                        
                        OrderedSegments[i].push_back(temp);
                        
                        //Remove segments from old network
                        i_tsp_ref->erase(i_seg1);
                        i_seg1--;
                        
                        link.OrderedNetwork[i].push_back(OrderedSegments[i]);
                        OrderedSegments[i].clear();
                    }
                }
                sobran[i]=false;
            }
            
            
            count=0;
            //Link Segments
            for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
            {
                i_last=i_tsp->end();
                count++;

                linked_Y=0;
                if(next_seg)
                {
                    i_tsp->erase(i_temp);
                    next_seg=false;
                }
                
                //Segmentu baten eboluzioa hartu, aukeratutako segmentua hauekin konparatzeko
                for(i_SegID=link.OrderedNetwork[i].begin(); i_SegID!=link.OrderedNetwork[i].end(); i_SegID++)
                {
                    if(i_SegID->size()<=t_step)
                    {
                        if(next_seg){break;}
                        //Eboluzioan lehenengoa hartu, azken timeStepean sartutakoa izanen baita
                        i_seg_Ord=i_SegID->begin();
                        
                        //Puntuz puntu, hartu Y-junction-a
                        for(i_point=i_seg1->begin(); i_point!=i_seg1->end(); i_point++)
                        {
                            if(next_seg){break;}
                            //Y-Junction-a hartuta
                            if(i_point->m==8)
                            {
                                //Puntuz puntu konparatu Ordenatutakoaren puntuekin
                                temp_list=i_seg_Ord->p_list;
                                //cout<<"EEEOOO "<<i_seg_Ord->time<<endl;
                                for(i_point_Ord=temp_list.begin(); i_point_Ord!=temp_list.end(); i_point_Ord++)
                                {
                                    //Hartu Y-Junction-a
                                    if(i_point_Ord->m==8)
                                    {
                                        //Ikusi ea kausalki lotuta dauden
                                        link.Y_current=*i_point;
                                        link.Y_next=*i_point_Ord;
                                        //cout<<"Current: "<<link.Y_current.x<<" "<<link.Y_current.y<<" "<<link.Y_current.z<<endl;
                                        //cout<<"Next: "<<link.Y_next.x<<" "<<link.Y_next.y<<" "<<link.Y_next.z<<endl;
                                        //cout<<"zenbat "<<linked_Y<<endl;
                                        //And now check if it is causaly connected
                                        //error=2---> to settings!
                                        if(link.CausalDist(1,2))
                                        {
                                            linked_Y++;
                                            
                                            temp.p_list=*i_seg1;
                                            temp.time=t_step;
                                            
                                            i_SegID->push_front(temp);
                                            
                                            i_temp=i_seg1;
                                            i_last--;
                                            if(i_seg1==i_last)
                                            {
                                                i_tsp->erase(i_temp);
                                            }
                                            next_seg=true;
                                            break;

                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(i_tsp->size()>0)
            {
                i_tsp_ref=i_tsp;
                sobran[i]=true;
            }
        }
    }

    //Store left unlinked segments od the last timestep (OK)
    for(int i=0; i<2; i++)
    {
        t_step=0;
        for(i_tsp=link.SegmentsNet[i].begin(); i_tsp!=link.SegmentsNet[i].end(); i_tsp++)
        {
            t_step++;
            
            while(i_tsp->size()>0)
            {
                for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
                {
                    temp.p_list=*i_seg1;
                    temp.time=t_step;
                    
                    OrderedSegments[i].push_back(temp);
                    
                    //Remove segments from old network
                    i_tsp->erase(i_seg1);
                    i_seg1--;
                    
                    link.OrderedNetwork[i].push_back(OrderedSegments[i]);
                    OrderedSegments[i].clear();
                }
            }
        }
    }

    
    //Pruebatako, uncomment to check
    /*for(int i=0; i<2; i++)
    {
        cout<<i<<endl;
        cout<<"SegmentsNet-en tamaina "<<link.SegmentsNet[i].size()<<endl;

        for(i_tsp=link.SegmentsNet[i].begin(); i_tsp!=link.SegmentsNet[i].end(); i_tsp++)
        {
            cout<<"Zenbat segmentu gelditu dira sobran? "<<i_tsp->size()<<endl;
        }
        cout<<"OrderedNetwork-en tamaina "<<link.OrderedNetwork[i].size()<<endl;
        for(i_SegID=link.OrderedNetwork[i].begin(); i_SegID!=link.OrderedNetwork[i].end(); i_SegID++)
        {
            denbora=i_SegID->begin()->time;
            //cout<<"Aber hau... "<<i_SegID->size()<<" azken denbora "<<denbora<<endl;
        }
    }*/
    
    float v=0;
    float v2=0;
    float v_mean=0;
    float v2_mean=0;
    Point_list::iterator i_last_point, i_prelast_point;
    Point_list::iterator i_Y_ref2;
    int Y_took=0;
    
    //Get Y-junction velocity
    for(int i=0; i<2; i++)
    {
        v=0;
        v2=0;
        Y_took=0; //How many Y's are considered in order to calculate mean velocities
        for(i_Y_Ord=link.OrderedYJunctions[i].begin(); i_Y_Ord!=link.OrderedYJunctions[i].end(); i_Y_Ord++)
        {
            v_mean=0;
            v2_mean=0;
            i_last_point=i_Y_Ord->end();
            i_last_point--;
            
            i_prelast_point=i_Y_Ord->end();
            i_prelast_point--;
            i_prelast_point--;

            if(i_Y_Ord->size()>6)
            {
                Y_took++;
                //cout<<"---------------------"<<endl;
                //cout<<"Eboluzioaren tamaina "<<i_Y_Ord->size()<<endl;
                for(i_Y_point_Ord=i_Y_Ord->begin(); i_Y_point_Ord!=i_Y_Ord->end(); i_Y_point_Ord++)
                {
                    if(i_Y_point_Ord==i_last_point){break;}
                    i_Y_ref=i_Y_point_Ord;
                    i_Y_ref++;
                    
                    link.Y_current=*i_Y_point_Ord;
                    link.Y_next=*i_Y_ref;
                    
                    /*if (i==0)
                    {
                        cout<<"Current x: "<<link.Y_current.x<<" "<<link.Y_current.y<<" "<<link.Y_current.z<<" "<<link.Y_current.m<<endl;
                        cout<<"Next x: "<<link.Y_next.x<<" "<<link.Y_next.y<<" "<<link.Y_next.z<<" "<<link.Y_next.m<<endl;
                        cout<<"Next x: "<<i_Y_ref->x<<" "<<i_Y_ref->y<<" "<<i_Y_ref->z<<" "<<i_Y_ref->m<<endl;
                    }*/
                    
                    
                    v_mean += link.Velocity();
                    
                    //cout<<"Ea ze abiadurak ematen ditton... "<<link.Velocity()<<endl;
                }
                v_mean /= (i_Y_Ord->size()-1);
                v += v_mean;
                //cout<<"Mean abiadura "<<v_mean<<endl;
                for(i_Y_point_Ord=i_Y_Ord->begin(); i_Y_point_Ord!=i_Y_Ord->end(); i_Y_point_Ord++)
                {
                    if(i_Y_point_Ord==i_prelast_point){break;}
                    i_Y_ref2=i_Y_point_Ord;
                    i_Y_ref2++;
                    i_Y_ref2++;
                    
                    link.Y_current=*i_Y_point_Ord;
                    link.Y_next=*i_Y_ref2;
                    
                    if (i==0)
                    {
                        cout<<"Current x: "<<link.Y_current.x<<" "<<link.Y_current.y<<" "<<link.Y_current.z<<" "<<link.Y_current.m<<endl;
                        cout<<"Next x: "<<link.Y_next.x<<" "<<link.Y_next.y<<" "<<link.Y_next.z<<" "<<link.Y_next.m<<endl;
                    }
                    
                    v2_mean += link.Velocity();
                    
                    //cout<<"Ea ze abiadurak ematen ditton... "<<link.Velocity()<<endl;
                }
                v2_mean /= (i_Y_Ord->size()-2);
                v2 += v2_mean;
            }
        }
        v /= Y_took;
        v2 /= Y_took;
        cout<<"########"<<endl;
        cout<<"Y-junction Velocity, 1 step: "<<v<<" 2 steps "<<v2<<endl;
        cout<<"########"<<endl;

    }
}

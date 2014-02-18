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
                    cout<<"Plane found... "<<endl;
                    
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
            link.YJunctionOutput(c);
        }
    }
    
    Network_list_time OrderedSegments[2];
    
    Network_list:: iterator i_seg1;
    List_time temp;
    
    
    int tms = 0;
    
    //Link Segmentuak Y-Junctionak erabiliz, causalitatearen arabera aurkitu y-junctionak denboran zear.
    
    bool to_first;
    bool flag_marked[2];
    
    bool next_point = 0;
    
    
    //Lehenengoa sartu ordered listan.

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
    
    //Create empty list
    link.CreateEmptyList();
    
//bool first;

    Network_time :: iterator i_SegID;
    Network_list_time :: iterator i_seg_Ord;
    Point_list::iterator i_point_Ord, i_point_Ord_Aux;
    Point_list temp_list;
    
    
    
    bool found_Y=0;
    Network_list::iterator i_seg_refY;
    Point Y_ref;
    int linked_Y=0;
    int t_step;
    int diff=0;
    int how_many=0;
    bool linked_seg=0;
    bool next_seg=false;

    for(int i=0; i<2; i++)
    {
        t_step=0;
        //Sartu denbora baten
        for(i_tsp=link.SegmentsNet[i].begin(); i_tsp!=link.SegmentsNet[i].end(); i_tsp++)
        {
            t_step++;
            //Hartu denbora honetako segmentu bat
            for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
            {
                //Gerorako
                //i_seg1=i_tsp->begin();
                linked_Y=0;
                next_seg=false;
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
                                            
                                            cout<<"Aurkittuta "<<endl;
                                            
                                            temp.p_list=*i_seg1;
                                            temp.time=t_step;
                                            
                                            i_SegID->push_front(temp);
                                            
                                            i_tsp->erase(i_seg1);
                                            if(i_seg1!=i_tsp->begin())
                                            {
                                                i_seg1--;
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
                /*if (linked_Y==0)
                {
                    //Inorrekin lotu ez dugun segmentua sartu berri modura. Aurretik lista hutsak sartu, tms-aren arabera.
                    temp.p_list=*i_seg1;
                    temp.time=t_step;
                    
                    i_SegID->push_front(temp);
                    cout<<"Fijo hemen dela,"<<endl;
                    //i_tsp->erase(i_seg1);
                    cout<<"ez?"<<endl;
                    if(i_seg1!=i_tsp->begin())
                    {
                        i_seg1--;
                    }
                }*/
            }
        }
    }

    cout<<"How many "<<t_step<<endl;
    //Pruebatako
    int denbora=0;
    for(int i=0; i<2; i++)
    {
        cout<<"SegmentsNet-en tamaina "<<link.SegmentsNet[i].size()<<endl;

        for(i_tsp=link.SegmentsNet[i].begin(); i_tsp!=link.SegmentsNet[i].end(); i_tsp++)
        {
            cout<<"Zenbat segmentu gelditu dira sobran? "<<i_tsp->size()<<endl;
        }
        cout<<"OrderedNetwork-en tamaina "<<link.OrderedNetwork[i].size()<<endl;
        for(i_SegID=link.OrderedNetwork[i].begin(); i_SegID!=link.OrderedNetwork[i].end(); i_SegID++)
        {
            denbora=i_SegID->begin()->time;
            cout<<"Aber hau... "<<i_SegID->size()<<" azken denbora "<<denbora<<endl;
        }
    }
    

        
        /*for(i_SegID=link.OrderedNetwork[i].begin(); i_SegID!=link.OrderedNetwork[i].end(); i_SegID++)
        {
            for(i_seg_Ord=i_SegID->begin(); i_seg_Ord!=i_SegID->end(); i_seg_Ord++)
            {
                /*linked_Y=0;
                linked_seg=0;
                found_Y=0;
                go_to_seg=0;*/
                //cout<<"Ordered segmentuen luzera "<<i_seg_Ord->size()<<endl;
               /* for(i_point_Ord=i_seg_Ord->begin(); i_point_Ord!=i_seg_Ord->end(); i_point_Ord++)
                {
                    /*if(linked_seg==1)
                    {
                        break;
                    }
                    if(found_Y==1)
                    {
                        go_to_seg=1;
                    }*/
                    //Take Y-junction
                  /*  if(i_point_Ord->m==8)
                    {
                        //And look where it is in the next timestep
                        i_tsp=link.SegmentsNet[i].begin();
                        for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
                        {
                            /*if(linked_seg==1)
                            {
                                break;
                            }
                            if(go_to_seg==1)
                            {
                                i_seg1=i_seg_refY;
                                
                            }
                            else if(found_Y==1)
                            {
                                break;
                            }*/
                            //cout<<"Segmentuen luzera "<<i_seg1->size()<<endl;
                         /*   for(i_point=i_seg1->begin(); i_point!=i_seg1->end(); i_point++)
                            {
                                if(i_point->m==8)
                                {
                                    link.Y_current=*i_point_Ord;
                                    link.Y_next=*i_point;
                                    /*cout<<"Current: "<<link.Y_current.x<<" "<<link.Y_current.y<<" "<<link.Y_current.z<<endl;
                                    cout<<"Next: "<<link.Y_next.x<<" "<<link.Y_next.y<<" "<<link.Y_next.z<<endl;
                                    cout<<"zenbat "<<linked_Y<<endl;*/
                                    //And now check if it is causaly connected
                                    //error=2---> to settings!
                               /*     if(link.CausalDist(1,2))
                                    {
                                        linked_Y++;
                                        cout<<"Aurkittu do causally connected! "<<endl;
                                        
                                        
                                        
                                        //Lehenengoa aurkittutakoak, gorde segmentuaren referentzia
                                        /*if(found_Y==0)
                                        {
                                            i_seg_refY = i_seg1;
                                            found_Y=1;
                                            break;
                                        }
                                        if(linked_Y==2)
                                        {
                                            linked_seg=1;
                                            break;
                                        }*/
                                        
                                        //Go to the next Y in the segment
                                        //i_point_Ord_Aux=i_point_Ord;
                                        //i_point_Ord_Aux++;
                                        /*for(i_point_Ord_Aux=i_point_Ord;i_point_Ord_Aux!=i_seg1->end(); i_point_Ord_Aux++)
                                        {
                                            if(i_point_Ord_Aux->m==8)
                                            {
                                                cout<<"Hurrengoa aurkituta! "<<endl;
                                                cout<<i_point_Ord_Aux->x<<" "<<i_point_Ord_Aux->y<<" "<<i_point_Ord_Aux->z<<endl;
                                            }
                                        }
                                  //  }
                                    else if(found_Y==1 && linked_Y==1)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }*/
    /*for(int i=0; i<2; i++)
    {
        for(i_tsp=SegmentsNet[i].begin(); i_tsp!=SegmentsNet[i].end(); i_tsp++)
        {
            for(i_seg1=i_tsp->begin(); i_seg1!=i_tsp->end(); i_seg1++)
            {
                //next_seg=0;
                //timestep honetako segmentua hartua
                //first=1;
                
                for(i_SegID=OrderedNetwork[i].begin(); i_SegID!=OrderedNetwork[i].end(); i_SegID++)
                {
                    /*if(next_seg)
                    {
                        next_seg=0;
                        break;
                    }*/

                    //Beti listako lehenengoa izanen da, aztertutako azkeneko timestepari dagokiona, push front egin baitugu.
                    
                    /*i_seg_Ord=i_SegID->begin();

                    //ID honi dagokion azkeneko timestepeko "bertsioa"
                    points_Common=0;
                    
					
					//hartu puntu bat
                    for(i_point=i_seg1->begin(); i_point!=i_seg1->end(); i_point++)
                    {
                        /*if(next_seg)
                        {
                            break;
                        }*/
						
						//Konparatu ordenatutako guztiekin
                        /*for(i_point_Ord=i_seg_Ord->begin(); i_point_Ord!=i_seg_Ord->end(); i_point_Ord++)
                        {
                            if(next_point || next_seg)
                            {
                                next_point=0;
                                break;
                            }
                            
                            if(*i_point==*i_point_Ord)
                            {
                                points_Common++;
                                
								//Hurrengo puntua hartu
                                next_point=1;
                                
                                if(points_Common > CommonPoints)
                                {
                                    next_point=0;
                                    //Beti listaren hasierara gehituko dugu, honela errexagoa izanen baita ondoz ondoko timestep-ak konparatzea
                                    i_SegID->push_front(*i_seg1);
                                    i_tsp->erase(i_seg1);
									
                                    next_seg=1;
                                }
                            }
                        }
                    }
                }
                //Punturik komunean ez dutenak edo eskatutakoa baino gutxiago
                if(points_Common==0 || points_Common < CommonPoints)
                {                    
                    OrderedSegments[i].push_back(*i_seg1);
                    i_tsp->erase(i_seg1);
                    i_seg1--;
                    OrderedNetwork[i].push_back(OrderedSegments[i]);
                    OrderedSegments[i].clear();
                }
            }
        }
        cout<<"Number of timesteps: "<<SegmentsNet[i].size()<<"  ||  Found segments "<<OrderedNetwork[i].size()<<endl;
    }
	
	cout<<"Checking..."<<endl;
	
	Network::iterator i_SegID_next;

	
    //PROBATZEKO
    
    for(int i=0; i<2; i++)
    {
        for(i_SegID=OrderedNetwork[i].begin(); i_SegID!=OrderedNetwork[i].end();i_SegID++)
        {
            cout<<i<<" Ordenatuetan ze tamaina du ID bakoitzak? "<<i_SegID->size()<<endl;
        }
        for(i_tsp=SegmentsNet[i].begin(); i_tsp!=SegmentsNet[i].end(); i_tsp++)
        {
            cout<<i<<" TimeStep bakoitzean segmentuak "<<i_tsp->size()<<endl;
        }
    }*/
	
	/*int Y_ID[2];
	int kk=0;
	
	Point_list YJunctions[2][2];
	Point_list::iterator i_Yref;
	Point Y_ref;
	
	bool firstL, firstR;

	
	for(int i=0; i<2; i++)
	{
		kk=0;
		firstL=firstR=1;
		for(i_SegID=OrderedNetwork[i].begin(); i_SegID!=OrderedNetwork[i].end();i_SegID++)
        {
			kk++;
			for(i_seg_Ord=i_SegID->begin(); i_seg_Ord!=i_SegID->end(); i_seg_Ord++)
			{
				Y_ID[i]=0;
				for(i_point_Ord=i_seg_Ord->begin(); i_point_Ord!=i_seg_Ord->end(); i_point_Ord++)
				{
					if(i_point_Ord->m==8)
					{
						Y_ID[i]++;
						
						//cout<<"Y-junction! "<<Y_ID[i]<<endl;
						
						if(kk==3)
						{
							cout<<"Coords: "<<i_point_Ord->x<<" "<<i_point_Ord->y<<" "<<i_point_Ord->z<<endl;
							i_point_Ord;
							if(!firstL && !firstR)
							{
								for(int c=0; c<2; c++)
								{
									//for(i_Yref=YJunctions[c].begin(); i_Yref!=YJunctions[c].end(); i_Yref++)
									//{
									Y_ref=YJunctions[i][c].back();
										if(near(*i_point_Ord,Y_ref,256,40))//DistYjunc)
										{
											YJunctions[i][c].push_back(*i_point_Ord);
											break;
										}
									//}
								}
							}
							if(!firstL && firstR)
							{
								cout<<"Bigarrena barrena"<<endl;
								YJunctions[i][1].push_back(*i_point_Ord);
								firstR=0;
							}
							if(firstL && firstR)
							{
								cout<<"Lehenengoa sartuta"<<endl;
								YJunctions[i][0].push_back(*i_point_Ord);
								firstL=0;
							}

							
						}
					}
				}
			}
		}
	}
    
	
	fstream fileYVelocity[2][2];
    string nameYVelocity[2][2];
	Point Yjunc;
	Point_list::iterator i_Y;
	float v;
	int NumY[2];
	
	nameYVelocity[0][0] = filename_root+output_path+"YvelocityAFront.dat";//_"+int2string(ID[i])+".dat";
	nameYVelocity[0][1] = filename_root+output_path+"YvelocityABack.dat";//_"+int2string(ID[i])+".dat";
	nameYVelocity[1][0] = filename_root+output_path+"YvelocityBFront.dat";//_"+int2string(ID[i])+".dat";
	nameYVelocity[1][1] = filename_root+output_path+"YvelocityBBack.dat";//_"+int2string(ID[i])+".dat";

	
	for(int i=0; i<2; i++)
	{
		for(int c=0; c<2; c++)
		{
			NumY[c]=0;
			
			fileYVelocity[i][c].open( nameYVelocity[i][c].c_str(), fstream::out );

			for(i_Y=YJunctions[i][c].begin(); i_Y!=YJunctions[i][c].end(); i_Y++)
			{
				NumY[c]++;
				if(NumY[c]==YJunctions[i][c].size())
				{
					break;
				}
				i_Yref = i_Y;
				i_Yref++;
				
				cout<<i_Yref->x<<" "<<i_Yref->y<<" "<<i_Yref->z<<endl;
				cout<<i_Y->x<<" "<<i_Y->y<<" "<<i_Y->z<<endl;
				
				v=velocity(*i_Yref,*i_Y,0.5,step);
				
				fileYVelocity[i][c]<<v<<endl;
				
				cout<<"Velozidadea "<<v<<endl;
				
				//cout<<i_Yref->x<<" "<<i_Yref->y<<" "<<i_Yref->z<<endl;
			}
			
			fileYVelocity[i][c].close();
		}
	}
		
	
	
	
	//////////////////////////////
    ////////////SAVE//////////////
    //////////////////////////////
    int ID[2];
    int SegID;
    
    fstream fileOrSegOutput[2];
    string nameOrSegOutput[2];

    
    for(int i=0; i<2; i++)
    {
        SegID=0;
        for(i_SegID=OrderedNetwork[i].begin(); i_SegID!=OrderedNetwork[i].end();i_SegID++)
        {
            SegID++;
            ID[i]=SegID;
            if(i==0)
            {
                nameOrSegOutput[i] = filename_root+output_path+"segmentsOrA_"+int2string(ID[i])+".dat";
            }
            else if(i==1)
            {
                nameOrSegOutput[i] = filename_root+output_path+"segmentsOrB_"+int2string(ID[i])+".dat";
            }
            
            fileOrSegOutput[i].open( nameOrSegOutput[i].c_str(), fstream::out );
            
            //How many timeSteps
            fileOrSegOutput[i]<<i_SegID->size()<<endl;
            
            for(i_seg_Ord=i_SegID->begin(); i_seg_Ord!=i_SegID->end(); i_seg_Ord++)
            {
                //How many points in each segment
                fileOrSegOutput[i]<<i_seg_Ord->size()<<endl;
            }
            for(i_seg_Ord=i_SegID->begin(); i_seg_Ord!=i_SegID->end(); i_seg_Ord++)
            {
                for(i_point_Ord=i_seg_Ord->begin(); i_point_Ord!=i_seg_Ord->end(); i_point_Ord++)
                {
                    //Segments points
                    fileOrSegOutput[i]<<i_point_Ord->x<<" "<<i_point_Ord->y<<" "<<i_point_Ord->z<<endl;
                }
            }
            fileOrSegOutput[i].close();
            
        }
    }*/
    
}
/*void YJunctionOutput(int c)
{
    fstream fileYjuncOutput;
    string nameYjuncOutput;
    if(c==0)
    {
        nameYjuncOutput = filename_root+output_path+"YjunctionsA_"+snapshot+".dat";
    }
    else
    {
        nameYjuncOutput = filename_root+output_path+"YjunctionsB_"+snapshot+".dat";
    }
    
    fileYjuncOutput.open( nameYjuncOutput].c_str(), fstream::out );
    
    for(i_point=Yjunction.begin();i_point!=Yjunction.end();++i_point)
    {
        fileYjuncOutput<<i_point->x<<" "<<i_point->y<<" "<<i_point->z<<" "<<i_point->m<<endl;
    }
    fileYjuncOutput.close();
    
}*/
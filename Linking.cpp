//
//  Linking.cpp
//  
//
//  Created by Joanes on 04/02/14.
//
//

#include "Linking.hpp"

void Linking::CleanSegments()
{
    for(int c=0; c<2; c++)
    {
        for(i_tms_glob=SegmentsNet[c].begin(); i_tms_glob!=SegmentsNet[c].end(); i_tms_glob++)
        {
            for(i_net=i_tms_glob->begin(); i_net!=i_tms_glob->end(); i_net++)
            {
                for(i_p=i_net->begin(); i_p!=i_net->end(); i_p++)
                {
                    i_p_Aux=i_p;
                    i_p_Aux++;
                    for(; i_p_Aux!=i_net->end(); i_p_Aux++)
                    {
                        if(same(*i_p,*i_p_Aux))
                        {
                            i_net->erase(i_p_Aux);
                        }
                    }
                }
            }
        }
    }
}

void Linking::YJunctionOutput(int c)
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
    
    fileYjuncOutput.open( nameYjuncOutput.c_str(), fstream::out );
    
    for(i_p=YJunction.begin();i_p!=YJunction.end();++i_p)
    {
        fileYjuncOutput<<i_p->x<<" "<<i_p->y<<" "<<i_p->z<<" "<<i_p->m<<endl;
    }
    fileYjuncOutput.close();
}

bool Linking::CausalDist(int looking_at, float error)
{
    float dx=0.5;
    float dt=dx*0.2;
    float MaxDist=0;
    int d_x, d_y, d_z;
    float dist=0;
    
    //Distance travelled by a photon in a step, in lattice units
    MaxDist = (step/10) * dt * looking_at;
    MaxDist /= dx;
	
    d_x = Y_current.x - Y_next.x;
    d_y = Y_current.y - Y_next.y;
    d_z = Y_current.z - Y_next.z;
    
    if(abs(d_x) > lattice_size-2)
    {
        d_x=1;
    }
    if(abs(d_y) > lattice_size-2)
    {
        d_y=1;
    }
    if(abs(d_z) > lattice_size-2)
    {
        d_z=1;
    }
    
    dist = sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
    
    if( dist <= MaxDist + error)
    {
        return true;
    }
    else{return false;}
    
}
float Linking::Velocity()
{
    float dx=0.5;
    float dt=dx*0.2;
    float MaxDist=0;
    int d_x, d_y, d_z;
    int Y_step=0;
    float dist=0;
    float vel=0;
    float d_t=0;
    
    Y_step = Y_current.m - Y_next.m;
    
    d_t=Y_step * dt * (step/10);
    
    d_x = Y_current.x - Y_next.x;
    d_y = Y_current.y - Y_next.y;
    d_z = Y_current.z - Y_next.z;
    
    if(abs(d_x) >= lattice_size-2)
    {
        d_x=d_x%lattice_size;
    }
    if(abs(d_y) >= lattice_size-2)
    {
        d_y=d_x%lattice_size;
    }
    if(abs(d_z) >= lattice_size-2)
    {
        d_z=d_x%lattice_size;
    }
    
    dist = sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
    dist *= dx;
    
    vel=dist/d_t;
    
    return vel;
    
}
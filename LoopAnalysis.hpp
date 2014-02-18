//
//  LoopAnalysis.hpp
//  
//
//  Created by Joanes on 15/10/13.
//
//

#include <list>
#include <math.h>
#include <cmath>

#ifndef _LoopAnalysis_hpp
#define _LoopAnalysis_hpp

struct Point{
    unsigned short x;
    unsigned short y;
    unsigned short z;
    short m;
};


bool operator==(const Point& p1,const Point& p2)
{
    return p1.x==p2.x && p1.y==p2.y && p1.z==p2.z;
}

typedef list<Point> Point_list;
typedef list<Point_list> Network_list;
typedef list<Network_list> Network;

struct List_time{
    Point_list p_list;
    int time;
};

typedef list<List_time> Network_list_time;
typedef list<Network_list_time> Network_time;

bool near(const Point& p1, const Point& p2, const int& size, const int& distMax)
{
    int d_x, d_y, d_z;
    float dist;
	
    d_x = p1.x - p2.x;
    d_y = p1.y - p2.y;
    d_z = p1.z - p2.z;
    
    if(abs(d_x) > size-2)
    {
        d_x=1;
    }
    if(abs(d_y) > size-2)
    {
        d_y=1;
    }
    if(abs(d_z) > size-2)
    {
        d_z=1;
    }
    
    dist = sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
    
    if( dist*dist <= distMax)
    {
        if( dist == 0){return false;}
        else{return true;}
    }
    else{return false;}

}

//Find previous or next nearest point
bool nearRight(const Point& p1, const Point& p2, const int& size, const int& distMax)
{
    int d_x, d_y, d_z;
    float dist;
    
    d_x = p1.x - p2.x;
    d_y = p1.y - p2.y;
    d_z = p1.z - p2.z;
    
    if(abs(d_x) > size-2)
    {
        d_x=1;
    }
    if(abs(d_y) > size-2)
    {
        d_y=1;
    }
    if(abs(d_z) > size-2)
    {
        d_z=1;
    }
    dist = sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
    
    if( dist*dist <= distMax)
    {
        if(d_x < 0)
        {
            return true;
        }
        else if(d_x == 0 && d_y < 0)
        {
            return true;
        }
        else if(d_x == 0 && d_y == 0 && d_z < 0)
        {
            return true;
        }
        else {return false;}
    }
    else if( dist == 0){return false;}
    else {return false;}
    
}

bool nearLeft(const Point& p1, const Point& p2, const int& size, const int& distMax)
{
    int d_x, d_y, d_z;
    float dist;
    
    d_x = p1.x - p2.x;
    d_y = p1.y - p2.y;
    d_z = p1.z - p2.z;
    
    if(abs(d_x) > size-2)
    {
        d_x=1;
    }
    if(abs(d_y) > size-2)
    {
        d_y=1;
    }
    if(abs(d_z) > size-2)
    {
        d_z=1;
    }
    dist = sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
    
    if( dist*dist <= distMax)
    {
        if(d_x > 0)
        {
            return true;
        }
        else if(d_x == 0 && d_y > 0)
        {
            return true;
        }
        else if(d_x == 0 && d_y == 0 && d_z > 0)
        {
            return true;
        }
        else {return false;}
    }
    else if( dist == 0){return false;}
    else {return false;}
	
}

int dist(const Point& p1, const Point& p2, const int& size)
{
    int d_x, d_y, d_z;
    float dist;
    
    d_x = p1.x - p2.x;
    d_y = p1.y - p2.y;
    d_z = p1.z - p2.z;
    
    if(abs(d_x) > size-2)
    {
        d_x=1;
    }
    if(abs(d_y) > size-2)
    {
        d_y=1;
    }
    if(abs(d_z) > size-2)
    {
        d_z=1;
    }
    return sqrt(d_x*d_x + d_y*d_y + d_z*d_z);
}

//Used to remove non-AB points from loops
bool nonmarked(const Point& p)
{
    if(p.m>=1){return false;}
    else{return true;}
}

//Used to remove AB points from loops
bool marked(const Point& p)
{
    if(p.m>=1){return true;}
    else{return false;}
}

//Used to remove duplicate points
bool same(const Point& p1, const Point& p2)
{
    if(p1==p2){return true;}
    else{return false;}
}


#endif

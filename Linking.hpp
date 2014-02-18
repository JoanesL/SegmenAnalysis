//
//  Linking.hpp
//  
//
//  Created by Joanes on 04/02/14.
//
//

#ifndef _Linking_hpp
#define _Linking_hpp

class Linking
{
public:
    
    Network SegmentsNet[2];
    
    Network_time OrderedNetwork[2];
    
    Network::iterator i_tms_glob;
    Network_list::iterator i_net;
    
    Point_list::iterator i_p, i_p_Aux;
    Point Y_current, Y_next;
    
    Point Empty;
    
    Point_list YJunction;
    
    Network_list YJunctionNet[2];
    Network_list OrderedYJunctions[2];
    
    Point_list EmptyList;
    
    void CleanSegments();
    void YJunctionOutput(int c);
    bool CausalDist(int looking_at, float error);
    
    float Velocity();
};

#endif

//
//  Inputs.hpp
//  
//
//  Created by Joanes on 15/10/13.
//
//

#ifndef _Inputs_hpp
#define _Inputs_hpp

string fileSettings;

string filename_root;
string filename_root_loops;
string output_path;
string type;
string snapshot;
int ascii;
int first, last, step, id;
int lattice_size;
int number=0;
bool bin;

int block_size=8*sizeof(short);
int block_sizeAB=8*sizeof(short);


bool noABSegments; //Find segments with no AB points
bool BigGap; //Repaint big GAPs
bool Normal_GAP; //Fill normal GAPs
bool Crossings_Before_GAPs; //Remove crossings before GAP filling
bool Detect_Y_Junctions;

int SegLength;  //Minimum legth for AB segment
int GAPlength;

int DistSame;  //For nearest points in the same type string
int DistAB; //For nearest points in opposite strings, i.e 2->Plaquette distance

bool outputYJunctions;
bool outputReconstructed;

int CommonPoints;

#endif

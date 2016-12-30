//
//  objload.h
//  showobj
//
//  Created by styleluo on 16/12/21.
//  Copyright © 2016年 styleluo. All rights reserved.
//

#ifndef objload_h
#define objload_h
class ObjModel
{
public:
    int vertices;
    float * positions;
    float * texels;
    float * normals;
    float * verticesIndex;
    float * texelsIndex;
    float * normaIndex;
    
    int materials;
    int * firsts;
    int * counts;
    
    float ** diffuses;
    float ** speculars;
    char ** textures;
    ObjModel()
    {
        vertices = 0;
        positions = NULL;
        texels = NULL;
        normals = NULL;
        materials =0;
        firsts = NULL;
        counts = NULL;
        diffuses = NULL;
        speculars = NULL;
    }
    void release()
    {
        
        vertices = 0;
        if(positions)
            delete positions;
        positions = NULL;
        if(texels)
            delete texels;
        texels = NULL;
        if(normals)
            delete normals;
        normals = NULL;
        materials =0;
        if(firsts)
            delete firsts;
        firsts = NULL;
        if(counts)
            delete counts;
        counts = NULL;
        if(diffuses)
            delete []diffuses;
        diffuses = NULL;
        if(speculars)
            delete []speculars;
        speculars = NULL;
        if(textures)
            delete textures;
        textures = NULL;
    }
    ~ObjModel()
    {
        release();
    }
};
int loadObj(const char * filepathOBJ,const char  *filepathMTL,ObjModel& objmodel);

#endif /* objload_h */

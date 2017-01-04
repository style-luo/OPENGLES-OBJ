//
//  main.cpp
//  blender2opengles
//
//  Created by RRC on 8/12/13.
//  Copyright (c) 2013 Ricardo Rendon Cepeda. All rights reserved.
//

// C++ Standard Library
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "objload.h"
using namespace std;

// Model Structure
typedef struct Model
{
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
    int materials;
}
Model;

Model getOBJinfo(string fp)
{
    Model model = {0};
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.good())
    {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while(!inOBJ.eof())
    {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        if(type.compare("v ") == 0)
            model.positions++;
        else if(type.compare("vt") == 0)
            model.texels++;
        else if(type.compare("vn") == 0)
            model.normals++;
        else if(type.compare("f ") == 0)
            model.faces++;
    }
    
    model.vertices = model.faces*3;
    
    // Close OBJ file
    inOBJ.close();
    
    return model;
}

int  extractOBJdata(string fp, vector< vector<float>>& positions, vector< vector<float>>&  texels, vector< vector<float>>&  normals, vector< vector<float>>&  faces, vector<string>& materials, int m)
{
    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int f = 0;
    int nodenum=0;
    // Index
    int mtl = 0;
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.good())
    {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while(!inOBJ.eof())
    {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        // Material
        if(type.compare("us") == 0)
        {
            // Extract token
            string l = "usemtl ";
            string material = line.substr(l.size());
            
            for(int i=0; i<m; i++)
            {
                if(material.compare(materials[i]) == 0)
                    mtl = i;
            }
        }
        
        // Positions
        else if(type.compare("v ") == 0)
        {
            // Copy line for parsing
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for(int i=0; i<3; i++)
                positions[p][i] = atof(strtok(NULL, " "));
            
            // Wrap up
            delete[] l;
            p++;
        }
        
        // Texels
        else if(type.compare("vt") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<2; i++)
            {
                texels[t][i] = atof(strtok(NULL, " "));
            }
            
            delete[] l;
            t++;
        }
        
        // Normals
        else if(type.compare("vn") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<3; i++)
                normals[n][i] = atof(strtok(NULL, " "));
            delete[] l;
            n++;
        }
        
        // Faces
        else if(type.compare("f ") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            char * p;
            while((p = strtok(NULL, " /")))
            {
                faces[f].push_back(atof(p));
                if(faces[f].size()>10)
                    nodenum ++;
            }
            
            // Append material
            faces[f][0] = mtl;
            nodenum += 3;
            
            delete[] l;
            f++;
        }
    }
    
    // Close OBJ file
    inOBJ.close();
    return nodenum;
}

void writeH(string fp, string name, Model model)
{
    // Create H file
    ofstream outH;
    outH.open(fp);
    if(!outH.good())
    {
        cout << "ERROR CREATING H FILE" << endl;
        exit(1);
    }
    
    // Write to H file
    outH << "// This is a .h file for the model: " << name << endl;
    outH << endl;
    
    // Write statistics
    outH << "// Positions: " << model.positions << endl;
    outH << "// Texels: " << model.texels << endl;
    outH << "// Normals: " << model.normals << endl;
    outH << "// Faces: " << model.faces << endl;
    outH << "// Vertices: " << model.vertices << endl;
    outH << "// Materials: " << model.materials << endl;
    outH << endl;
    
    // Write declarations
    outH << "const int " << name << "Vertices;" << endl;
    outH << "const float " << name << "Positions[" << model.vertices*3 << "];" << endl;
    outH << "const float " << name << "Texels[" << model.vertices*2 << "];" << endl;
    outH << "const float " << name << "Normals[" << model.vertices*3 << "];" << endl;
    outH << endl;
    
    outH << "const int " << name << "Materials;" << endl;
    outH << "const int " << name << "Firsts[" << model.materials << "];" << endl;
    outH << "const int " << name << "Counts[" << model.materials << "];" << endl;
    outH << endl;
    
    outH << "const float " << name << "Diffuses[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << "const float " << name << "Speculars[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << endl;
    
    // Close H file
    outH.close();
}

void writeCvertices(ObjModel & objmodel, Model model)
{
    objmodel.vertices = model.vertices;
}

void writeCpositions(ObjModel & objmodel, Model model, vector< vector<float>>&  faces, vector< vector<float>>&  positions, int counts[])
{
    objmodel.positions = new float[model.vertices*3];
    objmodel.verticesIndex = new float[model.vertices];
    // Positions
    int idx =0,idx2 = 0;
    for(int j=0; j<model.materials; j++)
    {
        counts[j] = 0;
        
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][0] == j)
            {
                for(int k =7;k<faces[i].size();k+=3)
                {
                    int vA= faces[i][1] - 1;
                    int vB =faces[i][k-3] - 1;
                    int vC =faces[i][k] - 1;
                    objmodel.positions[idx++] = positions[vA][0];
                    objmodel.positions[idx++] = positions[vA][1];
                    objmodel.positions[idx++] = positions[vA][2];
                    objmodel.positions[idx++] = positions[vB][0];
                    objmodel.positions[idx++] = positions[vB][1];
                    objmodel.positions[idx++] = positions[vB][2];
                    objmodel.positions[idx++] = positions[vC][0];
                    objmodel.positions[idx++] = positions[vC][1];
                    objmodel.positions[idx++] = positions[vC][2];
                    objmodel.verticesIndex[idx2++] = vA;
                    objmodel.verticesIndex[idx2++] = vB;
                    objmodel.verticesIndex[idx2++] = vC;
                    counts[j] +=3;
                }
            }
        }
    }
}
void writeCtexels(ObjModel & objmodel, Model model, vector< vector<float>>&  faces, vector< vector<float>>& texels)
{
    objmodel.texels =new float[ model.vertices*2];
    objmodel.texelsIndex = new float[model.vertices];
    int idx =0,idx2=0;
    for(int j=0; j<model.materials; j++)
    {
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][0] == j)
            {
                for(int k =8;k<faces[i].size();k+=3)
                {
                    int vA= faces[i][2] - 1;
                    int vB =faces[i][k-3] - 1;
                    int vC =faces[i][k] - 1;
                    objmodel.texels[idx++] = texels[vA][0];
                    objmodel.texels[idx++] = texels[vA][1];
                    objmodel.texels[idx++] = texels[vB][0];
                    objmodel.texels[idx++] = texels[vB][1];
                    objmodel.texels[idx++] = texels[vC][0];
                    objmodel.texels[idx++] = texels[vC][1];
                    objmodel.texelsIndex[idx2++] = vA;
                    objmodel.texelsIndex[idx2++] = vB;
                    objmodel.texelsIndex[idx2++] = vC;
                }
            }
            
        }
    }
}
void writeCnormals(ObjModel & objmodel, Model model, vector< vector<float>>& faces, vector< vector<float>>& normals)
{
    objmodel.normals = new float[model.vertices*3];
    objmodel.normalsIndex = new float[model.vertices];
    int idx =0,idx2=0;
    for(int j=0; j<model.materials; j++)
    {
        for(int i=0; i<model.faces; i++)
        {
            if(faces[i][0] == j)
            {
                for(int k =9;k<faces[i].size();k+=3)
                {
                    int vA= faces[i][3] - 1;
                    int vB =faces[i][k-3] - 1;
                    int vC =faces[i][k] - 1;
                    objmodel.normals[idx++] = normals[vA][0];
                    objmodel.normals[idx++] = normals[vA][1];
                    objmodel.normals[idx++] = normals[vA][2];
                    objmodel.normals[idx++] = normals[vB][0];
                    objmodel.normals[idx++] = normals[vB][1];
                    objmodel.normals[idx++] = normals[vB][2];
                    objmodel.normals[idx++] = normals[vC][0];
                    objmodel.normals[idx++] = normals[vC][1];
                    objmodel.normals[idx++] = normals[vC][2];
                    objmodel.normalsIndex[idx2++] = vA;
                    objmodel.normalsIndex[idx2++] = vB;
                    objmodel.normalsIndex[idx2++] = vC;

                }

            }
            
        }
    }
}




int getMTLinfo(string fp)
{
    int m = 0;
    
    // Open MTL file
    ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.good())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }
    
    // Read MTL file
    while(!inMTL.eof())
    {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);
        
        if(type.compare("ne") == 0)
            m++;
    }
    
    // Close MTL file
    inMTL.close();
    
    return m;
}

void extractMTLdata(string fp, vector<string> &materials,vector<string> &textures, vector< vector<float>>& diffuses, vector< vector<float>>& speculars)
{
    // Counters
    int m = 0;
    int d = 0;
    int s = 0;
    
    // Open MTL file
    ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.good())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }
    
    // Read MTL file
    while(!inMTL.eof())
    {
        string line;
        getline(inMTL, line);
        if(line.size()<=1)continue;
        char l[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        string type = string(strtok(l, " "));
        // Names
        if(type.compare("newmtl") == 0)
        {
            materials[m++] = string(strtok(NULL, " "));
        }
        
        // Diffuses
        else if(type.compare("Kd") == 0)
        {
            for(int i=0; i<3; i++)
                diffuses[m-1][i] = atof(strtok(NULL, " "));
        }
        
        // Speculars
        else if(type.compare("Ks") == 0)
        {
            for(int i=0; i<3; i++)
                speculars[m-1][i] = atof(strtok(NULL, " "));
        }
        else if(type.compare("map_Kd") == 0)
        {
            textures[m-1] = string(strtok(NULL, " "));
        }
    }
    
    // Close MTL file
    inMTL.close();
}

void writeCmaterials(ObjModel & objmodel, Model model, int firsts[], int counts[])
{
    
    objmodel.materials = model.materials;
    objmodel.firsts  = new int[model.materials];
    objmodel.counts  = new int[model.materials];
    for(int i=0; i<model.materials; i++)
    {
        if(i == 0)
            firsts[i] = 0;
        else
            firsts[i] = firsts[i-1]+counts[i-1];
        
        objmodel.firsts[i] = firsts[i];
    }
    for(int i=0; i<model.materials; i++)
    {
        objmodel.counts[i] = counts[i];

    }
    
}

void writeCtextrues(ObjModel & objmodel, Model model, vector< string>& textures)
{
    
    // Diffuses
    objmodel.textures  = new char *[model.materials];
    for(int i=0; i<model.materials; i++)
    {
        objmodel.textures[i] = new char[textures[i].size()+1];
        memcpy(objmodel.textures[i], textures[i].c_str(), textures[i].size()+1);
        
    }
}

void writeCdiffuses(ObjModel & objmodel, Model model, vector< vector<float>>& diffuses)
{
    
    // Diffuses
    objmodel.diffuses  = new float *[model.materials];
    for(int i=0; i<model.materials; i++)
    {
        objmodel.diffuses[i] = new float[3];
        objmodel.diffuses[i][0] =diffuses[i][0];
        objmodel.diffuses[i][1] =diffuses[i][1];
        objmodel.diffuses[i][2] =diffuses[i][2];

        
    }
}

void writeCspeculars(ObjModel & objmodel, Model model, vector< vector<float>>& speculars)
{
    
    // Speculars
    objmodel.speculars  = new float *[model.materials];
    for(int i=0; i<model.materials; i++)
    {
        objmodel.speculars[i] = new float[3];
        objmodel.speculars[i][0] = speculars[i][0];
        objmodel.speculars[i][1] = speculars[i][1];
        objmodel.speculars[i][2] = speculars[i][2];
        
    }
    
}
void normalObj(vector< vector<float> >& positions,Model& model)
{
    if(model.vertices<=0)return ;
    float minV[3] ={positions[0][0],positions[0][1],positions[0][2]};
    float maxV[3] ={positions[0][0],positions[0][1],positions[0][2]};
    for(int i=0;i<model.positions;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(positions[i][j]<minV[j])
                minV[j] = positions[i][j];
            if(positions[i][j]>maxV[j])
                maxV[j] = positions[i][j];
        }
        //cout<<i<<" "<<maxV[0]<<maxV[1]<<maxV[2]<<endl;
    }
    float maxd = max(maxV[0] - minV[0], max(maxV[1] - minV[1], maxV[2] - minV[2]));
    if(maxd<=0)return ;
    float scale = 1/maxd;
    float center[3] ={(maxV[0] + minV[0])/2.0f,(maxV[1] + minV[1])/2.0f,(maxV[2] + minV[2])/2.0f};
    for (int i = 0; i < model.positions; i++)
    {
        for(int j=0;j<3;j++)
        {
            positions[i][j] = positions[i][j] - center[j];
            positions[i][j] = positions[i][j]*scale;
        }
    }
}
int loadObj(const char*  filepathOBJ,const char* filepathMTL,ObjModel & objmodel)
{
    objmodel.release();
    // Model Info
    Model model = getOBJinfo(filepathOBJ);
    model.materials = getMTLinfo(filepathMTL);
    
    // Model Data
    vector< vector<float>>positions(model.positions,vector<float>(3,0));
    // float positions[model.positions][3];                // XYZ
    vector< vector<float>>texels(model.texels,vector<float>(2,0));
    //float texels[model.texels][2];                      // UV
    vector< vector<float>>normals(model.normals,vector<float>(3,0));
    //float normals[model.normals][3];                    // XYZ
    vector< vector<float>>faces(model.faces,vector<float>(1,0));
   // int faces[model.faces][10];                         // PTN PTN PTN M
    vector< string> materials(model.materials);    // Name
    vector< string> textures(model.materials);    // Texture Name
    vector< vector<float>> diffuses(model.materials,vector<float>(3,0));// RGB
    vector< vector<float>> speculars(model.materials,vector<float>(3,0));// RGB
    extractMTLdata(string(filepathMTL), materials,textures,diffuses, speculars);
    model.vertices =  extractOBJdata(string(filepathOBJ), positions, texels, normals, faces, materials, model.materials);

    int firsts[model.materials];	// Starting vertex
    int counts[model.materials];	// Number of vertices
    
    normalObj(positions,model);
    
    writeCvertices(objmodel, model);
    writeCpositions(objmodel, model, faces, positions, counts);
    writeCtexels(objmodel, model, faces, texels);
    writeCnormals(objmodel, model, faces, normals);
    writeCmaterials(objmodel, model, firsts, counts);
    writeCtextrues(objmodel, model, textures);
    writeCdiffuses(objmodel, model, diffuses);
    writeCspeculars(objmodel, model, speculars);
    
    return 0;
}


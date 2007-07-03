// $Id: $
//
//    File: DBCALShower_factory_IU.cc
// Created: Tue Jul  3 18:25:12 EDT 2007
// Creator: Matthew Shepherd
//

#include <cassert>
#include <math.h>
#include <map>

#include "BCAL/DBCALMCResponse.h"
#include "BCAL/DBCALGeometry.h"
#include "BCAL/DBCALShower_factory_IU.h"

#define BCAL_Z_OFFSET 26.03+123.4+65.0 //convert regina's coordinated to HDGeant's

using namespace std;

//------------------
// DBCALShower_factory_IU
//------------------
DBCALShower_factory_IU::DBCALShower_factory_IU()
{
	ethr_cell=0.00001;// MIN ENERGY THRESD OF cel in GeV
    
	elyr = 1;
	xlyr = 2; 
	ylyr = 3; 
	zlyr = 4; 
	tlyr = 5;
    
	r_thr = 40.0;  // CENTROID DISTANCE THRESHOLD
	t_thr = 2.5;   // CENTROID TIME THRESHOLD
	z_thr = 30.0;    // FIBER DISTANCE THRESHOLD
	rt_thr= 40.0; // CENTROID TRANSVERSE DISTANCE THRESHOLD
	ecmin = 0.007;  // MIN ENERGY THRESD OF CLUSTER IN GEV
	rmsmax= 5.0;    // T RMS THRESHOLD
    
	f_att= 1.0;
	lmbda1=300.; // Attenuation lenth and other parameters DL 3/27/07
	lmbda2=700.; // used in formula  attenuation factor
	mcprod=0;	//   mcprod=0 for MC data and mcprod=1 for real data
    
	C_EFFECTIVE=16.75; //Effective v of light in scintillator DL 3/27/07
	ECORR=0.13; // ECORR actually could be a function of E
}

//------------------
// brun
//------------------
jerror_t DBCALShower_factory_IU::brun(JEventLoop *loop, int runnumber)
{
    
    vector<const DBCALGeometry*> bcalGeomVect;
    eventLoop->Get( bcalGeomVect );
    const DBCALGeometry& bcalGeom = *(bcalGeomVect[0]);
    
    //////////////////////////////////////////////////////////////////
    // Calculate Cell Position
    //////////////////////////////////////////////////////////////////
    
    fiberLength = bcalGeom.BCALFIBERLENTH; // fiber lenth in cm
    int   modmin = 0;
    int   modmax = bcalGeom.NBCALMODS;
    int   rowmin1=0;
    int   rowmax1= bcalGeom.NBCALLAYS1;
    int   rowmin2= rowmax1;  
    int   rowmax2= bcalGeom.NBCALLAYS2+rowmin2; 
    int   colmin1=0;
    int   colmax1=bcalGeom.NBCALSECS1;
    int   colmin2=0;
    int   colmax2=bcalGeom.NBCALSECS2;
    
    float r_inner= bcalGeom.BCALINNERRAD;
    float r_middle= bcalGeom.BCALMIDRAD;
    float r_outer= bcalGeom.BCALOUTERRAD;
    
    for (int i = (rowmin1+1); i < (rowmax1+1); i++){
        float thick_innerlayers=(r_middle-r_inner)/(rowmax1-rowmin1); 
        rt[i]=thick_innerlayers/2+thick_innerlayers*(i-rowmin1-1);
    }
    
    for (int i = (rowmin2+1); i < (rowmax2+1); i++){
        float thick_outerlayers=(r_outer-r_middle)/(rowmax2-rowmin2);
        rt[i]=r_middle+thick_outerlayers/2+thick_outerlayers*(i-rowmin2-1)-r_inner;
    }
    
    float r[modulemax_bcal][layermax_bcal][colmax_bcal];
    float phi[modulemax_bcal][layermax_bcal][colmax_bcal];
    
    // Now start to extract cell position information from Geometry class
    for (int k = modmin; k < modmax; k++){
        for (int i = rowmin1; i < rowmax1; i++){
            for (int j = colmin1; j < colmax1; j++){
                int layer=rowmax1-rowmin1;
                float thick_inner=(r_middle-r_inner)/layer;
                r[k][i][j]=r_inner+thick_inner/2+thick_inner*(i-rowmin1);
                float b=360.0/(modmax-modmin);
                float c=b/(colmax1-colmin1);
                phi[k][i][j]=(c/2.0+c*(j-colmin1)+b*(k-modmin))*3.141593/180.0;
                xx[k][i][j]=r[k][i][j]*cos(phi[k][i][j]);
                yy[k][i][j]=r[k][i][j]*sin(phi[k][i][j]);          
            }
        }
        
        for (int i = rowmin2; i < rowmax2; i++){
            for (int j = colmin2; j < colmax2; j++){                 
                float thick_outer=(r_outer-r_middle)/(rowmax2-rowmin2);
                r[k][i][j]=r_middle+thick_outer/2+thick_outer*(i-rowmin2);
                float b=360.0/(modmax-modmin);
                float c=b/(colmax2-colmin2);            
                phi[k][i][j]=(c/2.0+c*(j-colmin2)+b*(k-modmin))*3.141593/180;
                xx[k][i][j]=r[k][i][j]*cos(phi[k][i][j]);
                yy[k][i][j]=r[k][i][j]*sin(phi[k][i][j]);          
            }
        }         
    }
    
    ////////////////////////////////////////////////////////////////////////////
    // Now the cell information are already contained in xx and yy arrays.
    // xx and yy arrays are private members of this class
    ////////////////////////////////////////////////////////////////////////////
    
    // these are timing offsets -- a global side offset and individual cell offsets
    ta0=0.0;
    tb0=0.0;    
    
    for (int i = 0; i < modulemax_bcal; i++){
        for (int j = 0; j < layermax_bcal; j++){
            for (int k = 0; k < colmax_bcal; k++){                       
                
                ta_offset[i][j][k]=0.0;
                tb_offset[i][j][k]=0.0;
            }
        }
    }
}

//------------------
// evnt
//------------------
jerror_t DBCALShower_factory_IU::evnt(JEventLoop *loop, int eventnumber)
{
    CellRecon(eventLoop);
    CeleToArray();   
    PreCluster(eventLoop); 
    ClusNorm();
    ClusAnalysis();
    Trakfit();
    
    vector<DBCALShower*> clusters;
    
    for (int i = 1; i < (clstot+1); i++){
        
        int  j=clspoi[i];
        
        // Time to cook a final shower
        DBCALShower *shower = new DBCALShower;
  
        shower->E                   = e_cls[j];    
        shower->Ecorr               = shower->E*(1+ECORR);
        shower->x                   = x_cls[j];
        shower->y                   = y_cls[j];
        shower->z                   = z_cls[j] + BCAL_Z_OFFSET;   
        shower->t                   = t_cls[j];
        shower->N_cell              = ncltot[j];
        shower->total_layer_cluster = nlrtot[j];
        shower->Apx_x               = apx[1][j];
        shower->Apx_y               = apx[2][j];
        shower->Apx_z               = apx[3][j];
        shower->error_Apx_x         = eapx[1][j];
        shower->error_Apx_y         = eapx[2][j];
        shower->error_Apx_z         = eapx[3][j];
        shower->Cx                  = ctrk[1][j];
        shower->Cy                  = ctrk[2][j];
        shower->Cz                  = ctrk[3][j];
        shower->error_Cx            = ectrk[1][j];
        shower->error_Cy            = ectrk[2][j];
        shower->error_Cz            = ectrk[3][j];
        shower->t_rms_a             = trms_a[j];
        shower->t_rms_b             = trms_b[j];
        
        _data.push_back(shower);  
    }
    
	return NOERROR;
}


//------------------
// CellRecon()
//------------------
void DBCALShower_factory_IU::CellRecon(JEventLoop *eventLoop)
{
    //======================================================================
    // This code is used to reconstruct cell information cell by cell.
    // This code is adapted from kloe code by Chuncheng Xu. June 29,2005
    
    //**********************************************************************
    // The main purpose of this function is extracting information
    // from  DBCALMCResponse class 
    // objects and form the array: ecel_a,tcel_a,ecel_b,tcel_b 
    // and xcel,ycel,zcel,tcel,ecel,tcell_anor,tcell_bnor;
    // Among these arrays,
    // ecel_a,tcel_a,ecel_b,tcel_b, xcel,ycel,zcel,tcel,ecel,tcell_anor and
    // tcell_bnor are the input of function CeleToArray()
    //********************************************************************** 
        
    // extract the BCAL hits
    
    vector<const DBCALMCResponse*> hits;
    eventLoop->Get(hits);
    if(hits.size() <= 0) return;
    
    
    /////////////////////////////////////////////////////////////////////
    // Now start to take take out  DBCALMCResponse to form our private 
    // data member ecel_a,tcel_a,ecel_b,tcel_b
    /////////////////////////////////////////////////////////////////////
    
    memset( ecel_a, 0, modulemax_bcal * layermax_bcal *
            colmax_bcal * sizeof( float ) );
    memset( tcel_a, 0, modulemax_bcal * layermax_bcal *
            colmax_bcal * sizeof( float ) );
    memset( ecel_b, 0, modulemax_bcal * layermax_bcal *
            colmax_bcal * sizeof( float ) );
    memset( tcel_b, 0, modulemax_bcal * layermax_bcal *
            colmax_bcal * sizeof( float ) );
    
    for (unsigned int i = 0; i < hits.size(); i++) {
        
        const  DBCALMCResponse *hit = hits[i];     
        int module = hit->module;
        int layer = hit->layer;
        int sector = hit->sector;
        int end = hit->end;
        float E = hit->E;
        float t = hit->t;
        
        // cache hits indexed by module/layer/sector
        // this allows end to end matching in next step
        
        if(end==0) {                         // upstream
            ecel_a[module-1][layer-1][sector-1] =  E;
            tcel_a[module-1][layer-1][sector-1] =  t;
        }  
        else if(end==1) {                //   downstream
            ecel_b[module-1][layer-1][sector-1] =  E;
            tcel_b[module-1][layer-1][sector-1] =  t;	      
        }
        else
        {
            cout<<"no such end, it is neither side A Nor B \n"; 
        }
    }  
    
    //////////////////////////////////////////////////////////////////////// 
    // Now data member ecel_a,tcel_a,ecel_b,tcel_b are readily filled.
    ////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////// 
    // Now start to reconstruct cell information from data member ecel_a,tcel_a
    // ecel_b,tcel_b. The reconstructed cell information are stored in 
    // data member arrays xcel,ycel,zcel,tcel,ecel,tcell_anor,tcell_bnor.
    ////////////////////////////////////////////////////////////////////////
    
    //
    // ************ Loop over all CELE elements ********************
    //
    // K module,I layer, J sector
    
    for (int k = 0; k < modulemax_bcal; k++){
        for (int i = 0; i < layermax_bcal; i++){
            for (int j = 0; j <  colmax_bcal; j++){    
                
                float  tai    =    tcel_a[k][i][j];    // Get TIMEs
                float  tbi    =    tcel_b[k][i][j];  
                
                if (tai==0 || tbi==0 || fabs(tai-tbi)>80.) {  // disregard cells
                                                              // with no t information
                                                              // but put 0 in cwrk   
                    xcel[k][i][j]=0.0;
                    ycel[k][i][j]=0.0;
                    zcel[k][i][j]=0.0;
                    tcel[k][i][j]=0.0;
                    ecel[k][i][j]=0.0;
                    continue;
                }
                //
                // ********* Get energy information ******************************
                //
                
                float eai= ecel_a[k][i][j];
                float ebi= ecel_b[k][i][j];
                
                if(eai<ethr_cell||ebi<ethr_cell){
                    xcel[k][i][j]=0.0;
                    ycel[k][i][j]=0.0;
                    zcel[k][i][j]=0.0;
                    tcel[k][i][j]=0.0;
                    ecel[k][i][j]=0.0;
                    continue;
                }
                
                //
                // Now retrieve and build all relevant information for each cell:
                // To give all information for Wkim clustering, we need also to correct 
                // TA,TB for the different cell lengths. So we have CWRK version #2.
                //
                
                //------ Make proper initialization -----------------------------------------
                
                float x = 0.;
                float y = 0.;
                float z = 0.;
                float t = 0.;
                float e = 0.;
                float tanor=0.0;
                float tbnor=0.0;  
                
                //----now start the cell information reconstruction;
                //----Here I use "Blocks and Scope" method to translate the functions
                //----So the following brackets starts actually the functions
                //	integer function getallemc (tain,tbin,eain,ebin,
                //     			 e,x,y,z,t,mod,row,col,tanor,tbnor)
                //    which is a Fortran Function appeared in Kloe code. 
                
                
                //   Now begins the function getallemc
                {
                    float  ta = tai-ta0-ta_offset[k][i][j];
                    float  tb = tbi-tb0-tb_offset[k][i][j];
                    tanor =ta;
                    tbnor =tb;
                    float  ea = eai;
                    float  eb = ebi;
                    
                    x    = xx[k][i][j];
                    y    = yy[k][i][j];
                    
                    float  z1 = -0.5*fiberLength;    // cm
                    float  z2 =  0.5*fiberLength;    // cm      
                    float  z0 = 0.5*(z1+z2);
                    
                    z = 0.5*C_EFFECTIVE*(ta-tb) + z0;
                    
                    if( z>(0.5*fiberLength)) z = 0.5*fiberLength;
                    else {
                        if (z<(-0.5*fiberLength)) z = -0.5*fiberLength;
                    }
                    t = 0.5*(ta+tb-fiberLength/C_EFFECTIVE);
                    float dpma = min((ta-t)*C_EFFECTIVE,fiberLength);
                    float dpmb = min((tb-t)*C_EFFECTIVE,fiberLength);
                    
                    
                    float atta=f_att*exp(-dpma/lmbda1)+(1.-f_att)*exp(-dpma/lmbda2);
                    float attb=f_att*exp(-dpmb/lmbda1)+(1.-f_att)*exp(-dpmb/lmbda2);
                    
                    e = ea/atta + eb/attb;
                    
                    float  datanor = f_att*exp(-0.5*fiberLength/lmbda1)
                        +(1.-f_att)*exp(-0.5*fiberLength/lmbda2);
                    
                    if( mcprod==0) e = e *datanor;
                    e = 0.5*e;
                    
                }
                
                //   Now ends the function getallemc
                
                xcel[k][i][j]=x;
                ycel[k][i][j]=y;
                zcel[k][i][j]=z;
                tcel[k][i][j]=t;
                ecel[k][i][j]=e;
                tcell_anor[k][i][j]=tanor;
                tcell_bnor[k][i][j]=tbnor;
                
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // data member arrays xcel,ycel,zcel,tcel,ecel,tcell_anor,tcell_bnor are filled
    ///////////////////////////////////////////////////////////////////////////////
    
}


//------------------
// CeleToArray()
//------------------
void DBCALShower_factory_IU::CeleToArray(void)
{
    //  THis code is adpapted from kloe code by Chuncheng Xu on June29,2005
    //  The following part is taken from kaloe's clurec_lib.f's subroutine
    //  cele_to_arr.
    //
    //  It's original purpose is to extract the data information from array RW
    //  and IW into array NARR(j,i), CELDATA(j,i), nclus(i),next(i),
    //  and e_cel(i), x_cel(i),y_cel(i), z_cel(i), t_cel(i),ta_cel(i)
    //  and tb_cel(i)
    //  In the above explanationn, i is the index for cell, and different j
    //  is for different component for such cell.
    //  for example, NARR(1,i), NARR(2,i),NARR(3,i) are for module number,
    //  layer number and sector number in halld bcal simulation 
    
    
    //  Now  we assume that the information is stored in arrays ECEL_A(k,i,j)
    //  , ECEL_B(k,i,j), TCEL_A(k,i,j),  TCEL_B(k,i,j), XCEL(k,I,J),YCEL(k,I,J), 
    // ZCEL(K,I,J), TCEL(K,I,J), ECEL(K,I,J),TCELL_ANOR(K,I,J),TCELL_BNOR(K,I,J),
    //  which are passed into here by event.inc through common block.  
    
    //   these values e_cel(i), x_cel(i),y_cel(i), z_cel(i), t_cel(i),ta_cel(i)
    //  and tb_cel(i)) are extremly useful in later's clusterization
    //  and they are passed by common block to precluster subroutine too. (through
    //  clurec_cal.inc)
    // 
    //  we hope this is a good "bridge" from raw data to precluster. 
    //  This way we can keep good use of their strategy of clusterization
    //  as much as possible, although we know that Fortran has it's bad fame 
    //  of not so easy to be reused.
    
    celtot=0;
    
    for (int k = 0; k < modulemax_bcal; k++){
        for (int i = 0; i < layermax_bcal; i++){
            for (int j = 0; j < colmax_bcal; j++){     
                
                float   ea  = ecel_a[k][i][j];
                float   eb  = ecel_b[k][i][j];
                float   ta  = tcel_a[k][i][j];
                float   tb  = tcel_b[k][i][j];
                
                if(min(ea,eb)>ethr_cell & fabs(ta-tb)<35.&ta!=0.&tb!=0.) 
                    celtot=celtot+1;             
                else {
                    continue;
                }
                
                
                if(celtot>cellmax_bcal) {
                    break;
                }
                
                narr[1][celtot]=k+1;    // these numbers will
                narr[2][celtot]=i+1;    //  be used by preclusters
                narr[3][celtot]=j+1;    //  which will start from index of 1
                                        // rather than from 0.
                
                
                //-----------------------------------------------------------------------
                
                celdata[1][celtot]=ea/0.145;
                celdata[2][celtot]=eb/0.145;
                
                nclus[celtot] = celtot;
                next[celtot]  = celtot;
                
                e_cel[celtot] = ecel[k][i][j];
                x_cel[celtot] = xcel[k][i][j];
                y_cel[celtot] = ycel[k][i][j];
                z_cel[celtot] = zcel[k][i][j];
                t_cel[celtot] = tcel[k][i][j];
                
                ta_cel[celtot]=tcell_anor[k][i][j];
                tb_cel[celtot]=tcell_bnor[k][i][j];
            }
        }
    }
    
    //    cout<<"celtot= "<<celtot<<"\n";
}



//------------------
// PreCluster()
//------------------        
void DBCALShower_factory_IU::PreCluster(JEventLoop *eventLoop)
{
    int k=1;     // NUMBER OF NEARBY ROWS &/OR TO LOOK FOR MAX E CELL
    
    // extract the BCAL Geometry
    vector<const DBCALGeometry*> bcalGeomVect;
    eventLoop->Get( bcalGeomVect );
    const DBCALGeometry& bcalGeom = *(bcalGeomVect[0]);
    
    // calculate cell position
    
    int   modmin = 0;
    int   modmax = bcalGeom.NBCALMODS;
    
    int   rowmax1= bcalGeom.NBCALLAYS1;
    int   rowmin2= rowmax1+1;  
    int   rowmax2= bcalGeom.NBCALLAYS2+rowmin2-1; 
    int   colmax1=bcalGeom.NBCALSECS1;
    int   colmax2=bcalGeom.NBCALSECS2;
    
    float r_inner= bcalGeom.BCALINNERRAD;
    float r_middle= bcalGeom.BCALMIDRAD;
    float r_outer= bcalGeom.BCALOUTERRAD;
    
    float thick_inner=(r_middle-r_inner)/rowmax1; //thickness for first 5 layers
                                                  //thickness for last 4 layers:
    float thick_outer=(r_outer-r_middle)/(rowmax2-rowmin2+1);
    // distance of two cells in row 5 and 6 of r direction:  
    float dis_in_out=(thick_outer+thick_inner)/2.0;
    
    float degree_permodule=360.0/(modmax-modmin);
    
    float half_degree_permodule=degree_permodule/2.0;
    float width_1=2.0*(r_middle-thick_inner/2.0)*
        sin(half_degree_permodule*3.141593/180)/colmax1;
    float width_2=2.0*(r_middle+thick_outer/2.0)*
        sin(half_degree_permodule*3.141593/180)/colmax2;
    float disthres=width_2*1.5-width_1*0.5+0.0001;
    //  cout<<"disthres="<<disthres<<"\n";
    
    for (int i = 1; i < (celtot+1); i++){
        
        int maxnn=0;
        float emin=0.;
        
        
        for (int j = 1; j < (celtot+1); j++){
            if(j!=i & nclus[j]!=nclus[i] & e_cel[j]>emin) {
                
                int k1= narr[1][i];
                int k2= narr[1][j];
                int i1= narr[2][i];
                int i2= narr[2][j];
                
                
                int  modiff = k1-k2;
                int amodif = abs(modiff);
                
                //  the following if is to check module and row distance.         
                if(abs(i1-i2)<=k&(amodif<=1||amodif==47)) { 
                    //   further check col distance 
                    int   j1= narr[3][i];
                    int   j2= narr[3][j];
                    
                    if(amodif==0) {   // same module       
                        
                        //   further check col distance if both are on first 5 rows
                        if(i1<=rowmax1 & i2<=rowmax1 & abs(j2-j1)<=k) {
                            emin=e_cel[j];
                            maxnn=j;
                        }
                        
                        //   further check col distance if both are on last 4 rows
                        
                        if(i1>=rowmin2 & i2>=rowmin2 & abs(j2-j1)<=k) {
                            emin=e_cel[j];
                            maxnn=j;
                        }
                    }
                    
                    
                    
                    if(amodif>0) {  // different module          
                        if(modiff==1||modiff==-47) {      
                            if(i1<=rowmax1 & i2<=rowmax1){ 
                                if(abs((j1+colmax1)-j2)<=k){
                                    emin=e_cel[j];
                                    maxnn=j;
                                }
                            }
                            
                            if(i1>=rowmin2 & i2>=rowmin2) {
                                if(abs((j1+colmax2)-j2)<=k){
                                    emin=e_cel[j];
                                    maxnn=j;
                                }
                            }              
                        }
                        
                        if(modiff==-1||modiff==47) {      
                            
                            if(i1<=rowmax1 & i2<=rowmax1){
                                if(abs((j2+colmax1)-j1)<=k){
                                    emin=e_cel[j];
                                    maxnn=j;
                                }
                            } 
                            
                            if(i1>=rowmin2 & i2>=rowmin2) {
                                if(abs((j2+colmax2)-j1)<=k){
                                    emin=e_cel[j];
                                    maxnn=j;
                                }
                            }              
                            
                        }
                        
                    }
                    
                    
                    
                    //    further check col distance if one is in row5, another is in row6
                    //    so that the two are between the boundary of two different size
                    //   of cells.
                    
                    if(i1==rowmax1 & i2==rowmin2) {
                        float delta_xx=xx[k1-1][i1-1][j1-1]-xx[k2-1][i2-1][j2-1];
                        float delta_yy=yy[k1-1][i1-1][j1-1]-yy[k2-1][i2-1][j2-1];
                        
                        float dis=sqrt(delta_xx*delta_xx+delta_yy*delta_yy);
                        dis=sqrt(dis*dis- dis_in_out*dis_in_out);               
                        if(dis<disthres){      
                            emin=e_cel[j];
                            maxnn=j;
                        }
                    }
                    
                    if(i1==rowmin2 & i2==rowmax1) {
                        float delta_xx=xx[k1-1][i1-1][j1-1]-xx[k2-1][i2-1][j2-1];
                        float delta_yy=yy[k1-1][i1-1][j1-1]-yy[k2-1][i2-1][j2-1];
                        
                        float dis=sqrt(delta_xx*delta_xx+delta_yy*delta_yy);
                        dis=sqrt(dis*dis-dis_in_out*dis_in_out);
                        if(dis<disthres) {      
                            emin=e_cel[j];
                            maxnn=j;
                        } 
                    }
                }             
            }
        }        // finish second loop
        
        
        if(maxnn>0){
            
            Connect(maxnn,i);
        }
    }       // finish first loop
}



//------------------
// Connect(n,m);
//------------------   
void DBCALShower_factory_IU::Connect(int n,int m)
{
    //----------------------------------------------------------------------
    //   Purpose and Methods :CONNECTS CELL M TO THE NEAREST MAX NEIGHBOR N
    //   Created  31-JUL-1992   WON KIM
    //----------------------------------------------------------------------
    
    if(nclus[n]!=nclus[m]){
        int j=m;
        nclus[j]=nclus[n];
        while(next[j]!=m){
            j=next[j];
            nclus[j]=nclus[n];
	    }        
        next[j]=next[n];
        next[n]=m;
    }
}


//------------------
// ClusNorm()
//------------------   
void DBCALShower_factory_IU::ClusNorm(void)
{    
    // fast initialization of arrays:
    memset( e_cls,  0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( x_cls,  0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( y_cls,  0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( z_cls,  0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( t_cls,  0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( ea_cls, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( eb_cls, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( ta_cls, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( tb_cls, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( tsqr_a, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( tsqr_b, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( trms_a, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( trms_b, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( e2_a,   0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( e2_b,   0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( clspoi, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( ncltot, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );
    memset( ntopol, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );

    clstot=0;
    
    for (int ix = 1; ix < (celtot+1); ix++){
        
        //----------------------------------------------------------------------
        // KEEP TALLY OF CLUSTERS
        //----------------------------------------------------------------------
        
        int n=nclus[ix];
        int j=0;
        
        for (int i = 1; i < (clstot+1); i++){
            if(n==clspoi[i]) j=i;
        }
        
        if(j==0) {
            clstot=clstot+1;
            clspoi[clstot]=n;
        }
        
        //----------------------------------------------------------------------
        // NORMALIZED QUANTITIES OF THE TOTAL CLUSTER
        //----------------------------------------------------------------------
        if(e_cel[ix]<0.000000001)continue; // just for protection
        
        x_cls[n]=(e_cls[n]*x_cls[n]+e_cel[ix]*x_cel[ix])
            /(e_cls[n]+e_cel[ix]);
        
        y_cls[n]=(e_cls[n]*y_cls[n]+e_cel[ix]*y_cel[ix])
            /(e_cls[n]+e_cel[ix]);
        
        z_cls[n]=(e_cls[n]*z_cls[n]+e_cel[ix]*z_cel[ix])
            /(e_cls[n]+e_cel[ix]);
        
        t_cls[n]=(e_cls[n]*t_cls[n]+e_cel[ix]*t_cel[ix])
            /(e_cls[n]+e_cel[ix]);
        
        e_cls[n]=e_cls[n]+e_cel[ix];
        
        //        write(*,*)' x-y-z-T-e done'
        //----------------------------------------------------------------------
        //       NORMALIZED QUANTITIES FOR EACH SIDE
        //----------------------------------------------------------------------
        
        //  CHECK!!! -- like ea_cls[n] getting used too early!?!?
        ta_cls[n]=(ea_cls[n]*ta_cls[n]+celdata[1][ix]*ta_cel[ix])
            /(ea_cls[n]+celdata[1][ix]);        
        tsqr_a[n]=(ea_cls[n]*tsqr_a[n]+celdata[1][ix]*ta_cel[ix]*
                   ta_cel[ix])/(ea_cls[n]+celdata[1][ix]);
        ea_cls[n]=ea_cls[n]+celdata[1][ix];
        e2_a[n]=e2_a[n]+celdata[1][ix]*celdata[1][ix];
        tb_cls[n]=(eb_cls[n]*tb_cls[n]+celdata[2][ix]*tb_cel[ix])/
            (eb_cls[n]+celdata[2][ix]);
        tsqr_b[n]=(eb_cls[n]*tsqr_b[n]+celdata[2][ix]*tb_cel[ix]*
                   tb_cel[ix])/(eb_cls[n]+celdata[2][ix]);
        eb_cls[n]=eb_cls[n]+celdata[2][ix];
        e2_b[n]=e2_b[n]+celdata[2][ix]*celdata[2][ix];
        
        //----------------------------------------------------------------------
        //       TOTAL CELLS AND CELLS IN OTHER MODULES
        //----------------------------------------------------------------------
        
        ncltot[n]++;

        if( narr[1][n] != narr[1][ix] || narr[2][n] != narr[2][ix] )
            ntopol[n]++;
        
        //----------------------------------------------------------------------
        
    }
    
    for (int n = 1; n < (clstot+1); n++){
        
        int ix=clspoi[n];
        if( ncltot[ix] > 1) {
            
            float effnum = ea_cls[ix] * ea_cls[ix] / e2_a[ix];
            trms_a[ix] = effnum / ( effnum - 1 ) * 
                ( tsqr_a[ix] - ta_cls[ix] * ta_cls[ix] );

            effnum = eb_cls[ix] * eb_cls[ix] / e2_b[ix];
            trms_b[ix] = effnum / ( effnum - 1 ) * 
                ( tsqr_b[ix] - tb_cls[ix] * tb_cls[ix] );

            if( trms_a[ix] <= 0.0 ) trms_a[ix] = 0.;
            if( trms_b[ix] <= 0.0 ) trms_b[ix] = 0.;
            trms_a[ix] = sqrt( trms_a[ix] );
            trms_b[ix] = sqrt( trms_b[ix] );
        }
        else {
            trms_a[ix] = 0.;
            trms_b[ix] = 0.;
        }
    }
}

//------------------
// ClusAnalysis()
//------------------  
void DBCALShower_factory_IU::ClusAnalysis()
{
    // track when clusters change to cut down
    // on excess calles to expensive ClusNorm
    bool newClust = false;

    //----------------------------------------------------------------------
    //  Check for overlapping clusters
    //----------------------------------------------------------------------
    
    for (int i = 0; i < 2; i++){
        for (int j = 1; j < (clstot+1); j++){
            int ix=clspoi[j];
            if(e_cls[ix]>0.0){
                float  dist=sqrt(trms_a[ix]*trms_a[ix]+trms_b[ix]*trms_b[ix]);
                if(dist>rmsmax) {
                    Clus_Break(ix);
                    newClust = true;
                }
            }
        }
        
        if( newClust ){
            
            ClusNorm();
            newClust = false;
        }
    }
    
    //----------------------------------------------------------------------       
    // merge clusters likely to be from the same shower
    //----------------------------------------------------------------------       

    int icls[3];
    for (int i = 1; i < clstot; i++){
        icls[1]=0;
        icls[2]=0;
        for (int j = (i+1); j < (clstot+1); j++){
            
            int ix=clspoi[i];
            int iy=clspoi[j];
            
            
            if(e_cls[ix]>0.0 & e_cls[iy]>0.0) {
                
                float delta_x=x_cls[ix]-x_cls[iy];
                float delta_y=y_cls[ix]-y_cls[iy];          
                float delta_z=z_cls[ix]-z_cls[iy];  
                float dist=sqrt(delta_x*delta_x+delta_y*delta_y+delta_z*delta_z);
                
                float  tdif=fabs(t_cls[ix]-t_cls[iy]);
                
                //	  float  distz=abs(z_cls[ix]-z_cls[iy]);
                //         cout<<"dist="<<dist<<" distz="<<distz<<" tdif="<<tdif<<"\n";    
                
                if(dist<r_thr & tdif<t_thr){
                    float zdif=fabs(z_cls[ix]-z_cls[iy]);
                    float distran=sqrt(delta_x*delta_x+delta_y*delta_y);
                    
                    if(zdif<z_thr & distran<rt_thr){
                        if(e_cls[ix]>=e_cls[iy]) {
                            icls[1]=ix;
                            icls[2]=iy;
                        }
                        else {
                            icls[1]=iy;
                            icls[2]=ix;
                        }
                    }
                }
                
            }
            
            if(min(icls[1],icls[2])>0){
                
                Connect(icls[1],icls[2]);
                newClust = true;
            }
        }
    }
    
    if( newClust ){
        
        ClusNorm();
    }
}

//------------------
// Clus_Break(ix);
//------------------
void DBCALShower_factory_IU::Clus_Break(int nclust)
{
    int   nseed[5],selnum,selcel[cellmax_bcal+1];
    float tdif,tdif_a,tdif_b,tseed[5];  
    
    //----------------------------------------------------------------------
    for (int i =0; i < 5; i++){
        nseed[i]=0;
        tseed[i]=0;
    }
    //----------------------------------------------------------------------
    //   Divide cluster cells into four quadrant groups
    //----------------------------------------------------------------------
    int n=nclust;
    tdif_a=ta_cel[n]-ta_cls[nclust];
    tdif_b=tb_cel[n]-tb_cls[nclust];
    selnum=0;
    
    //----------------------------------------------------------------------
    if(tdif_a>0.0) {
        if(tdif_b>0){
            selnum=1;
        }
        else {
            selnum=2;
        }
    }
    
    else {
        if(tdif_b>0.0) {
            selnum=3;
        }
        else {
            selnum=4;
        }
    }
    
    //------------------------------------------------------------------------
    
    if(selnum>0) {
        float tdif=sqrt(tdif_a*tdif_a+tdif_b*tdif_b);
        if(tdif>tseed[selnum]){
            nseed[selnum]=n;
            tseed[selnum]=tdif;
        }
        selcel[n]=selnum;
    }
    
    //-------------------------------------------------------------------------
    
    while(next[n]!=nclust) {
        n=next[n];
        tdif_a=ta_cel[n]-ta_cls[nclust];
        tdif_b=tb_cel[n]-tb_cls[nclust];
        selnum=0;
        
        //**************************************************************************
        
        if(tdif_a>0.0) {
            
            if(tdif_b>0.0) {
                selnum=1;
            }
            else {
                selnum=2;
            }
        }
        
        else {
            if(tdif_b>0.0) {
                selnum=3;
            } 
            else {
                selnum=4;
            }
        }
        
        //***************************************************************************
        
        //...........................................................................
        
        
        if(selnum>0){
            tdif=sqrt(tdif_a*tdif_a+tdif_b*tdif_b);
            
            if(tdif>tseed[selnum]){
                nseed[selnum]=n;
                tseed[selnum]=tdif;
            }
            
            selcel[n]=selnum;
        }
        
        //...........................................................................
        
    }             //this bracket is related to "while" above. 
    
    
    //----------------------------------------------------------------------
    // If successful, divide cluster chain into the new cluster chains
    //----------------------------------------------------------------------
    
    for (int i =1; i < 5; i++){
        
        if(nseed[i]>0) {
            
            nclus[nseed[i]]=nseed[i];
            next[nseed[i]]=nseed[i];

            for (int j =1; j < (celtot+1); j++){
                if(nclus[j]==nclust & j!=nseed[i]) {
                    if(selcel[j]==i) {
                        nclus[j]=j;
                        next[j]=j;
                        Connect(nseed[i],j);
                    }
                }
            }
        }
    }
}


//------------------
// Trakfit()
//------------------ 

// routine modified by MRS to do expensive filling of layer information
// contains code previously in ClusNorm which is called multiple times
// per event, but there is no need to call Trakfit multiple times per event
void DBCALShower_factory_IU::Trakfit( void )
{
 
    float emin=0.0001;

    memset( clslyr, 0, ( clsmax_bcal + 1 ) * 
            ( layermax_bcal + 1 ) * 6 * sizeof( float ) );
    
    memset( apx,   0, ( clsmax_bcal + 1 ) * 4 * sizeof( float ) );
    memset( eapx,  0, ( clsmax_bcal + 1 ) * 4 * sizeof( float ) );
    memset( ctrk,  0, ( clsmax_bcal + 1 ) * 4 * sizeof( float ) );
    memset( ectrk, 0, ( clsmax_bcal + 1 ) * 4 * sizeof( float ) );
    
    for (int ix = 1; ix < (celtot+1); ix++){
        
        int n = nclus[ix];
        
        //----------------------------------------------------------------------
        // NORMALIZED QUANTITIES OF THE TOTAL CLUSTER PER LAYER
        //----------------------------------------------------------------------
        
        int lyr = narr[2][ix];
        
        //	write(*,*)'X, E',E_CEL(ix),CLSLYR(XLYR,LYR,N)
        clslyr[xlyr][lyr][n]=(clslyr[elyr][lyr][n]*clslyr[xlyr][lyr][n]
                              +e_cel[ix]*x_cel[ix])/(clslyr[elyr][lyr][n]+e_cel[ix]);
        
        clslyr[ylyr][lyr][n]=(clslyr[elyr][lyr][n]*clslyr[ylyr][lyr][n]
                              +e_cel[ix]*y_cel[ix])/(clslyr[elyr][lyr][n]+e_cel[ix]);
        //	write(*,*)' Y'
        
        clslyr[zlyr][lyr][n]=(clslyr[elyr][lyr][n]*clslyr[zlyr][lyr][n]
                              +e_cel[ix]*z_cel[ix])/(clslyr[elyr][lyr][n]+e_cel[ix]);
        
        //	write(*,*)' z'
        clslyr[tlyr][lyr][n]=(clslyr[elyr][lyr][n]*clslyr[tlyr][lyr][n]
                              +e_cel[ix]*t_cel[ix])/(clslyr[elyr][lyr][n]+e_cel[ix]);
        
        //	write(*,*)'E'
        clslyr[elyr][lyr][n]=clslyr[elyr][lyr][n]+e_cel[ix];
        
        //        write(*,*)' slopes done'
    }

    memset( nlrtot, 0, ( clsmax_bcal + 1 ) * sizeof( float ) );

    for (int n = 1; n < ( clstot + 1 ); n++){

        int ix=clspoi[n];
        
        for (int i = 1; i < (layermax_bcal+1); i++){
            
            if( clslyr[elyr][i][ix] > 0.0 ) nlrtot[ix]++;
        }
                    
        for (int i = 0; i < ( layermax_bcal + 1 ); i++){
        
            x[i]=0.0;
            y[i]=0.0;  
            z[i]=0.0;
            e[i]=0.0;
            sigx[i]=0.0;  // cm
            sigy[i]=0.0; // cm
            sigz[i]=0.0;
        }
        
        int nltot=0;
    
        for (int il = 1; il < (layermax_bcal+1); il++){

            if(clslyr[elyr][il][ix]>emin) {

                nltot=nltot+1;
                x[nltot]= clslyr[xlyr][il][ix];
                y[nltot]= clslyr[ylyr][il][ix];
                z[nltot]= clslyr[zlyr][il][ix];         
                e[nltot]= clslyr[elyr][il][ix]; 

                sigy[nltot] =  1.0/e[nltot];
                sigx[nltot] =  1.0/e[nltot];
                sigz[nltot] =  1.0/sqrt(e[nltot]);
            }
        }
    
        // The following error bar is the estimation of error bar
        // based on the experience of my fortran code running
        // If the structure of BCAL changes drastically, you have to make changes
        // accordingly.  Xu Chuncheng , Jan 9, 2006
        
        // **** this seems strange since it appears to overwrite what is above
    
        sigx[1]=0.5;
        sigx[2]=0.5;
        sigx[3]=0.5;
        sigx[4]=0.5;
        sigx[5]=0.5;
        sigx[6]=0.8;
        sigx[7]=0.9;
        sigx[8]=1.2;
        sigx[9]=1.3;
        
        sigy[1]=0.5;
        sigy[2]=0.5;
        sigy[3]=0.5;
        sigy[4]=0.5;
        sigy[5]=0.5;
        sigy[6]=0.8;
        sigy[7]=0.9;
        sigy[8]=1.2;
        sigy[9]=1.3;
        
        
        sigz[1]=0.5;
        sigz[2]=0.5;
        sigz[3]=0.5;
        sigz[4]=0.5;
        sigz[5]=0.5;
        sigz[6]=0.8;
        sigz[7]=0.9;
        sigz[8]=1.2;
        sigz[9]=1.3;
        
        if( nltot > 1 ){ 
            
            Fit_ls();
            
            for (int i = 1; i < 4; i++){
                
                ctrk[i][ix]=ctrk_ix[i];
                ectrk[i][ix]=ectrk_ix[i];
                apx[i][ix]=apx_ix[i];
                eapx[i][ix]=eapx_ix[i];
            }
        }    
        else{
            
            // we have 1 or less layers hit -- no fit
            
            apx[1][ix] = x[1];
            apx[2][ix] = y[1];
            apx[3][ix] = z[1]; 
            eapx[1][ix] = sigx[1];
            eapx[2][ix] = sigy[1];
            eapx[3][ix] = sigz[1];  
            ectrk[1][ix] = 0.0;
            ectrk[2][ix] = 0.0;
            ectrk[3][ix] = 0.0;
            ctrk[1][ix] = 999.0;
            ctrk[2][ix] = 999.0;
            ctrk[3][ix] = 999.0;
        }            
    }
}

//------------------
// Fit_ls()
//------------------  
void DBCALShower_factory_IU::Fit_ls()
{      
    float a,b,c;
    float d,e,f,chi2,q,norm;
    float siga,sigb,sigc,sigd,sige,sigf;
    float sigb2,sigd2,sigf2;
    
    //    fitting for X=a+bt
    Linefit(1,1,a,b,siga,sigb,chi2,q);
    //    fitting for Y=c+dt
    Linefit(2,1,c,d,sigc,sigd,chi2,q);
    //    fitting for Z=e+ft
    Linefit(3,1,e,f,sige,sigf,chi2,q);
    sigb2=sigb*sigb;
    sigd2=sigd*sigd;
    sigf2=sigf*sigf;
    
    apx_ix[1]=a;
    apx_ix[2]=c;
    apx_ix[3]=e;
    eapx_ix[1]=siga;
    eapx_ix[2]=sigc;
    eapx_ix[3]=sige;
    
    //      write(*,*) "b,d,f = ",b,d,f
    //		     cout<<"b= "<<b<<" d="<<d<<" f="<<f<<"\n";
    
    //      write(*,*) "sigb,sigd,sigf = ",sigb,sigd,sigf
    
    norm=sqrt(b*b+d*d+f*f);
    
    ctrk_ix[1]=b/norm;
    ctrk_ix[2]=d/norm;
    ctrk_ix[3]=f/norm;
    
    float norm3=norm*norm*norm;
    
    ectrk_ix[1]=sqrt((d*d+f*f)*(d*d+f*f)*sigb2+b*b*d*d*sigd2+b*b*f*f*sigf2)/norm3;
    ectrk_ix[2]=sqrt((b*b+f*f)*(b*b+f*f)*sigd2+d*d*b*b*sigb2+d*d*f*f*sigf2)/norm3;
    ectrk_ix[3]=sqrt((b*b+d*d)*(b*b+d*d)*sigf2+f*f*b*b*sigb2+f*f*d*d*sigd2)/norm3;
    
    return;
}


//------------------
// Linefit(x,y,ndata,sig,mwt,a,b,siga,sigb,chi2,q)
//------------------  
void DBCALShower_factory_IU::Linefit(int ixyz,int mwt,float &a,
                                  float &b,float &siga,float &sigb,float &chi2,float &q)
{
    
    // This programme is taken from Garth's book
    //  "Numerical Recipes in Fortran"
    //  and we tested it. It works well.  Chuncheng Xu,2005
    
    
    float sig[layermax_bcal+1],etemp;
    float xtemp[layermax_bcal+1],ytemp[layermax_bcal+1];
    // uses gammq
    
    //  Given a set of data points X(1:ndata),Y(1:ndata) with individual standard
    //  deviations sig(1:ndata), fit them to a strait line y=a+bx by minimum 
    // chisq. Returned are a,b and their respective probable uncertainties
    // siga and sigb, the chisq, and the goodness-of-fit probability q(that the
    // fit would have chisq this large or larger). if mwt=0 on input, then the 
    // the standard deviations are assumed to be unavalable:q is returned as 
    // 1.0 and the normalization of chi2 is to the unit standard deviation on all
    // points.
    
    
    float sigdat,ss,st2,sx,sxoss,sy,t,wt;
    sx=0.0;             // Initialize sums to zero
    sy=0.0;
    st2=0.0;
    b=0.0;
    
    int ndata=0;
    
    
    
    if(ixyz==1) {
        for (int i = 1; i < (layermax_bcal+1); i++){
            xtemp[i]=rt[i];
            ytemp[i]=x[i];
            sig[i]=sigx[i];
            etemp=e[i];
            if(etemp>0.0001)ndata=ndata+1;
            
        }   
    }
    else if(ixyz==2) {
        for (int i = 1; i < (layermax_bcal+1); i++){
            xtemp[i]=rt[i];
            ytemp[i]=y[i];
            sig[i]=sigy[i];
            etemp=e[i];
            if(etemp>0.000001)ndata=ndata+1;   
        }
    }
    else if(ixyz==3) {
        for (unsigned int i = 1; i < (layermax_bcal+1); i++){
            xtemp[i]=rt[i];
            ytemp[i]=z[i]; 
            sig[i]=sigz[i];  
            etemp=e[i];
            if(etemp>0.000001)ndata=ndata+1;
        }
    }
    
    if(mwt!=0) {   // Accumulate sums
        ss=0.0;
        for (int i = 1; i < (ndata+1); i++){
            wt=1.0/(sig[i]*sig[i]);
            ss=ss+wt;
            sx=sx+xtemp[i]*wt;
            sy=sy+ytemp[i]*wt;
        }
    }
    
    else  {
        for (int i = 1; i < (ndata+1); i++){
            sx=sx+xtemp[i];
            sy=sy+ytemp[i];
        } 
        ss=float(ndata);
    }
    
    sxoss=sx/ss;
    
    if(mwt!=0) {
        for (int i = 1; i < (ndata+1); i++){
            t=(xtemp[i]-sxoss)/sig[i];
            st2=st2+t*t;
            b=b+t*ytemp[i]/sig[i];
        }
        
    }
    
    else  {
        
        for (int i = 1; i < (ndata+1); i++){
            t=xtemp[i]-sxoss;
            st2=st2+t*t;
            b=b+t*ytemp[i];
        }
    }
    
    b=b/st2;                // Solve for a,b,siga and sigb
    a=(sy-sx*b)/ss;
    siga=sqrt((1.0+sx*sx/(ss*st2))/ss);
    sigb=sqrt(1.0/st2);
    chi2=0.0;        //   Calculate chisq
    
    if(mwt==0) {
        for (int i = 1; i < (ndata+1); i++){
            chi2=chi2+(ytemp[i]-a-b*xtemp[i])*(ytemp[i]-a-b*xtemp[i]);
        }
        q=1.0;
        sigdat=sqrt(chi2/(ndata-2));
        siga=siga*sigdat;
        sigb=sigb*sigdat;
    }
    else  {
        for (int i = 1; i < (ndata+1); i++){
            chi2=chi2+((ytemp[i]-a-b*xtemp[i])/
                       sig[i])*((ytemp[i]-a-b*xtemp[i])/sig[i]);
        }
        q=Gammq(0.5*(ndata-2),0.5*chi2); 
    }
    
}



//------------------
// Gammq(a_gammq,x_gammq)
//------------------  
float DBCALShower_factory_IU::Gammq(float a_gammq,float x_gammq)
{
    
    //============================================================================
    
    
    float gammq;
    
    //    uses gcf,gser
    //Returns the incomplete gamma function Q(a_gammq,x_gammq)=1-P(a_gammq,x_gammq)
    
    
    float gammcf,gamser;
    
    if(a_gammq==0.0) { 
        gammq=999.0;
        return gammq;
    }
    
    
    
    if(x_gammq<0. || a_gammq<= 0.0) {
        //  cout<<"bad arguments in gammq"<<"\n";
        return 999.0;// pause 'bad arguments in gammq'
    }
    
    if(x_gammq<(a_gammq+1.)) {     // Use the series representation
        Gser(gamser,a_gammq,x_gammq);
        gammq=1.0-gamser;    // and take its complement
    }
    else  {               // Use continued fraction representation
        Gcf(gammcf,a_gammq,x_gammq);
        gammq=gammcf;
    }
    return gammq;
}


//------------------
// Gser(gamser,a_gser,x_gser,gln)
//------------------  
void DBCALShower_factory_IU::Gser(float &gamser,float a_gser,float x_gser)
{
    //============================================================================
    int itmax=100;
    float eps=3.0e-7;
    float gln;            
    
    //    uses gammln
    //    Returns the incomplete gamma function P(a,x) evaluated by its
    //    series representation as gamser. Also returns ln(Gamma(a)) as gln
    
	   float ap, del,sum;
       
       gln=Gammln(a_gser);
       
       if(x_gser<=0.0) {
           if(x_gser<0.0) cout<<"x_gser<0 in gser"<<"\n";
           gamser=0.0;
           return;
       }
       
       ap=a_gser;
       sum=1.0/a_gser;
       del=sum;
       
       
       for (int n = 1; n < (itmax+1); n++){
           ap=ap+1.0;
           del=del*x_gser/ap;
           sum=sum+del;
           
           if(fabs(del)<fabs(sum)*eps) {
               gamser=sum*exp(-x_gser+a_gser*log(x_gser)-gln);
               return;
           }
           
       }
       
       // cout<< "a too large, ITMAX too small in gser"<<"\n";
       return;
}


//------------------
//  Gcf(gammcf,a,x,gln)
//------------------  
void DBCALShower_factory_IU::Gcf(float &gammcf,float a_gcf,float x_gcf)
{
    //========================================================================
    
    int itmax=100;
    float eps=3.0e-7;
    float fpmin=1.0e-30;
    
    float gln;
    
    //   uses gammln
    //        Returns the incomplete gamma function Q(a,x) evaluated by 
    //   its continued fraction representation as gammcf. Also returns
    //   ln(Gamma(a)) as gln
    
    //   Parameters: ITMAX is the maximum allowed number of iterations;
    //   EPS is the relative accuracy; FPMIN is a number near the smallest
    //   representable floating-point number.
    
    float an,b,c,d,del,h;
    
    gln=Gammln(a_gcf);
    b=x_gcf+1.0-a_gcf;
    c=1.0/fpmin;
    d=1.0/b;
    h=d;
    
    
    for (int i = 1; i < (itmax+1); i++){
        an=-i*(i-a_gcf);
        b=b+2.0;
        d=an*d+b;
        if(fabs(d)<fpmin)d=fpmin;
        c=b+an/c;
        if(fabs(c)<fpmin)c=fpmin;
        d=1.0/d;
        del=d*c;
        h=h*del;
        if(fabs(del-1.0)<eps) {	 
            gammcf=exp(-x_gcf+a_gcf*log(x_gcf)-gln)*h; // Put factors in front 
            return;
        }
        // cout<< "a too large,ITMAX too small in gcf"<<"\n";    
        return;
    }
} //???

//------------------
// Gammln(xx)
//------------------  
float DBCALShower_factory_IU::Gammln(float xx_gln)
{
    //    Returns the value ln[Gamma(xx_gln)] for xx_gln>0.0
    
	   float ser,stp,tmp,x_gln,y_gln;
    float cof[7];
    float gammln; 
    
    //    Internal arithmetic will be done in double precision, a nicety that
    //    that you can omit if five-figure accuracy is good enoug
    //         save cof,stp
    //         data cof,stp/76.18009172947146d0,-86.50532032941677d0,
    //     * 24.01409824083091d0,-1.231739572450155d0,.1208650973866179d-2,
    //     * -.5395239384953d-5,2.5066282746310005d0/
    
    stp=2.5066282746310005;
    cof[1]=76.18009172947146;
    cof[2]=-86.50532032941677;
    cof[3]=24.01409824083091;
    cof[4]=-1.231739572450155;
    cof[5]=.1208650973866179e-2;
    cof[6]=-.5395239384953e-5;
    
    x_gln=xx_gln;
    y_gln=x_gln;
    tmp=x_gln+5.5;
    tmp=(x_gln+0.5)*log(tmp)-tmp;
    
    ser=1.000000000190015;
    
    for (int j = 1; j < 7; j++){
        y_gln=y_gln+1.0;
        ser=ser+cof[j]/y_gln;
    }
    
    
    gammln=tmp+log(stp*ser/x_gln);
    return gammln;
}


//------------------
// toString
//------------------
const string DBCALShower_factory_IU::toString(void)
{
    
	// Ensure our Get method has been called so _data is up to date
	Get();
	if(_data.size()<=0)
        return string(); // don't print anything if we have no data!
    
    printheader("row:      x:      y:      z:       t:     E_seen:     E_corr:");
    
    for(unsigned int i = 0; i < _data.size(); i++) {
		DBCALShower *s = _data[i];
        
		printnewrow();
		printcol("%d",	i);
		printcol("%5.2f",	s->x);
		printcol("%5.2f",	s->y);
		printcol("%5.2f",	s->z);
		printcol("%5.3f",	s->t);
		printcol("%5.3f",	s->E);
		printcol("%5.3f",	s->Ecorr);
		printrow();
	}
    
	return _table;
    
}

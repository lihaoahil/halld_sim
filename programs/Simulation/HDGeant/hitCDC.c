/*
 * hitCDC - registers hits for Central Drift Chamber
 *
 *	This is a part of the hits package for the
 *	HDGeant simulation program for Hall D.
 *
 *	version 1.0 	-Richard Jones July 16, 2001
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <hddm_s.h>
#include <geant3.h>
#include <bintree.h>

#define DRIFT_SPEED	.0022
#define TWO_HIT_RESOL	250.
#define MAX_HITS 	100
#define THRESH_KEV	1.

binTree_t* centralDCTree = 0;
static int strawCount = 0;
static int pointCount = 0;
static int stripCount = 0;


/* register hits during tracking (from gustep) */

void hitCentralDC (float xin[4], float xout[4],
                   float pin[5], float pout[5], float dEsum,
                   int track, int stack, int history)
{
   float x[3], t;
   float dx[3], dr;
   float dEdx;
   float xlocal[3],xoutlocal[3],xinlocal[3],trackdir[3];
	float xoutlocal_i[3], xout_i[3];
   float xcdc[3];
   float xHat[] = {1,0,0};
   float dradius,docaout,docain,docaout_i;

   x[0] = (xin[0] + xout[0])/2;
   x[1] = (xin[1] + xout[1])/2;
   x[2] = (xin[2] + xout[2])/2;
   t    = (xin[3] + xout[3])/2 * 1e9;
   transformCoord(x,"global",xlocal,"local");
   transformCoord(xHat,"local",xcdc,"CDC ");

	dx[0] = xin[0] - xout[0];
   dx[1] = xin[1] - xout[1];
   dx[2] = xin[2] - xout[2];
	
	/* If the particle exits from the end of the tube, then the
	midpoint from entrance to exit will not actually correspond
	to the DOCA. This is important since many tracks that exit
	through the CDC endplate will exit trough the end of the
	straw tube. In these cases, there are two possibilities:
	1.) The DOCA is at the exit point itself
	2.) The DOCA is at the mid-point between the entrance and where the
	    exit would have been if the tube were infinitely long.
		 
	First, we determine if the particle exited the end of the tube.
	If so, then calculate the DOCA (dradius) for both possibilities
	and keep the one that is smallest.
	*/
	transformCoord(xout,"global",xoutlocal,"local");
	transformCoord(xin,"global",xinlocal,"local");
	docaout = sqrt(xoutlocal[0]*xoutlocal[0] + xoutlocal[1]*xoutlocal[1]);
	docain = sqrt(xinlocal[0]*xinlocal[0] + xinlocal[1]*xinlocal[1]);
	dradius = sqrt(xlocal[0]*xlocal[0] + xlocal[1]*xlocal[1]);
	if(fabs(docaout-docain)>0.01){
		/* particle exited from end of the straw */
		
		/* Calculate exit point from an infinite straw */
		/* We do this by defining the direction of the */
		/* track and finding the amount we need to extend */
		/* in that direction in order to be at the tube */
		/* radius (determined by the entrance point). */
		/*
		/* xout_local = xin_local + alpha*trackdir
		/*
		/* where xout_local, xin_local, and trackdir are all
		/* vectors. "alpha" is a scaler multiplier to be
		/* be solved for. The values of xin_local and trackdir
		/* are detemined from xin and xout, while xout_local
		/* is to be calculated once alpha is determined.
		/*
		/* We solve for alpha by setting the transverse
		/* distance of xout_local to docain, the radius
		/* of the tube. This leads to an equation quadratic
		/* in alpha.
		/* 
		/**/
		float alpha,A,B,C;
		transformCoord(dx,"global",trackdir,"local");
		A = trackdir[0]*trackdir[0] + trackdir[1]*trackdir[1];
		B = 2.0*(trackdir[0]*xinlocal[0] + trackdir[1]*xinlocal[1]);
		C = docain*docain + xinlocal[0]*xinlocal[0] + xinlocal[1]*xinlocal[1];
		
		alpha = (-B + sqrt(B*B - 4.0*A*C))/(2.0*A);
		xoutlocal_i[0] = xinlocal[0] + alpha*trackdir[0];
		xoutlocal_i[1] = xinlocal[1] + alpha*trackdir[1];
		xoutlocal_i[2] = xinlocal[2] + alpha*trackdir[2];
		docaout_i = sqrt(xoutlocal_i[0]*xoutlocal_i[0] + xoutlocal_i[1]*xoutlocal_i[1]);
		transformCoord(xoutlocal_i,"local",xout_i,"global");
		
		/* Check which is the smallest DOCA and copy that into  */
		/* dradius so it will be used for the drift time below. */
		if(docaout<dradius){
			dradius = docaout;
			x[0]=xoutlocal[0];
			x[1]=xoutlocal[1];
			x[2]=xoutlocal[2];
		}
		if(docaout_i<dradius){
			dradius = docaout_i;
			x[0]=xoutlocal_i[0];
			x[1]=xoutlocal_i[1];
			x[2]=xoutlocal_i[2];
		}
	}

	/* Calculate dE/dx */
   dr = sqrt(dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2]);
   if (dr > 1e-3)
   {
      dEdx = dEsum/dr;
   }
   else
   {
      dEdx = 0;
   }

   /* post the hit to the truth tree */

   if (history == 0)
   {
      int mark = (1<<30) + pointCount;
      void** twig = getTwig(&centralDCTree, mark);
      if (*twig == 0)
      {
         s_CentralDC_t* cdc = *twig = make_s_CentralDC();
         s_CdcTruthPoints_t* points = make_s_CdcTruthPoints(1);
         points->in[0].primary = (stack == 0);
         points->in[0].track = track;
         points->in[0].z = x[2];
         points->in[0].r = sqrt(x[0]*x[0] + x[1]*x[1]);
         points->in[0].phi = atan2(x[1],x[0]);
         points->in[0].dradius = dradius;
         points->in[0].dEdx = dEdx;
         points->mult = 1;
         cdc->cdcTruthPoints = points;
         pointCount++;
      }
   }

   /* post the hit to the hits tree, mark sector as hit */

   if (dEsum > 0)
   {
      int nhit;
      s_CdcStrawHits_t* hits;
#if CATHODE_STRIPS_IN_CDC
      s_CdcCathodeStrips_t* chits;
      int cell = getcell_();
#endif
      int layer = getlayer_();
      int ring = getring_();
      int sector = getsector_();
      float tdrift = t + dradius/DRIFT_SPEED;
      if (layer == 0)		/* in a straw */
      {
         int mark = (ring<<20) + sector;
         void** twig = getTwig(&centralDCTree, mark);
         if (*twig == 0)
         {
            s_CentralDC_t* cdc = *twig = make_s_CentralDC();
            s_CdcStraws_t* straws = make_s_CdcStraws(1);
            straws->mult = 1;
            straws->in[0].ring = ring;
            straws->in[0].straw = sector;
            straws->in[0].cdcStrawHits = hits = make_s_CdcStrawHits(MAX_HITS);
            cdc->cdcStraws = straws;
            strawCount++;
         }
         else
         {
            s_CentralDC_t* cdc = (s_CentralDC_t*) *twig;
            hits = cdc->cdcStraws->in[0].cdcStrawHits;
         }

         for (nhit = 0; nhit < hits->mult; nhit++)
         {
            if (fabs(hits->in[nhit].t - tdrift) < TWO_HIT_RESOL)
            {
               break;
            }
         }
         if (nhit < hits->mult)		/* merge with former hit */
         {
            hits->in[nhit].t =
                    (hits->in[nhit].t * hits->in[nhit].dE + tdrift * dEsum)
                  / (hits->in[nhit].dE += dEsum);
         }
         else if (nhit < MAX_HITS)		/* create new hit */
         {
            hits->in[nhit].t = tdrift;
            hits->in[nhit].dE = dEsum;
            hits->mult++;
         }
         else
         {
            fprintf(stderr,"HDGeant error in hitCentralDC: ");
            fprintf(stderr,"max hit count %d exceeded, truncating!\n",MAX_HITS);
         }
      }
#if CATHODE_STRIPS_IN_CDC
      else			/* in one of the z-readout (strip) layers */
      {
         int nchit;
         int mark = (layer << 28) + (sector << 20) + (cell << 8);
         void** twig = getTwig(&centralDCTree, mark);
         if (*twig == 0)
         {
            s_CentralDC_t* cdc = *twig = make_s_CentralDC();
            s_CdcCathodeStrips strips = make_s_CdcCathodeStrips(1);
            strips->mult = 1;
            strips->in[0].layer = layer;
            strips->in[0].sector = sector;
            strips->in[0].strip = cell;
            strips->in[0].cdcCathodeHits = chits
                                       = make_s_CdcCathodeHits(MAX_HITS);
            cdc->cdcCathodeStrips = strips;
            stripCount++;
         }
         else
         {
            s_CentralDC_t* cdc = (s_CentralDC_t*) *twig;
            chits = cdc->cdcCathodeStrips->in[0].cdcCathodeHits;
         }

         for (nchit = 0; nchit < chits->mult; nchit++)
         {
            if (fabs(chits->in[nchit].t - tdrift) < TWO_HIT_RESOL)
            {
               break;
            }
         }
         if (nchit < chits->mult)		/* merge with former hit */
         {
            chits->in[nchit].t =
                    (chits->in[nchit].t * chits->in[nchit].dE + t * dEsum)
                  / (chits->in[nchit].dE += dEsum);
         }
         else if (nchit < MAX_HITS)		/* create new hit */
         {
            chits->in[nchit].t = t;
            chits->in[nchit].dE = dEsum;
            chits->mult++;
         }
         else
         {
            fprintf(stderr,"HDGeant error in hitCentralDC: ");
            fprintf(stderr,"max hit count %d exceeded, truncating!\n",MAX_HITS);
         }
      }
#endif
   }
}

/* entry points from fortran */

void hitcentraldc_(float* xin, float* xout,
                   float* pin, float* pout, float* dEsum,
                   int* track, int* stack, int* history)
{
   hitCentralDC(xin,xout,pin,pout,*dEsum,*track,*stack,*history);
}


/* pick and package the hits for shipping */

s_CentralDC_t* pickCentralDC ()
{
   s_CentralDC_t* box;
   s_CentralDC_t* item;

   if ((strawCount == 0) && (stripCount == 0) && (pointCount == 0))
   {
      return HDDM_NULL;
   }

   box = make_s_CentralDC();
   box->cdcStraws = make_s_CdcStraws(strawCount);
   box->cdcTruthPoints = make_s_CdcTruthPoints(pointCount);
#if CATHODE_STRIPS_IN_CDC
   box->cdcCathodeStrips = make_s_CdcCathodeStrips(stripCount);
#endif
   while (item = (s_CentralDC_t*) pickTwig(&centralDCTree))
   {
      s_CdcStraws_t* straws = item->cdcStraws;
#if CATHODE_STRIPS_IN_CDC
      s_CdcCathodetrips_t* strips = item->cdcCathodeStrips;
#endif
      s_CdcTruthPoints_t* points = item->cdcTruthPoints;
      if (straws != HDDM_NULL)
      {
         int m = box->cdcStraws->mult;
         int mok=0;

         /* compress out the hits below threshold */
         s_CdcStrawHits_t* hits = straws->in[0].cdcStrawHits;
         if (hits != HDDM_NULL)
         {
            int i,iok;
            for (iok=i=0; i < hits->mult; i++)
            {
               if (hits->in[i].dE >= THRESH_KEV/1e6)
               {
                  if (iok < i)
                  {
                     hits->in[iok] = hits->in[i];
                  }
                  ++iok;
                  ++mok;
               }
            }
            hits->mult = iok;

            if (mok)
            {
               box->cdcStraws->in[m] = straws->in[0];
               box->cdcStraws->mult++;
            }
            else
            {
               FREE(hits);
            }
         }
         FREE(straws);
      }
#if CATHODE_STRIPS_IN_CDC
      else if (strips != HDDM_NULL)
      {
         int m = box->cdcCathodeStrips->mult;
         int mok=0;

         /* compress out the hits below threshold */
         s_CdcCathodeHits* hits = strips->in[0].cdcCathodeHits;
         if (hits != HDDM_NULL)
         {
            int i,iok;
            for (iok=i=0; i < hits->mult; i++)
            {
               if (hits->in[i].dE >= THRESH_KEV/1e6)
               {
                  if (iok < i)
                  {
                     hits->in[iok] = hits->in[i];
                  }
                  ++iok;
                  ++mok;
               }
            }
            hits->mult = mok;

            if (mok)
            {
               box->cdcCathodeStrips->in[m] = strips->in[0];
               box->cdcCathodeStrips->mult++;
            }
            else
            {
               FREE(hits);
            }
         }
         FREE(item->cdcCathodeStrips);
      }
#endif
      else if (points != HDDM_NULL)
      {
         int m = box->cdcTruthPoints->mult++;
         box->cdcTruthPoints->in[m] = points->in[0];
         FREE(points);
      }
      FREE(item);
   }

   strawCount = stripCount = pointCount = 0;

#if CATHODE_STRIPS_IN_CDC
   if ((box->cdcCathodeStrips != HDDM_NULL) &&
       (box->cdcCathodeStrips->mult == 0))
   {
      FREE(box->cdcCathodeStrips);
      box->cdcCathodeStrips = HDDM_NULL;
   }
#endif
   if ((box->cdcStraws != HDDM_NULL) &&
       (box->cdcStraws->mult == 0))
   {
      FREE(box->cdcStraws);
      box->cdcStraws = HDDM_NULL;
   }
   if ((box->cdcTruthPoints != HDDM_NULL) &&
       (box->cdcTruthPoints->mult == 0))
   {
      FREE(box->cdcTruthPoints);
      box->cdcTruthPoints = HDDM_NULL;
   }
   if ((box->cdcStraws->mult == 0) &&
       (box->cdcTruthPoints->mult == 0))
   {
#if CATHODE_STRIPS_IN_CDC
    if (box->cdcCathodeStrips->mult == 0) {
#endif
      FREE(box);
      box = HDDM_NULL;
#if CATHODE_STRIPS_IN_CDC
    }
#endif
   }
   return box;
}

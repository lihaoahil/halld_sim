/*
 * hddm_s.c - DO NOT EDIT THIS FILE
 *
 * This file was generated automatically by hddm-c from the file
 * hddm/mc.xml
 * This c file contains the i/o interface to the c structures
 * described in the data model (from hddm/mc.xml). 
 * Any program that needs access to the data described in the model
 * can compile this source file, and make use of the input/output
 * services provided.
 *
 * The hddm data model tool set was written by
 * Richard Jones, University of Connecticut.
 *
 * For more information see the following web site
 *
 * http://zeus.phys.uconn.edu/halld/datamodel/doc
 */

int hddm_nullTarget=0;
#define HDDM_NULL (void*)&hddm_nullTarget

#include "hddm_s.h"


s_HDDM_t* make_s_HDDM()
{
   int size = sizeof(s_HDDM_t);
   s_HDDM_t* p = (s_HDDM_t*)MALLOC(size,"s_HDDM_t");
   p->physicsEvents = (s_PhysicsEvents_t*)&hddm_nullTarget;
   return p;
}

s_PhysicsEvents_t* make_s_PhysicsEvents(int n)
{
   int i;
   int rep = (n > 1) ? n-1 : 0;
   int size = sizeof(s_PhysicsEvents_t) + rep * sizeof(s_PhysicsEvent_t);
   s_PhysicsEvents_t* p = (s_PhysicsEvents_t*)MALLOC(size,"s_PhysicsEvents_t");
   p->mult = 0;
   for (i=0; i<n; i++) {
      s_PhysicsEvent_t* pp = &p->in[i];
      pp->eventNo = 0;
      pp->runNo = 0;
   pp->reactions = (s_Reactions_t*)&hddm_nullTarget;
   }
   return p;
}

s_Reactions_t* make_s_Reactions(int n)
{
   int i;
   int rep = (n > 1) ? n-1 : 0;
   int size = sizeof(s_Reactions_t) + rep * sizeof(s_Reaction_t);
   s_Reactions_t* p = (s_Reactions_t*)MALLOC(size,"s_Reactions_t");
   p->mult = 0;
   for (i=0; i<n; i++) {
      s_Reaction_t* pp = &p->in[i];
      pp->type = 0;
      pp->weight = 0;
   pp->beam = (s_Beam_t*)&hddm_nullTarget;
   pp->target = (s_Target_t*)&hddm_nullTarget;
   pp->vertices = (s_Vertices_t*)&hddm_nullTarget;
   }
   return p;
}

s_Beam_t* make_s_Beam()
{
   int size = sizeof(s_Beam_t);
   s_Beam_t* p = (s_Beam_t*)MALLOC(size,"s_Beam_t");
   p->type = 0;
   p->momentum = (s_Momentum_t*)&hddm_nullTarget;
   p->properties = (s_Properties_t*)&hddm_nullTarget;
   return p;
}

s_Momentum_t* make_s_Momentum()
{
   int size = sizeof(s_Momentum_t);
   s_Momentum_t* p = (s_Momentum_t*)MALLOC(size,"s_Momentum_t");
   p->E = 0;
   p->px = 0;
   p->py = 0;
   p->pz = 0;
   return p;
}

s_Properties_t* make_s_Properties()
{
   int size = sizeof(s_Properties_t);
   s_Properties_t* p = (s_Properties_t*)MALLOC(size,"s_Properties_t");
   p->charge = 0;
   p->mass = 0;
   return p;
}

s_Target_t* make_s_Target()
{
   int size = sizeof(s_Target_t);
   s_Target_t* p = (s_Target_t*)MALLOC(size,"s_Target_t");
   p->type = 0;
   p->momentum = (s_Momentum_t*)&hddm_nullTarget;
   p->properties = (s_Properties_t*)&hddm_nullTarget;
   return p;
}

s_Vertices_t* make_s_Vertices(int n)
{
   int i;
   int rep = (n > 1) ? n-1 : 0;
   int size = sizeof(s_Vertices_t) + rep * sizeof(s_Vertex_t);
   s_Vertices_t* p = (s_Vertices_t*)MALLOC(size,"s_Vertices_t");
   p->mult = 0;
   for (i=0; i<n; i++) {
      s_Vertex_t* pp = &p->in[i];
   pp->products = (s_Products_t*)&hddm_nullTarget;
   pp->origin = (s_Origin_t*)&hddm_nullTarget;
   }
   return p;
}

s_Products_t* make_s_Products(int n)
{
   int i;
   int rep = (n > 1) ? n-1 : 0;
   int size = sizeof(s_Products_t) + rep * sizeof(s_Product_t);
   s_Products_t* p = (s_Products_t*)MALLOC(size,"s_Products_t");
   p->mult = 0;
   for (i=0; i<n; i++) {
      s_Product_t* pp = &p->in[i];
      pp->decayVertex = 0;
      pp->type = 0;
   pp->momentum = (s_Momentum_t*)&hddm_nullTarget;
   pp->properties = (s_Properties_t*)&hddm_nullTarget;
   }
   return p;
}

s_Origin_t* make_s_Origin()
{
   int size = sizeof(s_Origin_t);
   s_Origin_t* p = (s_Origin_t*)MALLOC(size,"s_Origin_t");
   p->t = 0;
   p->vx = 0;
   p->vy = 0;
   p->vz = 0;
   return p;
}

char HDDM_s_DocumentString[] = 
"<HDDM class=\"s\" version=\"1.0\" xmlns=\"http://www.gluex.org/hddm\">\n"
"  <physicsEvent eventNo=\"int\" maxOccurs=\"unbounded\" runNo=\"int\">\n"
"    <reaction maxOccurs=\"unbounded\" minOccurs=\"0\" type=\"int\" weight=\"float\">\n"
"      <beam minOccurs=\"0\" type=\"Particle_t\">\n"
"        <momentum E=\"float\" px=\"float\" py=\"float\" pz=\"float\" />\n"
"        <properties charge=\"int\" mass=\"float\" />\n"
"      </beam>\n"
"      <target minOccurs=\"0\" type=\"Particle_t\">\n"
"        <momentum E=\"float\" px=\"float\" py=\"float\" pz=\"float\" />\n"
"        <properties charge=\"int\" mass=\"float\" />\n"
"      </target>\n"
"      <vertex maxOccurs=\"unbounded\">\n"
"        <product decayVertex=\"int\" maxOccurs=\"unbounded\" type=\"Particle_t\">\n"
"          <momentum E=\"float\" px=\"float\" py=\"float\" pz=\"float\" />\n"
"          <properties charge=\"int\" mass=\"float\" />\n"
"        </product>\n"
"        <origin t=\"float\" vx=\"float\" vy=\"float\" vz=\"float\" />\n"
"      </vertex>\n"
"    </reaction>\n"
"  </physicsEvent>\n"
"</HDDM>\n"
;


static s_HDDM_t* unpack_s_HDDM(XDR* xdrs, popNode* pop)
{
   s_HDDM_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_HDDM();
      {
         int p;
         void* (*ptr) = (void**) &this1->physicsEvents;
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_PhysicsEvents_t* unpack_s_PhysicsEvents(XDR* xdrs, popNode* pop)
{
   s_PhysicsEvents_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      int m;
      unsigned int mult;
      xdr_u_int(xdrs,&mult);
      this1 = make_s_PhysicsEvents(mult);
      this1->mult = mult;
      for (m = 0; m < mult; m++ )
      {
         int p;
         void* (*ptr) = (void**) &this1->in[m].reactions;
         xdr_int(xdrs,&this1->in[m].eventNo);
         xdr_int(xdrs,&this1->in[m].runNo);
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Reactions_t* unpack_s_Reactions(XDR* xdrs, popNode* pop)
{
   s_Reactions_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      int m;
      unsigned int mult;
      xdr_u_int(xdrs,&mult);
      this1 = make_s_Reactions(mult);
      this1->mult = mult;
      for (m = 0; m < mult; m++ )
      {
         int p;
         void* (*ptr) = (void**) &this1->in[m].beam;
         xdr_int(xdrs,&this1->in[m].type);
         xdr_float(xdrs,&this1->in[m].weight);
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Beam_t* unpack_s_Beam(XDR* xdrs, popNode* pop)
{
   s_Beam_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_Beam();
      {
         int p;
         void* (*ptr) = (void**) &this1->momentum;
         xdr_int(xdrs,(int*)&this1->type);
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Momentum_t* unpack_s_Momentum(XDR* xdrs, popNode* pop)
{
   s_Momentum_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_Momentum();
      {
         xdr_float(xdrs,&this1->E);
         xdr_float(xdrs,&this1->px);
         xdr_float(xdrs,&this1->py);
         xdr_float(xdrs,&this1->pz);
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Properties_t* unpack_s_Properties(XDR* xdrs, popNode* pop)
{
   s_Properties_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_Properties();
      {
         xdr_int(xdrs,&this1->charge);
         xdr_float(xdrs,&this1->mass);
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Target_t* unpack_s_Target(XDR* xdrs, popNode* pop)
{
   s_Target_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_Target();
      {
         int p;
         void* (*ptr) = (void**) &this1->momentum;
         xdr_int(xdrs,(int*)&this1->type);
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Vertices_t* unpack_s_Vertices(XDR* xdrs, popNode* pop)
{
   s_Vertices_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      int m;
      unsigned int mult;
      xdr_u_int(xdrs,&mult);
      this1 = make_s_Vertices(mult);
      this1->mult = mult;
      for (m = 0; m < mult; m++ )
      {
         int p;
         void* (*ptr) = (void**) &this1->in[m].products;
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Products_t* unpack_s_Products(XDR* xdrs, popNode* pop)
{
   s_Products_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      int m;
      unsigned int mult;
      xdr_u_int(xdrs,&mult);
      this1 = make_s_Products(mult);
      this1->mult = mult;
      for (m = 0; m < mult; m++ )
      {
         int p;
         void* (*ptr) = (void**) &this1->in[m].momentum;
         xdr_int(xdrs,&this1->in[m].decayVertex);
         xdr_int(xdrs,(int*)&this1->in[m].type);
         for (p = 0; p < pop->popListLength; p++)
         {
            popNode* pnode = pop->popList[p];
            if (pnode)
            {
               int kid = pnode->inParent;
               ptr[kid] = pnode->unpacker(xdrs,pnode);
            }
            else
            {
               unsigned int skip;
               xdr_u_int(xdrs,&skip);
               xdr_setpos(xdrs,xdr_getpos(xdrs)+skip);
            }
         }
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

static s_Origin_t* unpack_s_Origin(XDR* xdrs, popNode* pop)
{
   s_Origin_t* this1 = HDDM_NULL;
   unsigned int size;
   if (! xdr_u_int(xdrs,&size))
   {
       return this1;
   }
   else if (size > 0)
   {
      int start = xdr_getpos(xdrs);
      this1 = make_s_Origin();
      {
         xdr_float(xdrs,&this1->t);
         xdr_float(xdrs,&this1->vx);
         xdr_float(xdrs,&this1->vy);
         xdr_float(xdrs,&this1->vz);
      }
      xdr_setpos(xdrs,start+size);
   }
   return this1;
}

s_HDDM_t* read_s_HDDM(s_iostream_t* fp)
{
   s_HDDM_t* nextEvent = unpack_s_HDDM(fp->xdrs,fp->popTop);
   return (nextEvent == HDDM_NULL)? 0 : nextEvent;
}

static int pack_s_HDDM(XDR* xdrs, s_HDDM_t* this1);
static int pack_s_PhysicsEvents(XDR* xdrs, s_PhysicsEvents_t* this1);
static int pack_s_Reactions(XDR* xdrs, s_Reactions_t* this1);
static int pack_s_Beam(XDR* xdrs, s_Beam_t* this1);
static int pack_s_Momentum(XDR* xdrs, s_Momentum_t* this1);
static int pack_s_Properties(XDR* xdrs, s_Properties_t* this1);
static int pack_s_Target(XDR* xdrs, s_Target_t* this1);
static int pack_s_Vertices(XDR* xdrs, s_Vertices_t* this1);
static int pack_s_Products(XDR* xdrs, s_Products_t* this1);
static int pack_s_Origin(XDR* xdrs, s_Origin_t* this1);

static int pack_s_HDDM(XDR* xdrs, s_HDDM_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      if (this1->physicsEvents != (s_PhysicsEvents_t*)&hddm_nullTarget)
      {
         pack_s_PhysicsEvents(xdrs,this1->physicsEvents);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_PhysicsEvents(XDR* xdrs, s_PhysicsEvents_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   xdr_u_int(xdrs,&this1->mult);
   for (m = 0; m < this1->mult; m++)
   {
      xdr_int(xdrs,&this1->in[m].eventNo);
      xdr_int(xdrs,&this1->in[m].runNo);
      if (this1->in[m].reactions != (s_Reactions_t*)&hddm_nullTarget)
      {
         pack_s_Reactions(xdrs,this1->in[m].reactions);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Reactions(XDR* xdrs, s_Reactions_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   xdr_u_int(xdrs,&this1->mult);
   for (m = 0; m < this1->mult; m++)
   {
      xdr_int(xdrs,&this1->in[m].type);
      xdr_float(xdrs,&this1->in[m].weight);
      if (this1->in[m].beam != (s_Beam_t*)&hddm_nullTarget)
      {
         pack_s_Beam(xdrs,this1->in[m].beam);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->in[m].target != (s_Target_t*)&hddm_nullTarget)
      {
         pack_s_Target(xdrs,this1->in[m].target);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->in[m].vertices != (s_Vertices_t*)&hddm_nullTarget)
      {
         pack_s_Vertices(xdrs,this1->in[m].vertices);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Beam(XDR* xdrs, s_Beam_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      xdr_int(xdrs,(int*)&this1->type);
      if (this1->momentum != (s_Momentum_t*)&hddm_nullTarget)
      {
         pack_s_Momentum(xdrs,this1->momentum);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->properties != (s_Properties_t*)&hddm_nullTarget)
      {
         pack_s_Properties(xdrs,this1->properties);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Momentum(XDR* xdrs, s_Momentum_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      xdr_float(xdrs,&this1->E);
      xdr_float(xdrs,&this1->px);
      xdr_float(xdrs,&this1->py);
      xdr_float(xdrs,&this1->pz);
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Properties(XDR* xdrs, s_Properties_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      xdr_int(xdrs,&this1->charge);
      xdr_float(xdrs,&this1->mass);
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Target(XDR* xdrs, s_Target_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      xdr_int(xdrs,(int*)&this1->type);
      if (this1->momentum != (s_Momentum_t*)&hddm_nullTarget)
      {
         pack_s_Momentum(xdrs,this1->momentum);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->properties != (s_Properties_t*)&hddm_nullTarget)
      {
         pack_s_Properties(xdrs,this1->properties);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Vertices(XDR* xdrs, s_Vertices_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   xdr_u_int(xdrs,&this1->mult);
   for (m = 0; m < this1->mult; m++)
   {
      if (this1->in[m].products != (s_Products_t*)&hddm_nullTarget)
      {
         pack_s_Products(xdrs,this1->in[m].products);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->in[m].origin != (s_Origin_t*)&hddm_nullTarget)
      {
         pack_s_Origin(xdrs,this1->in[m].origin);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Products(XDR* xdrs, s_Products_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   xdr_u_int(xdrs,&this1->mult);
   for (m = 0; m < this1->mult; m++)
   {
      xdr_int(xdrs,&this1->in[m].decayVertex);
      xdr_int(xdrs,(int*)&this1->in[m].type);
      if (this1->in[m].momentum != (s_Momentum_t*)&hddm_nullTarget)
      {
         pack_s_Momentum(xdrs,this1->in[m].momentum);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
      if (this1->in[m].properties != (s_Properties_t*)&hddm_nullTarget)
      {
         pack_s_Properties(xdrs,this1->in[m].properties);
      }
      else
      {
         int zero=0;
         xdr_int(xdrs,&zero);
      }
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

static int pack_s_Origin(XDR* xdrs, s_Origin_t* this1)
{
   int m;
   unsigned int size=0;
   int base,start,end;
   base = xdr_getpos(xdrs);
   xdr_u_int(xdrs,&size);
   start = xdr_getpos(xdrs);

   m = 0; /* avoid warnings from -Wall */
   {
      xdr_float(xdrs,&this1->t);
      xdr_float(xdrs,&this1->vx);
      xdr_float(xdrs,&this1->vy);
      xdr_float(xdrs,&this1->vz);
   }
   FREE(this1);
   end = xdr_getpos(xdrs);
   xdr_setpos(xdrs,base);
   size = end-start;
   xdr_u_int(xdrs,&size);
   xdr_setpos(xdrs,end);
   return size;
}

int flush_s_HDDM(s_HDDM_t* this1,s_iostream_t* fp)
{
   if (this1 == 0)
   {
      return 0;
   }
   else if (fp == 0)
   {
      XDR* xdrs = (XDR*)malloc(sizeof(XDR));
      int max_buffer_size = 1000000;
      char* dump = (char*)malloc(max_buffer_size);
      xdrmem_create(xdrs,dump,max_buffer_size,XDR_ENCODE);
      pack_s_HDDM(xdrs,this1);
      xdr_destroy(xdrs);
      free(xdrs);
      free(dump);
      return 0;
   }
   else if (fp->iomode == HDDM_STREAM_OUTPUT)
   {
      pack_s_HDDM(fp->xdrs,this1);
      return 0;
   }
   return 0;
}

static int getTag(char* d, char* tag)
{
   int level;
   char* token;
   char line[500];
   strncpy(line,d,500);
   line[499] = 0;
   level = index(line,'<')-line;
   if (level < 500 &&
      (token = strtok(line+level+1," >")))
   {
      strncpy(tag,token,500);
      return level/2;
   }
   return -1;
}

static char* getEndTag(char* d, char* tag)
{
   char line[500];
   char endTag[510];
   strncpy(line,d,500);
   line[499] = 0;
   if (strstr(strtok(line,"\n"),"/>") == 0)
   {
      sprintf(endTag,"</%s>",tag);
   }
   else
   {
      strcpy(endTag,"/>");
   }
   return strstr(d,endTag);
}

static void collide(char* tag)
   {
      fprintf(stderr,"HDDM Error: ");
      fprintf(stderr,"input template model for tag ");
      fprintf(stderr,"%s does not match c code.", tag);
      fprintf(stderr,"\nPlease recompile.\n");
      exit(9);
   }

static popNode* matches(char* b, char* c)
{
   char btag[500];
   char ctag[500];
   int blevel, clevel;
   int ptrSeqNo = 0;
   blevel = getTag(b,btag);
   while ((clevel = getTag(c,ctag)) == blevel)
   {
      if ((clevel == blevel) && (strcmp(ctag,btag) == 0))
      {
         popNode* this1 = (popNode*)malloc(sizeof(popNode));
         int len = index(c+1,'\n') - c;
         if (strncmp(c,b,len) != 0)
         {
            collide(btag);
         }
         if (strcmp(btag,"HDDM") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_HDDM;
         }
         else if (strcmp(btag,"physicsEvent") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_PhysicsEvents;
         }
         else if (strcmp(btag,"reaction") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Reactions;
         }
         else if (strcmp(btag,"beam") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Beam;
         }
         else if (strcmp(btag,"momentum") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Momentum;
         }
         else if (strcmp(btag,"properties") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Properties;
         }
         else if (strcmp(btag,"target") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Target;
         }
         else if (strcmp(btag,"vertex") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Vertices;
         }
         else if (strcmp(btag,"product") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Products;
         }
         else if (strcmp(btag,"origin") == 0)
         {
            this1->unpacker = (void*(*)(XDR*,popNode*))unpack_s_Origin;
         }
         else
         {
            collide(btag);
         }
         this1->inParent = ptrSeqNo;
         this1->popListLength = 0;
         c = index(c+1,'\n');
         b = index(b+1,'\n');
         while (getTag(b,btag) > blevel)
         {
            this1->popList[this1->popListLength++] = matches(b,c);
            if (this1->popListLength > 99)
            {
               fprintf(stderr,"hddm error - posList overflow.\n");
               fprintf(stderr,"Increase MAX_POPLIST_LENGTH and recompile.\n");
               exit(9);
            }
            b = getEndTag(b,btag);
            b = index(b+1,'\n');
         }
         return this1;
      }
      else
      {
         c = getEndTag(c,ctag);
         c = index(c+1,'\n');
         ++ptrSeqNo;
      }
   }
   return 0;
}

s_iostream_t* open_s_HDDM(char* filename)
{
   s_iostream_t* fp = (s_iostream_t*)malloc(sizeof(s_iostream_t));
   char* p;
   char* head;
   if (filename)
   {
      fp->fd = fopen(filename,"r");
   }
   else
   {
      fp->fd = fdopen(0,"r");
   }
   if (fp->fd == 0)
   {
      free(fp);
      return 0;
   }
   fp->iomode = HDDM_STREAM_INPUT;
   head = (char*)malloc(1000000);
   *head = 0;
   for (p = head;
        strstr(head,"</HDDM>") == 0;
        p += strlen(p))
   {
      if (p-head < 999000)
      {
         fgets(p,1000,fp->fd);
      }
      else
      {
         break;
      }
   }
   fp->popTop = matches(head,HDDM_s_DocumentString);
   free(head);
   if (fp->popTop == 0)
   {
      fprintf(stderr,"HDDM Error: ");
      fprintf(stderr,"input template model ");
      fprintf(stderr,"does not match c code.");
      fprintf(stderr,"  Please recompile.\n");
      exit(9);
   }
   fp->filename = (char*)malloc(strlen(filename) + 1);
   strcpy(fp->filename,filename);
   fp->xdrs = (XDR*)malloc(sizeof(XDR));
   xdrstdio_create(fp->xdrs,fp->fd,XDR_DECODE);
   return fp;
}

s_iostream_t* init_s_HDDM(char* filename)
{
   int len;
   char* head;
   s_iostream_t* fp = (s_iostream_t*)malloc(sizeof(s_iostream_t));
   if (filename)
   {
      fp->fd = fopen(filename,"w");
   }
   else
   {
      fp->fd = fdopen(1,"w");
   }
   if (fp->fd == 0)
   {
      free(fp);
      return 0;
   }
   fp->iomode = HDDM_STREAM_OUTPUT;
   len = strlen(HDDM_s_DocumentString);
   head = (char*)malloc(len+1);
   strcpy(head,HDDM_s_DocumentString);
   if (fwrite(head,1,len,fp->fd) != len)
   {
      fprintf(stderr,"HDDM Error: ");
      fprintf(stderr,"error writing to ");
      fprintf(stderr,"output file %s\n",filename);
      exit(9);
   }
   fp->filename = (char*)malloc(strlen(filename) + 1);
   strcpy(fp->filename,filename);
   fp->popTop = 0;
   fp->xdrs = (XDR*)malloc(sizeof(XDR));
   xdrstdio_create(fp->xdrs,fp->fd,XDR_ENCODE);
   free(head);
   return fp;
}

void popaway(popNode* p)
{
   if (p)
   {
      int n;
      for (n = 0; n < p->popListLength; n++)
      {
         popaway(p->popList[n]);
      }
      free(p);
   }
}

void close_s_HDDM(s_iostream_t* fp)
{
   xdr_destroy(fp->xdrs);
   free(fp->xdrs);
   fclose(fp->fd);
   free(fp->filename);
   popaway(fp->popTop);
   free(fp);
}

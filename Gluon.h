#ifndef GLUON_H__INCLUDED
#define GLUON_H__INCLUDED

#include <map>
#include <vector>
#include "Particle.h"
#include "Boson.h"
#include "Molecule.h"
#include "Ensembleizer.h



class Gluon 
{

public:
    Gluon();
    ~Gluon();
    
	void loadConfigParams();
    //void* getBoson();
    bool addNewBosonToMap( Boson* b );
    bool recordNewTick( BosonId id, PropertyType p, TypeOfTick t, int size );
    bool recordNewTick( BosonId id, PropertyType p, TypeOfTick t, double price );
    
    // TODO: 
    //bool buildBosons();
    
    
    
private:
    
    // map tickerid to relevant object
    typedef std::map<BosonId, Boson*> BosonMap;
    BosonMap m_boson_map;
    BosonMap::iterator m_bmit;

	Ensembleizer* m_ptr_ensemble;
    
    Boson* getBoson( BosonId id );
    void updateDependencies( Boson* b, TypeOfTick t );
    
};


#endif
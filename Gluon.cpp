
#include "Ensembleizer.h"
#include "Gluon.h"



Gluon::Gluon()
	: m_ptr_ensemble(new Ensembleizer(this))
{
    m_boson_map.clear();
}

Gluon::~Gluon()
{
    
}

void Gluon::loadConfigParams()
{
	
	m_ptr_ensemble->loadConfigs();

}


Boson* Gluon::getBoson( BosonId id )
{
    m_bmit = m_boson_map.find(id);
    if ( m_bmit == m_boson_map.end() ) {
        std::cerr << "WARNING: Gluon.cpp: Could not find id (" 
                  << id << ") in map." << std::endl;
        return NULL;    
    }
    Boson* b = m_bmit->second;
    return b;
}
bool Gluon::addNewBosonToMap( Boson* b )
{
    BosonId id = b->getBosonId();
    m_bmit = m_boson_map.find(id);
    if ( m_bmit != m_boson_map.end() ) {
        Boson* tmp = m_bmit->second;
        std::cerr << "WARNING: Gluon.cpp: Boson (" 
                  << tmp->getBosonId() << ") already in map." << std::endl;
        return false;    
    }
    m_boson_map[id] = b;
    return true;
}


bool Gluon::recordNewTick( BosonId ticker_id, PropertyType p, TypeOfTick t, int size )
{
    if ( p != e_size_property ) {
        std::cerr << "ERROR: Gluon.cpp: Incorrect call of recordNewTick. p (" 
                  << p << ")" << std::endl;
        return false;
    }
    Boson* b = getBoson(ticker_id);
    b->recordSize(t, size);
    updateDependencies(b,t);
}
bool Gluon::recordNewTick( BosonId ticker_id, PropertyType p, TypeOfTick t, double price )
{
    if ( p != e_price_property ) {
        std::cerr << "ERROR: Gluon.cpp: Incorrect call of recordNewTick. p (" 
                  << p << ")" << std::endl;
        return false;
    }
    Boson* b = getBoson(ticker_id);
    b->recordPrice(t, price);
    updateDependencies(b,t);
}


void Gluon::updateDependencies( Boson* b, TypeOfTick t )
{
    // external call to the main computation loop.
    // for each algo (algo's have to register themselves), 
    // call the algo's updateState()
    b->updateMacroStates(t);
}





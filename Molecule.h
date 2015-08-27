#ifndef MOLECULE_H__INCLUDED
#define MOLECULE_H__INCLUDED


#include "Particle.h"

typedef std::vector<Boson*> VectorOfConsituents;
typedef string MoleculeId;

class Molecule
{

public:
    Molecule();
    ~Molecule();
    
    void updateState(TypeOfTick t);
    
    
private:
    
protected:
    //double m_prices [e_typeoftick_size];
    //int    m_sizes  [e_typeoftick_size];
    
    MoleculeId m_id;  
    VectorOfConsituents m_constituents;
    VectorOfConstituents::iterator m_vocit;
    
    //VectorOfConsituents m_constituents;
    //VectorOfConsituents::iterator m_vocit;
    
};


#endif

#include "Molecule.h"


Molecule::Molecule()
{
    m_constituents.clear();
    
    // TODO: populate m_constituents (with bosons)
}

Molecule::~Molecule()
{
    
}


void Molecule::updateState(TypeOfTick t)
{
    int i = 0;
    for (m_vocit=m_consituents.begin(); m_vocit!=m_constituents.end(); ++m_vocit)
    {
        Boson* boson = *m_vocit;
        double bid(-1.0); double ask(-1.0);
        boson->getPrices(bid,ask);
        i++;
        // TODO: do with prices the three shift operations
        // ...
    }
}
#ifndef ENSEMBLEIZER_H__INCLUDED
#define ENSEMBLEIZER_H__INCLUDED
#define NMONTHS 12

#include "Particle.h"


class Ensembleizer
{

public:
    Ensembleizer( Gluon* gluon );
    ~Ensembleizer();
    
    
    void loadConfigs();
    void createEnsembles();
    // void verify???
    
    
private:
    Gluon* m_ptr_gluon;
	char m_month_codes [NMONTHS];

	void populateMonthCodes();
    
protected:
    
};


#endif
#ifndef BOSON_H__INCLUDED
#define BOSON_H__INCLUDED


#include "Particle.h"
#include "Contract.h"

typedef std::vector<Molecule*> VectorOfMolecules;

struct ContractSpecs {
	Contract contract;	// IB specific
	std::string symbol;
	std::string sectype;
	std::string exchange;
	std::string currency;
}

class Boson
{

public:
    Boson();
    ~Boson();
    
    BosonId getBosonId();
	void setExpiryDate(time_t expDateTime);		// copy the time_t struct
	void setContractSpecs( std::string symb, std::string exch, std::string sect, std::string curr);

	void updateMacroStates(TypeOfTick t);

    void recordSize(TypeOfTick t, int size);
    void recordPrice(TypeOfTick t, double price);
    void getPriceAndSize(TypeOfTick t, int& size, double& price);
    void getMidPrice(double& price);
    void getPrices(double& bid, double& ask);
    
private:
    VectorOfMolecules m_dependents;
    VectorOfMolecules::iterator m_vomit;

	// IB specific
	void Boson::populateIBContract();
    
protected:
    double m_prices [e_typeoftick_size];
    int    m_sizes  [e_typeoftick_size];
    
    BosonId m_id;  
	time_t m_expiration_date;
	ContractSpecs m_contract_specs;
    
    //VectorOfConsituents m_constituents;
    //VectorOfConsituents::iterator m_vocit;
    
};


#endif
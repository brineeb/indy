
#include "Particle.h"
#include "Molecule.h"
#include "Boson.h"


Boson::Boson()
{
    for (int i=0; i < e_typeoftick_size; ++i) {
        m_sizes[i]  = -1;
        m_prices[i] = -1.0;
    }
    
    // TODO: set or generate an id
    // m_id = generateId( ticker );
    
    // TODO: populate m_dependents (vector of mols)
}

Boson::~Boson()
{
    
}

void Boson::recordSize( TypeOfTick t, int size )
{
    m_sizes[t] = size;
    // update anything that depends on size...
}
void Boson::recordPrice( TypeOfTick t, double price )
{
    m_prices[t] = price;
    // update anything that depends on price
    std::cout << printTimeStamp() << ", " << m_id << ", " << t 
              << ", " << price << ", " << m_sizes[t] 
              << std::endl;
}

void Boson::getPriceAndSize(TypeOfTick t, int& size, double& price)
{
    size  = m_sizes[t];
    price = m_prices[t];
}

void Boson::getMidPrice(double& price)
{
    price = -1.0;
    if ( m_prices[e_bid] > 0 && m_prices[e_ask] > 0 ) {
        price = (m_prices[e_bid] + m_prices[e_ask])*0.5;
    }
}

void Boson::getPrices(double& bid, double& ask)
{
    bid = m_prices[e_bid];
    ask = m_prices[e_ask];
}

void Boson::updateMacroStates(TypeOfTick t)
{
    for (m_vomit=m_dependents.begin(); m_vomit!=m_dependents.end(); ++m_vomit) 
    {
        Molecule* molecule = *m_vomit;
        molecule->updateState(t);
    }
}


BosonId Boson::getBosonId() { return m_id; }

void Boson::setExpiryDate(time_t expDateTime) { 
	m_expiration_date = expDateTime;
}

void Boson::setContractSpecs( std::string symb, std::string exch, std::string sect, std::string curr)
{
	m_contract_specs.symbol = symb;
	m_contract_specs.exchange = exch;
	m_contract_specs.sectype = sect;
	m_contract_specs.currency = curr;
	// IB specific
	populateIBContract();
}

// IB specific
void Boson::populateIBContract()
{
	//contract1->symbol = "VXX";	     contract1->secType = "STK";
	//contract1->exchange = "SMART";	 contract1->currency = "USD";
	//contract2->localSymbol = "VXF3"; contract2->secType = "FUT";
	//contract2->exchange = "CFE";     contract2->currency = "USD";
	m_contract_specs.contract.localSymbol = m_contract_specs.symbol;
	m_contract_specs.contract.exchange = m_contract_specs.exchange;
	m_contract_specs.contract.secType = m_contract_specs.sectype;
	m_contract_specs.contract.currency = m_contract_specs.currency;

}

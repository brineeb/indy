
#include <time.h>
#include <stdio.h>
#include "Ensembleizer.h"
#include "Boson.h"
#include "Gluon.h"


Ensembleizer::Ensembleizer( Gluon* gluon )
    : m_ptr_gluon(gluon)
{
	populateMonthCodes();
}

Ensembleizer::~Ensembleizer()
{   
}


void Ensembleizer::loadConfigs()
{
    time_t now = time( NULL);
	time_t expDateTime;
	struct tm* tm_now = localtime(&now);
	struct tm* expDate = localtime(&now);

	// TODO: create new molecules
	// new Molecule();

	// TODO: Load this from config file
	// 15/06/17		15/07/22	15/08/19	15/09/16	15/10/21	15/11/18	15/12/16	16/01/20	
	std::vector<std::string> exp_dates;
	std::vector<std::string>::iterator expdts_it;
	exp_dates.push_back("2015/06/17");
	exp_dates.push_back("2015/07/22");
	exp_dates.push_back("2015/08/19");
	exp_dates.push_back("2015/09/16");
	exp_dates.push_back("2015/10/21");
	exp_dates.push_back("2015/11/18");
	exp_dates.push_back("2015/12/16");
	exp_dates.push_back("2016/01/20");

	for (expdts_it = exp_dates.begin(); expdts_it != exp_dates.end(); ++expdts_it)
	{
		std::string ed = *expdts_it;
		// get year
		std::size_t pos;
		pos = ed.find("/");
		if (pos==std::string::npos)	std::cout << "ERROR Ensembleizer::loadConfigs Config file error. ed(" << ed << ")" << std::endl; exit(1);
		expDate->tm_year = atoi((ed.substr(0,pos)).c_str()) - 1900;
		// get month
		ed = ed.substr(++pos);
		pos = ed.find("/");
		if (pos==std::string::npos) std::cout << "ERROR Ensembleizer::loadConfigs Config file error. ed(" << ed << ")" << std::endl; exit(1);
		expDate->tm_mon = atoi((ed.substr(0,pos)).c_str()) - 1;
		// get month day
		ed = ed.substr(++pos);
		expDate->tm_mday = atoi(ed.c_str());
		// generate time_t
		expDateTime = mktime(expDate);
		//
		// create boson and load in all necessary details
		Boson* ptr_boson = new Boson();
		m_ptr_gluon->addNewBosonToMap(ptr_boson);
		ptr_boson->setExpiryDate(expDateTime);

		// TODO: get these values from config file // eg. "VXF3"
		std::string symb_prfx = "VX";
		std::string mcode = std::string(1, m_month_codes[expDate->tm_mon]);
		std::string yy = std::to_string(static_cast<long long>(expDate->tm_year + 1900));	// MS VisStudioC++ quirks
		std::string symb = symb_prfx + mcode + yy;
		ptr_boson->setContractSpecs( symb, "CFE", "FUT", "USD");
		// add molecule to boson
		// add boson to molecule

	}
	
	
}

void Ensembleizer::createEnsembles()
{
    // contract1 -> boson1 -> parent_molecule1, parent_molecule2
    // contract2 -> boson2 -> parent_molecule1
    // contract3 -> boson3 -> parent_molecule2
    // 
    // new Bosons();
    // m_ptr_gluon->addNewBosonToMap(new_boson);  // global map
    //
    // find existing parent molecule, or create a new one.
    // new Molecules();
    //
    // new_boson->addMoleculeToList(molecule);    // add molecule to boson's list of parent molecules
    // molecule->addBosonToMolecule(new_boson);  // add the boson to the molecule's list of child boson's
}





// misc
void Ensembleizer::populateMonthCodes()
{
	m_month_codes[0]  = 'F';	// january
	m_month_codes[1]  = 'G';
	m_month_codes[2]  = 'H';
	m_month_codes[3]  = 'J';
	m_month_codes[4]  = 'K';
	m_month_codes[5]  = 'M';	// June
	m_month_codes[6]  = 'N';
	m_month_codes[7]  = 'Q';
	m_month_codes[8]  = 'U';
	m_month_codes[9]  = 'V';
	m_month_codes[10] = 'X';
	m_month_codes[11] = 'Z';	// december
}
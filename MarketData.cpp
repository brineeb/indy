#include "MarketData.h"
#include "Contract.h"
#include "Order.h"

#include <fstream>
#include <ctime>

const int MIN_TICK_COUNT = 2; // minimum ticks for valid price

MarketData::MarketData() 
{
}

MarketData::~MarketData()
{
}


///////////////////////////////////////////////////////////
// member funcs
void MarketData::InitializeData(OrderId orderIdOne, OrderId orderIdTwo, OrderId orderIdThree)
{
	MktDataStruct tmp;
	double realAlpha[3];
	int alpha[3];
	time_t now = time( NULL);
	time_t expDateTime;
	struct tm* tm = localtime(&now);
	struct tm* expDate = localtime(&now);
	std::map<int, int> expiryDates;

	// 12/19	 01/15	 02/12	 03/19	 04/16	 05/21	 06/18	 07/16
	expiryDates.insert(std::pair<int, int>(12,19));
	expiryDates.insert(std::pair<int, int>(1,15));
	expiryDates.insert(std::pair<int, int>(2,12));
	expiryDates.insert(std::pair<int, int>(3,19));
	expiryDates.insert(std::pair<int, int>(4,16));
	expiryDates.insert(std::pair<int, int>(5,21));
	expiryDates.insert(std::pair<int, int>(6,18));
	expiryDates.insert(std::pair<int, int>(7,16));
	
	if (expiryDates[tm->tm_mon + 1] - tm->tm_mday > 0) {
		expDate->tm_mday = expiryDates[tm->tm_mon + 1];
	}
	else {
		if (tm->tm_mon == 11) { ++expDate->tm_year; } 
		expDate->tm_mon = tm->tm_mon + 1;
		expDate->tm_mday = expiryDates[tm->tm_mon + 2];
	}
	expDateTime = mktime(expDate);

	realAlpha[0] = 1;
	realAlpha[1] = ( difftime(expDateTime, now) / (3600 * 24) ) / 30;
	realAlpha[2] = (1-realAlpha[1]);
	alpha[0] = 1;
	alpha[1] = floor(10*realAlpha[1]+0.5);
	alpha[2] = (10 - alpha[1]);

	// Negative hedge side
	realAlpha[1] = -realAlpha[1];
	realAlpha[2] = -realAlpha[2];
	alpha[1] = -alpha[1];
	alpha[2] = -alpha[2];
	// Best rounding based on account
	printf("realAlpha1 = %f, realAlpha2 = %f -- alpha1 = %i, alpha2 = %i\n",realAlpha[1],realAlpha[2],alpha[1],alpha[2]);
	
	for (TickerId i=0; i<=2; i++) {	
		int ii = i;
		tmp.cID = i; tmp.hedgeWeight = realAlpha[ii];
		tmp.askPrice = -1; tmp.askSum = 0; tmp.askCount = 0; tmp.effectiveAsk = -1;
		tmp.bidPrice = -1; tmp.bidSum = 0; tmp.bidCount = 0; tmp.effectiveBid = -1;
		m_mktData.insert(std::pair<TickerId, MktDataStruct>(i,tmp));
	}

	//m_algoOrder.orderIsActive = 0;
	m_algoOrder.orderIsPlacing = 0;
	m_algoOrder.orderAction = 0;

	// load the saved state
	m_algoOrder.orderState = 0;

	m_algoOrder.ordersAreMade[orderIdOne] = 0;
	m_algoOrder.ordersAreMade[orderIdTwo] = 0;
	m_algoOrder.ordersAreMade[orderIdThree] = 0;


}


void MarketData::buildContractsForOrder(Contract *contract1, Contract *contract2, Contract *contract3)
{
		contract1->symbol = "VXX";	     contract1->secType = "STK";
		contract1->exchange = "SMART";	 contract1->currency = "USD";

		//contract->symbol = "VX"; contract->expiry = "201212";
		contract2->localSymbol = "VXF3"; contract2->secType = "FUT";
		contract2->exchange = "CFE";     contract2->currency = "USD";

		contract3->localSymbol = "VXG3"; contract3->secType = "FUT";
		contract3->exchange = "CFE";     contract3->currency = "USD";
}

void MarketData::decideOrderAction(Order *orderOne, Order *orderTwo, Order *orderThree) 
{
	// order.orderType = "LMT";
	// if (m_algoOrder.orderAction == 0) { } error check
	if (m_algoOrder.orderAction == 1) {
		orderOne->action = "BUY"; orderTwo->action = "SELL"; orderThree->action = "SELL"; 	} 
	else if (m_algoOrder.orderAction == -1) {
		orderOne->action = "SELL"; orderTwo->action = "BUY"; orderThree->action = "BUY"; 	}
	else {	
		return;
	}

	orderOne->totalQuantity = 2000; orderOne->orderType = "MKT"; orderOne->lmtPrice = 0;
	orderTwo->totalQuantity = 1; orderTwo->orderType = "MKT"; orderTwo->lmtPrice = 0;
	orderThree->totalQuantity = 1; orderThree->orderType = "MKT"; orderThree->lmtPrice = 0;

}

void MarketData::setPlacingOrders(OrderId orderId1, OrderId orderId2, OrderId orderId3) 
{
	m_algoOrder.orderIsPlacing = 1;
	m_algoOrder.ordersAreMade[orderId1] = 0; 
	m_algoOrder.ordersAreMade[orderId2] = 0; 
	m_algoOrder.ordersAreMade[orderId3] = 0; 
}

void MarketData::setOrderReceived(OrderId orderId) 
{
	m_algoOrder.ordersAreMade[orderId] = 1; 
}

bool MarketData::allOrdersExecuted() 
{
	std::map<TickerId, bool>::iterator ii;
	for (ii=m_algoOrder.ordersAreMade.begin(); ii!=m_algoOrder.ordersAreMade.end(); ++ii) {
		if (ii->second != 1) { return 0; }
	}
	m_algoOrder.orderIsPlacing = 0;
	return 1;
}

void MarketData::AddTick(TickerId tickerId, TickType tickType, double price)
{
	//time_t now = ::time(NULL);
	//int idx = tickerId;
	//m_mktData[tickerId].price = price;
	//m_mktData[idx].tStamp = now;

	MktDataStruct tmp = m_mktData[tickerId];
	//tmp.cID = tickerId;
	switch (tickType) {
		case BID:
			tmp.bidPrice = price;
			tmp.bidSum += price;
			tmp.bidCount += 1;
			//printf( "id=%i: tmp.bidSum=%f, tmp.bidCount=%i\n", tickerId, tmp.bidSum, tmp.bidCount);
		case ASK:
			tmp.askPrice = price;
			tmp.askSum += price;
			tmp.askCount += 1;
			//printf( "id=%i: tmp.askSum=%f, tmp.askCount=%i\n", tickerId, tmp.askSum, tmp.askCount);
		default:	
			break;
	}

	m_mktData[tickerId] = tmp;

}

bool MarketData::getTickWeightedAvg()
{
	bool gotAllData = 1;

	for (iter=m_mktData.begin(); iter!=m_mktData.end(); ++iter) {
		//printf("id=%i --\n bidCnt=%i\n",iter->second.cID,iter->second.bidCount);
		//printf("askCnt=%i\n",iter->second.askCount);
		if (iter->second.bidCount < MIN_TICK_COUNT || iter->second.askCount < MIN_TICK_COUNT) {
				gotAllData = 0;	
		} 
	}
	if (gotAllData) {
		for (iter=m_mktData.begin(); iter!=m_mktData.end(); ++iter) {
			iter->second.effectiveBid = iter->second.bidSum / iter->second.bidCount; 
			iter->second.bidSum = iter->second.effectiveBid;
			iter->second.bidCount = 1;		
			iter->second.effectiveAsk = iter->second.askSum / iter->second.askCount; 
			iter->second.askSum = iter->second.effectiveAsk;
			iter->second.askCount = 1;
		}
	}
	
	return gotAllData;
}

int MarketData::checkAlgoOne()
{
	double relativeValueLong = 0;
	double relativeValueShort = 0;
	std::ofstream myfile;

	myfile.open("output.txt", std::ios_base::out | std::ios_base::app);
	
	if (getTickWeightedAvg()) 
	{
		for (iter=m_mktData.begin(); iter!=m_mktData.end(); ++iter) {
			myfile << time( NULL) << ", " <<iter->second.cID << ", " << iter->second.effectiveAsk << ", " << iter->second.effectiveBid << ", " << iter->second.hedgeWeight << std::endl;
			if (iter->second.hedgeWeight > 0) {
				relativeValueLong  += iter->second.effectiveAsk * iter->second.hedgeWeight; 
				relativeValueShort += iter->second.effectiveBid * iter->second.hedgeWeight; }
			else {
				relativeValueLong  += iter->second.effectiveBid * iter->second.hedgeWeight;
				relativeValueShort += iter->second.effectiveAsk * iter->second.hedgeWeight; 
			}
		}

		printf( "rvLong=%f rvShort=%f\n", relativeValueLong, relativeValueShort);


		if (!m_algoOrder.orderIsPlacing) {
			//m_algoOrder.orderAction = 0;
			//if ( (relativeValueLong > 19.8) && (m_algoOrder.orderState > -1) ) {
			//	m_algoOrder.orderAction = -1; 
			//}
			//else if ( (relativeValueLong < 19.5) && (relativeValueLong > 19.3) ) {
			//	if (m_algoOrder.orderState>=1) { m_algoOrder.orderAction = -1; }
			//	else if (m_algoOrder.orderState<=-1) { m_algoOrder.orderAction = 1; }
			//}
			//else if ( (relativeValueShort < 19.0) && (m_algoOrder.orderState < 1) ) {
			//	m_algoOrder.orderAction = 1; 
			//}
			//if ( m_algoOrder.orderAction != 0 ) { return 1; }
		}
	} 

	myfile.close();

	return 0;
}



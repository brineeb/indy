
/*#include "CommonDefs.h"
#include "IBString.h"*/

#include "EWrapper.h"

//#include <ctime>
#include <map>


struct MktDataStruct {
	int cID; // Contract ID
	IBString tickType;
	double bidPrice, askPrice, bidSum, askSum;
	double effectiveBid, effectiveAsk;
	int bidCount, askCount;
	double hedgeWeight;
};

struct algoOrderInfo {
	int cID_one, cID_two, cID_three;
	bool orderIsActive, orderIsPlacing;
	int orderAction, orderState;
	std::map<TickerId, bool> ordersAreMade;
	// OrderId orderid;
	//Contract contractOne, contractTwo, contractThree;
};

class MarketData
{
public:
	MarketData();
	~MarketData();

	void AddTick(TickerId tickerId, TickType tickType, double price);
	void InitializeData(OrderId orderIdOne, OrderId orderIdTwo, OrderId orderIdThree);
	int checkAlgoOne();
	void buildContractsForOrder(Contract *contract1, Contract *contract2, Contract *contract3);
	void decideOrderAction(Order *orderOne, Order *orderTwo, Order *orderThree);
	void setPlacingOrders(OrderId orderId1, OrderId orderId2, OrderId orderId3);
	void setOrderReceived(OrderId orderId);
	bool allOrdersExecuted();


private:

	std::map<TickerId, MktDataStruct> m_mktData;
	std::map<TickerId, MktDataStruct>::iterator iter;

	algoOrderInfo m_algoOrder;

	bool getTickWeightedAvg();

};

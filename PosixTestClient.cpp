#define IB_USE_STD_STRING

#include "PosixTestClient.h"

#include "EPosixClientSocket.h"
#include "EPosixClientSocketPlatform.h"

#include "Contract.h"
#include "Order.h"

#include "Gluon.h"
#include "Ensembleizer.h"

// #include <fstream>

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds
const int SLEEP_BETWEEN_ALGOSWEEPS = 300; // seconds

// utility funcs
IBString getField( TickType tickType) {
	switch( tickType)
	{
		case BID_SIZE:	                    return "bidSize";
		case BID:		                    return "bidPrice";
		case ASK:		                    return "askPrice";
		case ASK_SIZE:	                    return "askSize";
		case LAST:		                    return "lastPrice";
		case LAST_SIZE:	                    return "lastSize";
		case HIGH:		                    return "high";
		case LOW:		                    return "low";
		case VOLUME:	                    return "volume";
		case CLOSE:		                    return "close";
		case BID_OPTION_COMPUTATION:		return "bidOptComp";
		case ASK_OPTION_COMPUTATION:		return "askOptComp";
		case LAST_OPTION_COMPUTATION:		return "lastOptComp";
		case MODEL_OPTION:					return "optionModel";
		case OPEN:                          return "open";
		case LOW_13_WEEK:                   return "13WeekLow";
		case HIGH_13_WEEK:                  return "13WeekHigh";
		case LOW_26_WEEK:                   return "26WeekLow";
		case HIGH_26_WEEK:                  return "26WeekHigh";
		case LOW_52_WEEK:                   return "52WeekLow";
		case HIGH_52_WEEK:                  return "52WeekHigh";
		case AVG_VOLUME:                    return "AvgVolume";
		case OPEN_INTEREST:                 return "OpenInterest";
		case OPTION_HISTORICAL_VOL:         return "OptionHistoricalVolatility";
		case OPTION_IMPLIED_VOL:            return "OptionImpliedVolatility";
		case OPTION_BID_EXCH:               return "OptionBidExchStr";
		case OPTION_ASK_EXCH:               return "OptionAskExchStr";
		case OPTION_CALL_OPEN_INTEREST:     return "OptionCallOpenInterest";
		case OPTION_PUT_OPEN_INTEREST:      return "OptionPutOpenInterest";
		case OPTION_CALL_VOLUME:            return "OptionCallVolume";
		case OPTION_PUT_VOLUME:             return "OptionPutVolume";
		case INDEX_FUTURE_PREMIUM:          return "IndexFuturePremium";
		case BID_EXCH:                      return "bidExch";
		case ASK_EXCH:                      return "askExch";
		case AUCTION_VOLUME:                return "auctionVolume";
		case AUCTION_PRICE:                 return "auctionPrice";
		case AUCTION_IMBALANCE:             return "auctionImbalance";
		case MARK_PRICE:                    return "markPrice";
		case BID_EFP_COMPUTATION:           return "bidEFP";
		case ASK_EFP_COMPUTATION:           return "askEFP";
		case LAST_EFP_COMPUTATION:          return "lastEFP";
		case OPEN_EFP_COMPUTATION:          return "openEFP";
		case HIGH_EFP_COMPUTATION:          return "highEFP";
		case LOW_EFP_COMPUTATION:           return "lowEFP";
		case CLOSE_EFP_COMPUTATION:         return "closeEFP";
		case LAST_TIMESTAMP:                return "lastTimestamp";
		case SHORTABLE:                     return "shortable";
		case FUNDAMENTAL_RATIOS:            return "fundamentals";
		case RT_VOLUME:                     return "RTVolume";
		case HALTED:                        return "halted";
		case BID_YIELD:                     return "bidYield";
		case ASK_YIELD:                     return "askYield";
		case LAST_YIELD:                    return "lastYield";             
		case CUST_OPTION_COMPUTATION:       return "custOptComp";
		case TRADE_COUNT:                   return "trades";
		case TRADE_RATE:                    return "trades/min";
		case VOLUME_RATE:                   return "volume/min";
		case LAST_RTH_TRADE:                return "lastRTHTrade";
		default:                            return "unknown";
	}
}



///////////////////////////////////////////////////////////
// member funcs
PosixTestClient::PosixTestClient()
	: m_pClient(new EPosixClientSocket(this))
	, m_ptr_gluon(new Gluon())
	, m_state(ST_CONNECT)
	, m_sleepDeadline(0)
	, m_orderId(0)
{
}

PosixTestClient::~PosixTestClient()
{
}

bool PosixTestClient::connect(const char *host, unsigned int port, int clientId)
{
	// trying to connect
	printf( "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);

	bool bRes = m_pClient->eConnect( host, port, clientId);

	if (bRes) {
		printf( "Connected to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
	}
	else
		printf( "Cannot connect to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);

	return bRes;
}

void PosixTestClient::disconnect() const
{
	m_pClient->eDisconnect();

	printf ( "Disconnected\n");
}

bool PosixTestClient::isConnected() const
{
	return m_pClient->isConnected();
}

bool PosixTestClient::initConfigParams()
{
	// TODO:
	m_ptr_gluon->loadConfigParams();
	
	


	//
	Contract contractOne, contractTwo, contractThree;
	int reqId;
	IBString tickList = "100,101,104,105,106,107,165,221,225,233,236,258,293,294,295,318";
	TickerId tickID;
	m_marketData->InitializeData(m_orderId, (m_orderId+1), (m_orderId+2));
	m_marketData->buildContractsForOrder(&contractOne, &contractTwo,  &contractThree);
	tickID = 0;	reqId = 0; 
	m_pClient->reqContractDetails(reqId, contractOne);
	m_pClient->reqMktData( tickID, contractOne, tickList, 0);
	tickID = 1;	reqId = 1; 
	m_pClient->reqContractDetails(reqId, contractTwo);
	m_pClient->reqMktData( tickID, contractTwo, tickList, 0);
	tickID = 2;	reqId = 2; 
	m_pClient->reqContractDetails(reqId, contractThree);
	m_pClient->reqMktData( tickID, contractThree, tickList, 0);
	m_algoSweepTrigger = time( NULL) + SLEEP_BETWEEN_ALGOSWEEPS;
	
}


// MAIN PROCESSING LOOP
void PosixTestClient::processMessages()
{
	fd_set readSet, writeSet, errorSet;

	struct timeval tval;
	tval.tv_usec = 0;
	tval.tv_sec = 0;

	time_t now = time(NULL);

	switch (m_state) {
		case ST_STARTMONITOR:
			startMonitor();
			break;
		case ST_PLACEORDER:
			placeOrder();
			break;
		case ST_PLACEORDER_ACK:
			break;
		case ST_CANCELORDER:
			cancelOrder();
			break;
		case ST_CANCELORDER_ACK:
			break;
		case ST_CHECKALGO:
			algoSweep();
			break;
		case ST_CHECKALGO_ACK:
			break;
		case ST_PING:
			reqCurrentTime();
			break;
		case ST_PING_ACK:
			if( m_sleepDeadline < now) {
				disconnect();
				return;
			}
			break;
		case ST_IDLE:
			if (m_algoSweepTrigger < now) {
				m_state = ST_CHECKALGO;
				return;
			}
			if( m_sleepDeadline < now) {
				m_state = ST_PING;
				return;
			}
			break;
	}

	if( m_sleepDeadline > 0) {
		// initialize timeout with m_sleepDeadline - now
		tval.tv_sec = m_sleepDeadline - now;
	}

	if( m_pClient->fd() >= 0 ) {

		FD_ZERO( &readSet);
		errorSet = writeSet = readSet;

		FD_SET( m_pClient->fd(), &readSet);

		if( !m_pClient->isOutBufferEmpty())
			FD_SET( m_pClient->fd(), &writeSet);

		FD_CLR( m_pClient->fd(), &errorSet);

		int ret = select( m_pClient->fd() + 1, &readSet, &writeSet, &errorSet, &tval);

		if( ret == 0) { // timeout
			return;
		}

		if( ret < 0) {	// error
			disconnect();
			return;
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &errorSet)) {
			// error on socket
			m_pClient->onError();
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &writeSet)) {
			// socket is ready for writing
			m_pClient->onSend();
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &readSet)) {
			// socket is ready for reading

			m_pClient->onReceive();
		}

	}
}

//////////////////////////////////////////////////////////////////
// methods
void PosixTestClient::reqCurrentTime()
{
	printf( "Requesting Current Time\n");

	// set ping deadline to "now + n seconds"
	m_sleepDeadline = time( NULL) + PING_DEADLINE;

	m_state = ST_PING_ACK;

	m_pClient->reqCurrentTime();

}

void PosixTestClient::placeOrder()
{
	Order orderOne, orderTwo, orderThree;
	Contract leg1, leg2, leg3;
	//TickerId tickID;

	
	m_marketData->buildContractsForOrder(&leg1, &leg2, &leg3);
	m_marketData->decideOrderAction(&orderOne, &orderTwo, &orderThree);
	m_marketData->setPlacingOrders(m_orderId, (m_orderId+1), (m_orderId+2));

	printf( "Placing Order %ld: %s %ld %s at lmt %f\n", m_orderId, orderOne.action.c_str(), orderOne.totalQuantity, leg1.symbol.c_str(), orderOne.lmtPrice);
	printf( "Placing Order %ld: %s %ld %s at lmt %f\n", (m_orderId+1), orderTwo.action.c_str(), orderTwo.totalQuantity, leg2.symbol.c_str(), orderTwo.lmtPrice);
	printf( "Placing Order %ld: %s %ld %s at lmt %f\n", (m_orderId+2), orderThree.action.c_str(), orderThree.totalQuantity, leg3.symbol.c_str(), orderThree.lmtPrice);

	m_state = ST_PLACEORDER_ACK;

	m_pClient->placeOrder(  m_orderId   , leg1, orderOne);
	m_pClient->placeOrder( (m_orderId+1), leg2, orderTwo);
	m_pClient->placeOrder( (m_orderId+2), leg3, orderThree);
}

void PosixTestClient::cancelOrder()
{
	printf( "Cancelling Order %ld\n", m_orderId);

	m_state = ST_CANCELORDER_ACK;

	m_pClient->cancelOrder( m_orderId);
}

///////////////////////////////////////////////////////////////////
// events
void PosixTestClient::orderStatus( OrderId orderId, const IBString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const IBString& whyHeld)

{
	//if( orderId == m_orderId) {
		//if( m_state == ST_PLACEORDER_ACK && (status == "PreSubmitted" || status == "Submitted"))
			//m_state = ST_CANCELORDER;

		//if( m_state == ST_CANCELORDER_ACK && status == "Cancelled")
		//	m_state = ST_PING;
	//}

	printf( "Order: id=%ld, status=%s\n", orderId, status.c_str());

	if ( (orderId == m_orderId) || (orderId == m_orderId+1) || (orderId == m_orderId+2) ) {
		if (status == "Filled") {
			m_marketData->setOrderReceived(orderId);
		}
	
		if (m_marketData->allOrdersExecuted()) {
			printf( "All orders placed\n" );
			m_state = ST_IDLE;
		}
	}
	
}

void PosixTestClient::nextValidId( OrderId orderId)
{
	m_orderId = orderId;

	m_state = ST_STARTMONITOR;
}

void PosixTestClient::currentTime( long time)
{
	if ( m_state == ST_PING_ACK) {
		time_t t = ( time_t)time;
		struct tm * timeinfo = localtime ( &t);
		printf( "The current date/time is: %s", asctime( timeinfo));

		time_t now = ::time(NULL);
		m_sleepDeadline = now + SLEEP_BETWEEN_PINGS;

		m_state = ST_IDLE;
	}
}

void PosixTestClient::error(const int id, const int errorCode, const IBString errorString)
{
	printf( "Error id=%d, errorCode=%d, msg=%s\n", id, errorCode, errorString.c_str());

	if( id == -1 && errorCode == 1100) // if "Connectivity between IB and TWS has been lost"
		disconnect();
}


// Non-IB functions
void PosixTestClient::startMonitor()
{
	Contract contractOne, contractTwo, contractThree;
	int reqId;
	IBString tickList = "100,101,104,105,106,107,165,221,225,233,236,258,293,294,295,318";
	TickerId tickID;

	
	m_marketData->InitializeData(m_orderId, (m_orderId+1), (m_orderId+2));

	m_marketData->buildContractsForOrder(&contractOne, &contractTwo,  &contractThree);

	tickID = 0;	reqId = 0; 
	m_pClient->reqContractDetails(reqId, contractOne);
	m_pClient->reqMktData( tickID, contractOne, tickList, 0);
	
	//m_pClient->reqHistoricalData(tickID, contractOne, "20121208 12:00:00", "2 D", "1 day", "BID", 1, 1);

	tickID = 1;	reqId = 1; 
	m_pClient->reqContractDetails(reqId, contractTwo);
	m_pClient->reqMktData( tickID, contractTwo, tickList, 0);
	
	tickID = 2;	reqId = 2; 
	m_pClient->reqContractDetails(reqId, contractThree);
	m_pClient->reqMktData( tickID, contractThree, tickList, 0);

	m_algoSweepTrigger = time( NULL) + SLEEP_BETWEEN_ALGOSWEEPS;

	m_state = ST_IDLE;
}

void PosixTestClient::algoSweep() 
{
	m_state = ST_CHECKALGO_ACK;
	m_algoSweepTrigger = time( NULL) + SLEEP_BETWEEN_ALGOSWEEPS;

	m_ptr_gluon->wallTimeCheckInterval();
	//if ( (m_marketData->checkAlgoOne()) != 0 ) {
		//m_state = ST_PLACEORDER; 
	//}
	
	m_state = ST_IDLE;
}


///////////////////
// More events
void PosixTestClient::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute)
{
	
	m_marketData->AddTick(tickerId, tickType, price);
	

	//printf( "id=%i  %s=%f  canAutoExecute=%d\n", tickerId, ((const IBString)getField(tickType)).c_str(), price, canAutoExecute);
	
}

void PosixTestClient::tickSize( TickerId tickerId, TickType tickType, int size)
{
	
	//printf( "id=%i %s=%i\n", tickerId, ((const IBString)getField(tickType)).c_str(), size);
}

void PosixTestClient::contractDetails( int reqId, const ContractDetails& contractDetails) 
{

	printf("id=%i name=%s lname=%s exch=%s \nmonth=%s hours=%s\n", reqId, contractDetails.marketName.c_str(), 
		      contractDetails.longName.c_str(), contractDetails.validExchanges.c_str(), 
			  contractDetails.contractMonth.c_str(), contractDetails.tradingHours.c_str());
	printf( "underConId=%i \n", contractDetails.underConId );
}


void PosixTestClient::historicalData(TickerId reqId, const IBString& date, double open, double high,
									  double low, double close, int volume, int barCount, double WAP, int hasGaps) 
{
	//std::fstream myfile;

	//myfile.open("histData.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	//myfile << reqId << ", " << date << ", " << open << ", " << close << ", " << WAP << ", " << hasGaps << std::endl;
	//myfile.close();
}


// Inherited needing overwrite
void PosixTestClient::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
											 double optPrice, double pvDividend,
											 double gamma, double vega, double theta, double undPrice) {}
void PosixTestClient::tickGeneric(TickerId tickerId, TickType tickType, double value) {}
void PosixTestClient::tickString(TickerId tickerId, TickType tickType, const IBString& value) {}
void PosixTestClient::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints,
							   double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry) {}
void PosixTestClient::openOrder( OrderId orderId, const Contract&, const Order&, const OrderState& ostate) {}
void PosixTestClient::openOrderEnd() {}
void PosixTestClient::winError( const IBString &str, int lastError) {}
void PosixTestClient::connectionClosed() {}
void PosixTestClient::updateAccountValue(const IBString& key, const IBString& val,
										  const IBString& currency, const IBString& accountName) {}
void PosixTestClient::updatePortfolio(const Contract& contract, int position,
		double marketPrice, double marketValue, double averageCost,
		double unrealizedPNL, double realizedPNL, const IBString& accountName){}
void PosixTestClient::updateAccountTime(const IBString& timeStamp) {}
void PosixTestClient::accountDownloadEnd(const IBString& accountName) {}
void PosixTestClient::bondContractDetails( int reqId, const ContractDetails& contractDetails) {}
void PosixTestClient::contractDetailsEnd( int reqId) {}
void PosixTestClient::execDetails( int reqId, const Contract& contract, const Execution& execution) {}
void PosixTestClient::execDetailsEnd( int reqId) {}

void PosixTestClient::updateMktDepth(TickerId id, int position, int operation, int side,
									  double price, int size) {}
void PosixTestClient::updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation,
										int side, double price, int size) {}
void PosixTestClient::updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch) {}
void PosixTestClient::managedAccounts( const IBString& accountsList) {}
void PosixTestClient::receiveFA(faDataType pFaDataType, const IBString& cxml) {}
void PosixTestClient::scannerParameters(const IBString &xml) {}
void PosixTestClient::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const IBString &distance, const IBString &benchmark, const IBString &projection,
	   const IBString &legsStr) {}
void PosixTestClient::scannerDataEnd(int reqId) {}
void PosixTestClient::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
								   long volume, double wap, int count) {}
void PosixTestClient::fundamentalData(TickerId reqId, const IBString& data) {}
void PosixTestClient::deltaNeutralValidation(int reqId, const UnderComp& underComp) {}
void PosixTestClient::tickSnapshotEnd(int reqId) {}
void PosixTestClient::marketDataType(TickerId reqId, int marketDataType) {}
void PosixTestClient::commissionReport( const CommissionReport& commissionReport) {}


#pragma once
#include "Parameters.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

using namespace std;


class InterfaceOrder {
public:
	InterfaceOrder();
	InterfaceOrder(int ID_in, int ClientID_in, int Category_in, int PriceType_in, char* Zqdm, float Price_in, int Quantity_in, char* Result_in, char* ErrInfo_in);
	~InterfaceOrder();

	void setRequest(OrderRequest req);
	int setID(string result);

	static string shareHolderCode;
	OrderRequest req;	
	string orderId;
	int tradedQuantity;
};

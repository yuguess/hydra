#include "stdafx.h"
#include "InterfaceOrder.h"


string InterfaceOrder::shareHolderCode;

InterfaceOrder::InterfaceOrder() {
	tradedQuantity = 0;
}



InterfaceOrder::~InterfaceOrder()
{
}

int InterfaceOrder::setID(string result) {
	this->orderId = result;
	return 0;
}



void InterfaceOrder::setRequest(OrderRequest req) {
	this->req = req;
}


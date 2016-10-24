#pragma once
#include "stdafx.h"

#include "QueueMap.h"
//开发文档
//

//1.交易API均是TdxApi.dll文件的导出函数，包括以下函数：
//void   OpenTdx(); 
//void   CloseTdx();
//   int  Logon(char* IP, short Port, char* Version, char* AccountNo,char* JyPassword,   char* TxPassword, char* ErrInfo);
// void  Logoff(int ClientID);
// void  QueryData(int ClientID, int Category, char* Result, char* ErrInfo);
//void   QueryHistoryData(int ClientID, int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);
// void  QueryDatas(int ClientID, int Category[], int Count, char* Result[], char* ErrInfo[]);
// void  SendOrder(int ClientID, int Category ,long PriceType,  char* Gddm,  char* Zqdm , float Price, long Quantity,  char* Result, char* ErrInfo);
// void  SendOrders(int ClientID, int Category[] , ,long PriceType[], char* Gddm[],  char* Zqdm[] , float Price[], long Quantity[],  int Count, char* Result[], char* ErrInfo[]);
// void  CancelOrder(int ClientID, char* hth, char* Result, char* ErrInfo);
// void  CancelOrders(int ClientID, char* hth[], int Count, char* Result[], char* ErrInfo[]);
// void  CancelOrder(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo);//招商证券普通账户版本
// void  CancelOrders(int ClientID, char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);//招商证券普通账户版本
// void  GetQuote(int ClientID, char* Zqdm, char* Result, char* ErrInfo);
// void  GetQuotes(int ClientID, char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);
// void  Repay(int ClientID, char* Amount, char* Result, char* ErrInfo);
// void  GetEdition(char* Result);

// void  QueryHKData(int ClientID, int Category, char* Result, char* ErrInfo);

//2.API使用流程为: 应用程序先调用Init初始化通达信实例，一个通达信实例下可以同时登录多个交易账户，每个交易账户称之为客户端, 通过ClientID区分各个客户端.
//通过调用Logon获得ClientID，然后可以调用其他API函数进行查询或下单; 应用程序退出时应调用Logoff注销客户端, 最后调用Exit退出通达信实例. 
//Init和Exit被在整个应用程序中只能被调用一次.

//3. 各个函数功能说明

/// <summary>
/// 初始化通达信实例
/// </summary>
typedef void(__stdcall* OpenTdxDelegate)();


/// <summary>
/// 退出通达信实例
/// </summary>
typedef void(__stdcall* CloseTdxDelegate)();


/// <summary>
/// 交易账户登录
/// </summary>
/// <param name="IP">交易服务器IP</param>
/// <param name="Port">交易服务器端口</param>
/// <param name="Version">用来设置通达信的版本号，如果设的太低，券商服务器会认为使用的通达信软件太旧要升级</param>
/// <param name="AccountNo">资金账号</param>
/// <param name="JyPassword">交易密码</param>
/// <param name="TxPassword">通讯密码</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。</param>
/// <returns>客户端ID，失败时返回-1</returns>
//typedef int(__stdcall* Tdx_LoginDelegate)(char* AccountNo, char* JyPassword, char* TxPassword, char* ErrInfo);
#ifdef TEST
typedef int(__stdcall* LogonDelegate)(char* IP, short Port, char* Version, short YybID, char* AccountNo, char* TradeAccount, char* JyPassword, char* TxPassword, char* ErrInfo);
#else
typedef int(__stdcall* LogonDelegate)(char* IP, short Port, char* Version, short YybID, unsigned char LoginMode, char* AccountNo, char* TradeAccount, char* JyPassword, char* TxPassword, char* ErrInfo);
#endif

/// <summary>
/// 交易账户注销
/// </summary>
/// <param name="ClientID">客户端ID</param>
typedef void(__stdcall* LogoffDelegate)(int ClientID);

/// <summary>
/// 查询各种交易数据
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示查询信息的种类，0资金  1股份   2当日委托  3当日成交     4撤单   5股东代码  6融资余额   7融券余额  8可融证券</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* QueryDataDelegate)(int ClientID, int Category, char* Result, char* ErrInfo);



/// <summary>
/// 查询港股通各种交易数据
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示查询信息的种类，0资金  1股份   2当日委托  3当日成交     4撤单   5股东代码  6融资余额   7融券余额  8可融证券</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* QueryHKDataDelegate)(int ClientID, int Category, char* Result, char* ErrInfo);


/// <summary>
/// 查询各种历史数据
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示查询信息的种类，0历史委托  1历史成交   2交割单</param>
/// <param name="StartDate">表示开始日期，格式为yyyyMMdd,比如2014年3月1日为  20140301
/// <param name="EndDate">表示结束日期，格式为yyyyMMdd,比如2014年3月1日为  20140301
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* QueryHistoryDataDelegate)(int ClientID, int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);

/// <summary>
/// 查询各种港股通历史数据
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示查询信息的种类，0历史委托  1历史成交 </param>
/// <param name="StartDate">表示开始日期，格式为yyyyMMdd,比如2014年3月1日为  20140301
/// <param name="EndDate">表示结束日期，格式为yyyyMMdd,比如2014年3月1日为  20140301
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* QueryHKHistoryDataDelegate)(int ClientID, int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);

/// <summary>
/// 批量查询各种交易数据,用数组传入每个委托的参数，数组第i个元素表示第i个查询的相应参数
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">信息的种类的数组, 第i个元素表示第i个查询的信息种类，0资金  1股份   2当日委托  3当日成交     4撤单   5股东代码  6融资余额   7融券余额  8可融证券</param>
/// <param name="Count">查询的个数，即数组的长度</param>
/// <param name="Result">返回数据的数组, 第i个元素表示第i个委托的返回信息. 此API执行返回后，Result[i]内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">错误信息的数组，第i个元素表示第i个委托的错误信息. 此API执行返回后，如果出错，ErrInfo[i]保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* QueryDatasDelegate)(int ClientID, int Category[], int Count, char* Result[], char* ErrInfo[]);



/// <summary>
/// 下委托交易证券
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示委托的种类，0买入 1卖出  2融资买入  3融券卖出   4买券还券   5卖券还款  6现券还券</param>
/// <param name="PriceType">表示报价方式 0  上海限价委托 深圳限价委托 1深圳对方最优价格  2深圳本方最优价格  3深圳即时成交剩余撤销  4上海五档即成剩撤 深圳五档即成剩撤 5深圳全额成交或撤销 6上海五档即成转限价
/// <param name="Gddm">股东代码</param>
/// <param name="Zqdm">证券代码</param>
/// <param name="Price">委托价格</param>
/// <param name="Quantity">委托数量</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* SendOrderDelegate)(int ClientID, int Category, int PriceType, char* Gddm, char* Zqdm, float Price, int Quantity, char* Result, char* ErrInfo);


/// <summary>
/// 下港股通委托交易港股
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">表示委托的种类，0买入 1卖出</param>
/// <param name="PriceType">表示报价方式  2竞价限价盘 3增强限价盘
/// <param name="Zqdm">证券代码</param>
/// <param name="Price">委托价格</param>
/// <param name="Quantity">委托数量</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* SendHKOrderDelegate)(int ClientID, int Category, int PriceType, int QuantityType, char* Gddm, char* Zqdm, char* Price, char* Quantity, char* Result, char* ErrInfo);


/// <summary>
/// 批量下委托交易证券，用数组传入每个委托的参数，数组第i个元素表示第i个委托的相应参数
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Category">委托种类的数组，第i个元素表示第i个委托的种类，0买入 1卖出  2融资买入  3融券卖出   4买券还券   5卖券还款  6现券还券</param>
/// <param name="PriceType">表示报价方式的数组,  第i个元素表示第i个委托的报价方式, 0  上海限价委托 深圳限价委托 1深圳对方最优价格  2深圳本方最优价格  3深圳即时成交剩余撤销  4上海五档即成剩撤 深圳五档即成剩撤 5深圳全额成交或撤销 6上海五档即成转限价
/// <param name="Gddm">股东代码数组，第i个元素表示第i个委托的股东代码</param>
/// <param name="Zqdm">证券代码数组，第i个元素表示第i个委托的证券代码</param>
/// <param name="Price">委托价格数组，第i个元素表示第i个委托的委托价格</param>
/// <param name="Quantity">委托数量数组，第i个元素表示第i个委托的委托数量</param>
/// <param name="Count">委托的个数，即数组的长度</param>
/// <param name="Result">返回数据的数组，第i个元素表示第i个委托的返回信息。此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">错误信息的数组，第i个元素表示第i个委托的错误信息。此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* SendOrdersDelegate)(int ClientID, int Category[], int PriceType[], char* Gddm[], char* Zqdm[], float Price[], int Quantity[], int Count, char* Result[], char* ErrInfo[]);


/// <summary>
/// 撤委托
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Gddm">股东代码</param>
/// <param name="ExchangeID">交易所ID，申万和东吴 东兴上海1，深圳0  ； 招商上海1，深圳2</param>
/// <param name="hth">表示要撤的目标委托的编号</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
///typedef void(__stdcall* CancelOrderDelegate)(int ClientID, char* hth, char* Result, char* ErrInfo);//招商证券信用版本
typedef void (WINAPI* CancelOrderDelegate)(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo);//招商证券普通版本
//typedef void (WINAPI* CancelOrderDelegate)(int ClientID,char* Gddm, char* ExchangeID, char* hth, char* Result, char* ErrInfo);


/// <summary>
/// 港股通撤委托
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Gddm">股东代码</param>
/// <param name="ExchangeID">交易所ID，申万和东吴 东兴上海1，深圳0  ； 招商上海1，深圳2</param>
/// <param name="hth">示要撤的目标委托的编号</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* CancelHKOrderDelegate)(int ClientID, char* hth, char* Result, char* ErrInfo);


/// <summary>
/// 批量撤委托, 用数组传入每个委托的参数，数组第i个元素表示第i个撤委托的相应参数
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Gddm">股东代码</param>
/// <param name="ExchangeID">交易所ID，申万和东吴上海1，深圳0  ； 招商上海1，深圳2</param>
/// <param name="hth">示要撤的目标委托的编号</param>
/// <param name="Count">委托的个数，即数组的长度</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
///typedef void(__stdcall* CancelOrdersDelegate)(int ClientID, char* hth[], int Count, char* Result[], char* ErrInfo[]);//招商证券信用版本
typedef void (WINAPI* CancelOrdersDelegate)(int ClientID, char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);//招商证券普通版本
//typedef void (WINAPI* CancelOrdersDelegate)(int ClientID, char* Gddm[], char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);

/// <summary>
/// 获取证券的实时五档行情
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Zqdm">证券代码</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* GetQuoteDelegate)(int ClientID, char* Zqdm, char* Result, char* ErrInfo);


/// <summary>
/// 获取港股的相关交易信息
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Zqdm">证券代码</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* GetHKStockTradeInfoDelegate)(int ClientID, int Category, char* Zqdm, char* Result, char* ErrInfo);

/// <summary>
/// 批量获取证券的实时五档行情
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Zqdm">证券代码</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* GetQuotesDelegate)(int ClientID, char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);


/// <summary>
/// 融资融券直接还款
/// </summary>
/// <param name="ClientID">客户端ID</param>
/// <param name="Amount">还款金额</param>
/// <param name="Result">此API执行返回后，Result内保存了返回的查询数据, 形式为表格数据，行数据之间通过\n字符分割，列数据之间通过\t分隔。一般要分配1024*1024字节的空间。出错时为空字符串。</param>
/// <param name="ErrInfo">此API执行返回后，如果出错，保存了错误信息说明。一般要分配256字节的空间。没出错时为空字符串。请注意演示版也用此显示功能限制信息。</param>
typedef void(__stdcall* RepayDelegate)(int ClientID, char* Amount, char* Result, char* ErrInfo);





class CTdxApi
{
public:
	CTdxApi(void);
	~CTdxApi(void);

	HMODULE m_TdxApiHMODULE;

	OpenTdxDelegate OpenTdx;
	CloseTdxDelegate CloseTdx;
	LogonDelegate Logon;
	LogoffDelegate Logoff;
	QueryDataDelegate QueryData;
	QueryDatasDelegate QueryDatas;
	QueryHistoryDataDelegate QueryHistoryData;
	SendOrderDelegate SendOrder;
	SendOrdersDelegate SendOrders;
	CancelOrderDelegate CancelOrder;
	CancelOrdersDelegate CancelOrders;
	GetQuoteDelegate GetQuote;
	GetQuotesDelegate GetQuotes;
	RepayDelegate Repay;


	QueryHKDataDelegate QueryHKData;
	QueryHKHistoryDataDelegate QueryHKHistoryData;
	GetHKStockTradeInfoDelegate GetHKStockTradeInfo;
	SendHKOrderDelegate SendHKOrder;
	CancelHKOrderDelegate CancelHKOrder;
};


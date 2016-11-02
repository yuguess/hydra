#ifndef _PARAMETER
#define _PARAMETER

#define CP_LENGTH 50

#define DISCONNECTED 0
#define CONNECTED 1

#define GDDM_SIZE 15
#define ZQDM_SIZE 15

#define STRING_HARD_LIMIT 0xFFFFFFFF

#define HU_HOLDER_CODE 0
#define SHEN_HOLDER_CODE 1

#define QUE_CODE_SHOW_BALANCE 0
#define QUE_CODE_CHECK_ORDER 2
#define QUE_CODE_CHECK_TRANSAC 3
#define QUE_CODE_CHECK_CANCELLABLE 4
#define QUE_CODE_HOLDER 5

#define RESULT_SIZE	1024*1024
#define RESULT_SIZE_MAIN RESULT_SIZE
#define RESULT_SIZE_QUEUEU RESULT_SIZE
#define RESULT_SIZE_SENDORDER RESULT_SIZE
#define ERROR_INFO_SIZE 256

#define ORDER_BUF_INIT_SIZE 1024

#define SH_EXC_API "1"
#define SZ_EXC_API "0"

#define NONE_ERROR 0
#define SEND_ERROR 1
#define CANCEL_ERROR 2
#define BAD_ORDER_ERROR 3
#define DEAL 4
#define CANCELLED 5
#define PARTIALLY_CANCELLED 6
#define PARTIALLY_DEAL 7

#define PRICE_TYPE_LIMIT 0
#define PRICE_TYPE_MARKET 4

#define BROKER_CODE_GUOJIN 1
#define BROKER_CODE_ZHONGXIN 2
#define BROKER_CODE_GTJA 3

#define BROKER_CODE_TEST 555

#endif 
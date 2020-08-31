//..\tcc32.exe -L. TradeX2-M.dl -run hq.c

//#include "stdio.h"
#include "tx_tool.c"

//180.153.18.170 @ref https://github.com/iorj/tdxhq-1/blob/master/tdx/hq/tdxhq.py
//14.17.75.71 @ref ./TradeX2Demo-M/TradeXDemo-M.cpp
//101.227.77.254 @ref https://github.com/plenari/economic/blob/187f11b32d03cea8479ac8ba643eb9ecd24b0eab/quantaxiss/QA_Trade_stock_query.py

//or check from "connect.cfg" of the software by broker !!!

//..\ffic_static_32.exe txhq.c
//..\tcc32 -I.. -L.. -run txhq.c
void main(int argc,char* argv[]) {

	tx_ffic();

	tx_a = tx_a_hq;

	if(argc<2){
		//char * argv_[] = {"conn","139.159.195.177","7709"};
		char * argv_[] = {"conn","114.118.82.209","7709"};
		argv = argv_;
		argc = tx_arr_len(argv_);
	}

	tx_call("conn",argv,argc,NULL);

	tx_thread(&handle_stdin,0);

	//long c = 0;
	while(!tx_flag_quit){
		tx_sleep(5000);
		if(nConn>=0){
			int count = hq_check_conn(NULL,0,NULL);
			if( count<=0 ){
				tx_flag_quit = 1;
				c_fprintf(stderr,"# quit for count %d\n", count);
			}else{
				//c_fprintf(stderr,"# count %d\n", count);
				c_fprintf(stderr,".");
			}
			c_fflush(stderr);
		}
	}
	c_fprintf(stderr,"# exit\n");
	c_fflush(stderr);
}


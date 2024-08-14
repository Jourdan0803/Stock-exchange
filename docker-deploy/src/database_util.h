#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>

using namespace std;
using namespace pqxx;

#ifndef _DATABASE_UTIL_
#define _DATABASE_UTIL_

class Order{
    private:
        string status;
        int order_id;
        int account_id;
        double amount;
        double price;
        string time;
    
    public:
        Order(string status, int order_id, int account_id, double amount, double price, string time): 
        status(status), order_id(order_id), account_id(account_id), amount(amount), price(price), time(time){};

        string toString();

        string get_status(){
            return status;
        }

        int get_order_id(){
            return order_id;
        }

        int get_account_id(){
            return account_id;
        }

        double get_amount(){
            return amount;
        }

        double get_price(){
            return price;
        }

        string get_time(){
            return time;
        }
};

// string add_escape_to_str(string str);

// vector<string> split_string(string str, const char * delimiter);
string add_original_order_string(int account_id, string symbol, double amount, double price_limit, string receive_time);
string add_executed_order_string(int order_id, double amount, double price, string execution_time);
string add_open_string(int order_id, double amount);
string add_position_string(int account_id, string symbol, double amount);


string get_order_id_string(int account_id, string symbol, double amount, double price_limit, string receive_time);
string get_potential_match_string(string symbol, double amount, double cur_order_price_limit);
string get_position_string(int account_id, string symbol);
string get_symbol_string(string symbol);

string update_account_balance_string(int account_id, string symbol, double new_balance);
string update_position_string(int account_id, string symbol, double amount);

string delete_open_order_string(int order_id);



vector<Order> process_result_into_order_list(pqxx::result result);
vector<Order> process_result_for_open_request(pqxx::result r);
void add_execution_to_vector(vector<Order> * order_list, pqxx::result r, string status);
void add_cancel_to_vector(vector<Order> * order_list, pqxx::result r, string status);

double process_result_into_double(pqxx::result r);
int process_result_into_int(pqxx::result r);

bool check_result_exist(pqxx::result result);

void create_database(string password);
connection * create_database_connection(string password);
void close_database_connection(connection * C);

string get_UTC_time_now();
void add_account_db(connection *C, int account_id, double balance);
void add_stock_symbol_db(connection *C, string symbol);
void add_position_db(connection *C, int account_id, string symbol, double amount);
int add_original_order_db(connection *C, int account_id, string symbol, double amount, double price_limit, string receive_time);
void add_open_db(connection *C, int order_id, double amount);
void add_executed_db(connection *C, int order_id, double amount, double price, string execution_time);
string add_canceled_db(connection *C, int order_id, double amount, string cancel_time);

void update_position_db(connection * C, int account_id, string symbol, double amount);

void match_order(connection *C, int order_id, int account_id, string symbol, double amount, double cur_order_price_limit);

void print_result(pqxx::result r);
// pqxx::result get_position_db(connection *C, int account_id, string symbol);
void print_position_db(connection *C, int account_id, string symbol);
// void print_position(connection *C, int account_id, string symbol);
void print_account_balance(connection *C, int account_id);
string insert_or_update_position_string(int account_id, string symbol, double amount);

string get_open_string();
string get_executed_string();
string get_original_order_string();
void print_open_table_db(connection *C);
void print_executed_table_db(connection *C);
void print_original_order_table_db(connection *C);


vector<Order> get_query_db(connection *C, int trans_id, int account_id);
void cancel_order(connection *C, int trans_id, int account_id);
void check_account_exist(connection*C, int account_id);

#endif //_DATABASE_UTIL_



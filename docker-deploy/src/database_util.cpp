#include "database_util.h"

//ss << "SELECT original_order.order_id, account_id, open.amount, price_limit, receive_time ";

string Order::toString(){
  std::stringstream ss;
  ss << "order_id = " << order_id << "\n";
  ss << "account_id = " << account_id << "\n";
  ss << "amount = " << amount << "\n";
  ss << "price = " << price << "\n";
  ss << "time = " << time << "\n";
  return ss.str();
  }

vector<Order> process_result_into_order_list(pqxx::result r){
  vector<Order> order_list;
  for (auto row = r.begin(); row != r.end(); row++)
    {
      auto field = row.begin();
      int order_id = stoi(field->c_str());
      field ++;
      int account_id = stoi(field->c_str());
      field ++;
      double open_amount = stod(field->c_str());
      field ++;
      double price_limit  = stod(field->c_str());
      field ++;
      string receive_time  = field->c_str();
      Order new_order = Order("" , order_id, account_id, open_amount, price_limit, receive_time);
      order_list.push_back(new_order);
    }
  return order_list;
}

vector<Order> process_result_for_open_request(pqxx::result r){
  vector<Order> order_list;
  for (auto row = r.begin(); row != r.end(); row++)
    {
      auto field = row.begin();
      int order_id = stoi(field->c_str());
      field ++;
      double amount = stod(field->c_str());
      // Order(string status, int order_id, int account_id, double open_amount, double price_limit, string receive_time)
      Order new_order = Order("open" , order_id, 0, amount, 0, "");
      order_list.push_back(new_order);
    }
  return order_list;
}

void add_execution_to_vector(vector<Order> * order_list, pqxx::result r, string status){
  for (auto row = r.begin(); row != r.end(); row++)
    {
      auto field = row.begin();
      int order_id = stoi(field->c_str());
      field ++;
      double amount = stod(field->c_str());
      field ++;
      double price  = stod(field->c_str());
      field ++;
      string time  = field->c_str();
      Order new_order = Order(status, order_id, 0, amount, price, time);
      order_list->push_back(new_order);
    }
}

void add_cancel_to_vector(vector<Order> * order_list, pqxx::result r, string status){
  for (auto row = r.begin(); row != r.end(); row++)
    {
      auto field = row.begin();
      int order_id = stoi(field->c_str());
      field ++;
      double amount = stod(field->c_str());
      field ++;
      string time  = field->c_str();
      Order new_order = Order(status, order_id, 0, amount, 0, time);
      order_list->push_back(new_order);
    }
}

double process_result_into_double(pqxx::result r){
  if (!check_result_exist(r)){
    throw invalid_argument("result is empty");
  }
  return stod(r.begin().begin()->c_str());
}

int process_result_into_int(pqxx::result r){
  if (!check_result_exist(r)){
    throw invalid_argument("result is empty");
  }
  return stoi(r.begin().begin()->c_str());
}

bool check_result_exist(pqxx::result r){
  int row_count = 0;
  for (auto row = r.begin(); row != r.end(); row++){
      row_count++;
    }
  if (row_count == 0){
    return false;
  }
  else {
    return true;
  }
}

void close_database_connection(connection * C){
  C->disconnect();
}

void create_database(string password){
  //Allocate & initialize a Postgres connection object
  connection *C;

  // try{
    //Establish a connection to the database
    C = new connection("host=db port=5432 dbname=stock_exchange user=postgres password=" + password);
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
    }
  // } catch (const std::exception &e){
  //   cerr << e.what() << std::endl;
  // }

  // drop tables first
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  txn.exec("DROP TABLE IF EXISTS open");
  txn.exec("DROP TABLE IF EXISTS executed");
  txn.exec("DROP TABLE IF EXISTS canceled");
  txn.exec("DROP TABLE IF EXISTS position");
  txn.exec("DROP TABLE IF EXISTS original_order");
  txn.exec("DROP TABLE IF EXISTS account");
  txn.exec("DROP TABLE IF EXISTS stock_symbol");
  
  //create tables
  // txn.exec("CREATE TABLE account (account_id INT NOT NULL, balance DECIMAL NOT NULL, PRIMARY KEY (account_id));");
  // txn.exec("CREATE TABLE stock_symbol (symbol VARCHAR(30) NOT NULL, PRIMARY KEY (symbol));");
  // txn.exec("CREATE TABLE position (account_id INT NOT NULL, symbol VARCHAR(30) NOT NULL, amount DECIMAL NOT NULL, PRIMARY KEY (account_id, symbol), FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE, FOREIGN KEY (symbol) REFERENCES stock_symbol(symbol) ON DELETE SET NULL ON UPDATE CASCADE);");
  // txn.exec("CREATE TABLE original_order (order_id SERIAL, account_id INT NOT NULL, symbol VARCHAR(30) NOT NULL, amount DECIMAL NOT NULL, price_limit DECIMAL NOT NULL, receive_time TIMESTAMP NOT NULL,  PRIMARY KEY (order_id), FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE, FOREIGN KEY (symbol) REFERENCES stock_symbol(symbol) ON DELETE SET NULL ON UPDATE CASCADE);");
  // txn.exec("CREATE TABLE open (order_id INT NOT NULL, amount DECIMAL NOT NULL, PRIMARY KEY (order_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id));");
  // txn.exec("CREATE TABLE executed (execution_id SERIAL, order_id INT NOT NULL, amount DECIMAL NOT NULL, price DECIMAL NOT NULL, execution_time TIMESTAMP NOT NULL,  PRIMARY KEY (execution_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id) ON DELETE SET NULL ON UPDATE CASCADE);");
  // txn.exec("CREATE TABLE canceled (order_id INT NOT NULL, amount DECIMAL NOT NULL, cancel_time TIMESTAMP NOT NULL,  PRIMARY KEY (order_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id) ON DELETE SET NULL ON UPDATE CASCADE);");
  // txn.commit();
  txn.exec("CREATE TABLE account (account_id INT NOT NULL, balance DECIMAL NOT NULL, PRIMARY KEY (account_id));");
  txn.exec("CREATE TABLE stock_symbol (symbol VARCHAR(30) NOT NULL, PRIMARY KEY (symbol));");
  txn.exec("CREATE TABLE position (account_id INT NOT NULL, symbol VARCHAR(30) NOT NULL, amount DECIMAL NOT NULL, PRIMARY KEY (account_id, symbol), FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE, FOREIGN KEY (symbol) REFERENCES stock_symbol(symbol) ON DELETE SET NULL ON UPDATE CASCADE);");
  txn.exec("CREATE TABLE original_order (order_id SERIAL, account_id INT NOT NULL, symbol VARCHAR(30) NOT NULL, amount DECIMAL NOT NULL, price_limit DECIMAL NOT NULL, receive_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  PRIMARY KEY (order_id), FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE, FOREIGN KEY (symbol) REFERENCES stock_symbol(symbol) ON DELETE SET NULL ON UPDATE CASCADE);");
  txn.exec("CREATE TABLE open (order_id INT NOT NULL, amount DECIMAL NOT NULL, PRIMARY KEY (order_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id));");
  txn.exec("CREATE TABLE executed (execution_id SERIAL, order_id INT NOT NULL, amount DECIMAL NOT NULL, price DECIMAL NOT NULL, execution_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  PRIMARY KEY (execution_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id) ON DELETE SET NULL ON UPDATE CASCADE);");
  txn.exec("CREATE TABLE canceled (order_id INT NOT NULL, amount DECIMAL NOT NULL, cancel_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  PRIMARY KEY (order_id), FOREIGN KEY (order_id) REFERENCES original_order(order_id) ON DELETE SET NULL ON UPDATE CASCADE);");
  txn.commit();

  close_database_connection(C);
}

connection * create_database_connection(string password){
  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    C = new connection("host=db port=5432 dbname=stock_exchange user=postgres password=" + password);
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
  }
  return C;
}

string get_UTC_time_now(){
    time_t now = time(0);
    struct tm * utc_time_now = gmtime(&now);
    return asctime(utc_time_now);
}

void add_account_db(connection *C, int account_id, double balance)
{
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  try {
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (";;
    ss << account_id << ", ";
    ss << balance << ");";
    txn.exec(ss.str());
    txn.commit();
  }
  catch (pqxx::unique_violation& e){
    txn.abort();
    throw invalid_argument(e.what());
  }

}

void add_stock_symbol_db(connection *C, string symbol)
{
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  try {
    std::stringstream ss;
    ss << "INSERT INTO stock_symbol (symbol) VALUES ('";;
    ss << symbol << "');";
    txn.exec(ss.str());
    txn.commit();
  }

  catch (pqxx::unique_violation& e){
    txn.abort();
  }
  
}

string add_position_string(int account_id, string symbol, double amount){
  std::stringstream ss;
  ss << "INSERT INTO position (account_id, symbol, amount) VALUES (";
  ss << account_id << ", '";
  ss << symbol << "', ";
  ss << amount << ");";
  return ss.str();
}

string insert_or_update_position_string(int account_id, string symbol, double amount){
  std::stringstream ss;
  ss << "INSERT INTO position (account_id, symbol, amount) VALUES (";
  ss << account_id << ", '";
  ss << symbol << "', ";
  ss << amount << ") ON CONFLICT (account_id, symbol) DO UPDATE SET amount = position.amount + ";
  ss << amount << ";";
  return ss.str();
}

void add_position_db(connection *C, int account_id, string symbol, double amount)
{
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  try{
    txn.exec(insert_or_update_position_string(account_id, symbol, amount));
    txn.commit();
  }
  catch (pqxx::foreign_key_violation& e){
    txn.abort();
    throw invalid_argument(e.what());
  }
  // catch (const exception& e){
  //   txn.abort();
  //   throw runtime_error(e.what());
  // }
}

// void add_position_db(connection *C, int account_id, string symbol, double amount)
// {
//   work txn(*C);
//   string get_position = get_position_string(account_id, symbol);
//   pqxx::result result = txn.exec(get_position);
//   if (check_result_exist(result)){
//     // if account and symbol already exist, update the row
//     try {
//       double updated_amount = process_result_into_double(result) + amount;
//       txn.exec(update_position_string(account_id, symbol, updated_amount));
//       txn.commit();
//     }
//     catch (const exception& e){
//       txn.abort();
//       throw runtime_error(e.what());
//     }
//   }
//   else {
//     // if account and symbol does not exist, insert into position
//     try{
//       txn.exec(add_position_string(account_id, symbol, amount));
//       txn.commit();
//     }
//     catch (pqxx::foreign_key_violation e){
//       txn.abort();
//       throw invalid_argument(e.what());
//     }
//     catch (const exception& e){
//       txn.abort();
//       throw runtime_error(e.what());
//     }
//   }  
// }

string add_original_order_string(int account_id, string symbol, double amount, double price_limit, string receive_time){
  std::stringstream ss;
  ss << "INSERT INTO original_order (account_id, symbol, amount, price_limit) VALUES (";
  ss << account_id << ", '";
  ss << symbol << "', ";
  ss << amount << ", ";
  ss << price_limit << ");";
  return ss.str();
}

string update_account_balance_string(int account_id, string symbol, double new_balance){
  std::stringstream ss;
  ss << "UPDATE account SET balance = ";
  ss << new_balance;
  ss << " WHERE account_id = ";
  ss << account_id << ";";
  return ss.str();
}

string get_order_id_string(int account_id, string symbol, double amount, double price_limit,string receive_time){
  std::stringstream ss;
  ss << "SELECT max(order_id) FROM original_order WHERE account_id = ";
  ss << account_id;
  ss << " AND symbol = '" << symbol;
  ss << "' AND amount = " << amount;
  ss << " AND price_limit = " << price_limit<< ";";
  return ss.str();
}

string get_account_balance_string(int account_id){
  std::stringstream ss;
    ss << "SELECT balance ";
    ss << "FROM account WHERE account_id = ";
    ss << account_id << ";";
  return ss.str();
}

int add_original_order_db(connection *C, int account_id, string symbol, double amount, double price_limit, string receive_time)
{
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);

  if (amount == 0){
    throw invalid_argument("amount can't be 0");
  }
  if (price_limit <= 0){
    throw invalid_argument("price can't be 0 or negative");
  }
  pqxx::result account_balance = txn.exec(get_account_balance_string(account_id));
  if (!check_result_exist(account_balance)){
    throw invalid_argument("account doesn't exist");
  }
  pqxx::result symbol_res = txn.exec(get_symbol_string(symbol));
  if (!check_result_exist(symbol_res)){
    throw invalid_argument("symbol does not exist");
  }
  if (amount > 0){
    // check sell or buy
    // try {
      // buy order
      // check account has enough money
      double new_balance = process_result_into_double(account_balance) - amount * price_limit;
      if (new_balance < 0){
        throw invalid_argument("don't have enough balance to buy");
      }
      // add to order
      string add_order = add_original_order_string(account_id, symbol, amount, price_limit, receive_time);
      txn.exec(add_order);
      string ger_order_id = get_order_id_string(account_id, symbol, amount, price_limit, receive_time);
      pqxx::result order_id = txn.exec(ger_order_id);

      // deduct money from account first as reservation
      string update_account_balance = update_account_balance_string(account_id, symbol, new_balance);
      txn.exec(update_account_balance);
      txn.commit();
      return process_result_into_int(order_id);
    // }
    // catch (pqxx::foreign_key_violation e){
    //   txn.abort();
    //   throw invalid_argument("account or symbol doesn't exist");
    // }
    // catch (pqxx::usage_error e){
    //   txn.abort();
    //   throw runtime_error(e.what());
    // }
  }
  else {
    // try{
      // sell order
      // check has enough position to sell
      pqxx::result result = txn.exec(get_position_string(account_id, symbol));
      double new_amount = process_result_into_double(result) + amount;
      if (new_amount < 0){
        throw invalid_argument("don't have enough share to sell");
      }

      // add to original_order
      string add_order = add_original_order_string(account_id, symbol, amount, price_limit, receive_time);
      txn.exec(add_order);
      string ger_order_id = get_order_id_string(account_id, symbol, amount, price_limit, receive_time);
      pqxx::result order_id = txn.exec(ger_order_id);
      
      // update position to deduct amount as reservation
      std::stringstream ss_update;
      ss_update << "UPDATE position SET amount = ";
      ss_update << new_amount;
      ss_update << " WHERE account_id = ";
      ss_update << account_id;
      ss_update << " AND symbol = '";
      ss_update << symbol << "';";
      txn.exec(ss_update.str());
      txn.commit();
      return process_result_into_int(order_id);
    // }
    // catch (pqxx::foreign_key_violation e){
    //   txn.abort();
    //   throw invalid_argument("account or symbol doesn't exist");
    // }
    // catch (pqxx::usage_error e){
    //   txn.abort();
    //   throw runtime_error(e.what());
    // }
  }
}

string get_potential_match_string(string symbol, double amount, double cur_order_price_limit){
  std::stringstream ss;
  ss << "SELECT original_order.order_id, account_id, open.amount, price_limit, receive_time ";
  ss << "FROM open join original_order on open.order_id = original_order.order_id WHERE symbol = '";
  ss << symbol;
  // the new order is a buy order
  if (amount > 0){
    // look for potential sell order which has a lower price
    ss << "' AND open.amount < 0 ";
    ss << "AND price_limit <= " << cur_order_price_limit;
    ss << " ORDER BY price_limit asc, receive_time asc;";
  }
  else {
    // the new order is a sell order, look for potential buyer which has a higher price
    ss << "' AND open.amount > 0 ";
    ss << " AND price_limit >= " << cur_order_price_limit;
    ss << " ORDER BY price_limit desc, receive_time asc;";
  }
  return ss.str();
}

string delete_open_order_string(int order_id){
  std::stringstream ss;
  ss << "DELETE FROM open WHERE order_id = " << order_id << ";";
  return ss.str();
}

string add_executed_order_string(int order_id, double amount, double price, string execution_time)
{
  std::stringstream ss;
  ss << "INSERT INTO executed (order_id, amount, price) VALUES (";
  ss << order_id << ", ";
  ss << amount << ", ";
  ss << price << ");";
  return ss.str();
}

string update_open_table_string(int order_id, double new_balance){
  std::stringstream ss;
  ss << "UPDATE open SET amount = ";
  ss << new_balance;
  ss << " WHERE order_id = ";
  ss << order_id << ";";
  return ss.str();
}

string get_symbol_string(string symbol){
  std::stringstream ss;
  ss << "SELECT symbol FROM stock_symbol where symbol = '";
  ss << symbol << "';";
  return ss.str();
}

void match_order(connection *C, int order_id, int account_id, string symbol, double amount, double cur_order_price_limit){
  if (amount == 0){
    throw invalid_argument("order amount can not be 0");
  }
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  try{
    // 0. error checking
    pqxx::result account = txn.exec(get_account_balance_string(account_id));
    if (!check_result_exist(account)){
      throw invalid_argument("input account does not exist");
    }
    pqxx::result symbol_res = txn.exec(get_symbol_string(symbol));
    if (!check_result_exist(symbol_res)){
      throw invalid_argument("input symbol does not exist");
    }
    // 1. check open table and get potential match back
    string get_potential_match = get_potential_match_string(symbol, amount, cur_order_price_limit);
    pqxx::result result = txn.exec(get_potential_match);
    print_result(result); // TODO delete print
    vector<Order> potential_match = process_result_into_order_list(result);

    // 2. go through potential match to execute order, until either we run out of potential_match or we have fulfill all amount requested
    double amount_left = amount;
    for (int i = 0; i < potential_match.size(); i++){
      Order match_order = potential_match[i];
      string execution_time = get_UTC_time_now();
      int buyer_account_id;
      int seller_account_id;
      double buyer_amount; // positive
      double seller_amount; // negative
      double execution_amount; // positive
      double buyer_price;
      double execution_price;
      if (amount_left == 0){
        // if we have already fulfilled all amounts, we should break out the loop
        break;
      }
      // we have not yet fulfilled all amounts needed and should continue to match
      // check current order is buy or sell
      if (amount > 0){
        // current order is buy, matched order is sell
        buyer_account_id = account_id;
        seller_account_id = match_order.get_account_id();
        buyer_amount = amount;
        seller_amount = match_order.get_amount();
        execution_price = match_order.get_price();
        buyer_price = cur_order_price_limit;
        if (amount_left >= seller_amount * -1){
          // the match_order can be executed as a whole
          execution_amount = seller_amount * -1;
          // delete the match order from open table
          txn.exec(delete_open_order_string(match_order.get_order_id()));
        }
        else {
          // split match order
          execution_amount = amount_left;
          // update match_order's amount in open table
          double original_amount = match_order.get_amount();
          double new_amount = original_amount + execution_amount;
          cout << "original_amount = " << original_amount << endl;
          cout << "execution_amount = " << execution_amount << endl;
          txn.exec(update_open_table_string(match_order.get_order_id(), new_amount));
        }
        // add match order to executed table
        txn.exec(add_executed_order_string(match_order.get_order_id(), execution_amount * -1, execution_price, execution_time));
        // add incoming order to executed table
        txn.exec(add_executed_order_string(order_id, execution_amount, execution_price, execution_time));
        // update amount_left
        amount_left -= execution_amount;
      }
      else {
        // current order is sell, matched order is buy
        buyer_account_id = match_order.get_account_id();
        seller_account_id = account_id;
        buyer_amount = match_order.get_amount();
        seller_amount = amount;
        buyer_price = match_order.get_price();
        execution_price = match_order.get_price();
        if (buyer_amount > amount_left * -1){
          // split match_order
          execution_amount = amount_left * -1;
          // update match_order's amount in open table
          double new_amount = match_order.get_amount() - execution_amount;
          txn.exec(update_open_table_string(match_order.get_order_id(), new_amount));
        }
        else {
          // the match_order can be executed as a whole
          execution_amount = buyer_amount;
          // delete the match order from open table
          txn.exec(delete_open_order_string(match_order.get_order_id()));
        }
        // add match order to executed table
        txn.exec(add_executed_order_string(match_order.get_order_id(), execution_amount, execution_price, execution_time));
        // add incoming order to executed table
        txn.exec(add_executed_order_string(order_id, execution_amount * -1, execution_price, execution_time));
        // update amount_left
        amount_left += execution_amount;
      }

      // update buyer's account to reflect the new balance (with adjusted price)
      pqxx::result buyer_balance = txn.exec(get_account_balance_string(buyer_account_id));
      if (!check_result_exist(buyer_balance)){
        throw invalid_argument("buyer account does not exist");
      }
      double original_balance = process_result_into_double(buyer_balance);
      // cout << "original_balance = " << original_balance << endl;
      double price_adjustment = (buyer_price - execution_price) * execution_amount;
      // cout << "price_adjustment = " << price_adjustment << endl;
      double new_balance = original_balance + price_adjustment;
      txn.exec(update_account_balance_string(buyer_account_id, symbol, new_balance));

      // add money to the seller's account 
      pqxx::result seller_account_balance = txn.exec(get_account_balance_string(seller_account_id));
      if (!check_result_exist(seller_account_balance)){
        throw invalid_argument("seller account does not exist");
      }
      double new_seller_balance = process_result_into_double(seller_account_balance) + execution_price * execution_amount;
      txn.exec(update_account_balance_string(seller_account_id, symbol, new_seller_balance));

      // add shares to buyer's position
      txn.exec(insert_or_update_position_string(buyer_account_id, symbol, execution_amount));
      // pqxx::result buyer_position = txn.exec(get_position_string(buyer_account_id, symbol));
      // if (check_result_exist(buyer_position)){
      //   // update position
      //   // cout << "original position amount = " << process_result_into_double(buyer_position) << endl;
      //   // cout << "execution_amount = " << execution_amount << endl;
      //   double new_position = process_result_into_double(buyer_position) + execution_amount;
      //   txn.exec(update_position_string(buyer_account_id, symbol, new_position));
      // }
      // else {
      //   // insert new position
      //   txn.exec(add_position_string(buyer_account_id, symbol, execution_amount));
      // }
    }

    // 3. if after we go through all the potential match there are still some amount not satisfied, add the remaining part to open
    if (amount_left != 0){
      txn.exec(add_open_string(order_id, amount_left));
    }

    txn.commit();
  }
  catch (pqxx::foreign_key_violation& e){
    txn.abort();
    throw invalid_argument("order doesn't exist in original_order table");
  }
  catch (pqxx::unique_violation& e){
    txn.abort();
    throw invalid_argument("order already exist in open table");
  }
  // catch (pqxx::usage_error e){
  //   txn.abort();
  //   throw runtime_error(e.what());
  // }
  // TODO: add error checking
}

string add_open_string(int order_id, double amount){
  std::stringstream ss;
  ss << "INSERT INTO open (order_id, amount) VALUES (";
  ss << order_id << ", ";
  ss << amount << ");";
  return ss.str();
}

void add_open_db(connection *C, int order_id, double amount){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  string add_open = add_open_string(order_id, amount);
  txn.exec(add_open);
  txn.commit();
}

void add_executed_db(connection *C, int order_id, double amount, double price, string execution_time)
{
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  std::stringstream ss;
  ss << "INSERT INTO executed (order_id, amount, price) VALUES (";
  ss << order_id << ", ";
  ss << amount << ", ";
  ss << price << ");";
  txn.exec(ss.str());
  txn.commit();
}

string add_canceled_db(connection *C, int order_id, double amount, string cancel_time)
{
  // work txn(*C);
  // pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  std::stringstream ss;
  ss << "INSERT INTO canceled (order_id, amount) VALUES (";
  ss << order_id << ", ";
  ss << amount << ");";
  return ss.str();
  // txn.exec(ss.str());
  // txn.commit();
}

string update_position_string(int account_id, string symbol, double amount){
  std::stringstream ss;
  ss << "UPDATE position SET amount = ";
  ss << amount << " WHERE account_id = ";
  ss << account_id << " AND symbol = '";
  ss << symbol << "';";
  return ss.str();
}


void update_position_db(connection * C, int account_id, string symbol, double amount){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  // try{
    string update_position = update_position_string(account_id, symbol, amount);
    txn.exec(update_position);
    txn.commit();
  // }
  // catch (const exception& e){
  //   txn.abort();
  //   throw runtime_error(e.what());
  // }
}

void print_result(pqxx::result r){
  for (auto c = 0; c < r.columns(); c++){
    std::cout << r.column_name(c) << ' ';
  }
  std::cout << std::endl;
  
  for (auto row = r.begin(); row != r.end(); row++)
    {
      for (auto field = row.begin(); field != row.end(); field++)
	std::cout << field->c_str() << ' ';
      std::cout << std::endl;
    }
}

void print_account_balance(connection *C, int account_id){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  std::stringstream ss;
  ss << "SELECT balance ";
  ss << "FROM account WHERE account_id = '";
  ss << account_id;
  ss << "';";
  pqxx::result result = txn.exec(ss.str());
  txn.commit();
  cout << "account " << account_id << " has balance " << process_result_into_double(result) << endl;
  // return result;
}

string get_position_string(int account_id, string symbol){
  std::stringstream ss;
  ss << "SELECT amount ";
  ss << "FROM position WHERE account_id = ";
  ss << account_id;
  ss << " AND symbol = '";
  ss << symbol;
  ss << "';";
  return ss.str();
}

void print_position_db(connection *C, int account_id, string symbol){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  string position = get_position_string(account_id, symbol);
  pqxx::result result = txn.exec(position);
  txn.commit();
  if (!check_result_exist(result)){
    cout << "no position found for symbol " << symbol << endl; 
  }
  else {
    cout << "acount " << account_id << " has " << process_result_into_double(result) << " shares for " << symbol << endl;
  }
  // print_result(result);
  // return result;
}

string get_open_string(){
  std::stringstream ss;
  ss << "SELECT * ";
  ss << "FROM open";
  ss << ";";
  return ss.str();
}

void print_open_table_db(connection *C){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  string open_table = get_open_string();
  pqxx::result result = txn.exec(open_table);
  txn.commit();
  cout << "===============open table====================" << endl;
  print_result(result);
  cout << "===============end of table====================" << endl;
}

string get_executed_string(){
  std::stringstream ss;
  ss << "SELECT * ";
  ss << "FROM executed";
  ss << ";";
  return ss.str();
}

void print_executed_table_db(connection *C){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  string open_table = get_executed_string();
  pqxx::result result = txn.exec(open_table);
  txn.commit();
  cout << "===============executed table====================" << endl;
  print_result(result);
  cout << "===============end of table====================" << endl;
}

string get_original_order_string(){
  std::stringstream ss;
  ss << "SELECT * ";
  ss << "FROM original_order";
  ss << ";";
  return ss.str();
}

void print_original_order_table_db(connection *C){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  string open_table = get_original_order_string();
  pqxx::result result = txn.exec(open_table);
  txn.commit();
  cout << "===============original order table====================" << endl;
  print_result(result);
  cout << "===============end of table====================" << endl;
}


vector<Order> get_query_db(connection *C, int trans_id,int account_id ){
  work txn(*C);
  std::stringstream ss;
  ss << "SELECT * ";
  ss << "FROM original_order WHERE order_id = ";
  ss << trans_id<<" AND account_id = ";
  ss << account_id<<";";
  pqxx::result result = txn.exec(ss.str());
  if (!check_result_exist(result)){
      throw invalid_argument("transaction does not exists under current account");
  }else{
    std::stringstream ss;
    ss << "SELECT open.order_id as order_id,open.amount as amount ";
    ss << "FROM open JOIN original_order ON open.order_id = original_order.order_id WHERE open.order_id = ";
    ss << trans_id<<" AND original_order.account_id = ";
    ss << account_id<<";";
    pqxx::result result = txn.exec(ss.str());
    vector<Order> list =  process_result_for_open_request(result);

    ss << "SELECT canceled.order_id as order_id,canceled.amount as amount, canceled.cancel_time as time ";
    ss << "FROM canceled JOIN original_order ON canceled.order_id = original_order.order_id WHERE canceled.order_id = ";
    ss << trans_id<<" AND original_order.account_id = ";
    ss << account_id<<";";
    result = txn.exec(ss.str());
    add_cancel_to_vector(&list,result,"canceled");

    ss << "SELECT executed.order_id as order_id,executed.amount as amount, executed.price as price,executed.execution_time as time ";
    ss << "FROM executed JOIN original_order ON executed.order_id = original_order.order_id WHERE executed.order_id = ";
    ss << trans_id<<" AND original_order.account_id = ";
    ss << account_id<<";";
    result = txn.exec(ss.str());
    add_execution_to_vector(&list,result,"executed");



    txn.commit();
    //print_result(result);
    return list;
  }

}

void cancel_order(connection *C, int trans_id, int account_id){
  work txn(*C);
  std::stringstream ss;
  ss << "SELECT * ";
  ss << "FROM original_order WHERE order_id = ";
  ss << trans_id<<" AND account_id = ";
  ss << account_id<<";";
  pqxx::result result = txn.exec(ss.str());
  if (!check_result_exist(result)){
      throw invalid_argument("transaction does not exists under current account");
  }else{
    // try{
      std::stringstream ss;
      ss << "SELECT open.order_id ,original_order.account_id, open.amount,original_order.price_limit, original_order.receive_time ";
      ss << "FROM open JOIN original_order ON open.order_id = original_order.order_id WHERE open.order_id = ";
      ss << trans_id <<" AND original_order.account_id = ";
      ss << account_id<<";";
      pqxx::result result = txn.exec(ss.str());
      print_result(result);
      if (!check_result_exist(result)){
        throw invalid_argument("No open order to be canceled");
      }
      vector<Order> list =  process_result_into_order_list(result);
      std::stringstream ss_delete;
      ss_delete << "DELETE FROM open WHERE order_id = ";
      ss_delete << trans_id<<";";
      txn.exec(ss_delete.str());
      string time = get_UTC_time_now();
      double amount = list[0].get_amount();
      int account_id = list[0].get_account_id();
      double price = list[0].get_price();
      string add_cancel = add_canceled_db(C, trans_id, abs(amount), time);
      txn.exec(add_cancel);
      if(amount>0){
        std::stringstream ssnew;
        ssnew << "SELECT balance ";
        ssnew << "FROM account WHERE account_id = ";
        ssnew << account_id << ";";
        pqxx::result result = txn.exec(ssnew.str());
        double new_balance = process_result_into_double(result) + amount * price;
        string update_account_balance = update_account_balance_string(account_id, "", new_balance);
        txn.exec(update_account_balance);
      }else{
        std::stringstream ss;
        ss << "SELECT symbol ";
        ss << "FROM original_order WHERE order_id = ";
        ss << trans_id << ";";
        pqxx::result r = txn.exec(ss.str());
        string sym;
        for (auto row = r.begin(); row != r.end(); row++)
          {
            for (auto field = row.begin(); field != row.end(); field++){
              sym =  field->c_str();
            }
          }
        std::stringstream ssnew;
        ssnew << "SELECT amount ";
        ssnew << "FROM position WHERE account_id = ";
        ssnew << account_id;
        ssnew << " AND symbol = '";
        ssnew << sym << "';";
        pqxx::result result = txn.exec(ssnew.str());
        double new_amount = process_result_into_double(result) - amount;
        cout << "************************"<<new_amount;
        std::stringstream ss_update;
        ss_update << "UPDATE position SET amount = ";
        ss_update << new_amount;
        ss_update << " WHERE account_id = ";
        ss_update << account_id;
        ss_update << " AND symbol = '";
        ss_update << sym << "';";
        txn.exec(ss_update.str());
      }
      txn.commit();
    // }
    // catch (pqxx::usage_error e){
    //   txn.abort();
    //   throw runtime_error(e.what());
    // }

  }
}
void check_account_exist(connection*C, int account_id){
  // work txn(*C);
  pqxx::transaction<pqxx::isolation_level::serializable> txn(*C);
  pqxx::result balance = txn.exec(get_account_balance_string(account_id));
  cout<<"======================"<<endl;
  cout<<account_id<<" "<<process_result_into_double(balance)<<endl;
  if (!check_result_exist(balance)){
    throw invalid_argument("account does not exist");
  }
  txn.commit();
}
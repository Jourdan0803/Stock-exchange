#include "parser.hpp"


using namespace std;
using namespace pugi;

string parse_create(xml_document &doc,connection *c)
{
    xml_document response;
    xml_node results = response.append_child("results");
    xml_node items = doc.child("create");
    vector<string> symbols;
    for (pugi::xml_node node : items.children()) {
    //std::cout << "Node name: " << node.name() << std::endl;
        if(string(node.name()) == "account"){
            int account;
            double balance;
            xml_attribute attr1 = node.first_attribute();
            string acc_str = attr1.value();
            if(acc_str.empty() || !all_of(acc_str.begin(), acc_str.end(), ::isdigit)){
                xml_node error = results.append_child("error");
                error.append_attribute("id") = acc_str.c_str();
                error.text().set("Invalid account");
            }else{
                account = stoi(acc_str);
                xml_attribute attr2 = node.last_attribute();
                balance = stof(attr2.value());
                if(balance<=0){
                    xml_node error = results.append_child("error");
                    error.append_attribute("id") = account;
                    error.text().set("Balance must be positive");
                }else{           
                /**************** check account in database ****************/
                    while(true){
                        try {
                            /**************** add  account and balance in database ****************/
                            add_account_db(c,account,balance);
                            xml_node created = results.append_child("created");
                            created.append_attribute("id") = account;
                            break;
                        }
                        catch (invalid_argument& e){
                            //cout << e.what() << endl;
                            xml_node error = results.append_child("error");
                            error.append_attribute("id") = account;
                            error.text().set("Account already exists");
                            break;
                        }
                        catch(exception& e){
                            cout<<"add account--52"<<endl;
                            continue;
                        }
                    }
                }
            }

        }
    // Special handling for symbol node with nested account
    else if (std::string(node.name()) == "symbol") {
        for (pugi::xml_node child : node.children()) {
            int account_id = atoi(child.first_attribute().value());
            //cout <<"account id = " << account_id << endl;
            string symbol = node.first_attribute().as_string();
            double shares = child.text().as_double();
            //cout << "shares: " << shares << endl;
            if(symbol.empty() || !all_of(symbol.begin(), symbol.end(), ::isalnum)){
                xml_node error = results.append_child("error");
                error.append_attribute("sym") = symbol.c_str();
                error.append_attribute("id") = account_id;
                error.text().set("invalid symbol input");
            }else if(shares<0){
                xml_node error = results.append_child("error");
                error.append_attribute("sym") = symbol.c_str();
                error.append_attribute("id") = account_id;
                error.text().set("Amount can not be negative");
            }else{
                while(true){
                    try{
                        add_stock_symbol_db(c,symbol);
                        break;
                    }catch(exception& e){
                        cout<<"add stock--82"<<endl;
                        continue;
                    }
                }
                
                while(true){
                    try{
                        add_position_db(c,account_id,symbol,shares);
                        xml_node created = results.append_child("created");
                        created.append_attribute("sym") = symbol.c_str();
                        created.append_attribute("id") = account_id;
                        break;
                    }
                    catch(invalid_argument& e){
                        xml_node error = results.append_child("error");
                        error.append_attribute("id") = account_id;
                        error.text().set("Account or symbol not exists");
                        break;
                    }
                    catch(exception& e){
                        cout<<"add position--102"<<endl;
                        continue;
                    }
                }
            }
        }
    }else{
        xml_node error = results.append_child("error");
        error.text().set("Illegal tag");
    }
}
stringstream ss;
response.print(ss);
string result = ss.str(); 
return result;
}


string parse_transaction(xml_document &doc,connection *c){
    xml_document response;
    xml_node results = response.append_child("results");
    xml_node transactions = doc.child("transactions");
    xml_attribute attr = transactions.first_attribute();
    int account_id = stoi(attr.value());
     /**************** check account in database ****************/
    cout<<"account_id = " <<account_id<<endl;
    for(xml_node child : transactions.children()){
        bool flag = false;
        while(true){
            try{
                check_account_exist(c, account_id);
                break;
            }catch(invalid_argument& e){
                xml_node error = results.append_child("error");
                error.append_attribute("id") = account_id;
                error.text().set("account does not exist");
                flag = true;
                break;
            }
            catch(exception& e){
                cout<<"check account--142"<<endl;
                cout<<e.what()<<endl;
                continue;
            } 
        }
        if(flag){
            continue;
        }
        
        string nodename = string(child.name());
        if(nodename == "order"){
            string symbol = child.attribute("sym").value();
            double amount = child.attribute("amount").as_double();
            double limit = child.attribute("limit").as_double();
            string time = get_UTC_time_now();
            while(true){
                try{
                    //cout<<"118 "<<endl;
                    int trans_id = add_original_order_db(c, account_id, symbol, amount, limit, time);
                    cout << "finish add_original_order_db" << endl;
                    match_order(c,trans_id,account_id,symbol,amount,limit);
                    // cout<<"nodename = " <<nodename<<endl;
                    xml_node opened = results.append_child("opened");
                    opened.append_attribute("sym") = symbol.c_str();
                    opened.append_attribute("amount") = amount;
                    opened.append_attribute("limit") = limit;
                    opened.append_attribute("id") = trans_id;
                    break;
                }catch(invalid_argument& e){
                    xml_node error = results.append_child("error");
                    error.append_attribute("sym") = symbol.c_str();
                    error.append_attribute("amount") = amount;
                    error.append_attribute("limit") = limit;
                    error.text().set(e.what());
                    break;
                }
                // catch(pqxx::usage_error e){
                //     cout<<"add original,match order--178"<<endl;
                //     cout<<e.what()<<endl;
                //     continue;
                // }
                // catch(pqxx::deadlock_detected e){
                //     cout<<"add original,match order--183"<<endl;
                //     cout<<e.what()<<endl;
                //     continue;
                // }
                // catch(pqxx::serialization_failure e){
                //     cout<<"add original,match order--188"<<endl;
                //     cout<<e.what()<<endl;
                //     continue;
                // }
                catch(exception& e){
                    // cout<<"check account--142"<<endl;
                    cout<<e.what()<<endl;
                    continue;
                } 


            }
        }
        else if(nodename == "query"){
            int trans_id = child.first_attribute().as_int();
            while(true){
                try{
                    vector<Order> orders = get_query_db(c,trans_id,account_id);
                    xml_node status = results.append_child("status");
                    status.append_attribute("id") = trans_id;
                    for(auto order:orders){
                        if(order.get_status() == "open"){
                            xml_node open = status.append_child("open");
                            open.append_attribute("shares") = order.get_amount();
                        }else if(order.get_status() == "canceled"){
                            xml_node canceled = status.append_child("canceled");
                            canceled.append_attribute("shares") = order.get_amount();
                            canceled.append_attribute("time") = order.get_time().c_str();
                        }else if(order.get_status() == "executed"){
                            xml_node execute = status.append_child("executed");
                            execute.append_attribute("shares") = order.get_amount();
                            execute.append_attribute("price") = order.get_price();
                            execute.append_attribute("time") = order.get_time().c_str();
                        }
                    }
                    break;     
                }catch(invalid_argument& e){
                    xml_node error = results.append_child("error");
                    error.text().set(e.what());
                    break;
                }
                catch(exception& e){
                    cout<<"query--210"<<endl;
                    continue;
                } 
            }

            //cout<<"transaction id = "<<trans_id<<endl;

        }
        else if(nodename == "cancel"){
            int cancel_id = child.first_attribute().as_int();
            cout<<"cancel id = "<<cancel_id<<endl;
            while(true){
                try{
                    cancel_order(c,cancel_id,account_id);
                    vector<Order> orders = get_query_db(c,cancel_id,account_id);
                    xml_node cancel = results.append_child("canceled");
                    cancel.append_attribute("id") = cancel_id;
                    for(auto order:orders){
                    if(order.get_status() == "canceled"){
                        xml_node canceled = cancel.append_child("canceled");
                        canceled.append_attribute("shares") = order.get_amount();
                        canceled.append_attribute("time") = order.get_time().c_str();
                    }else if(order.get_status() == "executed"){
                        xml_node execute = cancel.append_child("executed");
                        execute.append_attribute("shares") = order.get_amount();
                        execute.append_attribute("price") = order.get_price();
                        execute.append_attribute("time") = order.get_time().c_str();
                        }
                    }
                    break;   
                }
                catch(invalid_argument& e){
                    xml_node error = results.append_child("error");
                    error.text().set(e.what());
                    break;
                }
                catch(exception& e){
                    cout<<"cancel--247"<<endl;
                    continue;
                }
            }
        }
        else{
            cerr<<"Illegal Tag"<<endl;
        }
    }
    stringstream ss;
    response.print(ss);
    string result = ss.str(); 
    cout<<result<<endl;
    return result;
}

string handleRequest(string req,connection *c){
    const char* request = req.c_str();
    xml_document doc;
    xml_parse_result res = doc.load_string(request);
    if(!res){
        cerr << "xml parser error: " << res.description();
    }
    string response;
    if(doc.child("create")){
        response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + parse_create(doc,c);
    }else if(doc.child("transactions")){
        response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + parse_transaction(doc,c);
    }else{
        response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error> XML Tag illegal</error>\n";
    }
    int length = response.length();
    string final_res = to_string(length) + "\n" +response;
    return final_res;
}
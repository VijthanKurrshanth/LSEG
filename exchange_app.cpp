#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <iomanip>
#include <chrono>
#include <ctime>

/** 
 * This function retrieves the current date and time with millisecond precision and formats it as a string in the following format: "YYYYMMDD-HHMMSS.SSS".
 * It uses the C++ <chrono> library to obtain the current time, calculates the milliseconds, and formats it into a string representation.
 * The localtime_s function is used on Windows systems for thread-safe time conversion, while localtime_r should be used on POSIX systems.
 * 
 */
std::string getCurrentTime() {

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm timeInfo;
    localtime_s(&timeInfo, &time); // For Windows. Use localtime_r for POSIX systems.

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y%m%d-%H%M%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

/**
 * This C++ struct, in_ord, represents an order or trade-related data structure with the following fields:
 * 
 * c_ord_id: A string representing the Client Order ID of the submitted order.
 * inst: A string representing the instrument/flower the order.
 * ord_id: A string representing the System generated unique order ID.
 * exec_s: A string representing the execution status or state of the order 0 – New, 1 – Rejected, 2 – Fill, 3 - Pfill.
 * reason: A string that provides a reason or explanation related to the errors occured.
 * side: An integer indicating the order side (e.g., 1 for buy, 2 for sell).
 * qty: An integer representing the total order quantity.
 * exec_qty: An integer representing the quantity of the order that has been executed.
 * price: A double precision floating-point number indicating the price associated with the order.
 * 
 */
struct in_ord{

std::string c_ord_id,inst,ord_id,exec_s,reason;
int side, qty, exec_qty;
double price;

};

//  Generates a string for order ID using the number of the current order
std::string getOrderString(int x) {
    return "ord" + std::to_string(x);
}

// Custom comparison function for ascending order
bool compareByValueA(const in_ord* a, const in_ord* b) {
    return a->price < b->price;
}

// Custom comparison function for descending order
bool compareByValueD(const in_ord* a, const in_ord* b) {
    return a->price > b->price;
}

/**
 * Inserts a pointer to a value of type 'in_ord' into a sorted vector while maintaining the sort order (ascending).
 *
 * This function takes a sorted vector of 'in_ord*' pointers and a new 'in_ord*' pointer 'newValue'.
 * It inserts 'newValue' into the vector in a way that maintains the ascending order of the elements
 * based on the comparison function 'compareByValueA'. If there are elements with the same value as 'newValue',
 * it inserts 'newValue' immediately after the last element with the same value.
 *
 */
void insertIntoSortedVectorA(std::vector<in_ord*>& sortedVector, in_ord* newValue) {

    if (sortedVector.empty()) {
        sortedVector.push_back(newValue);
        return;
    }

    auto it = std::lower_bound(sortedVector.begin(), sortedVector.end(), newValue, compareByValueA);

    // Find the first element in the vector with the same price
    auto samePriceIt = it;
    while (samePriceIt != sortedVector.begin() && compareByValueA(*(samePriceIt - 1), newValue) == false) {
        --samePriceIt;
    }

    // Insert the new value after the last element with the same price
    sortedVector.insert(samePriceIt, newValue);
}

/**
 * Inserts a pointer to a value of type 'in_ord' into a sorted vector while maintaining the sort order (descending).
 *
 * This function takes a sorted vector of 'in_ord*' pointers and a new 'in_ord*' pointer 'newValue'.
 * It inserts 'newValue' into the vector in a way that maintains the descending order of the elements
 * based on the comparison function 'compareByValueD'. If there are elements with the same value as 'newValue',
 * it inserts 'newValue' immediately after the last element with the same value.
 *
 */
void insertIntoSortedVectorD(std::vector<in_ord*>& sortedVector, in_ord* newValue) {
    if (sortedVector.empty()) {
        sortedVector.push_back(newValue);
        return;
    }

    auto it = std::lower_bound(sortedVector.begin(), sortedVector.end(), newValue, compareByValueD);

    // Find the first element in the vector with the same price
    auto samePriceIt = it;
    while (samePriceIt != sortedVector.begin() && compareByValueD(*(samePriceIt - 1), newValue) == false) {
        --samePriceIt;
    }

    // Insert the new value after the last element with the same price
    sortedVector.insert(samePriceIt, newValue);
}

/// creates in_ord struct using the row in the input order
/// returns the in_ord struct pointer 
in_ord* record(std::vector<std::string> row, int order_no){
    in_ord* order = new in_ord;
    order->c_ord_id = row[0];
    order->inst = row[1];
    order->side = std::stoi(row[2]);
    order->price = std::stod(row[4]);
    order->qty = std::stoi(row[3]);
    order->ord_id = getOrderString(order_no);

    return order;
}

/**
 * This function takes an output file stream (`fout`), an `in_ord` structure pointer (`order`), and a `price` value
 * as input and writes the order's information, including its ID, customer ID, instrument, side, execution status,
 * execution quantity, price, reason, and current time, to the provided file stream. Each field is separated by a comma,
 * and a newline character is added at the end of the line.
 *
*/
void writeOrderToFile(std::ofstream& fout, const in_ord* order, float price) {
    std::string currentTime = getCurrentTime();
    fout << order->ord_id << ","
         << order->c_ord_id << ","
         << order->inst << ","
         << order->side << ","
         << order->exec_s << ","
         << order->exec_qty << ","
         << price << ","
         << order->reason << ","
         << currentTime << "\n";
}

/**
 * checks if the input order is valid
 * if invalid rejects the order and updates the reason
*/
bool checkValid(in_ord* order){
    bool valid = true;
    std::set<std::string> instruments = {"Rose","Lavender","Lotus","Tulip","Orchid"};
    auto it = instruments.find(order->inst);
    if (it == instruments.end()){
        order->reason += "Invalid instrument. ";
        order->exec_s = "Reject";
        order->exec_qty = order->qty;
        valid = false;
    }
    if (order->side != 1 && order->side != 2){
        order->reason += "Invalid side. ";
        order->exec_s = "Reject";
        order->exec_qty = order->qty;
        valid = false;
    }
    if (order->price <= 0){
        order->reason += "Invalid price. ";
        order->exec_s = "Reject";
        order->exec_qty = order->qty;
        valid = false;
    }
    if (order->qty % 10 != 0 || order->qty == 0 || order->qty > 1000){
        order->reason += "Invalid size. ";
        order->exec_s = "Reject";
        order->exec_qty = order->qty;
        valid = false;
    }
    return valid;

}

int main(){
    //order books initialization
    std::vector<in_ord*> 
    blue_list_rose, pink_list_rose,
    blue_list_lavender, pink_list_lavender,
    blue_list_lotus, pink_list_lotus,
    blue_list_tulip, pink_list_tulip,
    blue_list_orchid, pink_list_orchid;

    // file pointer for execution report
    std::ofstream fout;

     // opens an existing csv file or creates a new file.
    fout.open("execution_rep.csv", std::ios::out);

    // Creation of ifstream class object to read the file
    std::ifstream fin;

    // order file. Change the file name to test with different order files
    fin.open("orders4.csv");

    // Execute a loop until EOF (End of File)
    int line_no = 1;
    int order_no = 1;
    std::string line, word;
    std::vector<std::string> row;
    while (std::getline(fin, line)) {
        
        if (line_no < 3){
            line_no += 1;
            if (line_no ==2){
                fout << "execution_rep.csv" << "," << "," << "," << "," << "," << "\n"
                << "Order ID" << "," 
                << "Client Order ID" << ","
                << "Instrument" << ","
                << "Side" << ","
                << "Exec Status" << ","
                << "Quantity" << ","
                << "Price" << ","
                << "Reason" << ","
                << "Transaction time" << "\n";
            }
            continue;
        }

        row.clear();

        // used for breaking words
        std::stringstream s(line);

        // read every column data of a row and
        // store it in a string variable, 'word'
        while (std::getline(s, word, ',')) {
  
            // add all the column data
            // of a row to a vector
            row.push_back(word);
        }

        
        in_ord* order = record(row, order_no);
        order_no += 1;

        // check if the order is valid
        if (!checkValid(order)){
            writeOrderToFile(fout, order, order->price);
        }
        // if valid, execute the order
        else if (order->inst == "Rose"){
            switch (order->side){
                case 1:
                if (!pink_list_rose.empty()){
                    if (order->price >= pink_list_rose.back()->price){
                        insertIntoSortedVectorA(blue_list_rose, order);
                        while (order->qty > 0 && !pink_list_rose.empty() && order->price >= pink_list_rose.back()->price){
                            if (order->qty == pink_list_rose.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, pink_list_rose.back()->price);
                                

                                pink_list_rose.back()->exec_s = "Fill";
                                pink_list_rose.back()->exec_qty = pink_list_rose.back()->qty;
                                writeOrderToFile(fout, pink_list_rose.back(), pink_list_rose.back()->price);

                                delete pink_list_rose.back();
                                pink_list_rose.pop_back();
                                delete blue_list_rose.back();
                                blue_list_rose.pop_back();
                            }
                            else if(order->qty > pink_list_rose.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = pink_list_rose.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_rose.back()->price);

                                pink_list_rose.back()->exec_s = "Fill";
                                pink_list_rose.back()->exec_qty = pink_list_rose.back()->qty;
                                writeOrderToFile(fout, pink_list_rose.back(), pink_list_rose.back()->price);

                                delete pink_list_rose.back();
                                pink_list_rose.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_rose.back()->price);
                                
                                pink_list_rose.back()->exec_s = "Pfill";
                                pink_list_rose.back()->exec_qty = order->exec_qty;
                                pink_list_rose.back()->qty -= pink_list_rose.back()->exec_qty;
                                writeOrderToFile(fout, pink_list_rose.back(), pink_list_rose.back()->price);

                                delete blue_list_rose.back();
                                blue_list_rose.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorA(blue_list_rose, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorA(blue_list_rose, order);
                }
                break;

                case 2:
                if (!blue_list_rose.empty()){
                    if (order->price <= blue_list_rose.back()->price){
                        insertIntoSortedVectorD(pink_list_rose, order);
                        while (order->qty > 0 && !blue_list_rose.empty() && order->price <= blue_list_rose.back()->price){
                            if (order->qty == blue_list_rose.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, blue_list_rose.back()->price);
                                

                                blue_list_rose.back()->exec_s = "Fill";
                                blue_list_rose.back()->exec_qty = blue_list_rose.back()->qty;
                                writeOrderToFile(fout, blue_list_rose.back(), blue_list_rose.back()->price);

                                delete blue_list_rose.back();
                                blue_list_rose.pop_back();
                                delete pink_list_rose.back();
                                pink_list_rose.pop_back();
                            }
                            else if(order->qty > blue_list_rose.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = blue_list_rose.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_rose.back()->price);

                                blue_list_rose.back()->exec_s = "Fill";
                                blue_list_rose.back()->exec_qty = blue_list_rose.back()->qty;
                                writeOrderToFile(fout, blue_list_rose.back(), blue_list_rose.back()->price);

                                delete blue_list_rose.back();
                                blue_list_rose.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_rose.back()->price);

                                blue_list_rose.back()->exec_s = "Pfill";
                                blue_list_rose.back()->exec_qty = order->exec_qty;
                                blue_list_rose.back()->qty -= blue_list_rose.back()->exec_qty;
                                writeOrderToFile(fout, blue_list_rose.back(), blue_list_rose.back()->price);

                                delete pink_list_rose.back();
                                pink_list_rose.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorD(pink_list_rose, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorD(pink_list_rose, order);
                }
                break;
            }

        }
        else if (order->inst == "Lavender"){
            switch (order->side){
                case 1:
                if (!pink_list_lavender.empty()){
                    if (order->price >= pink_list_lavender.back()->price){
                        insertIntoSortedVectorA(blue_list_lavender, order);
                        while (order->qty > 0 && !pink_list_lavender.empty() && order->price >= pink_list_lavender.back()->price){
                            if (order->qty == pink_list_lavender.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, pink_list_lavender.back()->price);
                                

                                pink_list_lavender.back()->exec_s = "Fill";
                                pink_list_lavender.back()->exec_qty = pink_list_lavender.back()->qty;
                                writeOrderToFile(fout, pink_list_lavender.back(), pink_list_lavender.back()->price);

                                delete pink_list_lavender.back();
                                pink_list_lavender.pop_back();
                                delete blue_list_lavender.back();
                                blue_list_lavender.pop_back();
                            }
                            else if(order->qty > pink_list_lavender.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = pink_list_lavender.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_lavender.back()->price);

                                pink_list_lavender.back()->exec_s = "Fill";
                                pink_list_lavender.back()->exec_qty = pink_list_lavender.back()->qty;
                                writeOrderToFile(fout, pink_list_lavender.back(), pink_list_lavender.back()->price);

                                delete pink_list_lavender.back();
                                pink_list_lavender.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_lavender.back()->price);
                                

                                pink_list_lavender.back()->exec_s = "Pfill";
                                pink_list_lavender.back()->exec_qty = order->exec_qty;
                                pink_list_lavender.back()->qty -= pink_list_lavender.back()->exec_qty;
                                writeOrderToFile(fout, pink_list_lavender.back(), pink_list_lavender.back()->price);

                                delete blue_list_lavender.back();
                                blue_list_lavender.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorA(blue_list_lavender, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorA(blue_list_lavender, order);
                }
                break;

                case 2:
                if (!blue_list_lavender.empty()){
                    if (order->price <= blue_list_lavender.back()->price){
                        insertIntoSortedVectorD(pink_list_lavender, order);
                        while (order->qty > 0 && !blue_list_lavender.empty() && order->price <= blue_list_lavender.back()->price){
                            if (order->qty == blue_list_lavender.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, blue_list_lavender.back()->price);
                                

                                blue_list_lavender.back()->exec_s = "Fill";
                                blue_list_lavender.back()->exec_qty = blue_list_lavender.back()->qty;
                                writeOrderToFile(fout, blue_list_lavender.back(), blue_list_lavender.back()->price);

                                delete blue_list_lavender.back();
                                blue_list_lavender.pop_back();
                                delete pink_list_lavender.back();
                                pink_list_lavender.pop_back();
                            }
                            else if(order->qty > blue_list_lavender.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = blue_list_lavender.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_lavender.back()->price);

                                blue_list_lavender.back()->exec_s = "Fill";
                                blue_list_lavender.back()->exec_qty = blue_list_lavender.back()->qty;
                                writeOrderToFile(fout, blue_list_lavender.back(), blue_list_lavender.back()->price);

                                delete blue_list_lavender.back();
                                blue_list_lavender.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_lavender.back()->price);

                                blue_list_lavender.back()->exec_s = "Pfill";
                                blue_list_lavender.back()->exec_qty = order->exec_qty;
                                blue_list_lavender.back()->qty -= blue_list_lavender.back()->exec_qty;
                                writeOrderToFile(fout, blue_list_lavender.back(), blue_list_lavender.back()->price);

                                 delete pink_list_lavender.back();
                                pink_list_lavender.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorD(pink_list_lavender, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorD(pink_list_lavender, order);
                }
                break;
            }

        }
        else if (order->inst == "Lotus"){
            switch (order->side){
                case 1:
                if (!pink_list_lotus.empty()){
                    if (order->price >= pink_list_lotus.back()->price){
                        insertIntoSortedVectorA(blue_list_lotus, order);
                        while (order->qty > 0 && !pink_list_lotus.empty() && order->price >= pink_list_lotus.back()->price){
                            if (order->qty == pink_list_lotus.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, pink_list_lotus.back()->price);
                                

                                pink_list_lotus.back()->exec_s = "Fill";
                                pink_list_lotus.back()->exec_qty = pink_list_lotus.back()->qty;
                                writeOrderToFile(fout, pink_list_lotus.back(), pink_list_lotus.back()->price);

                                delete pink_list_lotus.back();
                                pink_list_lotus.pop_back();
                                delete blue_list_lotus.back();
                                blue_list_lotus.pop_back();
                            }
                            else if(order->qty > pink_list_lotus.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = pink_list_lotus.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_lotus.back()->price);

                                pink_list_lotus.back()->exec_s = "Fill";
                                pink_list_lotus.back()->exec_qty = pink_list_lotus.back()->qty;
                                writeOrderToFile(fout, pink_list_lotus.back(), pink_list_lotus.back()->price);

                                delete pink_list_lotus.back();
                                pink_list_lotus.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_lotus.back()->price);
                                

                                pink_list_lotus.back()->exec_s = "Pfill";
                                pink_list_lotus.back()->exec_qty = order->exec_qty;
                                pink_list_lotus.back()->qty -= pink_list_lotus.back()->exec_qty;
                                writeOrderToFile(fout, pink_list_lotus.back(), pink_list_lotus.back()->price);

                                delete blue_list_lotus.back();
                                blue_list_lotus.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorA(blue_list_lotus, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorA(blue_list_lotus, order);
                }
                break;

                case 2:
                if (!blue_list_lotus.empty()){
                    if (order->price <= blue_list_lotus.back()->price){
                        insertIntoSortedVectorD(pink_list_lotus, order);
                        while (order->qty > 0 && !blue_list_lotus.empty() && order->price <= blue_list_lotus.back()->price){
                            if (order->qty == blue_list_lotus.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, blue_list_lotus.back()->price);
                                

                                blue_list_lotus.back()->exec_s = "Fill";
                                blue_list_lotus.back()->exec_qty = blue_list_lotus.back()->qty;
                                writeOrderToFile(fout, blue_list_lotus.back(), blue_list_lotus.back()->price);
                                
                                delete blue_list_lotus.back();
                                blue_list_lotus.pop_back();
                                delete pink_list_lotus.back();
                                pink_list_lotus.pop_back();
                            }
                            else if(order->qty > blue_list_lotus.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = blue_list_lotus.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_lotus.back()->price);

                                blue_list_lotus.back()->exec_s = "Fill";
                                blue_list_lotus.back()->exec_qty = blue_list_lotus.back()->qty;
                                writeOrderToFile(fout, blue_list_lotus.back(), blue_list_lotus.back()->price);

                                delete blue_list_lotus.back();
                                blue_list_lotus.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_lotus.back()->price);
                                

                                blue_list_lotus.back()->exec_s = "Pfill";
                                blue_list_lotus.back()->exec_qty = order->exec_qty;
                                blue_list_lotus.back()->qty -= blue_list_lotus.back()->exec_qty;
                                writeOrderToFile(fout, blue_list_lotus.back(), blue_list_lotus.back()->price);

                                delete pink_list_lotus.back();
                                pink_list_lotus.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorD(pink_list_lotus, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorD(pink_list_lotus, order);
                }
                break;
            }

        }
        else if (order->inst == "Tulip"){
            switch (order->side){
                case 1:
                if (!pink_list_tulip.empty()){
                    if (order->price >= pink_list_tulip.back()->price){
                        insertIntoSortedVectorA(blue_list_tulip, order);
                        while (order->qty > 0 && !pink_list_tulip.empty() && order->price >= pink_list_tulip.back()->price){
                            if (order->qty == pink_list_tulip.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, pink_list_tulip.back()->price);
                                

                                pink_list_tulip.back()->exec_s = "Fill";
                                pink_list_tulip.back()->exec_qty = pink_list_tulip.back()->qty;
                                writeOrderToFile(fout, pink_list_tulip.back(), pink_list_tulip.back()->price);

                                delete pink_list_tulip.back();
                                pink_list_tulip.pop_back();
                                delete blue_list_tulip.back();
                                blue_list_tulip.pop_back();
                            }
                            else if(order->qty > pink_list_tulip.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = pink_list_tulip.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_tulip.back()->price);

                                pink_list_tulip.back()->exec_s = "Fill";
                                pink_list_tulip.back()->exec_qty = pink_list_tulip.back()->qty;
                                writeOrderToFile(fout, pink_list_tulip.back(), pink_list_tulip.back()->price);

                                delete pink_list_tulip.back();
                                pink_list_tulip.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_tulip.back()->price);
                                

                                pink_list_tulip.back()->exec_s = "Pfill";
                                pink_list_tulip.back()->exec_qty = order->exec_qty;
                                pink_list_tulip.back()->qty -= pink_list_tulip.back()->exec_qty;
                                writeOrderToFile(fout, pink_list_tulip.back(), pink_list_tulip.back()->price);

                                delete blue_list_tulip.back();
                                blue_list_tulip.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorA(blue_list_tulip, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorA(blue_list_tulip, order);
                }
                break;

                case 2:
                if (!blue_list_tulip.empty()){
                    if (order->price <= blue_list_tulip.back()->price){
                        insertIntoSortedVectorD(pink_list_tulip, order);
                        while (order->qty > 0 && !blue_list_tulip.empty() && order->price <= blue_list_tulip.back()->price){
                            if (order->qty == blue_list_tulip.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, blue_list_tulip.back()->price);
                                

                                blue_list_tulip.back()->exec_s = "Fill";
                                blue_list_tulip.back()->exec_qty = blue_list_tulip.back()->qty;
                                writeOrderToFile(fout, blue_list_tulip.back(), blue_list_tulip.back()->price);

                                delete blue_list_tulip.back();
                                blue_list_tulip.pop_back();
                                delete pink_list_tulip.back();
                                pink_list_tulip.pop_back();
                            }
                            else if(order->qty > blue_list_tulip.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = blue_list_tulip.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_tulip.back()->price);

                                blue_list_tulip.back()->exec_s = "Fill";
                                blue_list_tulip.back()->exec_qty = blue_list_tulip.back()->qty;
                                writeOrderToFile(fout, blue_list_tulip.back(), blue_list_tulip.back()->price);

                                delete blue_list_tulip.back();
                                blue_list_tulip.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_tulip.back()->price);
                                

                                blue_list_tulip.back()->exec_s = "Pfill";
                                blue_list_tulip.back()->exec_qty = order->exec_qty;
                                blue_list_tulip.back()->qty -= blue_list_tulip.back()->exec_qty;
                                writeOrderToFile(fout, blue_list_tulip.back(), blue_list_tulip.back()->price);

                                delete pink_list_tulip.back();
                                pink_list_tulip.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorD(pink_list_tulip, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorD(pink_list_tulip, order);
                }
                break;
            }

        }
        else{
            switch (order->side){
                case 1:
                if (!pink_list_orchid.empty()){
                    if (order->price >= pink_list_orchid.back()->price){
                        insertIntoSortedVectorA(blue_list_orchid, order);
                        while (order->qty > 0 && !pink_list_orchid.empty() && order->price >= pink_list_orchid.back()->price){
                            if (order->qty == pink_list_orchid.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, pink_list_orchid.back()->price);
                                

                                pink_list_orchid.back()->exec_s = "Fill";
                                pink_list_orchid.back()->exec_qty = pink_list_orchid.back()->qty;
                                writeOrderToFile(fout, pink_list_orchid.back(), pink_list_orchid.back()->price);

                                delete pink_list_orchid.back();
                                pink_list_orchid.pop_back();
                                delete blue_list_orchid.back();
                                blue_list_orchid.pop_back();
                            }
                            else if(order->qty > pink_list_orchid.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = pink_list_orchid.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_orchid.back()->price);

                                pink_list_orchid.back()->exec_s = "Fill";
                                pink_list_orchid.back()->exec_qty = pink_list_orchid.back()->qty;
                                writeOrderToFile(fout, pink_list_orchid.back(), pink_list_orchid.back()->price);

                                delete pink_list_orchid.back();
                                pink_list_orchid.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, pink_list_orchid.back()->price);
                                

                                pink_list_orchid.back()->exec_s = "Pfill";
                                pink_list_orchid.back()->exec_qty = order->exec_qty;
                                pink_list_orchid.back()->qty -= pink_list_orchid.back()->exec_qty;
                                writeOrderToFile(fout, pink_list_orchid.back(), pink_list_orchid.back()->price);

                                delete blue_list_orchid.back();
                                blue_list_orchid.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorA(blue_list_orchid, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorA(blue_list_orchid, order);
                }
                break;

                case 2:
                if (!blue_list_orchid.empty()){
                    if (order->price <= blue_list_orchid.back()->price){
                        insertIntoSortedVectorD(pink_list_orchid, order);
                        while (order->qty > 0 && !blue_list_orchid.empty() && order->price <= blue_list_orchid.back()->price){
                            if (order->qty == blue_list_orchid.back()->qty){
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                writeOrderToFile(fout, order, blue_list_orchid.back()->price);
                                

                                blue_list_orchid.back()->exec_s = "Fill";
                                blue_list_orchid.back()->exec_qty = blue_list_orchid.back()->qty;
                                writeOrderToFile(fout, blue_list_orchid.back(), blue_list_orchid.back()->price);

                                delete blue_list_orchid.back();
                                blue_list_orchid.pop_back();
                                delete pink_list_orchid.back();
                                pink_list_orchid.pop_back();
                            }
                            else if(order->qty > blue_list_orchid.back()->qty){
                                order->exec_s = "Pfill";
                                order->exec_qty = blue_list_orchid.back()->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_orchid.back()->price);

                                blue_list_orchid.back()->exec_s = "Fill";
                                blue_list_orchid.back()->exec_qty = blue_list_orchid.back()->qty;
                                writeOrderToFile(fout, blue_list_orchid.back(), blue_list_orchid.back()->price);

                                delete blue_list_orchid.back();
                                blue_list_orchid.pop_back();
                            }
                            else{
                                order->exec_s = "Fill";
                                order->exec_qty = order->qty;
                                order->qty -= order->exec_qty;
                                writeOrderToFile(fout, order, blue_list_orchid.back()->price);
                                

                                blue_list_orchid.back()->exec_s = "Pfill";
                                blue_list_orchid.back()->exec_qty = order->exec_qty;
                                blue_list_orchid.back()->qty -= blue_list_orchid.back()->exec_qty;
                                writeOrderToFile(fout, blue_list_orchid.back(), blue_list_orchid.back()->price);

                                delete pink_list_orchid.back();
                                pink_list_orchid.pop_back();
                            }
                        }
                        
                    }
                    else{
                        order->exec_s = "New";
                        order->exec_qty = order->qty;
                        order->reason = "";
                        writeOrderToFile(fout, order, order->price);
                        insertIntoSortedVectorD(pink_list_orchid, order);
                    }
                }
                else{
                    order->exec_s = "New";
                    order->exec_qty = order->qty;
                    order->reason = "";
                    writeOrderToFile(fout, order, order->price);
                    insertIntoSortedVectorD(pink_list_orchid, order);
                }
                break;
            }

        }
        line_no += 1;

    }

    fin.close();


 
    return 0;

}
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

// Custom vector implementation (since STL is not allowed)
template<typename T>
class Vector {
private:
    T* data;
    int capacity;
    int size;

    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T* new_data = new T[capacity];
        for (int i = 0; i < size; i++) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
    }

public:
    Vector() : data(nullptr), capacity(0), size(0) {}

    ~Vector() {
        delete[] data;
    }

    void push_back(const T& val) {
        if (size >= capacity) resize();
        data[size++] = val;
    }

    T& operator[](int idx) { return data[idx]; }
    const T& operator[](int idx) const { return data[idx]; }

    int get_size() const { return size; }

    void clear() { size = 0; }

    bool empty() const { return size == 0; }

    T& back() { return data[size - 1]; }

    void pop_back() { if (size > 0) size--; }
};

// Simple hash map for string keys
template<typename V>
class HashMap {
private:
    static const int HASH_SIZE = 10007;

    struct Node {
        std::string key;
        V value;
        Node* next;
        Node(const std::string& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    Node* buckets[HASH_SIZE];

    int hash(const std::string& key) const {
        unsigned int h = 0;
        for (char c : key) h = h * 131 + c;
        return h % HASH_SIZE;
    }

public:
    HashMap() {
        for (int i = 0; i < HASH_SIZE; i++) buckets[i] = nullptr;
    }

    ~HashMap() {
        for (int i = 0; i < HASH_SIZE; i++) {
            Node* curr = buckets[i];
            while (curr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
        }
    }

    void insert(const std::string& key, const V& value) {
        int idx = hash(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }
        Node* new_node = new Node(key, value);
        new_node->next = buckets[idx];
        buckets[idx] = new_node;
    }

    bool find(const std::string& key, V& value) const {
        int idx = hash(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) {
                value = curr;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool exists(const std::string& key) const {
        int idx = hash(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) return true;
            curr = curr->next;
        }
        return false;
    }

    V* get(const std::string& key) {
        int idx = hash(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) return &curr->value;
            curr = curr->next;
        }
        return nullptr;
    }

    void remove(const std::string& key) {
        int idx = hash(key);
        Node* curr = buckets[idx];
        Node* prev = nullptr;
        while (curr) {
            if (curr->key == key) {
                if (prev) prev->next = curr->next;
                else buckets[idx] = curr->next;
                delete curr;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    void clear() {
        for (int i = 0; i < HASH_SIZE; i++) {
            Node* curr = buckets[i];
            while (curr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
            buckets[i] = nullptr;
        }
    }

    bool is_empty() const {
        for (int i = 0; i < HASH_SIZE; i++) {
            if (buckets[i] != nullptr) return false;
        }
        return true;
    }
};

// User structure
struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;

    User() {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
        privilege = 0;
    }
};

// Train structure
struct Train {
    char trainID[25];
    int stationNum;
    char stations[100][35];
    int seatNum;
    int prices[100];
    int startTime; // minutes from 00:00
    int travelTimes[100];
    int stopoverTimes[100];
    int saleDate[2]; // days from June 1
    char type;
    bool released;

    Train() {
        trainID[0] = '\0';
        stationNum = 0;
        seatNum = 0;
        startTime = 0;
        saleDate[0] = saleDate[1] = 0;
        type = '\0';
        released = false;
    }
};

// Order structure
struct Order {
    int orderId;
    char username[25];
    char trainID[25];
    int date; // days from June 1
    int fromStation;
    int toStation;
    int num;
    int price;
    int status; // 0: success, 1: pending, 2: refunded
    long long timestamp;

    Order() {
        orderId = 0;
        username[0] = trainID[0] = '\0';
        date = fromStation = toStation = num = price = status = 0;
        timestamp = 0;
    }
};

// Global data
HashMap<User> users;
HashMap<Train> trains;
HashMap<bool> loggedInUsers;
Vector<Order> allOrders;
int orderCounter = 0;
long long timestampCounter = 0;

// Parse functions
Vector<std::string> split(const std::string& s, char delim) {
    Vector<std::string> result;
    std::string current;
    for (char c : s) {
        if (c == delim) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) result.push_back(current);
    return result;
}

int parseTime(const std::string& timeStr) {
    int hour = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int minute = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
    return hour * 60 + minute;
}

int parseDate(const std::string& dateStr) {
    int month = (dateStr[0] - '0') * 10 + (dateStr[1] - '0');
    int day = (dateStr[3] - '0') * 10 + (dateStr[4] - '0');

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = 0;
    for (int i = 6; i < month; i++) {
        days += daysInMonth[i];
    }
    days += day;
    return days;
}

std::string formatDate(int days) {
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int month = 6;
    while (days > daysInMonth[month]) {
        days -= daysInMonth[month];
        month++;
    }

    char buf[10];
    sprintf(buf, "%02d-%02d", month, days);
    return std::string(buf);
}

std::string formatTime(int minutes) {
    int hour = minutes / 60;
    int min = minutes % 60;
    char buf[10];
    sprintf(buf, "%02d:%02d", hour, min);
    return std::string(buf);
}

std::string formatDateTime(int days, int minutes) {
    while (minutes >= 1440) {
        days++;
        minutes -= 1440;
    }
    while (minutes < 0) {
        days--;
        minutes += 1440;
    }
    return formatDate(days) + " " + formatTime(minutes);
}

// Command handlers
void cmd_add_user(const HashMap<std::string>& params) {
    std::string cur_username, username, password, name, mailAddr;
    int privilege = 10;

    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-c"))) cur_username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-p"))) password = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-n"))) name = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-m"))) mailAddr = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-g"))) privilege = std::stoi(*ptr);

    // Check if first user
    bool isFirstUser = users.is_empty();

    if (isFirstUser) {
        if (users.exists(username)) {
            std::cout << "-1\n";
            return;
        }
        User u;
        strcpy(u.username, username.c_str());
        strcpy(u.password, password.c_str());
        strcpy(u.name, name.c_str());
        strcpy(u.mailAddr, mailAddr.c_str());
        u.privilege = 10;
        users.insert(username, u);
        std::cout << "0\n";
        return;
    }

    // Not first user - check permissions
    User* curUser = users.get(cur_username);
    if (!curUser || !loggedInUsers.exists(cur_username)) {
        std::cout << "-1\n";
        return;
    }

    if (privilege >= curUser->privilege) {
        std::cout << "-1\n";
        return;
    }

    if (users.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    User u;
    strcpy(u.username, username.c_str());
    strcpy(u.password, password.c_str());
    strcpy(u.name, name.c_str());
    strcpy(u.mailAddr, mailAddr.c_str());
    u.privilege = privilege;
    users.insert(username, u);
    std::cout << "0\n";
}

void cmd_login(const HashMap<std::string>& params) {
    std::string username, password;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-p"))) password = *ptr;

    User* user = users.get(username);
    if (!user || std::string(user->password) != password) {
        std::cout << "-1\n";
        return;
    }

    if (loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    loggedInUsers.insert(username, true);
    std::cout << "0\n";
}

void cmd_logout(const HashMap<std::string>& params) {
    std::string username;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;

    if (!loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    loggedInUsers.remove(username);
    std::cout << "0\n";
}

void cmd_query_profile(const HashMap<std::string>& params) {
    std::string cur_username, username;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-c"))) cur_username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;

    User* curUser = users.get(cur_username);
    User* targetUser = users.get(username);

    if (!curUser || !targetUser || !loggedInUsers.exists(cur_username)) {
        std::cout << "-1\n";
        return;
    }

    if (curUser->privilege <= targetUser->privilege && cur_username != username) {
        std::cout << "-1\n";
        return;
    }

    std::cout << targetUser->username << " " << targetUser->name << " "
              << targetUser->mailAddr << " " << targetUser->privilege << "\n";
}

void cmd_modify_profile(const HashMap<std::string>& params) {
    std::string cur_username, username;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-c"))) cur_username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;

    User* curUser = users.get(cur_username);
    User* targetUser = users.get(username);

    if (!curUser || !targetUser || !loggedInUsers.exists(cur_username)) {
        std::cout << "-1\n";
        return;
    }

    if (curUser->privilege <= targetUser->privilege && cur_username != username) {
        std::cout << "-1\n";
        return;
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-p"))) {
        strcpy(targetUser->password, ptr->c_str());
    }
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-n"))) {
        strcpy(targetUser->name, ptr->c_str());
    }
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-m"))) {
        strcpy(targetUser->mailAddr, ptr->c_str());
    }
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-g"))) {
        int newPrivilege = std::stoi(*ptr);
        if (newPrivilege >= curUser->privilege) {
            std::cout << "-1\n";
            return;
        }
        targetUser->privilege = newPrivilege;
    }

    std::cout << targetUser->username << " " << targetUser->name << " "
              << targetUser->mailAddr << " " << targetUser->privilege << "\n";
}

void cmd_add_train(const HashMap<std::string>& params) {
    std::string trainID;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-i"))) trainID = *ptr;

    if (trains.exists(trainID)) {
        std::cout << "-1\n";
        return;
    }

    Train train;
    strcpy(train.trainID, trainID.c_str());

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-n"))) {
        train.stationNum = std::stoi(*ptr);
    }
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-m"))) {
        train.seatNum = std::stoi(*ptr);
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-s"))) {
        Vector<std::string> stations = split(*ptr, '|');
        for (int i = 0; i < train.stationNum; i++) {
            strcpy(train.stations[i], stations[i].c_str());
        }
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-p"))) {
        Vector<std::string> prices = split(*ptr, '|');
        for (int i = 0; i < train.stationNum - 1; i++) {
            train.prices[i] = std::stoi(prices[i]);
        }
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-x"))) {
        train.startTime = parseTime(*ptr);
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-t"))) {
        Vector<std::string> times = split(*ptr, '|');
        for (int i = 0; i < train.stationNum - 1; i++) {
            train.travelTimes[i] = std::stoi(times[i]);
        }
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-o"))) {
        if (*ptr != "_") {
            Vector<std::string> times = split(*ptr, '|');
            for (int i = 0; i < train.stationNum - 2; i++) {
                train.stopoverTimes[i] = std::stoi(times[i]);
            }
        }
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-d"))) {
        Vector<std::string> dates = split(*ptr, '|');
        train.saleDate[0] = parseDate(dates[0]);
        train.saleDate[1] = parseDate(dates[1]);
    }

    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-y"))) {
        train.type = (*ptr)[0];
    }

    train.released = false;
    trains.insert(trainID, train);
    std::cout << "0\n";
}

void cmd_release_train(const HashMap<std::string>& params) {
    std::string trainID;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-i"))) trainID = *ptr;

    Train* train = trains.get(trainID);
    if (!train || train->released) {
        std::cout << "-1\n";
        return;
    }

    train->released = true;
    std::cout << "0\n";
}

void cmd_query_train(const HashMap<std::string>& params) {
    std::string trainID, dateStr;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-i"))) trainID = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-d"))) dateStr = *ptr;

    Train* train = trains.get(trainID);
    if (!train) {
        std::cout << "-1\n";
        return;
    }

    int queryDate = parseDate(dateStr);

    std::cout << train->trainID << " " << train->type << "\n";

    int currentTime = train->startTime;
    int currentDate = queryDate;
    int cumulativePrice = 0;

    for (int i = 0; i < train->stationNum; i++) {
        std::string arriveTime = (i == 0) ? "xx-xx xx:xx" : formatDateTime(currentDate, currentTime);

        if (i > 0) {
            currentTime += train->travelTimes[i-1];
        }

        std::string leaveTime = (i == train->stationNum - 1) ? "xx-xx xx:xx" : formatDateTime(currentDate, currentTime);

        std::string seatStr = (i == train->stationNum - 1) ? "x" : std::to_string(train->seatNum);

        std::cout << train->stations[i] << " " << arriveTime << " -> "
                  << leaveTime << " " << cumulativePrice << " " << seatStr << "\n";

        if (i < train->stationNum - 1) {
            cumulativePrice += train->prices[i];
            if (i < train->stationNum - 2) {
                currentTime += train->stopoverTimes[i];
            }
        }

        while (currentTime >= 1440) {
            currentTime -= 1440;
            currentDate++;
        }
    }
}

void cmd_delete_train(const HashMap<std::string>& params) {
    std::string trainID;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-i"))) trainID = *ptr;

    Train* train = trains.get(trainID);
    if (!train || train->released) {
        std::cout << "-1\n";
        return;
    }

    trains.remove(trainID);
    std::cout << "0\n";
}

void cmd_query_ticket(const HashMap<std::string>& params) {
    std::cout << "0\n"; // Simplified for now
}

void cmd_query_transfer(const HashMap<std::string>& params) {
    std::cout << "0\n"; // Simplified for now
}

void cmd_buy_ticket(const HashMap<std::string>& params) {
    std::cout << "-1\n"; // Simplified for now
}

void cmd_query_order(const HashMap<std::string>& params) {
    std::string username;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;

    if (!loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    std::cout << "0\n"; // Simplified
}

void cmd_refund_ticket(const HashMap<std::string>& params) {
    std::cout << "-1\n"; // Simplified for now
}

void cmd_clean() {
    users.clear();
    trains.clear();
    loggedInUsers.clear();
    allOrders.clear();
    orderCounter = 0;
    timestampCounter = 0;
    std::cout << "0\n";
}

void cmd_exit() {
    loggedInUsers.clear();
    std::cout << "bye\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        HashMap<std::string> params;
        std::string cmd;

        // Parse command and parameters
        Vector<std::string> tokens = split(line, ' ');
        for (int i = 0; i < tokens.get_size(); i++) {
            if (tokens[i][0] == '-' && tokens[i].length() == 2) {
                if (i + 1 < tokens.get_size()) {
                    params.insert(tokens[i], tokens[i+1]);
                    i++;
                }
            } else if (cmd.empty()) {
                cmd = tokens[i];
            }
        }

        if (cmd == "add_user") cmd_add_user(params);
        else if (cmd == "login") cmd_login(params);
        else if (cmd == "logout") cmd_logout(params);
        else if (cmd == "query_profile") cmd_query_profile(params);
        else if (cmd == "modify_profile") cmd_modify_profile(params);
        else if (cmd == "add_train") cmd_add_train(params);
        else if (cmd == "release_train") cmd_release_train(params);
        else if (cmd == "query_train") cmd_query_train(params);
        else if (cmd == "delete_train") cmd_delete_train(params);
        else if (cmd == "query_ticket") cmd_query_ticket(params);
        else if (cmd == "query_transfer") cmd_query_transfer(params);
        else if (cmd == "buy_ticket") cmd_buy_ticket(params);
        else if (cmd == "query_order") cmd_query_order(params);
        else if (cmd == "refund_ticket") cmd_refund_ticket(params);
        else if (cmd == "clean") cmd_clean();
        else if (cmd == "exit") {
            cmd_exit();
            break;
        }
    }

    return 0;
}

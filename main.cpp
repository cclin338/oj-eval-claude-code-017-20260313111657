#include <iostream>
#include <cstring>
#include <string>
#include <cstdio>

// Custom vector implementation
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
    ~Vector() { delete[] data; }

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

    void sort_by(bool (*cmp)(const T&, const T&)) {
        for (int i = 0; i < size - 1; i++) {
            for (int j = i + 1; j < size; j++) {
                if (cmp(data[j], data[i])) {
                    T tmp = data[i];
                    data[i] = data[j];
                    data[j] = tmp;
                }
            }
        }
    }
};

// Simple hash map
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

// Data structures
struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    User() { username[0] = password[0] = name[0] = mailAddr[0] = '\0'; privilege = 0; }
};

struct Train {
    char trainID[25];
    int stationNum;
    char stations[100][35];
    int seatNum;
    int prices[100];
    int startTime;
    int travelTimes[100];
    int stopoverTimes[100];
    int saleDate[2];
    char type;
    bool released;

    Train() {
        trainID[0] = '\0';
        stationNum = seatNum = startTime = 0;
        saleDate[0] = saleDate[1] = 0;
        type = '\0';
        released = false;
        for (int i = 0; i < 100; i++) {
            stations[i][0] = '\0';
            prices[i] = travelTimes[i] = stopoverTimes[i] = 0;
        }
    }
};

struct TrainSeat {
    char trainID[25];
    int date;
    int seats[100]; // remaining seats for each segment

    TrainSeat() {
        trainID[0] = '\0';
        date = 0;
        for (int i = 0; i < 100; i++) seats[i] = 0;
    }
};

struct Order {
    int orderId;
    char username[25];
    char trainID[25];
    int date;
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

struct PendingOrder {
    int orderId;
    char trainID[25];
    int date;
    int fromStation;
    int toStation;
    int num;

    PendingOrder() {
        orderId = 0;
        trainID[0] = '\0';
        date = fromStation = toStation = num = 0;
    }
};

// Global data
HashMap<User> users;
HashMap<Train> trains;
Vector<std::string> trainIDs; // List of all train IDs for iteration
HashMap<bool> loggedInUsers;
Vector<Order> allOrders;
Vector<PendingOrder> pendingQueue;
HashMap<Vector<TrainSeat>*> trainSeats;
int orderCounter = 0;
long long timestampCounter = 0;

// Utility functions
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
    for (int i = 6; i < month; i++) days += daysInMonth[i];
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
    int days = 0;
    while (minutes >= 1440) {
        days++;
        minutes -= 1440;
    }
    while (minutes < 0) {
        days--;
        minutes += 1440;
    }
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

TrainSeat* getTrainSeat(const std::string& trainID, int date, const Train* train) {
    std::string key = trainID + "_" + std::to_string(date);
    Vector<TrainSeat>** seatsPtr = trainSeats.get(trainID);

    Vector<TrainSeat>* seats = nullptr;
    if (!seatsPtr) {
        seats = new Vector<TrainSeat>();
        trainSeats.insert(trainID, seats);
    } else {
        seats = *seatsPtr;
    }

    for (int i = 0; i < seats->get_size(); i++) {
        if ((*seats)[i].date == date) {
            return &(*seats)[i];
        }
    }

    // Create new seat record
    TrainSeat newSeat;
    strcpy(newSeat.trainID, trainID.c_str());
    newSeat.date = date;
    if (train) {
        for (int i = 0; i < train->stationNum - 1; i++) {
            newSeat.seats[i] = train->seatNum;
        }
    }
    seats->push_back(newSeat);
    return &(*seats)[seats->get_size() - 1];
}

// Command implementations
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
    trainIDs.push_back(trainID);
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
    int startDate = queryDate;

    std::cout << train->trainID << " " << train->type << "\n";

    TrainSeat* seat = nullptr;
    if (train->released) {
        seat = getTrainSeat(trainID, startDate, train);
    }

    int cumulativePrice = 0;
    int currentDate = startDate;
    int currentTime = train->startTime;

    for (int i = 0; i < train->stationNum; i++) {
        // Print arrival time
        std::string arriveTime;
        if (i == 0) {
            arriveTime = "xx-xx xx:xx";
        } else {
            arriveTime = formatDateTime(currentDate, currentTime);
        }

        // Add stopover time (only for middle stations)
        if (i > 0 && i < train->stationNum - 1) {
            currentTime += train->stopoverTimes[i - 1];
            while (currentTime >= 1440) {
                currentTime -= 1440;
                currentDate++;
            }
        }

        // Print leaving time
        std::string leaveTime;
        if (i == train->stationNum - 1) {
            leaveTime = "xx-xx xx:xx";
        } else {
            leaveTime = formatDateTime(currentDate, currentTime);
        }

        // Get seat count
        std::string seatStr;
        if (i == train->stationNum - 1) {
            seatStr = "x";
        } else if (seat) {
            seatStr = std::to_string(seat->seats[i]);
        } else {
            seatStr = std::to_string(train->seatNum);
        }

        std::cout << train->stations[i] << " " << arriveTime << " -> "
                  << leaveTime << " " << cumulativePrice << " " << seatStr << "\n";

        // Update for next station
        if (i < train->stationNum - 1) {
            cumulativePrice += train->prices[i];
            currentTime += train->travelTimes[i];
            while (currentTime >= 1440) {
                currentTime -= 1440;
                currentDate++;
            }
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
    // Remove from trainIDs list
    for (int i = 0; i < trainIDs.get_size(); i++) {
        if (trainIDs[i] == trainID) {
            for (int j = i; j < trainIDs.get_size() - 1; j++) {
                trainIDs[j] = trainIDs[j + 1];
            }
            trainIDs.pop_back();
            break;
        }
    }
    std::cout << "0\n";
}

struct TicketInfo {
    char trainID[25];
    char fromStation[35];
    char toStation[35];
    int fromIndex;
    int toIndex;
    int leaveDate;
    int leaveTime;
    int arriveDate;
    int arriveTime;
    int price;
    int seat;
    int travelTime;

    TicketInfo() {
        trainID[0] = fromStation[0] = toStation[0] = '\0';
        fromIndex = toIndex = leaveDate = leaveTime = arriveDate = arriveTime = 0;
        price = seat = travelTime = 0;
    }
};

bool cmp_time(const TicketInfo& a, const TicketInfo& b) {
    if (a.travelTime != b.travelTime) return a.travelTime < b.travelTime;
    return std::string(a.trainID) < std::string(b.trainID);
}

bool cmp_cost(const TicketInfo& a, const TicketInfo& b) {
    if (a.price != b.price) return a.price < b.price;
    return std::string(a.trainID) < std::string(b.trainID);
}

void cmd_query_ticket(const HashMap<std::string>& params) {
    std::string fromStation, toStation, dateStr, sortType = "time";
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-s"))) fromStation = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-t"))) toStation = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-d"))) dateStr = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-p"))) sortType = *ptr;

    int queryDate = parseDate(dateStr);

    Vector<TicketInfo> results;

    // Iterate through all trains
    for (int i = 0; i < trainIDs.get_size(); i++) {
        Train* train = trains.get(trainIDs[i]);
        if (!train || !train->released) continue;

        // Find station indices
        int fromIdx = -1, toIdx = -1;
        for (int j = 0; j < train->stationNum; j++) {
            if (std::string(train->stations[j]) == fromStation) fromIdx = j;
            if (std::string(train->stations[j]) == toStation) toIdx = j;
        }

        if (fromIdx == -1 || toIdx == -1 || fromIdx >= toIdx) continue;

        // Calculate when this train departs from fromStation
        int timeOffset = train->startTime;
        int dateOffset = 0;
        for (int j = 0; j < fromIdx; j++) {
            timeOffset += train->travelTimes[j];
            while (timeOffset >= 1440) {
                timeOffset -= 1440;
                dateOffset++;
            }
            if (j < fromIdx - 1) {
                timeOffset += train->stopoverTimes[j];
                while (timeOffset >= 1440) {
                    timeOffset -= 1440;
                    dateOffset++;
                }
            }
        }

        // startDate is when train actually starts
        int startDate = queryDate - dateOffset;

        // Check if within sale period
        if (startDate < train->saleDate[0] || startDate > train->saleDate[1]) continue;

        // Calculate arrival time at toStation
        int arriveTime = timeOffset;
        int arriveDateOffset = dateOffset;
        for (int j = fromIdx; j < toIdx; j++) {
            arriveTime += train->travelTimes[j];
            while (arriveTime >= 1440) {
                arriveTime -= 1440;
                arriveDateOffset++;
            }
            if (j < toIdx - 1) {
                arriveTime += train->stopoverTimes[j];
                while (arriveTime >= 1440) {
                    arriveTime -= 1440;
                    arriveDateOffset++;
                }
            }
        }

        // Calculate price
        int totalPrice = 0;
        for (int j = fromIdx; j < toIdx; j++) {
            totalPrice += train->prices[j];
        }

        // Get minimum seats
        TrainSeat* seat = getTrainSeat(trainIDs[i], startDate, train);
        int minSeat = seat->seats[fromIdx];
        for (int j = fromIdx; j < toIdx; j++) {
            if (seat->seats[j] < minSeat) minSeat = seat->seats[j];
        }

        // Save result
        TicketInfo info;
        strcpy(info.trainID, train->trainID);
        strcpy(info.fromStation, train->stations[fromIdx]);
        strcpy(info.toStation, train->stations[toIdx]);
        info.fromIndex = fromIdx;
        info.toIndex = toIdx;
        info.leaveDate = queryDate;
        info.leaveTime = timeOffset;
        info.arriveDate = queryDate + (arriveDateOffset - dateOffset);
        info.arriveTime = arriveTime;
        info.price = totalPrice;
        info.seat = minSeat;
        info.travelTime = (arriveDateOffset - dateOffset) * 1440 + (arriveTime - timeOffset);

        results.push_back(info);
    }

    // Sort results
    if (sortType == "cost") {
        results.sort_by(cmp_cost);
    } else {
        results.sort_by(cmp_time);
    }

    // Output
    std::cout << results.get_size() << "\n";
    for (int i = 0; i < results.get_size(); i++) {
        TicketInfo& info = results[i];
        std::cout << info.trainID << " " << info.fromStation << " "
                  << formatDateTime(info.leaveDate, info.leaveTime) << " -> "
                  << info.toStation << " "
                  << formatDateTime(info.arriveDate, info.arriveTime) << " "
                  << info.price << " " << info.seat << "\n";
    }
}

void cmd_query_transfer(const HashMap<std::string>& params) {
    std::cout << "0\n"; // Simplified
}

void cmd_buy_ticket(const HashMap<std::string>& params) {
    std::string username, trainID, dateStr, from, to;
    int num = 0;
    bool queue = false;

    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-i"))) trainID = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-d"))) dateStr = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-n"))) num = std::stoi(*ptr);
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-f"))) from = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-t"))) to = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-q"))) queue = (*ptr == "true");

    if (!loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    Train* train = trains.get(trainID);
    if (!train || !train->released) {
        std::cout << "-1\n";
        return;
    }

    int queryDate = parseDate(dateStr);

    // Find station indices
    int fromIdx = -1, toIdx = -1;
    for (int i = 0; i < train->stationNum; i++) {
        if (std::string(train->stations[i]) == from) fromIdx = i;
        if (std::string(train->stations[i]) == to) toIdx = i;
    }

    if (fromIdx == -1 || toIdx == -1 || fromIdx >= toIdx) {
        std::cout << "-1\n";
        return;
    }

    // Calculate total price
    int totalPrice = 0;
    for (int i = fromIdx; i < toIdx; i++) {
        totalPrice += train->prices[i];
    }
    totalPrice *= num;

    if (num > train->seatNum) {
        std::cout << "-1\n";
        return;
    }

    // Calculate train start date
    int currentTime = train->startTime;
    int daysOffset = 0;
    for (int i = 0; i < fromIdx; i++) {
        currentTime += train->travelTimes[i];
        if (i < fromIdx - 1) currentTime += train->stopoverTimes[i];
        while (currentTime >= 1440) {
            currentTime -= 1440;
            daysOffset++;
        }
    }
    int startDate = queryDate - daysOffset;

    if (startDate < train->saleDate[0] || startDate > train->saleDate[1]) {
        std::cout << "-1\n";
        return;
    }

    // Get seat information
    TrainSeat* seat = getTrainSeat(trainID, startDate, train);

    // Check if enough seats
    int minSeat = seat->seats[fromIdx];
    for (int i = fromIdx; i < toIdx; i++) {
        if (seat->seats[i] < minSeat) minSeat = seat->seats[i];
    }

    if (minSeat >= num) {
        // Buy ticket successfully
        for (int i = fromIdx; i < toIdx; i++) {
            seat->seats[i] -= num;
        }

        Order order;
        order.orderId = orderCounter++;
        strcpy(order.username, username.c_str());
        strcpy(order.trainID, trainID.c_str());
        order.date = startDate;
        order.fromStation = fromIdx;
        order.toStation = toIdx;
        order.num = num;
        order.price = totalPrice;
        order.status = 0; // success
        order.timestamp = timestampCounter++;
        allOrders.push_back(order);

        std::cout << totalPrice << "\n";
    } else if (queue) {
        // Add to pending queue
        Order order;
        order.orderId = orderCounter++;
        strcpy(order.username, username.c_str());
        strcpy(order.trainID, trainID.c_str());
        order.date = startDate;
        order.fromStation = fromIdx;
        order.toStation = toIdx;
        order.num = num;
        order.price = totalPrice;
        order.status = 1; // pending
        order.timestamp = timestampCounter++;
        allOrders.push_back(order);

        PendingOrder porder;
        porder.orderId = order.orderId;
        strcpy(porder.trainID, trainID.c_str());
        porder.date = startDate;
        porder.fromStation = fromIdx;
        porder.toStation = toIdx;
        porder.num = num;
        pendingQueue.push_back(porder);

        std::cout << "queue\n";
    } else {
        std::cout << "-1\n";
    }
}

void cmd_query_order(const HashMap<std::string>& params) {
    std::string username;
    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;

    if (!loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    Vector<Order*> userOrders;
    for (int i = 0; i < allOrders.get_size(); i++) {
        if (std::string(allOrders[i].username) == username) {
            userOrders.push_back(&allOrders[i]);
        }
    }

    std::cout << userOrders.get_size() << "\n";

    for (int i = userOrders.get_size() - 1; i >= 0; i--) {
        Order* order = userOrders[i];
        Train* train = trains.get(std::string(order->trainID));
        if (!train) continue;

        std::string status;
        if (order->status == 0) status = "success";
        else if (order->status == 1) status = "pending";
        else status = "refunded";

        // Calculate times
        int currentTime = train->startTime;
        int leaveDate = order->date;
        int arriveDate = order->date;
        int leaveTime = currentTime;

        for (int j = 0; j < order->fromStation; j++) {
            currentTime += train->travelTimes[j];
            while (currentTime >= 1440) {
                currentTime -= 1440;
                leaveDate++;
            }
            if (j < order->fromStation - 1) currentTime += train->stopoverTimes[j];
        }
        leaveTime = currentTime;

for (int j = order->fromStation; j < order->toStation; j++) {
            currentTime += train->travelTimes[j];
            while (currentTime >= 1440) {
                currentTime -= 1440;
                arriveDate++;
            }
            if (j < order->toStation - 1) currentTime += train->stopoverTimes[j];
        }
        int arriveTime = currentTime;

        std::cout << "[" << status << "] " << order->trainID << " "
                  << train->stations[order->fromStation] << " "
                  << formatDateTime(leaveDate, leaveTime) << " -> "
                  << train->stations[order->toStation] << " "
                  << formatDateTime(arriveDate, arriveTime) << " "
                  << order->price << " " << order->num << "\n";
    }
}

void processPendingQueue() {
    for (int i = 0; i < pendingQueue.get_size(); ) {
        PendingOrder& porder = pendingQueue[i];
        Train* train = trains.get(std::string(porder.trainID));
        if (!train) {
            i++;
            continue;
        }

        TrainSeat* seat = getTrainSeat(std::string(porder.trainID), porder.date, train);

        int minSeat = seat->seats[porder.fromStation];
        for (int j = porder.fromStation; j < porder.toStation; j++) {
            if (seat->seats[j] < minSeat) minSeat = seat->seats[j];
        }

        if (minSeat >= porder.num) {
            // Can fulfill this order
            for (int j = porder.fromStation; j < porder.toStation; j++) {
                seat->seats[j] -= porder.num;
            }

            // Update order status
            for (int j = 0; j < allOrders.get_size(); j++) {
                if (allOrders[j].orderId == porder.orderId) {
                    allOrders[j].status = 0; // success
                    break;
                }
            }

            // Remove from pending queue
            for (int j = i; j < pendingQueue.get_size() - 1; j++) {
                pendingQueue[j] = pendingQueue[j + 1];
            }
            pendingQueue.pop_back();
        } else {
            i++;
        }
    }
}

void cmd_refund_ticket(const HashMap<std::string>& params) {
    std::string username;
    int n = 1;

    std::string* ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-u"))) username = *ptr;
    if ((ptr = const_cast<HashMap<std::string>&>(params).get("-n"))) n = std::stoi(*ptr);

    if (!loggedInUsers.exists(username)) {
        std::cout << "-1\n";
        return;
    }

    Vector<Order*> userOrders;
    for (int i = 0; i < allOrders.get_size(); i++) {
        if (std::string(allOrders[i].username) == username) {
            userOrders.push_back(&allOrders[i]);
        }
    }

    if (n > userOrders.get_size() || n <= 0) {
        std::cout << "-1\n";
        return;
    }

    Order* order = userOrders[userOrders.get_size() - n];

    if (order->status == 2) { // already refunded
        std::cout << "-1\n";
        return;
    }

    if (order->status == 0) { // success, need to return seats
        Train* train = trains.get(std::string(order->trainID));
        if (train) {
            TrainSeat* seat = getTrainSeat(std::string(order->trainID), order->date, train);
            for (int i = order->fromStation; i < order->toStation; i++) {
                seat->seats[i] += order->num;
            }
            processPendingQueue();
        }
    } else if (order->status == 1) { // pending, remove from queue
        for (int i = 0; i < pendingQueue.get_size(); i++) {
            if (pendingQueue[i].orderId == order->orderId) {
                for (int j = i; j < pendingQueue.get_size() - 1; j++) {
                    pendingQueue[j] = pendingQueue[j + 1];
                }
                pendingQueue.pop_back();
                break;
            }
        }
    }

    order->status = 2; // refunded
    std::cout << "0\n";
}

void cmd_clean() {
    users.clear();
    trains.clear();
    trainIDs.clear();
    loggedInUsers.clear();
    allOrders.clear();
    pendingQueue.clear();
    trainSeats.clear();
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

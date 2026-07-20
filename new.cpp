#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <ctime>

using namespace std;

// Forward declarations
class User;
class HotelOwner;
class Customer;
class Hotel;
class Room;
class Reservation;
class Package;
class SystemStats;

// Enum for user types
enum class UserType { HOTEL_OWNER, CUSTOMER };

// Enum for room types
enum class RoomType { STANDARD, DELUXE, SUITE, PRESIDENTIAL };

// Enum for reservation status
enum class ReservationStatus { PENDING, CONFIRMED, CANCELLED, CHECKED_IN, CHECKED_OUT };

// Package Class
class Package {
private:
    string packageName;
    string description;
    double priceMultiplier;
    vector<string> inclusions;

public:
    Package(const string& name, const string& desc, double multiplier)
        : packageName(name), description(desc), priceMultiplier(multiplier) {}

    void addInclusion(const string& inclusion) {
        inclusions.push_back(inclusion);
    }

    double applyPackage(double basePrice) const {
        return basePrice * priceMultiplier;
    }

    void displayPackage() const {
        cout << "📦 " << packageName << " (x" << priceMultiplier << " price)\n";
        cout << "   📝 " << description << "\n";
        cout << "   ✅ Inclusions: ";
        for (const auto& inc : inclusions) {
            cout << inc << ", ";
        }
        cout << "\n";
    }

    string getName() const { return packageName; }
    double getMultiplier() const { return priceMultiplier; }
};

// Statistics Class
class SystemStats {
private:
    static SystemStats* instance;
    int totalLogins;
    int ownerLogins;
    int customerLogins;
    vector<string> loginHistory;

    SystemStats() : totalLogins(0), ownerLogins(0), customerLogins(0) {}

public:
    static SystemStats* getInstance() {
        if (!instance) {
            instance = new SystemStats();
        }
        return instance;
    }

    void recordLogin(UserType type, const string& username) {
        totalLogins++;
        if (type == UserType::HOTEL_OWNER) {
            ownerLogins++;
        } else {
            customerLogins++;
        }
        
        time_t now = time(0);
        string timeStr = ctime(&now);
        timeStr.pop_back();
        
        string loginRecord = timeStr + " - " + username + 
                                 (type == UserType::HOTEL_OWNER ? " (Owner)" : " (Customer)");
        loginHistory.push_back(loginRecord);
    }

    void displayStats() const {
        cout << "\n📊 SYSTEM STATISTICS\n";
        cout << "===================\n";
        cout << "👥 Total Logins: " << totalLogins << "\n";
        cout << "🏨 Owner Logins: " << ownerLogins << "\n";
        cout << "👤 Customer Logins: " << customerLogins << "\n";
        
        cout << "\n📋 Recent Login History:\n";
        int start = max(0, (int)loginHistory.size() - 5);
        for (int i = start; i < loginHistory.size(); i++) {
            cout << "   " << loginHistory[i] << "\n";
        }
    }
};

SystemStats* SystemStats::instance = nullptr;

// Base User Class
class User {
protected:
    static int nextUserId;
    int userId;
    string username;
    string password;
    string email;
    string phoneNumber;
    UserType userType;
    string registrationDate;

public:
    User(const string& uname, const string& pwd, const string& mail, 
         const string& phone, UserType type)
        : userId(++nextUserId), username(uname), password(pwd), email(mail), 
          phoneNumber(phone), userType(type) {
        time_t now = time(0);
        registrationDate = ctime(&now);
        registrationDate.pop_back();
    }

    virtual ~User() = default;

    bool authenticate(const string& uname, const string& pwd) const {
        return (username == uname && password == pwd);
    }

    int getUserId() const { return userId; }
    string getUsername() const { return username; }
    string getEmail() const { return email; }
    string getPhone() const { return phoneNumber; }
    UserType getUserType() const { return userType; }
    string getRegistrationDate() const { return registrationDate; }

    virtual void displayDashboard() = 0;
};

int User::nextUserId = 0;

// Room Class Hierarchy
class Room {
protected:
    int roomNumber;
    double basePricePerNight;
    bool isAvailable;
    RoomType type;
    string amenities;
    int maxOccupancy;

public:
    Room(int roomNo, double price, RoomType t, int occupancy = 2) 
        : roomNumber(roomNo), basePricePerNight(price), isAvailable(true), 
          type(t), maxOccupancy(occupancy) {}

    virtual ~Room() = default;

    virtual double calculateTotalCost(int nights, const Package* package = nullptr) const {
        double price = basePricePerNight * nights;
        if (package) {
            price = package->applyPackage(price);
        }
        return price;
    }

    virtual void displayDetails() const {
        cout << "🛏️  Room " << roomNumber << " - $" << basePricePerNight << "/night\n";
        cout << "   Type: " << getRoomTypeString() << "\n";
        cout << "   Status: " << (isAvailable ? "✅ Available" : "❌ Occupied") << "\n";
        cout << "   Max Occupancy: " << maxOccupancy << " people\n";
        cout << "   Amenities: " << amenities << "\n";
    }

    string getRoomTypeString() const {
        switch(type) {
            case RoomType::STANDARD: return "Standard";
            case RoomType::DELUXE: return "Deluxe";
            case RoomType::SUITE: return "Suite";
            case RoomType::PRESIDENTIAL: return "Presidential";
            default: return "Unknown";
        }
    }

    int getRoomNumber() const { return roomNumber; }
    double getBasePrice() const { return basePricePerNight; }
    bool getAvailability() const { return isAvailable; }
    RoomType getRoomType() const { return type; }
    int getMaxOccupancy() const { return maxOccupancy; }
    void setAvailability(bool available) { isAvailable = available; }
    void setAmenities(const string& amt) { amenities = amt; }
};

class StandardRoom : public Room {
public:
    StandardRoom(int roomNo) : Room(roomNo, 80.0, RoomType::STANDARD, 2) {
        setAmenities("WiFi, TV, AC, Daily Breakfast, Housekeeping");
    }
};

class DeluxeRoom : public Room {
public:
    DeluxeRoom(int roomNo) : Room(roomNo, 120.0, RoomType::DELUXE, 3) {
        setAmenities("WiFi, TV, AC, Breakfast, Mini Bar, Balcony, Room Service");
    }
};

class Suite : public Room {
public:
    Suite(int roomNo) : Room(roomNo, 200.0, RoomType::SUITE, 4) {
        setAmenities("WiFi, TV, AC, Breakfast, Mini Bar, Jacuzzi, Living Room, Premium Service");
    }
};

class PresidentialSuite : public Room {
public:
    PresidentialSuite(int roomNo) : Room(roomNo, 500.0, RoomType::PRESIDENTIAL, 6) {
        setAmenities("WiFi, Multiple TVs, AC, Premium Breakfast, Full Bar, Private Jacuzzi, Dining Area, Butler Service");
    }
};

// Hotel Class
class Hotel {
private:
    static int nextHotelId;
    int hotelId;
    string name;
    string address;
    string city;
    string description;
    string hotelPhone;
    string hotelEmail;
    vector<shared_ptr<Room>> rooms;
    vector<shared_ptr<Package>> packages;
    vector<shared_ptr<Reservation>> reservations;
    shared_ptr<class HotelOwner> owner;

public:
    Hotel(const string& hotelName, const string& addr, const string& ct, 
          const string& desc, const string& phone, const string& email,
          shared_ptr<class HotelOwner> own);

    void initializePackages();
    void addRoom(shared_ptr<Room> room);
    void displayInfo() const;
    void displayDetailedInfo() const;
    void displayAvailableRooms() const;
    void displayAvailablePackages() const;
    int getAvailableRoomsCount() const;
    int getTotalRooms() const;
    shared_ptr<Room> findRoomByNumber(int roomNo) const;
    vector<shared_ptr<Room>> getAvailableRooms() const;
    shared_ptr<Package> findPackageByName(const string& packageName) const;
    void addReservation(const shared_ptr<Reservation>& reservation);
    void displayReservations() const;
    void displayStatistics() const;
    static string RoomTypeToString(RoomType type);

    int getHotelId() const { return hotelId; }
    string getName() const { return name; }
    string getAddress() const { return address + ", " + city; }
    string getCity() const { return city; }
    string getPhone() const { return hotelPhone; }
    string getEmail() const { return hotelEmail; }
    shared_ptr<class HotelOwner> getOwner() const { return owner; }
    const vector<shared_ptr<Package>>& getPackages() const { return packages; }
};

int Hotel::nextHotelId = 0;

// Hotel Owner Class
class HotelOwner : public User {
private:
    vector<shared_ptr<Hotel>> ownedHotels;

public:
    HotelOwner(const string& uname, const string& pwd, const string& mail, const string& phone)
        : User(uname, pwd, mail, phone, UserType::HOTEL_OWNER) {}

    void addHotel(const shared_ptr<Hotel>& hotel) {
        ownedHotels.push_back(hotel);
    }

    void displayOwnedHotels() const;
    void displayDashboard() override;
    vector<shared_ptr<Hotel>> getOwnedHotels() const { return ownedHotels; }
};

// Customer Class
class Customer : public User {
private:
    vector<shared_ptr<Reservation>> reservations;

public:
    Customer(const string& uname, const string& pwd, const string& mail, const string& phone)
        : User(uname, pwd, mail, phone, UserType::CUSTOMER) {}

    void addReservation(const shared_ptr<Reservation>& reservation) {
        reservations.push_back(reservation);
    }

    void viewBookingHistory() const;
    void displayDashboard() override;
    vector<shared_ptr<Reservation>> getReservations() const { return reservations; }
};

// Reservation Class
class Reservation : public enable_shared_from_this<Reservation> {
private:
    static int nextReservationId;
    int reservationId;
    weak_ptr<Customer> customer;
    weak_ptr<Hotel> hotel;
    weak_ptr<Room> room;
    weak_ptr<Package> package;
    string checkInDate;
    string checkOutDate;
    int numberOfNights;
    int numberOfGuests;
    double totalCost;
    ReservationStatus status;
    string paymentMethod;

public:
    Reservation(shared_ptr<Customer> cust, shared_ptr<Hotel> ht, 
                shared_ptr<Room> rm, shared_ptr<Package> pkg,
                const string& checkIn, const string& checkOut, 
                int nights, int guests, const string& payment);

    void displayReservationDetails() const;
    void displayBill() const;  // Bill display function
    void cancelReservation();

    int getReservationId() const { return reservationId; }
    double getTotalCost() const { return totalCost; }
    shared_ptr<Customer> getCustomer() const { return customer.lock(); }
    shared_ptr<Hotel> getHotel() const { return hotel.lock(); }
    ReservationStatus getStatus() const { return status; }
};

int Reservation::nextReservationId = 0;

// Hotel Method Implementations
Hotel::Hotel(const string& hotelName, const string& addr, const string& ct, 
      const string& desc, const string& phone, const string& email,
      shared_ptr<HotelOwner> own)
    : hotelId(++nextHotelId), name(hotelName), address(addr), city(ct), 
      description(desc), hotelPhone(phone), hotelEmail(email), owner(own) {
    initializePackages();
}

void Hotel::initializePackages() {
    auto basic = make_shared<Package>("Basic", "Room only", 1.0);
    packages.push_back(basic);

    auto breakfast = make_shared<Package>("Breakfast Included", "Room + Breakfast", 1.2);
    breakfast->addInclusion("Daily Breakfast");
    packages.push_back(breakfast);

    auto allInclusive = make_shared<Package>("All Inclusive", "Room + Meals + Services", 1.5);
    allInclusive->addInclusion("Breakfast");
    allInclusive->addInclusion("Lunch");
    allInclusive->addInclusion("Dinner");
    allInclusive->addInclusion("Spa Access");
    packages.push_back(allInclusive);
}

void Hotel::addRoom(shared_ptr<Room> room) {
    rooms.push_back(room);
}

void Hotel::displayInfo() const {
    cout << "\n🏨 " << name << " (ID: " << hotelId << ")\n";
    cout << "📍 " << address << ", " << city << "\n";
    cout << "📞 " << hotelPhone << " | ✉️  " << hotelEmail << "\n";
    cout << "📝 " << description << "\n";
    cout << "🛏️  Total Rooms: " << rooms.size() << " | Available: " << getAvailableRoomsCount() << "\n";
    cout << "👨‍💼 Owner: " << owner->getUsername() << "\n";
}

void Hotel::displayDetailedInfo() const {
    displayInfo();
    cout << "\n📊 ROOM BREAKDOWN:\n";
    cout << "=================\n";
    
    map<RoomType, int> totalRooms;
    map<RoomType, int> availableRooms;
    
    for (const auto& room : rooms) {
        totalRooms[room->getRoomType()]++;
        if (room->getAvailability()) {
            availableRooms[room->getRoomType()]++;
        }
    }
    
    for (const auto& pair : totalRooms) {
        cout << "   " << RoomTypeToString(pair.first) << ": " 
                  << availableRooms[pair.first] << "/" << pair.second << " available\n";
    }
    
    displayAvailablePackages();
}

void Hotel::displayAvailableRooms() const {
    cout << "\n🛏️ AVAILABLE ROOMS at " << name << ":\n";
    cout << "========================================\n";
    bool found = false;
    for (const auto& room : rooms) {
        if (room->getAvailability()) {
            room->displayDetails();
            cout << "   💰 Base Price for 1 night: $" << room->getBasePrice() << "\n";
            cout << "------------------------\n";
            found = true;
        }
    }
    if (!found) {
        cout << "❌ No available rooms found.\n";
    }
}

void Hotel::displayAvailablePackages() const {
    cout << "\n📦 AVAILABLE PACKAGES:\n";
    cout << "====================\n";
    for (const auto& package : packages) {
        package->displayPackage();
    }
}

int Hotel::getAvailableRoomsCount() const {
    int count = 0;
    for (const auto& room : rooms) {
        if (room->getAvailability()) {
            count++;
        }
    }
    return count;
}

int Hotel::getTotalRooms() const {
    return rooms.size();
}

shared_ptr<Room> Hotel::findRoomByNumber(int roomNo) const {
    for (const auto& room : rooms) {
        if (room->getRoomNumber() == roomNo) {
            return room;
        }
    }
    return nullptr;
}

vector<shared_ptr<Room>> Hotel::getAvailableRooms() const {
    vector<shared_ptr<Room>> available;
    for (const auto& room : rooms) {
        if (room->getAvailability()) {
            available.push_back(room);
        }
    }
    return available;
}

shared_ptr<Package> Hotel::findPackageByName(const string& packageName) const {
    for (const auto& package : packages) {
        if (package->getName() == packageName) {
            return package;
        }
    }
    return nullptr;
}

void Hotel::addReservation(const shared_ptr<Reservation>& reservation) {
    reservations.push_back(reservation);
}

void Hotel::displayReservations() const {
    cout << "\n📊 RESERVATIONS for " << name << ":\n";
    cout << "========================================\n";
    if (reservations.empty()) {
        cout << "No reservations found.\n";
        return;
    }
    for (const auto& reservation : reservations) {
        reservation->displayReservationDetails();
    }
}

void Hotel::displayStatistics() const {
    cout << "\n📈 HOTEL STATISTICS - " << name << "\n";
    cout << "==============================\n";
    cout << "🛏️  Total Rooms: " << rooms.size() << "\n";
    cout << "✅ Available Rooms: " << getAvailableRoomsCount() << "\n";
    cout << "📅 Total Reservations: " << reservations.size() << "\n";
    
    double totalRevenue = 0;
    for (const auto& reservation : reservations) {
        totalRevenue += reservation->getTotalCost();
    }
    cout << "💰 Total Revenue: $" << totalRevenue << "\n";
    
    map<RoomType, int> roomCount;
    for (const auto& room : rooms) {
        roomCount[room->getRoomType()]++;
    }
    
    cout << "\n🏠 ROOM TYPE DISTRIBUTION:\n";
    for (const auto& pair : roomCount) {
        cout << "   " << RoomTypeToString(pair.first) << ": " << pair.second << " rooms\n";
    }
}

string Hotel::RoomTypeToString(RoomType type) {
    switch(type) {
        case RoomType::STANDARD: return "Standard";
        case RoomType::DELUXE: return "Deluxe";
        case RoomType::SUITE: return "Suite";
        case RoomType::PRESIDENTIAL: return "Presidential";
        default: return "Unknown";
    }
}

// HotelOwner Method Implementations
void HotelOwner::displayOwnedHotels() const {
    cout << "\n🏨 YOUR HOTELS (" << ownedHotels.size() << "):\n";
    cout << "========================================\n";
    for (size_t i = 0; i < ownedHotels.size(); ++i) {
        cout << i + 1 << ". " << ownedHotels[i]->getName() 
                  << " - " << ownedHotels[i]->getCity() << "\n";
        cout << "   📞 " << ownedHotels[i]->getPhone() 
                  << " | ✉️  " << ownedHotels[i]->getEmail() << "\n";
        cout << "   🛏️  " << ownedHotels[i]->getAvailableRoomsCount() 
                  << "/" << ownedHotels[i]->getTotalRooms() << " rooms available\n";
    }
}

void HotelOwner::displayDashboard() {
    cout << "\n🎯 HOTEL OWNER DASHBOARD\n";
    cout << "========================\n";
    cout << "👋 Welcome, " << username << "!\n";
    cout << "📅 Registered: " << registrationDate << "\n";
    cout << "📞 Phone: " << phoneNumber << "\n";
    cout << "✉️  Email: " << email << "\n";
    cout << "🏨 Owned Hotels: " << ownedHotels.size() << "\n\n";

    displayOwnedHotels();

    cout << "\n🛠️  AVAILABLE ACTIONS:\n";
    cout << "1. Add New Hotel\n";
    cout << "2. View Hotel Details\n";
    cout << "3. View Hotel Statistics\n";
    cout << "4. View All Reservations\n";
    cout << "5. Logout\n";
}

// Customer Method Implementations
void Customer::viewBookingHistory() const {
    cout << "\n📅 YOUR BOOKING HISTORY:\n";
    cout << "========================\n";
    if (reservations.empty()) {
        cout << "No reservations found.\n";
        return;
    }
    for (const auto& reservation : reservations) {
        reservation->displayReservationDetails();
    }
}

void Customer::displayDashboard() {
    cout << "\n🎯 CUSTOMER DASHBOARD\n";
    cout << "====================\n";
    cout << "👋 Welcome, " << username << "!\n";
    cout << "📅 Registered: " << registrationDate << "\n";
    cout << "📞 Phone: " << phoneNumber << "\n";
    cout << "✉️  Email: " << email << "\n";
    cout << "📋 Total Bookings: " << reservations.size() << "\n\n";

    cout << "🛠️  AVAILABLE ACTIONS:\n";
    cout << "1. Search Hotels\n";
    cout << "2. View All Hotels\n";
    cout << "3. View Booking History\n";
    cout << "4. Make Reservation\n";
    cout << "5. Logout\n";
}

// Reservation Method Implementations
Reservation::Reservation(shared_ptr<Customer> cust, shared_ptr<Hotel> ht, 
            shared_ptr<Room> rm, shared_ptr<Package> pkg,
            const string& checkIn, const string& checkOut, 
            int nights, int guests, const string& payment)
    : reservationId(++nextReservationId), customer(cust), hotel(ht), room(rm),
      package(pkg), checkInDate(checkIn), checkOutDate(checkOut), 
      numberOfNights(nights), numberOfGuests(guests), paymentMethod(payment),
      status(ReservationStatus::CONFIRMED) {
    
    totalCost = rm->calculateTotalCost(nights, pkg.get());
    rm->setAvailability(false);
}

void Reservation::displayReservationDetails() const {
    auto cust = customer.lock();
    auto ht = hotel.lock();
    auto rm = room.lock();
    auto pkg = package.lock();
    
    if (!cust || !ht || !rm) {
        cout << "❌ Reservation data is incomplete!\n";
        return;
    }
    
    cout << "\n📋 RESERVATION #" << reservationId << "\n";
    cout << "================\n";
    cout << "🏨 Hotel: " << ht->getName() << "\n";
    cout << "🛏️  Room: " << rm->getRoomNumber() << " (" << rm->getRoomTypeString() << ")\n";
    cout << "👤 Customer: " << cust->getUsername() << "\n";
    if (pkg) {
        cout << "📦 Package: " << pkg->getName() << "\n";
    }
    cout << "📅 Dates: " << checkInDate << " to " << checkOutDate << "\n";
    cout << "⏰ Nights: " << numberOfNights << " | 👥 Guests: " << numberOfGuests << "\n";
    cout << "💰 Total Cost: $" << totalCost << "\n";
    cout << "💳 Payment Method: " << paymentMethod << "\n";
    cout << "✅ Status: Confirmed\n";
}

// Bill display function
void Reservation::displayBill() const {
    auto cust = customer.lock();
    auto ht = hotel.lock();
    auto rm = room.lock();
    auto pkg = package.lock();
    
    if (!cust || !ht || !rm) {
        cout << "❌ Cannot generate bill - reservation data is incomplete!\n";
        return;
    }
    
    cout << "\n🧾 ============ HOTEL BILL ============\n";
    cout << "🔖 Reservation ID: #" << reservationId << "\n";
    
    // Get current date and time
    time_t now = time(0);
    string currentTime = ctime(&now);
    currentTime.pop_back();
    cout << "📅 Generated On: " << currentTime << "\n";
    
    cout << "----------------------------------------\n";
    cout << "🏨 Hotel: " << ht->getName() << "\n";
    cout << "📍 Address: " << ht->getAddress() << "\n";
    cout << "📞 Hotel Phone: " << ht->getPhone() << "\n";
    cout << "----------------------------------------\n";
    cout << "👤 Guest Name: " << cust->getUsername() << "\n";
    cout << "📧 Guest Email: " << cust->getEmail() << "\n";
    cout << "📱 Guest Phone: " << cust->getPhone() << "\n";
    cout << "----------------------------------------\n";
    cout << "🛏️ Room Details:\n";
    cout << "   • Room No: " << rm->getRoomNumber() << "\n";
    cout << "   • Room Type: " << rm->getRoomTypeString() << "\n";
    cout << "   • Max Occupancy: " << rm->getMaxOccupancy() << " persons\n";
    cout << "----------------------------------------\n";
    cout << "📅 Stay Details:\n";
    cout << "   • Check-in: " << checkInDate << "\n";
    cout << "   • Check-out: " << checkOutDate << "\n";
    cout << "   • Nights: " << numberOfNights << "\n";
    cout << "   • Guests: " << numberOfGuests << "\n";
    cout << "----------------------------------------\n";
    
    // Bill breakdown
    cout << "💰 BILL BREAKDOWN:\n";
    double baseCost = rm->getBasePrice() * numberOfNights;
    cout << "   • Room (" << rm->getRoomTypeString() << "): $" 
         << rm->getBasePrice() << " x " << numberOfNights << " nights = $" << baseCost << "\n";
    
    if (pkg) {
        double packageCost = baseCost * (pkg->getMultiplier() - 1.0);
        cout << "   • " << pkg->getName() << " Package: +$" << packageCost << "\n";
        cout << "   • Package Multiplier: x" << pkg->getMultiplier() << "\n";
    }
    
    cout << "----------------------------------------\n";
    cout << "💵 SUBTOTAL: $" << baseCost << "\n";
    if (pkg) {
        cout << "📦 PACKAGE CHARGE: $" << (baseCost * (pkg->getMultiplier() - 1.0)) << "\n";
    }
    cout << "💳 PAYMENT METHOD: " << paymentMethod << "\n";
    cout << "💲 TOTAL AMOUNT: $" << totalCost << "\n";
    cout << "----------------------------------------\n";
    cout << "📝 Terms & Conditions:\n";
    cout << "   • Cancellation allowed up to 24 hours before check-in\n";
    cout << "   • Early check-in subject to availability\n";
    cout << "   • Government taxes and service charges included\n";
    cout << "🧾 ============ END OF BILL ============\n\n";
    
    cout << "✅ Thank you for your reservation! Your booking is confirmed.\n";
    cout << "📞 For any queries, contact: " << ht->getPhone() << "\n";
    cout << "✉️  Email: " << ht->getEmail() << "\n";
}

void Reservation::cancelReservation() {
    auto rm = room.lock();
    if (rm) {
        rm->setAvailability(true);
    }
    status = ReservationStatus::CANCELLED;
    cout << "✅ Reservation #" << reservationId << " cancelled successfully!\n";
}

// Authentication System
class AuthSystem {
private:
    vector<shared_ptr<User>> users;
    vector<shared_ptr<Hotel>> hotels;
    shared_ptr<User> currentUser;
    SystemStats* stats;

public:
    AuthSystem() : stats(SystemStats::getInstance()) {
        initializeSampleData();
    }

    void initializeSampleData() {
        auto owner1 = make_shared<HotelOwner>("owner1", "pass123", "owner1@email.com", "111-111-1111");
        auto owner2 = make_shared<HotelOwner>("owner2", "pass123", "owner2@email.com", "222-222-2222");
        
        auto customer1 = make_shared<Customer>("customer1", "pass123", "customer1@email.com", "333-333-3333");
        auto customer2 = make_shared<Customer>("customer2", "pass123", "customer2@email.com", "444-444-4444");
        
        users.push_back(owner1);
        users.push_back(owner2);
        users.push_back(customer1);
        users.push_back(customer2);

        auto hotel1 = make_shared<Hotel>("Grand Plaza Hotel", "123 Main St", "New York", 
                                         "Luxury 5-star hotel in downtown Manhattan with premium amenities",
                                         "555-100-1001", "info@grandplaza.com", owner1);
        
        auto hotel2 = make_shared<Hotel>("Ocean View Resort", "456 Beach Rd", "Miami", 
                                         "Beachfront resort with amazing ocean views and spa services",
                                         "555-200-2002", "reservations@oceanview.com", owner2);

        for (int i = 101; i <= 110; i++) hotel1->addRoom(make_shared<StandardRoom>(i));
        for (int i = 201; i <= 205; i++) hotel1->addRoom(make_shared<DeluxeRoom>(i));
        for (int i = 301; i <= 303; i++) hotel1->addRoom(make_shared<Suite>(i));
        hotel1->addRoom(make_shared<PresidentialSuite>(401));

        for (int i = 102; i <= 112; i++) hotel2->addRoom(make_shared<StandardRoom>(i));
        for (int i = 202; i <= 207; i++) hotel2->addRoom(make_shared<DeluxeRoom>(i));
        for (int i = 302; i <= 304; i++) hotel2->addRoom(make_shared<Suite>(i));
        hotel2->addRoom(make_shared<PresidentialSuite>(402));

        owner1->addHotel(hotel1);
        owner2->addHotel(hotel2);

        hotels.push_back(hotel1);
        hotels.push_back(hotel2);
    }

    // Check if username already exists
    bool usernameExists(const string& username) const {
        for (const auto& user : users) {
            if (user->getUsername() == username) {
                return true;
            }
        }
        return false;
    }

    bool login() {
        string username, password;
        
        cout << "\n🔐 LOGIN PORTAL\n";
        cout << "===============\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        for (const auto& user : users) {
            if (user->authenticate(username, password)) {
                currentUser = user;
                stats->recordLogin(user->getUserType(), username);
                cout << "\n✅ Login successful! Welcome, " << username << "!\n";
                return true;
            }
        }

        cout << "\n❌ Invalid username or password!\n";
        return false;
    }

    void registerUser() {
        string username, password, email, phone;
        int userType;

        cout << "\n📝 REGISTRATION PORTAL\n";
        cout << "=====================\n";
        
        // Get account type
        cout << "Choose account type:\n";
        cout << "1. Hotel Owner\n";
        cout << "2. Customer\n";
        cout << "Enter choice (1-2): ";
        cin >> userType;

        // Get username with validation
        while (true) {
            cout << "Username: ";
            cin >> username;
            
            if (usernameExists(username)) {
                cout << "❌ Username '" << username << "' already exists! Please choose a different username.\n";
            } else {
                break;
            }
        }

        cout << "Password: ";
        cin >> password;
        cout << "Email: ";
        cin >> email;
        cout << "Phone: ";
        cin >> phone;

        shared_ptr<User> newUser;
        if (userType == 1) {
            newUser = make_shared<HotelOwner>(username, password, email, phone);
        } else {
            newUser = make_shared<Customer>(username, password, email, phone);
        }

        users.push_back(newUser);
        cout << "\n✅ Registration successful! You can now login.\n";
    }

    void showMainMenu() {
        int choice;
        do {
            cout << "\n🏨 HOTEL RESERVATION SYSTEM\n";
            cout << "==========================\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. View System Statistics\n";
            cout << "4. Exit\n";
            cout << "Enter your choice (1-4): ";
            cin >> choice;

            switch (choice) {
                case 1:
                    if (login()) {
                        showUserDashboard();
                    }
                    break;
                case 2:
                    registerUser();
                    break;
                case 3:
                    stats->displayStats();
                    break;
                case 4:
                    cout << "👋 Thank you for using our system!\n";
                    break;
                default:
                    cout << "❌ Invalid choice! Please try again.\n";
            }
        } while (choice != 4);
    }

    void showUserDashboard() {
        if (!currentUser) return;

        int choice;
        do {
            currentUser->displayDashboard();
            cout << "Enter your choice: ";
            cin >> choice;

            if (currentUser->getUserType() == UserType::HOTEL_OWNER) {
                handleHotelOwnerActions(choice);
            } else {
                handleCustomerActions(choice);
            }
        } while (choice != 5);
    }

    void handleHotelOwnerActions(int choice) {
        auto owner = dynamic_pointer_cast<HotelOwner>(currentUser);
        
        switch (choice) {
            case 1:
                addNewHotel(owner);
                break;
            case 2:
                owner->displayOwnedHotels();
                break;
            case 3:
                if (!owner->getOwnedHotels().empty()) {
                    owner->getOwnedHotels()[0]->displayStatistics();
                } else {
                    cout << "❌ You don't have any hotels yet!\n";
                }
                break;
            case 4:
                if (!owner->getOwnedHotels().empty()) {
                    owner->getOwnedHotels()[0]->displayReservations();
                } else {
                    cout << "❌ You don't have any hotels yet!\n";
                }
                break;
            case 5:
                cout << "👋 Logging out...\n";
                currentUser = nullptr;
                break;
            default:
                cout << "❌ Invalid choice!\n";
        }
    }

    void handleCustomerActions(int choice) {
        auto customer = dynamic_pointer_cast<Customer>(currentUser);
        
        switch (choice) {
            case 1:
                searchHotels();
                break;
            case 2:
                displayAllHotels();
                break;
            case 3:
                customer->viewBookingHistory();
                break;
            case 4:
                makeReservation(customer);
                break;
            case 5:
                cout << "👋 Logging out...\n";
                currentUser = nullptr;
                break;
            default:
                cout << "❌ Invalid choice!\n";
        }
    }

    void addNewHotel(shared_ptr<HotelOwner> owner) {
        string name, address, city, description, phone, email;
        
        cout << "\n🏨 ADD NEW HOTEL\n";
        cout << "================\n";
        cin.ignore();
        cout << "Hotel Name: ";
        getline(cin, name);
        cout << "Address: ";
        getline(cin, address);
        cout << "City: ";
        getline(cin, city);
        cout << "Description: ";
        getline(cin, description);
        cout << "Phone: ";
        getline(cin, phone);
        cout << "Email: ";
        getline(cin, email);

        auto newHotel = make_shared<Hotel>(name, address, city, description, phone, email, owner);
        
        newHotel->addRoom(make_shared<StandardRoom>(101));
        newHotel->addRoom(make_shared<StandardRoom>(102));
        newHotel->addRoom(make_shared<DeluxeRoom>(201));
        newHotel->addRoom(make_shared<Suite>(301));

        owner->addHotel(newHotel);
        hotels.push_back(newHotel);
        
        cout << "✅ Hotel '" << name << "' added successfully with 4 sample rooms!\n";
    }

    void searchHotels() {
        string city;
        cout << "\n🔍 SEARCH HOTELS\n";
        cout << "================\n";
        cout << "Enter city to search: ";
        cin >> city;

        cout << "\n🏨 HOTELS in " << city << ":\n";
        cout << "========================================\n";
        
        bool found = false;
        for (const auto& hotel : hotels) {
            if (hotel->getCity() == city) {
                hotel->displayDetailedInfo();
                cout << "\n";
                found = true;
            }
        }

        if (!found) {
            cout << "❌ No hotels found in " << city << ".\n";
        }
    }

    void displayAllHotels() {
        cout << "\n🏨 ALL AVAILABLE HOTELS:\n";
        cout << "========================\n";
        for (const auto& hotel : hotels) {
            hotel->displayInfo();
            cout << "------------------------\n";
        }
    }

    void makeReservation(shared_ptr<Customer> customer) {
        int hotelId, roomNumber, nights, guests;
        string checkIn, checkOut, packageChoice, paymentMethod;

        cout << "\n📅 MAKE RESERVATION\n";
        cout << "===================\n";
        
        displayAllHotels();

        cout << "Select hotel (1-" << hotels.size() << "): ";
        cin >> hotelId;

        if (hotelId < 1 || hotelId > static_cast<int>(hotels.size())) {
            cout << "❌ Invalid hotel selection!\n";
            return;
        }

        auto selectedHotel = hotels[hotelId - 1];
        selectedHotel->displayAvailableRooms();

        cout << "Enter room number: ";
        cin >> roomNumber;

        auto selectedRoom = selectedHotel->findRoomByNumber(roomNumber);
        if (!selectedRoom || !selectedRoom->getAvailability()) {
            cout << "❌ Room not available or invalid room number!\n";
            return;
        }

        cout << "Enter number of guests: ";
        cin >> guests;
        if (guests > selectedRoom->getMaxOccupancy()) {
            cout << "❌ This room can only accommodate " << selectedRoom->getMaxOccupancy() << " guests!\n";
            return;
        }

        selectedHotel->displayAvailablePackages();
        cout << "Choose package (Basic/Breakfast Included/All Inclusive): ";
        cin >> packageChoice;
        
        shared_ptr<Package> selectedPackage = selectedHotel->findPackageByName(packageChoice);
        if (!selectedPackage) {
            cout << "❌ Invalid package selection! Using Basic package.\n";
            selectedPackage = selectedHotel->findPackageByName("Basic");
        }

        cout << "Check-in date (YYYY-MM-DD): ";
        cin >> checkIn;
        cout << "Check-out date (YYYY-MM-DD): ";
        cin >> checkOut;
        cout << "Number of nights: ";
        cin >> nights;

        cout << "Payment method (Credit Card/Debit Card/Cash/UPI): ";
        cin >> paymentMethod;

        double totalCost = selectedRoom->calculateTotalCost(nights, selectedPackage.get());
        cout << "\n💰 PAYMENT SUMMARY:\n";
        cout << "=================\n";
        cout << "Room: $" << selectedRoom->getBasePrice() << " x " << nights << " nights = $" << (selectedRoom->getBasePrice() * nights) << "\n";
        if (selectedPackage) {
            cout << "Package: " << selectedPackage->getName() << " (x" << selectedPackage->getMultiplier() << ")\n";
        }
        cout << "Total Cost: $" << totalCost << "\n";

        char confirm;
        cout << "Confirm booking? (y/n): ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            auto reservation = make_shared<Reservation>(customer, selectedHotel, selectedRoom, 
                                                       selectedPackage, checkIn, checkOut, 
                                                       nights, guests, paymentMethod);
            
            // Add reservation to hotel and customer
            selectedHotel->addReservation(reservation);
            customer->addReservation(reservation);
            
            cout << "\n✅ RESERVATION CREATED SUCCESSFULLY!\n";
            cout << "\n🎫 Here is your booking confirmation and bill:\n";
            reservation->displayReservationDetails();
            reservation->displayBill();  // Display bill after reservation
        } else {
            cout << "❌ Reservation cancelled.\n";
        }
    }
};

// Main function
int main() {
    AuthSystem authSystem;
    authSystem.showMainMenu();
    return 0;
}
/**
 * ============================================================
 *  Smart Parking Management System
 *  Author  : <Your Name>
 *  Version : 1.0
 *  Language: C++17
 * ============================================================
 *
 *  Features:
 *   - View all parking slots with live availability
 *   - Book a slot by ID with vehicle & duration details
 *   - Release / cancel a booking and free the slot
 *   - Make payment for a booking
 *   - View booking details
 *   - Input-validated, menu-driven console interface
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

// ─────────────────────────────────────────────────────────────
//  Utility helpers
// ─────────────────────────────────────────────────────────────

/**
 * @brief Clears any leftover characters from std::cin and prints
 *        an error message, then waits for the user to press Enter.
 */
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/** @brief Prints a styled section divider. */
void printDivider(char ch = '=', int width = 50) {
    std::cout << std::string(width, ch) << "\n";
}

/** @brief Pauses execution until the user presses Enter. */
void pause() {
    std::cout << "\nPress Enter to continue...";
    clearInputBuffer();
    std::cin.get();
}

// ─────────────────────────────────────────────────────────────
//  CLASS: ParkingSlot
// ─────────────────────────────────────────────────────────────

/**
 * @brief Represents a single physical parking slot in the facility.
 */
class ParkingSlot {
public:
    std::string slotID;       ///< Unique identifier, e.g. "A1"
    std::string location;     ///< Human-readable location label
    bool        isAvailable;  ///< true = free, false = occupied
    int         pricePerHour; ///< Hourly rate in Rs.

    /**
     * @param id    Slot identifier
     * @param loc   Location description
     * @param price Hourly parking price (Rs.)
     */
    ParkingSlot(const std::string& id,
                const std::string& loc,
                int price)
        : slotID(id), location(loc),
          pricePerHour(price), isAvailable(true) {}

    /** @brief Prints a single formatted row for the slot table. */
    void display() const {
        std::cout << std::left
                  << std::setw(8)  << slotID
                  << std::setw(12) << location
                  << std::setw(12) << (isAvailable ? "Available" : "Booked")
                  << "Rs." << pricePerHour << "/hr\n";
    }
};

// ─────────────────────────────────────────────────────────────
//  CLASS: Booking
// ─────────────────────────────────────────────────────────────

/**
 * @brief Holds all data related to a single parking reservation.
 */
class Booking {
public:
    int         bookingID;   ///< Auto-generated unique booking number
    std::string slotID;      ///< The slot that was reserved
    std::string vehicleNo;   ///< Vehicle registration / licence plate
    int         hours;       ///< Booked duration in hours
    int         totalAmount; ///< Pre-calculated cost (hours × price)
    bool        isPaid;      ///< Payment status
    bool        isActive;    ///< false once the booking is cancelled/released

    /**
     * @param id        Booking identifier
     * @param slot      Slot ID being reserved
     * @param vehicle   Vehicle number
     * @param h         Duration in hours
     * @param pricePerHr Slot's hourly rate
     */
    Booking(int id,
            const std::string& slot,
            const std::string& vehicle,
            int h,
            int pricePerHr)
        : bookingID(id), slotID(slot), vehicleNo(vehicle),
          hours(h), totalAmount(h * pricePerHr),
          isPaid(false), isActive(true) {}

    /** @brief Prints detailed information about this booking. */
    void display() const {
        printDivider('-', 40);
        std::cout << "  Booking ID  : " << bookingID               << "\n"
                  << "  Slot        : " << slotID                   << "\n"
                  << "  Vehicle No  : " << vehicleNo                << "\n"
                  << "  Duration    : " << hours << " hr(s)"        << "\n"
                  << "  Amount      : Rs." << totalAmount            << "\n"
                  << "  Payment     : " << (isPaid ? "Paid" : "Pending") << "\n"
                  << "  Status      : " << (isActive ? "Active" : "Released") << "\n";
        printDivider('-', 40);
    }
};

// ─────────────────────────────────────────────────────────────
//  CLASS: ParkingSystem  (core logic)
// ─────────────────────────────────────────────────────────────

/**
 * @brief Encapsulates all parking-management operations.
 *
 * Keeps slots and bookings fully self-contained so that global
 * state is avoided and the class is easy to unit-test or extend.
 */
class ParkingSystem {
private:
    std::vector<ParkingSlot> slots_;
    std::vector<Booking>     bookings_;
    int                      bookingCounter_;

    // ── helpers ──────────────────────────────────────────────

    /** @brief Returns a pointer to the slot with the given ID, or nullptr. */
    ParkingSlot* findSlot(const std::string& id) {
        for (auto& s : slots_)
            if (s.slotID == id) return &s;
        return nullptr;
    }

    /** @brief Returns a pointer to the booking with the given ID, or nullptr. */
    Booking* findBooking(int id) {
        for (auto& b : bookings_)
            if (b.bookingID == id) return &b;
        return nullptr;
    }

public:
    // ── constructor ──────────────────────────────────────────

    ParkingSystem() : bookingCounter_(100) {
        // Seed the system with initial slots.
        // In a real application this would be loaded from a file/DB.
        slots_.emplace_back("A1", "Basement",  40);
        slots_.emplace_back("A2", "Basement",  40);
        slots_.emplace_back("B1", "Ground",    30);
        slots_.emplace_back("B2", "Ground",    30);
        slots_.emplace_back("C1", "Rooftop",   20);
        slots_.emplace_back("C2", "Rooftop",   20);
    }

    // ── public operations ─────────────────────────────────────

    /**
     * @brief Lists every slot with its current availability.
     */
    void viewSlots() const {
        std::cout << "\n";
        printDivider();
        std::cout << "             PARKING SLOTS\n";
        printDivider();
        std::cout << std::left
                  << std::setw(8)  << "SlotID"
                  << std::setw(12) << "Location"
                  << std::setw(12) << "Status"
                  << "Price\n";
        printDivider('-');
        for (const auto& s : slots_)
            s.display();
        printDivider();
    }

    /**
     * @brief Interactively books an available slot.
     *
     * Validates that the slot exists and is free before creating
     * the booking record.
     */
    void bookSlot() {
        std::string slotID, vehicleNo;
        int hours = 0;

        std::cout << "\nEnter Slot ID: ";
        std::cin >> slotID;

        // Normalise to upper-case for case-insensitive matching
        std::transform(slotID.begin(), slotID.end(), slotID.begin(), ::toupper);

        ParkingSlot* slot = findSlot(slotID);
        if (!slot) {
            std::cout << "[ERROR] Slot \"" << slotID << "\" does not exist.\n";
            return;
        }
        if (!slot->isAvailable) {
            std::cout << "[ERROR] Slot " << slotID << " is already booked.\n";
            return;
        }

        std::cout << "Enter Vehicle Number: ";
        std::cin >> vehicleNo;

        // Validate duration
        while (true) {
            std::cout << "Enter Duration (hours, 1-72): ";
            if (std::cin >> hours && hours >= 1 && hours <= 72)
                break;
            std::cout << "[ERROR] Please enter a whole number between 1 and 72.\n";
            clearInputBuffer();
        }

        slot->isAvailable = false;
        bookings_.emplace_back(++bookingCounter_, slotID, vehicleNo,
                                hours, slot->pricePerHour);

        std::cout << "\n[SUCCESS] Booking confirmed!\n";
        std::cout << "  Booking ID : " << bookingCounter_          << "\n"
                  << "  Slot       : " << slotID                    << "\n"
                  << "  Amount Due : Rs." << hours * slot->pricePerHour << "\n";
    }

    /**
     * @brief Processes payment for a pending booking.
     */
    void makePayment() {
        int id = 0;
        std::cout << "\nEnter Booking ID: ";
        if (!(std::cin >> id)) {
            clearInputBuffer();
            std::cout << "[ERROR] Invalid input.\n";
            return;
        }

        Booking* b = findBooking(id);
        if (!b) {
            std::cout << "[ERROR] Booking ID " << id << " not found.\n";
            return;
        }
        if (!b->isActive) {
            std::cout << "[ERROR] This booking has already been released.\n";
            return;
        }
        if (b->isPaid) {
            std::cout << "[INFO] Booking " << id << " is already paid.\n";
            return;
        }

        std::cout << "  Amount Due : Rs." << b->totalAmount << "\n";
        std::cout << "  Processing payment...\n";
        b->isPaid = true;
        std::cout << "[SUCCESS] Payment of Rs." << b->totalAmount
                  << " received for Booking " << id << ".\n";
    }

    /**
     * @brief Displays detailed information for a specific booking.
     */
    void viewBooking() const {
        int id = 0;
        std::cout << "\nEnter Booking ID: ";
        if (!(std::cin >> id)) {
            const_cast<ParkingSystem*>(this); // just to keep const signature valid
            clearInputBuffer();
            std::cout << "[ERROR] Invalid input.\n";
            return;
        }

        for (const auto& b : bookings_) {
            if (b.bookingID == id) {
                b.display();
                return;
            }
        }
        std::cout << "[ERROR] Booking ID " << id << " not found.\n";
    }

    /**
     * @brief Lists all bookings (active and released).
     */
    void viewAllBookings() const {
        if (bookings_.empty()) {
            std::cout << "\n[INFO] No bookings have been made yet.\n";
            return;
        }
        std::cout << "\n";
        printDivider();
        std::cout << "           ALL BOOKINGS\n";
        printDivider();
        for (const auto& b : bookings_)
            b.display();
    }

    /**
     * @brief Releases (cancels) a booking and frees the slot.
     *
     * A paid booking cannot be cancelled to prevent revenue loss.
     * In a production system you would implement refund logic here.
     */
    void releaseSlot() {
        int id = 0;
        std::cout << "\nEnter Booking ID to release: ";
        if (!(std::cin >> id)) {
            clearInputBuffer();
            std::cout << "[ERROR] Invalid input.\n";
            return;
        }

        Booking* b = findBooking(id);
        if (!b) {
            std::cout << "[ERROR] Booking ID " << id << " not found.\n";
            return;
        }
        if (!b->isActive) {
            std::cout << "[INFO] Booking " << id << " is already released.\n";
            return;
        }
        if (b->isPaid) {
            std::cout << "[ERROR] Cannot cancel a paid booking. "
                         "Please contact support for a refund.\n";
            return;
        }

        ParkingSlot* slot = findSlot(b->slotID);
        if (slot) slot->isAvailable = true;

        b->isActive = false;
        std::cout << "[SUCCESS] Booking " << id << " released. "
                  << "Slot " << b->slotID << " is now available.\n";
    }
};

// ─────────────────────────────────────────────────────────────
//  Menu helpers
// ─────────────────────────────────────────────────────────────

void printMenu() {
    std::cout << "\n";
    printDivider();
    std::cout << "       SMART PARKING MANAGEMENT SYSTEM\n";
    printDivider();
    std::cout << "  1. View Parking Slots\n"
              << "  2. Book a Slot\n"
              << "  3. Make Payment\n"
              << "  4. View a Booking\n"
              << "  5. View All Bookings\n"
              << "  6. Release / Cancel Booking\n"
              << "  0. Exit\n";
    printDivider();
    std::cout << "  Enter choice: ";
}

// ─────────────────────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────────────────────

int main() {
    ParkingSystem system;
    int choice = -1;

    while (true) {
        printMenu();

        if (!(std::cin >> choice)) {
            // Non-integer input
            clearInputBuffer();
            std::cout << "[ERROR] Please enter a number from the menu.\n";
            continue;
        }

        switch (choice) {
            case 1: system.viewSlots();       break;
            case 2: system.bookSlot();        break;
            case 3: system.makePayment();     break;
            case 4: system.viewBooking();     break;
            case 5: system.viewAllBookings(); break;
            case 6: system.releaseSlot();     break;
            case 0:
                std::cout << "\nThank you for using Smart Parking System. Goodbye!\n\n";
                return 0;
            default:
                std::cout << "[ERROR] Invalid choice. Please enter 0-6.\n";
        }

        pause();
    }
}

Smart Parking Management System
A console-based C++17 application for managing a small parking facility — book slots, process payments, view and cancel reservations, all from a clean, screen-clearing menu-driven interface.

✨ Features
Feature	Details
View Slots	Live availability table with location & price
Book a Slot	Case-insensitive slot ID, vehicle number, duration validation
Make Payment	Marks booking as paid, shows amount due
View Booking	Detailed receipt for any booking ID
View All Bookings	Full history including released bookings
Release / Cancel	Frees the slot; prevents cancellation of paid bookings
Clean Terminal UI	Screen clears between every menu, welcome banner on launch
Input Validation	Handles non-integer input and out-of-range values gracefully
🏗️ Project Structure
smart_parking/
├── parking.cpp   ← single-file implementation (C++17)
└── README.md
🔧 Build & Run
Prerequisites
A C++17-compatible compiler — g++ 7+, clang++ 5+, or MSVC 2017+
Windows (MinGW / MSYS2)
bash
g++ -std=c++17 -Wall -Wextra -o parking.exe parking.cpp
parking.exe
Windows (MSVC — Developer Command Prompt)
bash
cl /std:c++17 /W4 /EHsc parking.cpp /Fe:parking.exe
parking.exe


# Fully Automatic Car Wash Machine - Microcontroller Project

📄 [Download Project Report (PDF)](https://github.com/YEK-Kayra/Microchip_PIC16F887_FinalProject/blob/main/Project%20Report.pdf)

## 📌 Project Overview
This project implements a fully automatic car wash system using PIC microcontrollers (PIC16F887 and PIC16F877A). The system provides a user-friendly interface for customized car washing services including foaming, washing, drying, and polishing operations with secure password authentication and data persistence.

## 🔑 Key Features

#### 🧠 Dual-Microcontroller Architecture:
-> Master (PIC16F887): Controls washing processes and user interface    
-> Slave (PIC16F877A): Handles password authentication via keypad   

#### 🧼 Customizable Wash Programs:
-> Adjustable timing and intensity for each wash phase  
-> Polishing volume selection (100-500ml)   

#### 🖥️ User Interface:
-> 16x2 LCD display for menu navigation         
-> 3x 7-segment displays for countdown timers      
-> Potentiometers for parameter adjustment  
-> Tactile buttons for control  

#### 💾 Data Persistence:
-> Saves user preferences and wash history in EEPROM    

#### 🔐 Secure Authentication:
-> 4-digit password system with two pre-configured user accounts    


## 🧠 Software Implementation

#### 🕹️ Master Controller

- Manages all wash processes  
- Handles user interface and input  
- Stores/retrieves data from EEPROM  
- Communicates with slave controller

#### 🔐 Slave Controller

- Handles password input and verification  
- Implements keypad scanning algorithm  
- Communicates authentication status



## ⚙️ Installation & Usage

1. Connect all hardware components as per schematic (in real life or proteus sim) 
2. Flash the respective firmware to each microcontroller  
3. Power on the system  
4. Use default passwords (`1234` or `4321`) for authentication  
5. Navigate menus using potentiometers and buttons




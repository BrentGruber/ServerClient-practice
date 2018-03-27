#!/bin/sh
./client ./myserverfifo steve add 031516 0900 1100 Test1 
./client ./myserverfifo steve add 031516 1300 1400 Meeting1 
./client ./myserverfifo steve add 031516 1600 1830 Class 
./client ./myserverfifo tim add 031516 1500 1630 Class 
./client ./myserverfifo tim add 031616 1500 1530 Seminar 
./client ./myserverfifo steve add 031616 0800 0930 Dentist 
./client ./myserverfifo tim add 031516 14.30 15.30 Gym 
./client ./myserverfifo tim remove 031516 1500
./client ./myserverfifo martin add 031716 0900 1130 Shopping
./client ./myserverfifo steve update 031616 0800 1000 Dentist 
./client ./myserverfifo tim add 031716 0900 1200 Squash
./client ./myserverfifo martin update 031716 0800 1130 Shopping 
./client ./myserverfifo tim get 031616 1500
./client ./myserverfifo martin get 031716 0800 
./client ./myserverfifo steve get 031516
